#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../src/truncate.h"

static void test_init_defaults(void)
{
    truncate_ctx_t ctx;
    truncate_init(&ctx, 0); /* 0 => use default */
    assert(ctx.max_payload == TRUNCATE_DEFAULT_LEN);
    assert(ctx.enabled == 1);
    assert(ctx.packets_truncated == 0);
    assert(ctx.total_bytes_dropped == 0);
    printf("PASS test_init_defaults\n");
}

static void test_init_custom(void)
{
    truncate_ctx_t ctx;
    truncate_init(&ctx, 256);
    assert(ctx.max_payload == 256);
    printf("PASS test_init_custom\n");
}

static void test_no_truncation_needed(void)
{
    truncate_ctx_t ctx;
    uint8_t buf[64];
    size_t  len = 64;

    truncate_init(&ctx, 128);
    size_t dropped = truncate_apply(&ctx, buf, &len);

    assert(dropped == 0);
    assert(len == 64);
    assert(ctx.packets_truncated == 0);
    printf("PASS test_no_truncation_needed\n");
}

static void test_truncation_applied(void)
{
    truncate_ctx_t ctx;
    uint8_t buf[512];
    size_t  len = 512;

    memset(buf, 0xAB, sizeof(buf));
    truncate_init(&ctx, 128);
    size_t dropped = truncate_apply(&ctx, buf, &len);

    assert(dropped == 384);
    assert(len == 128);
    assert(ctx.packets_truncated == 1);
    assert(ctx.total_bytes_dropped == 384);
    printf("PASS test_truncation_applied\n");
}

static void test_disabled_passthrough(void)
{
    truncate_ctx_t ctx;
    uint8_t buf[200];
    size_t  len = 200;

    truncate_init(&ctx, 64);
    ctx.enabled = 0;
    size_t dropped = truncate_apply(&ctx, buf, &len);

    assert(dropped == 0);
    assert(len == 200);
    printf("PASS test_disabled_passthrough\n");
}

static void test_reset_stats(void)
{
    truncate_ctx_t ctx;
    uint8_t buf[300];
    size_t  len = 300;

    truncate_init(&ctx, 100);
    truncate_apply(&ctx, buf, &len);
    assert(ctx.packets_truncated == 1);

    truncate_reset_stats(&ctx);
    assert(ctx.packets_truncated == 0);
    assert(ctx.total_bytes_dropped == 0);
    assert(ctx.max_payload == 100); /* config unchanged */
    printf("PASS test_reset_stats\n");
}

int main(void)
{
    test_init_defaults();
    test_init_custom();
    test_no_truncation_needed();
    test_truncation_applied();
    test_disabled_passthrough();
    test_reset_stats();
    printf("All truncate tests passed.\n");
    return 0;
}
