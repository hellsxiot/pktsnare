#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/ring.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN(fn) do { tests_run++; fn(); tests_passed++; } while(0)

static void test_create_destroy(void) {
    ring_buf_t *rb = ring_create(8);
    assert(rb != NULL);
    assert(ring_count(rb) == 0);
    assert(ring_dropped(rb) == 0);
    ring_destroy(rb);

    /* default capacity */
    rb = ring_create(0);
    assert(rb != NULL);
    ring_destroy(rb);
    printf("  [PASS] create/destroy\n");
}

static void test_push_pop(void) {
    ring_buf_t *rb = ring_create(4);
    uint8_t pkt[] = {0x01, 0x02, 0x03};

    assert(ring_push(rb, pkt, sizeof(pkt), 1000) == 0);
    assert(ring_count(rb) == 1);

    ring_entry_t e;
    assert(ring_pop(rb, &e) == 0);
    assert(e.len == sizeof(pkt));
    assert(memcmp(e.data, pkt, sizeof(pkt)) == 0);
    assert(e.timestamp_us == 1000);
    assert(ring_count(rb) == 0);

    /* pop from empty */
    assert(ring_pop(rb, &e) == 1);
    ring_destroy(rb);
    printf("  [PASS] push/pop\n");
}

static void test_overflow_drops(void) {
    ring_buf_t *rb = ring_create(3);
    uint8_t pkt[4] = {0xAA, 0xBB, 0xCC, 0xDD};

    for (int i = 0; i < 3; i++)
        ring_push(rb, pkt, sizeof(pkt), (uint64_t)i);

    assert(ring_count(rb) == 3);
    assert(ring_dropped(rb) == 0);

    /* push one more — should overwrite oldest */
    pkt[0] = 0xFF;
    ring_push(rb, pkt, sizeof(pkt), 999);
    assert(ring_count(rb) == 3);
    assert(ring_dropped(rb) == 1);

    /* oldest should now be the second packet pushed (ts=1) */
    const ring_entry_t *p = ring_peek(rb);
    assert(p != NULL);
    assert(p->timestamp_us == 1);

    ring_destroy(rb);
    printf("  [PASS] overflow/drop tracking\n");
}

static void test_invalid_args(void) {
    ring_buf_t *rb = ring_create(4);
    uint8_t buf[8] = {0};

    assert(ring_push(NULL, buf, 8, 0) == -1);
    assert(ring_push(rb, NULL, 8, 0)  == -1);
    assert(ring_push(rb, buf, 0, 0)   == -1);

    ring_entry_t e;
    assert(ring_pop(NULL, &e) == -1);
    assert(ring_pop(rb, NULL) == -1);

    assert(ring_peek(NULL) == NULL);
    ring_destroy(rb);
    printf("  [PASS] invalid args\n");
}

int main(void) {
    printf("=== ring buffer tests ===\n");
    RUN(test_create_destroy);
    RUN(test_push_pop);
    RUN(test_overflow_drops);
    RUN(test_invalid_args);
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
