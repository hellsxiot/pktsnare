#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/iface.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_iface_exists_loopback(void) {
    /* loopback should always exist on linux */
    int result = iface_exists("lo");
    assert(result == 1);
    printf("  [pass] iface_exists loopback\n");
}

static void test_iface_exists_bogus(void) {
    int result = iface_exists("notarealif99");
    assert(result == 0);
    printf("  [pass] iface_exists bogus name\n");
}

static void test_iface_exists_null(void) {
    int result = iface_exists(NULL);
    assert(result == 0);
    printf("  [pass] iface_exists null\n");
}

static void test_iface_exists_empty(void) {
    int result = iface_exists("");
    assert(result == 0);
    printf("  [pass] iface_exists empty string\n");
}

static void test_iface_is_up_loopback(void) {
    /* lo is typically up */
    int result = iface_is_up("lo");
    assert(result == 1);
    printf("  [pass] iface_is_up loopback\n");
}

static void test_iface_is_up_bogus(void) {
    int result = iface_is_up("notarealif99");
    assert(result == 0);
    printf("  [pass] iface_is_up bogus\n");
}

static void test_iface_get_mtu_loopback(void) {
    int mtu = iface_get_mtu("lo");
    assert(mtu > 0);
    printf("  [pass] iface_get_mtu loopback mtu=%d\n", mtu);
}

static void test_iface_get_mtu_bogus(void) {
    int mtu = iface_get_mtu("notarealif99");
    assert(mtu == -1);
    printf("  [pass] iface_get_mtu bogus returns -1\n");
}

static void test_iface_list(void) {
    char names[32][IFNAMSIZ];
    int count = iface_list(names, 32);
    assert(count >= 1);
    int found_lo = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], "lo") == 0) found_lo = 1;
    }
    assert(found_lo == 1);
    printf("  [pass] iface_list found %d ifaces including lo\n", count);
}

int main(void) {
    printf("=== test_iface ===\n");
    RUN_TEST(test_iface_exists_loopback);
    RUN_TEST(test_iface_exists_bogus);
    RUN_TEST(test_iface_exists_null);
    RUN_TEST(test_iface_exists_empty);
    RUN_TEST(test_iface_is_up_loopback);
    RUN_TEST(test_iface_is_up_bogus);
    RUN_TEST(test_iface_get_mtu_loopback);
    RUN_TEST(test_iface_get_mtu_bogus);
    RUN_TEST(test_iface_list);
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
