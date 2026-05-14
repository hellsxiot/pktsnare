#include <string.h>
#include <stdio.h>
#include <time.h>
#include "mactrack.h"

void mactrack_init(mactrack_table_t *tbl) {
    if (!tbl) return;
    memset(tbl, 0, sizeof(*tbl));
}

static int mac_eq(const uint8_t *a, const uint8_t *b) {
    return memcmp(a, b, MACTRACK_MAC_LEN) == 0;
}

int mactrack_update(mactrack_table_t *tbl, const uint8_t *mac, uint32_t ip,
                    uint32_t pkt_len) {
    if (!tbl || !mac) return -1;

    time_t now = time(NULL);

    /* search existing */
    for (int i = 0; i < MACTRACK_MAX_ENTRIES; i++) {
        mactrack_entry_t *e = &tbl->entries[i];
        if (e->active && mac_eq(e->mac, mac)) {
            e->last_ip   = ip;
            e->pkt_count++;
            e->byte_count += pkt_len;
            e->last_seen  = now;
            return 0;
        }
    }

    /* find free slot */
    for (int i = 0; i < MACTRACK_MAX_ENTRIES; i++) {
        mactrack_entry_t *e = &tbl->entries[i];
        if (!e->active) {
            memcpy(e->mac, mac, MACTRACK_MAC_LEN);
            e->last_ip    = ip;
            e->pkt_count  = 1;
            e->byte_count = pkt_len;
            e->first_seen = now;
            e->last_seen  = now;
            e->active     = 1;
            tbl->count++;
            return 0;
        }
    }

    return -1; /* table full */
}

mactrack_entry_t *mactrack_lookup(mactrack_table_t *tbl, const uint8_t *mac) {
    if (!tbl || !mac) return NULL;
    for (int i = 0; i < MACTRACK_MAX_ENTRIES; i++) {
        mactrack_entry_t *e = &tbl->entries[i];
        if (e->active && mac_eq(e->mac, mac))
            return e;
    }
    return NULL;
}

void mactrack_expire(mactrack_table_t *tbl, time_t max_age) {
    if (!tbl) return;
    time_t now = time(NULL);
    for (int i = 0; i < MACTRACK_MAX_ENTRIES; i++) {
        mactrack_entry_t *e = &tbl->entries[i];
        if (e->active && (now - e->last_seen) > max_age) {
            memset(e, 0, sizeof(*e));
            tbl->count--;
        }
    }
}

void mactrack_print(const mactrack_table_t *tbl) {
    if (!tbl) return;
    printf("%-20s %-16s %10s %12s\n", "MAC", "Last IP", "Packets", "Bytes");
    for (int i = 0; i < MACTRACK_MAX_ENTRIES; i++) {
        const mactrack_entry_t *e = &tbl->entries[i];
        if (!e->active) continue;
        printf("%02x:%02x:%02x:%02x:%02x:%02x  %u.%u.%u.%u  %10u %12llu\n",
               e->mac[0], e->mac[1], e->mac[2],
               e->mac[3], e->mac[4], e->mac[5],
               (e->last_ip >> 24) & 0xff, (e->last_ip >> 16) & 0xff,
               (e->last_ip >>  8) & 0xff,  e->last_ip & 0xff,
               e->pkt_count, (unsigned long long)e->byte_count);
    }
}

void mactrack_reset(mactrack_table_t *tbl) {
    mactrack_init(tbl);
}
