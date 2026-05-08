#ifndef IPDEFRAG_H
#define IPDEFRAG_H

#include <stdint.h>
#include <stddef.h>

/* Maximum number of concurrent reassembly contexts */
#define IPDEFRAG_MAX_FLOWS   64
/* Maximum time (seconds) to hold fragments before expiry */
#define IPDEFRAG_TIMEOUT_SEC 30
/* Maximum reassembled datagram size */
#define IPDEFRAG_MAX_SIZE    65535

typedef struct ipdefrag_ctx ipdefrag_ctx_t;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t ip_id;
    uint8_t  protocol;
} ipdefrag_key_t;

typedef struct {
    uint8_t  *data;
    size_t    len;
    ipdefrag_key_t key;
} ipdefrag_result_t;

/* Allocate a new defragmentation context */
ipdefrag_ctx_t *ipdefrag_create(void);

/* Free context and all pending fragments */
void ipdefrag_destroy(ipdefrag_ctx_t *ctx);

/*
 * Feed a raw IP fragment into the reassembler.
 * Returns 1 and populates *out if reassembly is complete.
 * Returns 0 if more fragments are needed.
 * Returns -1 on error.
 * Caller must free out->data when done.
 */
int ipdefrag_feed(ipdefrag_ctx_t *ctx,
                  const uint8_t *pkt, size_t pkt_len,
                  ipdefrag_result_t *out);

/* Expire flows older than IPDEFRAG_TIMEOUT_SEC; returns number evicted */
int ipdefrag_expire(ipdefrag_ctx_t *ctx, uint32_t now_sec);

/* Return number of active reassembly flows */
int ipdefrag_active(const ipdefrag_ctx_t *ctx);

#endif /* IPDEFRAG_H */
