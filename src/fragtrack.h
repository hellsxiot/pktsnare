#ifndef FRAGTRACK_H
#define FRAGTRACK_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define FRAGTRACK_MAX_ENTRIES  64
#define FRAGTRACK_TIMEOUT_SEC  30
#define FRAGTRACK_MAX_FRAGS    16

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t ip_id;
    uint8_t  protocol;
    uint8_t  frag_count;
    uint16_t frag_offsets[FRAGTRACK_MAX_FRAGS];
    uint16_t frag_sizes[FRAGTRACK_MAX_FRAGS];
    uint16_t total_len;
    int      complete;
    time_t   first_seen;
    time_t   last_seen;
} frag_entry_t;

typedef struct {
    frag_entry_t entries[FRAGTRACK_MAX_ENTRIES];
    int          count;
    uint64_t     reassembled;
    uint64_t     dropped;
    uint64_t     expired;
} fragtrack_t;

void fragtrack_init(fragtrack_t *ft);
int  fragtrack_add(fragtrack_t *ft, uint32_t src, uint32_t dst,
                   uint16_t ip_id, uint8_t proto,
                   uint16_t offset, uint16_t size, int more_frags);
int  fragtrack_is_complete(fragtrack_t *ft, uint32_t src, uint32_t dst,
                            uint16_t ip_id, uint8_t proto);
void fragtrack_expire(fragtrack_t *ft, time_t now);
void fragtrack_remove(fragtrack_t *ft, uint32_t src, uint32_t dst,
                      uint16_t ip_id, uint8_t proto);
void fragtrack_stats(const fragtrack_t *ft, uint64_t *reassembled,
                     uint64_t *dropped, uint64_t *expired);

#endif /* FRAGTRACK_H */
