#ifndef MACTRACK_H
#define MACTRACK_H

#include <stdint.h>
#include <time.h>

#define MACTRACK_MAX_ENTRIES 256
#define MACTRACK_MAC_LEN 6

typedef struct {
    uint8_t  mac[MACTRACK_MAC_LEN];
    uint32_t last_ip;       /* last seen IPv4 address */
    uint32_t pkt_count;
    uint64_t byte_count;
    time_t   first_seen;
    time_t   last_seen;
    int      active;
} mactrack_entry_t;

typedef struct {
    mactrack_entry_t entries[MACTRACK_MAX_ENTRIES];
    int count;
} mactrack_table_t;

void mactrack_init(mactrack_table_t *tbl);
int  mactrack_update(mactrack_table_t *tbl, const uint8_t *mac, uint32_t ip,
                     uint32_t pkt_len);
mactrack_entry_t *mactrack_lookup(mactrack_table_t *tbl, const uint8_t *mac);
void mactrack_expire(mactrack_table_t *tbl, time_t max_age);
void mactrack_print(const mactrack_table_t *tbl);
void mactrack_reset(mactrack_table_t *tbl);

#endif /* MACTRACK_H */
