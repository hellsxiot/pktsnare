#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/tcpreasm.h"

static void test_init(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    assert(ctx.stream_count == 0);
    assert(ctx.reassembled == 0);
    assert(ctx.dropped_ooo == 0);
    printf("[PASS] test_init\n");
}

static void test_get_stream_new(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    tcpreasm_stream_t *s = tcpreasm_get_stream(&ctx,
        0xC0A80001, 0xC0A80002, 1234, 80);
    assert(s != NULL);
    assert(s->in_use == 1);
    assert(s->src_port == 1234);
    assert(ctx.stream_count == 1);
    printf("[PASS] test_get_stream_new\n");
}

static void test_get_stream_existing(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    tcpreasm_stream_t *s1 = tcpreasm_get_stream(&ctx,
        0xC0A80001, 0xC0A80002, 1234, 80);
    tcpreasm_stream_t *s2 = tcpreasm_get_stream(&ctx,
        0xC0A80001, 0xC0A80002, 1234, 80);
    assert(s1 == s2);
    assert(ctx.stream_count == 1);
    printf("[PASS] test_get_stream_existing\n");
}

static void test_add_and_get_segment(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    tcpreasm_stream_t *s = tcpreasm_get_stream(&ctx,
        0xC0A80001, 0xC0A80002, 5000, 443);
    assert(s != NULL);

    uint8_t payload[] = "hello world";
    int rc = tcpreasm_add_segment(&ctx, s, 100, payload,
        (uint16_t)sizeof(payload));
    assert(rc == 0);
    assert(s->seg_count == 1);

    uint8_t buf[256];
    size_t out_len = 0;
    rc = tcpreasm_get_data(s, buf, sizeof(buf), &out_len);
    assert(rc == 0);
    assert(out_len == sizeof(payload));
    assert(memcmp(buf, payload, sizeof(payload)) == 0);
    printf("[PASS] test_add_and_get_segment\n");
}

static void test_expire(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    tcpreasm_stream_t *s = tcpreasm_get_stream(&ctx,
        0x0A000001, 0x0A000002, 2000, 8080);
    assert(s != NULL);
    s->last_seen = 0;
    tcpreasm_expire(&ctx, TCPREASM_TIMEOUT_SEC + 1);
    assert(ctx.expired == 1);
    assert(ctx.stream_count == 0);
    printf("[PASS] test_expire\n");
}

static void test_stats(void) {
    tcpreasm_ctx_t ctx;
    tcpreasm_init(&ctx);
    uint64_t r, d, e;
    tcpreasm_stats(&ctx, &r, &d, &e);
    assert(r == 0 && d == 0 && e == 0);
    printf("[PASS] test_stats\n");
}

int main(void) {
    test_init();
    test_get_stream_new();
    test_get_stream_existing();
    test_add_and_get_segment();
    test_expire();
    test_stats();
    printf("All tcpreasm tests passed.\n");
    return 0;
}
