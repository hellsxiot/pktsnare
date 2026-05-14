#include <string.h>
#include <stdio.h>
#include <time.h>
#include "dnstrack.h"

void dnstrack_init(dnstrack_ctx_t *ctx, int ttl_seconds)
{
    if (!ctx) return;
    memset(ctx, 0, sizeof(*ctx));
    ctx->ttl_seconds = (ttl_seconds > 0) ? ttl_seconds : DNSTRACK_TTL_DEFAULT;
}

void dnstrack_reset(dnstrack_ctx_t *ctx)
{
    if (!ctx) return;
    ctx->count = 0;
    ctx->total_queries = 0;
    ctx->total_responses = 0;
    ctx->expired = 0;
    memset(ctx->entries, 0, sizeof(ctx->entries));
}

int dnstrack_add_query(dnstrack_ctx_t *ctx, uint16_t txid,
                       const char *qname, uint16_t qtype, uint32_t src_ip)
{
    if (!ctx || !qname) return -1;
    if (ctx->count >= DNSTRACK_MAX_ENTRIES) return -1;

    dns_entry_t *e = &ctx->entries[ctx->count++];
    e->txid      = txid;
    e->qtype     = qtype;
    e->src_ip    = src_ip;
    e->timestamp = time(NULL);
    e->answered  = 0;
    e->resolved_ip = 0;
    strncpy(e->qname, qname, DNSTRACK_MAX_NAME_LEN - 1);
    e->qname[DNSTRACK_MAX_NAME_LEN - 1] = '\0';

    ctx->total_queries++;
    return 0;
}

int dnstrack_match_response(dnstrack_ctx_t *ctx, uint16_t txid,
                            uint32_t resolved_ip)
{
    if (!ctx) return -1;
    for (int i = 0; i < ctx->count; i++) {
        dns_entry_t *e = &ctx->entries[i];
        if (e->txid == txid && !e->answered) {
            e->answered    = 1;
            e->resolved_ip = resolved_ip;
            ctx->total_responses++;
            return 0;
        }
    }
    return -1;
}

void dnstrack_expire(dnstrack_ctx_t *ctx)
{
    if (!ctx) return;
    time_t now = time(NULL);
    int i = 0;
    while (i < ctx->count) {
        dns_entry_t *e = &ctx->entries[i];
        if ((now - e->timestamp) > ctx->ttl_seconds) {
            ctx->expired++;
            ctx->entries[i] = ctx->entries[--ctx->count];
        } else {
            i++;
        }
    }
}

int dnstrack_lookup(const dnstrack_ctx_t *ctx, uint16_t txid,
                    dns_entry_t *out)
{
    if (!ctx || !out) return -1;
    for (int i = 0; i < ctx->count; i++) {
        if (ctx->entries[i].txid == txid) {
            *out = ctx->entries[i];
            return 0;
        }
    }
    return -1;
}

void dnstrack_print_stats(const dnstrack_ctx_t *ctx)
{
    if (!ctx) return;
    printf("[dnstrack] queries=%llu responses=%llu expired=%llu active=%d\n",
           (unsigned long long)ctx->total_queries,
           (unsigned long long)ctx->total_responses,
           (unsigned long long)ctx->expired,
           ctx->count);
}
