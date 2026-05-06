#include "conntrack.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

struct conntrack_ctx {
    conn_entry_t *entries;
    int           max;
    int           count;
};

static uint32_t conn_hash(const conn_key_t *k, int max) {
    uint32_t h = k->src_ip ^ k->dst_ip;
    h ^= ((uint32_t)k->src_port << 16) | k->dst_port;
    h ^= k->proto;
    h ^= (h >> 16);
    return h % (uint32_t)max;
}

static int key_eq(const conn_key_t *a, const conn_key_t *b) {
    return memcmp(a, b, sizeof(conn_key_t)) == 0;
}

conntrack_ctx_t *conntrack_init(int max_entries) {
    if (max_entries <= 0) max_entries = CONNTRACK_MAX_ENTRIES;
    conntrack_ctx_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) return NULL;
    ctx->entries = calloc((size_t)max_entries, sizeof(conn_entry_t));
    if (!ctx->entries) { free(ctx); return NULL; }
    ctx->max = max_entries;
    ctx->count = 0;
    return ctx;
}

void conntrack_free(conntrack_ctx_t *ctx) {
    if (!ctx) return;
    free(ctx->entries);
    free(ctx);
}

int conntrack_update(conntrack_ctx_t *ctx, const conn_key_t *key,
                     uint32_t pkt_len, int is_reverse) {
    if (!ctx || !key) return -1;
    uint32_t idx = conn_hash(key, ctx->max);
    uint32_t start = idx;
    do {
        conn_entry_t *e = &ctx->entries[idx];
        if (e->state == CONN_STATE_CLOSED ||
            (e->state == CONN_STATE_NEW && e->first_seen == 0)) {
            /* empty slot */
            e->key = *key;
            e->state = CONN_STATE_NEW;
            e->first_seen = time(NULL);
            e->last_seen  = e->first_seen;
            e->bytes_fwd  = is_reverse ? 0 : pkt_len;
            e->bytes_rev  = is_reverse ? pkt_len : 0;
            e->pkts_fwd   = is_reverse ? 0 : 1;
            e->pkts_rev   = is_reverse ? 1 : 0;
            ctx->count++;
            return 0;
        }
        if (key_eq(&e->key, key)) {
            e->last_seen = time(NULL);
            if (is_reverse) { e->bytes_rev += pkt_len; e->pkts_rev++; }
            else            { e->bytes_fwd += pkt_len; e->pkts_fwd++; }
            if (e->state == CONN_STATE_NEW) e->state = CONN_STATE_ESTABLISHED;
            return 0;
        }
        idx = (idx + 1) % (uint32_t)ctx->max;
    } while (idx != start);
    return -1; /* table full */
}

conn_entry_t *conntrack_lookup(conntrack_ctx_t *ctx, const conn_key_t *key) {
    if (!ctx || !key) return NULL;
    uint32_t idx = conn_hash(key, ctx->max);
    uint32_t start = idx;
    do {
        conn_entry_t *e = &ctx->entries[idx];
        if (e->first_seen != 0 && key_eq(&e->key, key)) return e;
        idx = (idx + 1) % (uint32_t)ctx->max;
    } while (idx != start);
    return NULL;
}

int conntrack_expire(conntrack_ctx_t *ctx, int timeout_sec) {
    if (!ctx) return -1;
    time_t now = time(NULL);
    int expired = 0;
    for (int i = 0; i < ctx->max; i++) {
        conn_entry_t *e = &ctx->entries[i];
        if (e->first_seen != 0 && e->state != CONN_STATE_CLOSED) {
            if ((now - e->last_seen) >= timeout_sec) {
                e->state = CONN_STATE_CLOSED;
                ctx->count--;
                expired++;
            }
        }
    }
    return expired;
}

int conntrack_count(const conntrack_ctx_t *ctx) {
    return ctx ? ctx->count : 0;
}

void conntrack_dump(const conntrack_ctx_t *ctx) {
    if (!ctx) return;
    printf("[conntrack] active=%d max=%d\n", ctx->count, ctx->max);
    for (int i = 0; i < ctx->max; i++) {
        const conn_entry_t *e = &ctx->entries[i];
        if (e->first_seen == 0 || e->state == CONN_STATE_CLOSED) continue;
        printf("  %08x:%u -> %08x:%u proto=%u state=%d fwd=%llu/%u rev=%llu/%u\n",
               e->key.src_ip, e->key.src_port,
               e->key.dst_ip, e->key.dst_port,
               e->key.proto, e->state,
               (unsigned long long)e->bytes_fwd, e->pkts_fwd,
               (unsigned long long)e->bytes_rev, e->pkts_rev);
    }
}
