#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/fragtrack.h"

static void test_init(void) {
    fragtrack_t ft;
    fragtrack_init(&ft);
    assert(ft.count == 0);
    assert(ft.reassembled == 0);
    assert(ft.dropped == 0);
    assert(ft.expired == 0);
    printf("PASS test_init\n");
}

static void test_add_and_complete(void) {
    fragtrack_t ft;
    fragtrack_init(&ft);

    /* First fragment: offset=0, size=512, more_frags=1 */
    int r = fragtrack_add(&ft, 0xC0A80001, 0xC0A80002, 0xABCD, 6, 0, 512, 1);
    assert(r == 0);
    assert(!fragtrack_is_complete(&ft, 0xC0A80001, 0xC0A80002, 0xABCD, 6));

    /* Last fragment: offset=512, size=200, more_frags=0 */
    r = fragtrack_add(&ft, 0xC0A80001, 0xC0A80002, 0xABCD, 6, 512, 200, 0);
    assert(r == 0);
    assert(fragtrack_is_complete(&ft, 0xC0A80001, 0xC0A80002, 0xABCD, 6));

    uint64_t reassembled, dropped, expired;
    fragtrack_stats(&ft, &reassembled, &dropped, &expired);
    assert(reassembled == 1);
    assert(dropped == 0);
    printf("PASS test_add_and_complete\n");
}

static void test_remove(void) {
    fragtrack_t ft;
    fragtrack_init(&ft);

    fragtrack_add(&ft, 0x01010101, 0x02020202, 0x1111, 17, 0, 300, 1);
    assert(ft.count == 1);
    fragtrack_remove(&ft, 0x01010101, 0x02020202, 0x1111, 17);
    assert(ft.count == 0);
    /* Removing non-existent entry should be a no-op */
    fragtrack_remove(&ft, 0x01010101, 0x02020202, 0x1111, 17);
    assert(ft.count == 0);
    printf("PASS test_remove\n");
}

static void test_expire(void) {
    fragtrack_t ft;
    fragtrack_init(&ft);

    fragtrack_add(&ft, 0xAABBCCDD, 0x11223344, 0x5566, 6, 0, 100, 1);
    assert(ft.count == 1);

    /* Force expiry by passing a future timestamp */
    time_t future = time(NULL) + FRAGTRACK_TIMEOUT_SEC + 5;
    fragtrack_expire(&ft, future);

    assert(ft.count == 0);
    assert(ft.expired == 1);
    printf("PASS test_expire\n");
}

static void test_stats(void) {
    fragtrack_t ft;
    fragtrack_init(&ft);

    uint64_t r, d, e;
    fragtrack_stats(&ft, &r, &d, &e);
    assert(r == 0 && d == 0 && e == 0);

    fragtrack_stats(&ft, NULL, NULL, NULL); /* should not crash */
    printf("PASS test_stats\n");
}

int main(void) {
    test_init();
    test_add_and_complete();
    test_remove();
    test_expire();
    test_stats();
    printf("All fragtrack tests passed.\n");
    return 0;
}
