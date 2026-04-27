#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "session.h"

static session_entry_t session_table[SESSION_TABLE_SIZE];
static int session_count = 0;

void session_init(void) {
    memset(session_table, 0, sizeof(session_table));
    session_count = 0;
}

static uint32_t session_hash(uint32_t src_ip, uint32_t dst_ip,
                              uint16_t src_port, uint16_t dst_port,
                              uint8_t proto) {
    uint32_t h = src_ip ^ dst_ip;
    h ^= ((uint32_t)src_port << 16) | dst_port;
    h ^= proto;
    h ^= (h >> 16);
    h *= 0x45d9f3b;
    h ^= (h >> 16);
    return h % SESSION_TABLE_SIZE;
}

session_entry_t *session_lookup(uint32_t src_ip, uint32_t dst_ip,
                                 uint16_t src_port, uint16_t dst_port,
                                 uint8_t proto) {
    uint32_t idx = session_hash(src_ip, dst_ip, src_port, dst_port, proto);
    session_entry_t *e = &session_table[idx];
    if (e->active &&
        ((e->src_ip == src_ip && e->dst_ip == dst_ip &&
          e->src_port == src_port && e->dst_port == dst_port) ||
         (e->src_ip == dst_ip && e->dst_ip == src_ip &&
          e->src_port == dst_port && e->dst_port == src_port)) &&
        e->proto == proto) {
        return e;
    }
    return NULL;
}

session_entry_t *session_create(uint32_t src_ip, uint32_t dst_ip,
                                 uint16_t src_port, uint16_t dst_port,
                                 uint8_t proto) {
    uint32_t idx = session_hash(src_ip, dst_ip, src_port, dst_port, proto);
    session_entry_t *e = &session_table[idx];
    e->src_ip    = src_ip;
    e->dst_ip    = dst_ip;
    e->src_port  = src_port;
    e->dst_port  = dst_port;
    e->proto     = proto;
    e->pkt_count = 0;
    e->byte_count = 0;
    e->first_seen = time(NULL);
    e->last_seen  = e->first_seen;
    e->active     = 1;
    session_count++;
    return e;
}

void session_update(session_entry_t *e, uint32_t pkt_len) {
    if (!e) return;
    e->pkt_count++;
    e->byte_count += pkt_len;
    e->last_seen = time(NULL);
}

void session_expire(uint32_t timeout_secs) {
    time_t now = time(NULL);
    for (int i = 0; i < SESSION_TABLE_SIZE; i++) {
        if (session_table[i].active &&
            (uint32_t)(now - session_table[i].last_seen) > timeout_secs) {
            memset(&session_table[i], 0, sizeof(session_entry_t));
            session_count--;
        }
    }
}

int session_get_count(void) {
    return session_count;
}

void session_dump(FILE *fp) {
    fprintf(fp, "Active sessions: %d\n", session_count);
    for (int i = 0; i < SESSION_TABLE_SIZE; i++) {
        session_entry_t *e = &session_table[i];
        if (!e->active) continue;
        fprintf(fp, "  [%d] proto=%u %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u "
                "pkts=%lu bytes=%lu\n", i, e->proto,
                (e->src_ip >> 24) & 0xff, (e->src_ip >> 16) & 0xff,
                (e->src_ip >> 8) & 0xff, e->src_ip & 0xff, e->src_port,
                (e->dst_ip >> 24) & 0xff, (e->dst_ip >> 16) & 0xff,
                (e->dst_ip >> 8) & 0xff, e->dst_ip & 0xff, e->dst_port,
                e->pkt_count, e->byte_count);
    }
}
