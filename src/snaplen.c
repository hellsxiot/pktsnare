#include <stdio.h>
#include <string.h>
#include "snaplen.h"

int snaplen_init(snaplen_ctx_t *ctx, uint32_t len)
{
    if (!ctx)
        return -1;

    memset(ctx, 0, sizeof(*ctx));

    if (len < SNAPLEN_MIN || len > SNAPLEN_MAX)
        return -1;

    ctx->value = len;
    return 0;
}

size_t snaplen_clamp(snaplen_ctx_t *ctx, size_t pkt_len)
{
    if (!ctx)
        return pkt_len;

    ctx->total++;

    if (pkt_len > (size_t)ctx->value) {
        ctx->truncated++;
        return (size_t)ctx->value;
    }

    return pkt_len;
}

int snaplen_set(snaplen_ctx_t *ctx, uint32_t len)
{
    if (!ctx)
        return -1;

    if (len < SNAPLEN_MIN || len > SNAPLEN_MAX)
        return -1;

    ctx->value = len;
    return 0;
}

uint32_t snaplen_get(const snaplen_ctx_t *ctx)
{
    if (!ctx)
        return 0;
    return ctx->value;
}

void snaplen_print_stats(const snaplen_ctx_t *ctx)
{
    if (!ctx)
        return;

    printf("snaplen: %u bytes | total: %u | truncated: %u (%.1f%%)",
           ctx->value,
           ctx->total,
           ctx->truncated,
           ctx->total > 0
               ? (100.0 * ctx->truncated / ctx->total)
               : 0.0);
    printf("\n");
}
