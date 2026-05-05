#ifndef SNAPLEN_H
#define SNAPLEN_H

#include <stdint.h>
#include <stddef.h>

/* Default and boundary values for snapshot length */
#define SNAPLEN_DEFAULT   65535
#define SNAPLEN_MIN       14
#define SNAPLEN_MAX       65535

typedef struct {
    uint32_t value;      /* current snap length in bytes */
    uint32_t truncated;  /* count of packets truncated */
    uint32_t total;      /* total packets seen */
} snaplen_ctx_t;

/*
 * Initialize snapshot length context.
 * Returns 0 on success, -1 if len is out of bounds.
 */
int snaplen_init(snaplen_ctx_t *ctx, uint32_t len);

/*
 * Clamp packet length to the configured snap length.
 * Updates truncation counters. Returns clamped length.
 */
size_t snaplen_clamp(snaplen_ctx_t *ctx, size_t pkt_len);

/*
 * Update snap length at runtime.
 * Returns 0 on success, -1 on invalid value.
 */
int snaplen_set(snaplen_ctx_t *ctx, uint32_t len);

/* Return current snap length value. */
uint32_t snaplen_get(const snaplen_ctx_t *ctx);

/* Print truncation stats to stdout. */
void snaplen_print_stats(const snaplen_ctx_t *ctx);

#endif /* SNAPLEN_H */
