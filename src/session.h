#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include <time.h>
#include <netinet/in.h>

#define SESSION_MAX_ENTRIES 1024
#define SESSION_TIMEOUT_SEC 60

typedef enum {
    PROTO_TCP = 6,
    PROTO_UDP = 17
} session_proto_t;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
} session_key_t;

typedef struct {
    session_key_t key;
    uint64_t      bytes_fwd;
    uint64_t      bytes_rev;
    uint32_t      pkts_fwd;
    uint32_t      pkts_rev;
    time_t        first_seen;
    time_t        last_seen;
    int           active;
} session_entry_t;

typedef struct {
    session_entry_t entries[SESSION_MAX_ENTRIES];
    int             count;
} session_table_t;

void session_table_init(session_table_t *tbl);

session_entry_t *session_lookup(session_table_t *tbl, const session_key_t *key);

session_entry_t *session_insert(session_table_t *tbl, const session_key_t *key);

void session_update(session_entry_t *entry, uint32_t pkt_len, int is_forward);

int  session_expire(session_table_t *tbl, time_t now);

void session_dump(const session_table_t *tbl);

#endif /* SESSION_H */
