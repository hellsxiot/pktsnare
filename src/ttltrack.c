#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "ttltrack.h"

void ttltrack_init(ttltrack_table_t *tbl) {
    if (!tbl) return;
    memset(tbl, 0, sizeof(*tbl));
}

static uint8_t guess_baseline(uint8_t ttl) {
    if (ttl <= 64)  return 64;
    if (ttl <= 128) return 128;
    return 255;
}

ttltrack_entry_t *ttltrack_lookup(ttltrack_table_t *tbl, uint32_t src_ip) {
    for (int i = 0; i < tbl->count; i++) {
        if (tbl->entries[i].src_ip == src_ip)
            return &tbl->entries[i];
    }
    return NULL;
}

int ttltrack_is_anomaly(const ttltrack_entry_t *entry, uint8_t ttl) {
    if (!entry) return 0;
    int delta = (int)entry->baseline_ttl - (int)ttl;
    return (delta > TTLTRACK_TTL_ANOMALY_THRESHOLD);
}

int ttltrack_update(ttltrack_table_t *tbl, uint32_t src_ip, uint8_t ttl) {
    if (!tbl) return -1;

    ttltrack_entry_t *e = ttltrack_lookup(tbl, src_ip);

    if (!e) {
        if (tbl->count >= TTLTRACK_MAX_ENTRIES)
            return -1;
        e = &tbl->entries[tbl->count++];
        memset(e, 0, sizeof(*e));
        e->src_ip       = src_ip;
        e->baseline_ttl = guess_baseline(ttl);
        e->first_seen   = time(NULL);
    }

    e->observed_ttl = ttl;
    e->pkt_count++;
    e->last_seen = time(NULL);

    if (ttltrack_is_anomaly(e, ttl)) {
        if (!e->anomaly_flagged) {
            e->anomaly_flagged = 1;
            tbl->anomalies_detected++;
        }
        return 1;
    }

    e->anomaly_flagged = 0;
    return 0;
}

void ttltrack_reset(ttltrack_table_t *tbl) {
    if (!tbl) return;
    memset(tbl, 0, sizeof(*tbl));
}

void ttltrack_dump(const ttltrack_table_t *tbl) {
    if (!tbl) return;
    printf("[ttltrack] %d entries, %u anomalies detected\n",
           tbl->count, tbl->anomalies_detected);
    for (int i = 0; i < tbl->count; i++) {
        const ttltrack_entry_t *e = &tbl->entries[i];
        uint32_t ip = e->src_ip;
        printf("  %u.%u.%u.%u  ttl=%u baseline=%u pkts=%u%s\n",
               (ip >> 24) & 0xff, (ip >> 16) & 0xff,
               (ip >> 8)  & 0xff,  ip        & 0xff,
               e->observed_ttl, e->baseline_ttl, e->pkt_count,
               e->anomaly_flagged ? " [ANOMALY]" : "");
    }
}
