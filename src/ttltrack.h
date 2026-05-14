#ifndef TTLTRACK_H
#define TTLTRACK_H

#include <stdint.h>
#include <time.h>

#define TTLTRACK_MAX_ENTRIES 256
#define TTLTRACK_TTL_ANOMALY_THRESHOLD 10

typedef struct {
    uint32_t src_ip;
    uint8_t  observed_ttl;
    uint8_t  baseline_ttl;
    uint32_t pkt_count;
    time_t   first_seen;
    time_t   last_seen;
    int      anomaly_flagged;
} ttltrack_entry_t;

typedef struct {
    ttltrack_entry_t entries[TTLTRACK_MAX_ENTRIES];
    int count;
    uint32_t anomalies_detected;
} ttltrack_table_t;

void ttltrack_init(ttltrack_table_t *tbl);
int  ttltrack_update(ttltrack_table_t *tbl, uint32_t src_ip, uint8_t ttl);
ttltrack_entry_t *ttltrack_lookup(ttltrack_table_t *tbl, uint32_t src_ip);
int  ttltrack_is_anomaly(const ttltrack_entry_t *entry, uint8_t ttl);
void ttltrack_reset(ttltrack_table_t *tbl);
void ttltrack_dump(const ttltrack_table_t *tbl);

#endif /* TTLTRACK_H */
