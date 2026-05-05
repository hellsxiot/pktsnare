#ifndef TRUNCATE_H
#define TRUNCATE_H

#include <stddef.h>
#include <stdint.h>

/*
 * truncate.h - packet payload truncation helpers
 *
 * Provides utilities to truncate captured packet payloads to a
 * configured maximum byte limit, useful for privacy or storage
 * constraints on embedded targets.
 */

#define TRUNCATE_MAX_PAYLOAD 65535
#define TRUNCATE_DEFAULT_LEN 128

typedef struct {
    size_t max_payload;   /* max bytes to retain per packet */
    int    enabled;       /* 0 = passthrough, 1 = truncate active */
    uint64_t total_bytes_dropped; /* cumulative bytes discarded */
    uint64_t packets_truncated;   /* how many packets were trimmed */
} truncate_ctx_t;

/* Initialise context with given max payload length */
void truncate_init(truncate_ctx_t *ctx, size_t max_payload);

/* Reset stats counters, keep config */
void truncate_reset_stats(truncate_ctx_t *ctx);

/*
 * Apply truncation to buf of length *len.
 * Updates *len in-place. Returns bytes removed (0 if none).
 */
size_t truncate_apply(truncate_ctx_t *ctx, uint8_t *buf, size_t *len);

/* Print truncation stats to stdout */
void truncate_print_stats(const truncate_ctx_t *ctx);

#endif /* TRUNCATE_H */
