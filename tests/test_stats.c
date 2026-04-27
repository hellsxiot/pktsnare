#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "../src/stats.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [TEST] %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define FAIL(msg)  do { printf("FAIL: %s\n", msg); } while(0)

static void test_init(void) {
    TEST("stats_init zeros counters");
    pktsnare_stats_t s;
    stats_init(&s);
    assert(s.total_packets == 0);
    assert(s.total_bytes   == 0);
    assert(s.tcp_packets   == 0);
    assert(s.dropped_packets == 0);
    PASS();
}

static void test_update_tcp(void) {
    TEST("stats_update increments tcp counter");
    pktsnare_stats_t s;
    stats_init(&s);
    stats_update(&s, 6, 100);
    assert(s.total_packets == 1);
    assert(s.total_bytes   == 100);
    assert(s.tcp_packets   == 1);
    assert(s.udp_packets   == 0);
    PASS();
}

static void test_update_udp(void) {
    TEST("stats_update increments udp counter");
    pktsnare_stats_t s;
    stats_init(&s);
    stats_update(&s, 17, 64);
    assert(s.udp_packets == 1);
    assert(s.tcp_packets == 0);
    PASS();
}

static void test_update_other(void) {
    TEST("stats_update increments other for unknown proto");
    pktsnare_stats_t s;
    stats_init(&s);
    stats_update(&s, 253, 40);
    assert(s.other_packets == 1);
    assert(s.total_packets == 1);
    PASS();
}

static void test_drop(void) {
    TEST("stats_drop increments dropped counter");
    pktsnare_stats_t s;
    stats_init(&s);
    stats_drop(&s);
    stats_drop(&s);
    assert(s.dropped_packets == 2);
    assert(s.total_packets   == 0);
    PASS();
}

static void test_null_safety(void) {
    TEST("null pointer safety");
    stats_init(NULL);
    stats_update(NULL, 6, 100);
    stats_drop(NULL);
    stats_print(NULL);
    PASS();
}

int main(void) {
    printf("Running stats tests...\n");
    test_init();
    test_update_tcp();
    test_update_udp();
    test_update_other();
    test_drop();
    test_null_safety();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
