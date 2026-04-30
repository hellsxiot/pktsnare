#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "../src/ratelimit.h"

static void sleep_ms(int ms)
{
    struct timespec ts = { .tv_sec = ms / 1000,
                           .tv_nsec = (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

static void test_init(void)
{
    ratelimit_t rl;
    assert(ratelimit_init(&rl, 100.0, 10.0) == 0);
    assert(rl.rate     == 100.0);
    assert(rl.capacity == 10.0);
    assert(rl.tokens   == 10.0);
    assert(rl.dropped  == 0);
    assert(rl.allowed  == 0);
    printf("PASS test_init\n");
}

static void test_init_invalid(void)
{
    ratelimit_t rl;
    assert(ratelimit_init(NULL, 10.0, 5.0) != 0);
    assert(ratelimit_init(&rl, 0.0,  5.0) != 0);
    assert(ratelimit_init(&rl, 10.0, 0.0) != 0);
    printf("PASS test_init_invalid\n");
}

static void test_burst_allows_up_to_capacity(void)
{
    ratelimit_t rl;
    ratelimit_init(&rl, 1.0, 5.0); /* 1 pps, burst 5 */
    int allowed = 0;
    for (int i = 0; i < 10; i++)
        allowed += ratelimit_check(&rl);
    /* first 5 should pass (burst), rest dropped */
    assert(allowed == 5);
    assert(rl.dropped == 5);
    printf("PASS test_burst_allows_up_to_capacity\n");
}

static void test_refill_over_time(void)
{
    ratelimit_t rl;
    ratelimit_init(&rl, 100.0, 1.0); /* 100 pps, burst 1 */
    /* drain the single token */
    assert(ratelimit_check(&rl) == 1);
    assert(ratelimit_check(&rl) == 0);
    /* wait ~20 ms => ~2 tokens at 100 pps, capped at 1 */
    sleep_ms(20);
    assert(ratelimit_check(&rl) == 1);
    printf("PASS test_refill_over_time\n");
}

static void test_drop_ratio(void)
{
    ratelimit_t rl;
    ratelimit_init(&rl, 1.0, 2.0);
    ratelimit_check(&rl); /* allowed */
    ratelimit_check(&rl); /* allowed */
    ratelimit_check(&rl); /* dropped */
    ratelimit_check(&rl); /* dropped */
    double ratio = ratelimit_drop_ratio(&rl);
    assert(ratio >= 0.49 && ratio <= 0.51);
    printf("PASS test_drop_ratio\n");
}

static void test_reset(void)
{
    ratelimit_t rl;
    ratelimit_init(&rl, 1.0, 3.0);
    ratelimit_check(&rl);
    ratelimit_check(&rl);
    ratelimit_reset(&rl);
    assert(rl.tokens  == 3.0);
    assert(rl.allowed == 0);
    assert(rl.dropped == 0);
    printf("PASS test_reset\n");
}

int main(void)
{
    test_init();
    test_init_invalid();
    test_burst_allows_up_to_capacity();
    test_refill_over_time();
    test_drop_ratio();
    test_reset();
    printf("All ratelimit tests passed.\n");
    return 0;
}
