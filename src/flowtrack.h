#ifndef FLOWTRACK_H
#define FLOWTRACK_H

#include <stdint.h>
#include <time.h>

#define FLOW_TABLE_SIZE 256
#define FLOW_KEY_LEN    38

typedef struct {
    uint8_t  src_ip[16];  /* supports IPv4-mapped IPv6 */
    uint8_t  dst_ip[16];
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t  proto;
} flow_key_t;

typedef struct {
    flow_key_t  key;
    uint64_t    pkt_count;
    uint64_t    byte_count;
    struct timespec first_seen;
    struct timespec last_seen;
    int         active;
} flow_entry_t;

typedef struct {
    flow_entry_t entries[FLOW_TABLE_SIZE];
    int          count;
} flow_table_t;

void         flow_table_init(flow_table_t *ft);
flow_entry_t *flow_lookup(flow_table_t *ft, const flow_key_t *key);
flow_entry_t *flow_insert(flow_table_t *ft, const flow_key_t *key);
void         flow_update(flow_entry_t *fe, uint32_t pkt_len);
void         flow_expire(flow_table_t *ft, uint32_t timeout_sec);
int          flow_count(const flow_table_t *ft);
void         flow_table_dump(const flow_table_t *ft);

#endif /* FLOWTRACK_H */
