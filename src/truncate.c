#include <stdio.h>
#include <string.h>
#include "truncate.h"

void truncate_init(truncate_ctx_t *ctx, size_t max_payload)
{
    if (!ctx)
        return;

    memset(ctx, 0, sizeof(*ctx));

    if (max_payload == 0 || max_payload > TRUNCATE_MAX_PAYLOAD)
        ctx->max_payload = TRUNCATE_DEFAULT_LEN;
    else
        ctx->max_payload = max_payload;

    ctx->enabled = 1;
}

void truncate_reset_stats(truncate_ctx_t *ctx)
{
    if (!ctx)
        return;
    ctx->total_bytes_dropped = 0;
    ctx->packets_truncated   = 0;
}

size_t truncate_apply(truncate_ctx_t *ctx, uint8_t *buf, size_t *len)
{
    size_t dropped = 0;

    if (!ctx || !buf || !len)
        return 0;

    if (!ctx->enabled)
        return 0;

    if (*len > ctx->max_payload) {
        dropped = *len - ctx->max_payload;
        *len    = ctx->max_payload;
        ctx->total_bytes_dropped += dropped;
        ctx->packets_truncated++;
    }

    return dropped;
}

void truncate_print_stats(const truncate_ctx_t *ctx)
{
    if (!ctx)
        return;

    printf("[truncate] enabled=%d max_payload=%zu\n",
           ctx->enabled, ctx->max_payload);
    printf("[truncate] packets_truncated=%-8llu total_bytes_dropped=%llu\n",
           (unsigned long long)ctx->packets_truncated,
           (unsigned long long)ctx->total_bytes_dropped);
}
