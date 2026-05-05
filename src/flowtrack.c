#include <string.h>
#include <stdio.h>
#include <time.h>
#include "flowtrack.h"

static int key_eq(const flow_key_t *a, const flow_key_t *b) {
    return memcmp(a, b, sizeof(flow_key_t)) == 0;
}

void flow_table_init(flow_table_t *ft) {
    memset(ft, 0, sizeof(*ft));
}

flow_entry_t *flow_lookup(flow_table_t *ft, const flow_key_t *key) {
    for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
        if (ft->entries[i].active && key_eq(&ft->entries[i].key, key))
            return &ft->entries[i];
    }
    return NULL;
}

flow_entry_t *flow_insert(flow_table_t *ft, const flow_key_t *key) {
    for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
        if (!ft->entries[i].active) {
            flow_entry_t *fe = &ft->entries[i];
            memset(fe, 0, sizeof(*fe));
            fe->key = *key;
            fe->active = 1;
            clock_gettime(CLOCK_MONOTONIC, &fe->first_seen);
            fe->last_seen = fe->first_seen;
            ft->count++;
            return fe;
        }
    }
    return NULL; /* table full */
}

void flow_update(flow_entry_t *fe, uint32_t pkt_len) {
    if (!fe) return;
    fe->pkt_count++;
    fe->byte_count += pkt_len;
    clock_gettime(CLOCK_MONOTONIC, &fe->last_seen);
}

void flow_expire(flow_table_t *ft, uint32_t timeout_sec) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
        if (!ft->entries[i].active) continue;
        time_t delta = now.tv_sec - ft->entries[i].last_seen.tv_sec;
        if ((uint32_t)delta >= timeout_sec) {
            ft->entries[i].active = 0;
            ft->count--;
        }
    }
}

int flow_count(const flow_table_t *ft) {
    return ft->count;
}

void flow_table_dump(const flow_table_t *ft) {
    for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
        const flow_entry_t *fe = &ft->entries[i];
        if (!fe->active) continue;
        printf("flow proto=%u sport=%u dport=%u pkts=%llu bytes=%llu\n",
               fe->key.proto, fe->key.src_port, fe->key.dst_port,
               (unsigned long long)fe->pkt_count,
               (unsigned long long)fe->byte_count);
    }
}
