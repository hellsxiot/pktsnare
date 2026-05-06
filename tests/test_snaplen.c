#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/snaplen.h"

static int passed = 0;
static int failed = 0;

#define RUN(name, expr) \
    do { \
        if (expr) { printf("  PASS  %s\n", name); passed++; } \
        else      { printf("  FAIL  %s\n", name); failed++; } \
    } while (0)

static void test_init(void)
{
    snaplen_ctx_t ctx;

    RUN("init default",    snaplen_init(&ctx, SNAPLEN_DEFAULT) == 0);
    RUN("init min",        snaplen_init(&ctx, SNAPLEN_MIN)     == 0);
    RUN("init max",        snaplen_init(&ctx, SNAPLEN_MAX)     == 0);
    RUN("init below min",  snaplen_init(&ctx, 0)               == -1);
    RUN("init above max",  snaplen_init(&ctx, 70000)           == -1);
    RUN("init null ctx",   snaplen_init(NULL, 128)             == -1);
}

static void test_clamp(void)
{
    snaplen_ctx_t ctx;
    snaplen_init(&ctx, 128);

    size_t r;

    r = snaplen_clamp(&ctx, 64);
    RUN("clamp below snap",  r == 64);
    RUN("no truncation",     ctx.truncated == 0);

    r = snaplen_clamp(&ctx, 128);
    RUN("clamp equal snap",  r == 128);
    RUN("no truncation eq",  ctx.truncated == 0);

    r = snaplen_clamp(&ctx, 512);
    RUN("clamp above snap",  r == 128);
    RUN("truncated count",   ctx.truncated == 1);
    RUN("total count",       ctx.total == 3);

    RUN("clamp null ctx",    snaplen_clamp(NULL, 64) == 64);
}

static void test_set_get(void)
{
    snaplen_ctx_t ctx;
    snaplen_init(&ctx, 256);

    RUN("get initial",       snaplen_get(&ctx) == 256);
    RUN("set valid",         snaplen_set(&ctx, 512) == 0);
    RUN("get after set",     snaplen_get(&ctx) == 512);
    RUN("set invalid high",  snaplen_set(&ctx, 70000) == -1);
    RUN("get unchanged",     snaplen_get(&ctx) == 512);
    RUN("set invalid low",   snaplen_set(&ctx, 0) == -1);
    RUN("get null",          snaplen_get(NULL) == 0);
    RUN("set null",          snaplen_set(NULL, 128) == -1);
}

static void test_reset(void)
{
    snaplen_ctx_t ctx;
    snaplen_init(&ctx, 128);

    /* generate some stats by clamping a few packets */
    snaplen_clamp(&ctx, 64);
    snaplen_clamp(&ctx, 512);
    snaplen_clamp(&ctx, 512);

    RUN("pre-reset total",      ctx.total     == 3);
    RUN("pre-reset truncated",  ctx.truncated == 2);

    snaplen_reset_stats(&ctx);

    RUN("post-reset total",     ctx.total     == 0);
    RUN("post-reset truncated", ctx.truncated == 0);
    /* snaplen value itself should be preserved after reset */
    RUN("snaplen preserved",    snaplen_get(&ctx) == 128);
}

int main(void)
{
    printf("=== test_snaplen ===\n");
    test_init();
    test_clamp();
    test_set_get();
    test_reset();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
