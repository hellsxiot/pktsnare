#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/iface.h"

static void test_iface_list_returns_entries(void)
{
    iface_list_t list;
    memset(&list, 0, sizeof(list));

    int rc = iface_list(&list);
    /* On any Linux system there should be at least loopback */
    assert(rc == 0);
    assert(list.count > 0);
    assert(list.count <= IFACE_MAX_COUNT);
    printf("[PASS] iface_list: found %d interface(s)\n", list.count);
}

static void test_iface_list_names_nonempty(void)
{
    iface_list_t list;
    memset(&list, 0, sizeof(list));
    assert(iface_list(&list) == 0);

    for (int i = 0; i < list.count; i++) {
        assert(list.entries[i].name[0] != '\0');
    }
    printf("[PASS] iface_list: all names non-empty\n");
}

static void test_iface_get_loopback(void)
{
    iface_info_t info;
    memset(&info, 0, sizeof(info));

    int rc = iface_get("lo", &info);
    assert(rc == 0);
    assert(strcmp(info.name, "lo") == 0);
    assert(info.flags & IFACE_FLAG_LOOPBACK);
    printf("[PASS] iface_get: loopback found, mtu=%u\n", info.mtu);
}

static void test_iface_get_nonexistent(void)
{
    iface_info_t info;
    memset(&info, 0, sizeof(info));

    int rc = iface_get("nonexistent99", &info);
    assert(rc != 0);
    printf("[PASS] iface_get: nonexistent interface returns error\n");
}

static void test_iface_is_up_loopback(void)
{
    int up = iface_is_up("lo");
    assert(up == 1);
    printf("[PASS] iface_is_up: loopback is up\n");
}

static void test_iface_is_up_nonexistent(void)
{
    int up = iface_is_up("ghost0");
    assert(up == 0);
    printf("[PASS] iface_is_up: nonexistent returns 0\n");
}

int main(void)
{
    printf("=== test_iface ===\n");
    test_iface_list_returns_entries();
    test_iface_list_names_nonempty();
    test_iface_get_loopback();
    test_iface_get_nonexistent();
    test_iface_is_up_loopback();
    test_iface_is_up_nonexistent();
    printf("All iface tests passed.\n");
    return 0;
}
