#include "fragtrack.h"
#include <string.h>
#include <stdio.h>

static frag_entry_t *find_entry(fragtrack_t *ft, uint32_t src, uint32_t dst,
                                 uint16_t ip_id, uint8_t proto) {
    for (int i = 0; i < ft->count; i++) {
        frag_entry_t *e = &ft->entries[i];
        if (e->src_ip == src && e->dst_ip == dst &&
            e->ip_id == ip_id && e->protocol == proto)
            return e;
    }
    return NULL;
}

void fragtrack_init(fragtrack_t *ft) {
    memset(ft, 0, sizeof(*ft));
}

int fragtrack_add(fragtrack_t *ft, uint32_t src, uint32_t dst,
                  uint16_t ip_id, uint8_t proto,
                  uint16_t offset, uint16_t size, int more_frags) {
    time_t now = time(NULL);
    fragtrack_expire(ft, now);

    frag_entry_t *e = find_entry(ft, src, dst, ip_id, proto);
    if (!e) {
        if (ft->count >= FRAGTRACK_MAX_ENTRIES) {
            ft->dropped++;
            return -1;
        }
        e = &ft->entries[ft->count++];
        memset(e, 0, sizeof(*e));
        e->src_ip   = src;
        e->dst_ip   = dst;
        e->ip_id    = ip_id;
        e->protocol = proto;
        e->first_seen = now;
    }

    if (e->frag_count >= FRAGTRACK_MAX_FRAGS) {
        ft->dropped++;
        return -1;
    }

    e->frag_offsets[e->frag_count] = offset;
    e->frag_sizes[e->frag_count]   = size;
    e->frag_count++;
    e->last_seen = now;

    if (!more_frags)
        e->total_len = offset + size;

    if (e->total_len > 0) {
        uint16_t covered = 0;
        for (int i = 0; i < e->frag_count; i++)
            covered += e->frag_sizes[i];
        if (covered >= e->total_len) {
            e->complete = 1;
            ft->reassembled++;
        }
    }
    return 0;
}

int fragtrack_is_complete(fragtrack_t *ft, uint32_t src, uint32_t dst,
                           uint16_t ip_id, uint8_t proto) {
    frag_entry_t *e = find_entry(ft, src, dst, ip_id, proto);
    return e ? e->complete : 0;
}

void fragtrack_expire(fragtrack_t *ft, time_t now) {
    for (int i = 0; i < ft->count; ) {
        if ((now - ft->entries[i].last_seen) > FRAGTRACK_TIMEOUT_SEC) {
            if (!ft->entries[i].complete)
                ft->expired++;
            ft->entries[i] = ft->entries[--ft->count];
        } else {
            i++;
        }
    }
}

void fragtrack_remove(fragtrack_t *ft, uint32_t src, uint32_t dst,
                      uint16_t ip_id, uint8_t proto) {
    for (int i = 0; i < ft->count; i++) {
        frag_entry_t *e = &ft->entries[i];
        if (e->src_ip == src && e->dst_ip == dst &&
            e->ip_id == ip_id && e->protocol == proto) {
            ft->entries[i] = ft->entries[--ft->count];
            return;
        }
    }
}

void fragtrack_stats(const fragtrack_t *ft, uint64_t *reassembled,
                     uint64_t *dropped, uint64_t *expired) {
    if (reassembled) *reassembled = ft->reassembled;
    if (dropped)     *dropped     = ft->dropped;
    if (expired)     *expired     = ft->expired;
}
