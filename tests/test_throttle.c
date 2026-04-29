#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "../src/throttle.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS: %s\n", msg); passed++; } \
    else       { printf("  FAIL: %s\n", msg); failed++; } \
} while(0)

static void test_init(void)
{
    throttle_t t;
    printf("[test_init]\n");
    CHECK(throttle_init(&t, 100, 4096) == 0, "init returns 0");
    CHECK(t.max_pps == 100,  "max_pps set");
    CHECK(t.max_bps == 4096, "max_bps set");
    CHECK(t.dropped_pkts == 0, "dropped starts at 0");
    CHECK(throttle_init(NULL, 0, 0) == -1, "init NULL returns -1");
}

static void test_unlimited(void)
{
    throttle_t t;
    printf("[test_unlimited]\n");
    throttle_init(&t, 0, 0);
    /* With unlimited settings all packets should pass */
    for (int i = 0; i < 500; i++)
        CHECK(throttle_check(&t, 64) == 1, "unlimited allows packet");
    /* Only check once to avoid spam */
    (void)passed; /* suppress unused warning in loop */
}

static void test_pps_limit(void)
{
    throttle_t t;
    int allowed = 0, dropped = 0;
    printf("[test_pps_limit]\n");
    /* 10 pps -> window allows 10*100/1000 = 1 packet per 100ms window */
    throttle_init(&t, 10, 0);
    for (int i = 0; i < 20; i++) {
        if (throttle_check(&t, 64))
            allowed++;
        else
            dropped++;
    }
    CHECK(allowed >= 1,  "at least one packet allowed");
    CHECK(dropped >= 1,  "some packets dropped");
    CHECK(throttle_dropped(&t) == (uint32_t)dropped, "dropped count matches");
}

static void test_bps_limit(void)
{
    throttle_t t;
    int allowed = 0;
    printf("[test_bps_limit]\n");
    /* 1000 bps -> window allows 100 bytes per 100ms window */
    throttle_init(&t, 0, 1000);
    /* Each packet is 64 bytes; should allow 1 (64 bytes) then drop */
    for (int i = 0; i < 5; i++) {
        if (throttle_check(&t, 64))
            allowed++;
    }
    CHECK(allowed >= 1 && allowed <= 2, "bps limit restricts byte flow");
    CHECK(t.dropped_pkts >= 1, "dropped incremented on bps exceed");
}

static void test_reset(void)
{
    throttle_t t;
    printf("[test_reset]\n");
    throttle_init(&t, 5, 0);
    for (int i = 0; i < 10; i++)
        throttle_check(&t, 64);
    throttle_reset(&t);
    CHECK(t.pkt_count == 0,  "pkt_count reset");
    CHECK(t.byte_count == 0, "byte_count reset");
    /* After reset a packet should be allowed again */
    CHECK(throttle_check(&t, 64) == 1, "packet allowed after reset");
}

int main(void)
{
    printf("=== test_throttle ===\n");
    test_init();
    test_unlimited();
    test_pps_limit();
    test_bps_limit();
    test_reset();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
