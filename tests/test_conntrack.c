#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/conntrack.h"

static conn_key_t make_key(uint32_t sip, uint32_t dip,
                           uint16_t sp, uint16_t dp, uint8_t proto) {
    conn_key_t k;
    k.src_ip   = sip;
    k.dst_ip   = dip;
    k.src_port = sp;
    k.dst_port = dp;
    k.proto    = proto;
    return k;
}

static void test_init_free(void) {
    conntrack_ctx_t *ctx = conntrack_init(64);
    assert(ctx != NULL);
    assert(conntrack_count(ctx) == 0);
    conntrack_free(ctx);
    printf("  [PASS] test_init_free\n");
}

static void test_update_lookup(void) {
    conntrack_ctx_t *ctx = conntrack_init(64);
    assert(ctx);
    conn_key_t k = make_key(0x0a000001, 0x0a000002, 1234, 80, 6);
    assert(conntrack_update(ctx, &k, 512, 0) == 0);
    assert(conntrack_count(ctx) == 1);
    conn_entry_t *e = conntrack_lookup(ctx, &k);
    assert(e != NULL);
    assert(e->bytes_fwd == 512);
    assert(e->pkts_fwd == 1);
    assert(e->bytes_rev == 0);
    conntrack_free(ctx);
    printf("  [PASS] test_update_lookup\n");
}

static void test_reverse_update(void) {
    conntrack_ctx_t *ctx = conntrack_init(64);
    assert(ctx);
    conn_key_t k = make_key(0x0a000001, 0x0a000002, 5000, 443, 6);
    conntrack_update(ctx, &k, 100, 0);
    conntrack_update(ctx, &k, 200, 1);
    conn_entry_t *e = conntrack_lookup(ctx, &k);
    assert(e);
    assert(e->bytes_fwd == 100);
    assert(e->bytes_rev == 200);
    assert(e->pkts_fwd == 1);
    assert(e->pkts_rev == 1);
    assert(e->state == CONN_STATE_ESTABLISHED);
    conntrack_free(ctx);
    printf("  [PASS] test_reverse_update\n");
}

static void test_multiple_connections(void) {
    conntrack_ctx_t *ctx = conntrack_init(128);
    assert(ctx);
    for (int i = 0; i < 50; i++) {
        conn_key_t k = make_key(0x0a000001, 0x08080808,
                                (uint16_t)(1024 + i), 53, 17);
        assert(conntrack_update(ctx, &k, 64, 0) == 0);
    }
    assert(conntrack_count(ctx) == 50);
    conntrack_free(ctx);
    printf("  [PASS] test_multiple_connections\n");
}

static void test_lookup_missing(void) {
    conntrack_ctx_t *ctx = conntrack_init(32);
    assert(ctx);
    conn_key_t k = make_key(0x01020304, 0x05060708, 9999, 22, 6);
    assert(conntrack_lookup(ctx, &k) == NULL);
    conntrack_free(ctx);
    printf("  [PASS] test_lookup_missing\n");
}

static void test_expire(void) {
    conntrack_ctx_t *ctx = conntrack_init(32);
    assert(ctx);
    conn_key_t k = make_key(0x0a000001, 0x0a000002, 2000, 80, 6);
    conntrack_update(ctx, &k, 128, 0);
    assert(conntrack_count(ctx) == 1);
    /* expire with timeout=0 should mark everything stale */
    int n = conntrack_expire(ctx, 0);
    assert(n == 1);
    assert(conntrack_count(ctx) == 0);
    conntrack_free(ctx);
    printf("  [PASS] test_expire\n");
}

int main(void) {
    printf("=== test_conntrack ===\n");
    test_init_free();
    test_update_lookup();
    test_reverse_update();
    test_multiple_connections();
    test_lookup_missing();
    test_expire();
    printf("All conntrack tests passed.\n");
    return 0;
}
