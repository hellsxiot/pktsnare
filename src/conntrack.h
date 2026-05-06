#ifndef CONNTRACK_H
#define CONNTRACK_H

#include <stdint.h>
#include <time.h>

#define CONNTRACK_MAX_ENTRIES 1024
#define CONNTRACK_TIMEOUT_SEC 60

typedef enum {
    CONN_STATE_NEW = 0,
    CONN_STATE_ESTABLISHED,
    CONN_STATE_CLOSING,
    CONN_STATE_CLOSED
} conn_state_t;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
} conn_key_t;

typedef struct {
    conn_key_t   key;
    conn_state_t state;
    uint64_t     bytes_fwd;
    uint64_t     bytes_rev;
    uint32_t     pkts_fwd;
    uint32_t     pkts_rev;
    time_t       first_seen;
    time_t       last_seen;
} conn_entry_t;

typedef struct conntrack_ctx conntrack_ctx_t;

conntrack_ctx_t *conntrack_init(int max_entries);
void             conntrack_free(conntrack_ctx_t *ctx);
int              conntrack_update(conntrack_ctx_t *ctx, const conn_key_t *key,
                                  uint32_t pkt_len, int is_reverse);
conn_entry_t    *conntrack_lookup(conntrack_ctx_t *ctx, const conn_key_t *key);
int              conntrack_expire(conntrack_ctx_t *ctx, int timeout_sec);
int              conntrack_count(const conntrack_ctx_t *ctx);
void             conntrack_dump(const conntrack_ctx_t *ctx);

#endif /* CONNTRACK_H */
