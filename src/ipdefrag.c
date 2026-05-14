#include <stdlib.h>
#include <string.h>
#include "ipdefrag.h"

#define MAX_FRAGS 64

static frag_entry_t frag_table[MAX_FRAGS];
static int frag_count = 0;

void ipdefrag_init(void) {
    memset(frag_table, 0, sizeof(frag_table));
    frag_count = 0;
}

static frag_entry_t *find_entry(uint16_t id, uint32_t src, uint32_t dst) {
    for (int i = 0; i < frag_count; i++) {
        if (frag_table[i].id == id &&
            frag_table[i].src == src &&
            frag_table[i].dst == dst) {
            return &frag_table[i];
        }
    }
    return NULL;
}

static frag_entry_t *alloc_entry(uint16_t id, uint32_t src, uint32_t dst) {
    if (frag_count >= MAX_FRAGS) return NULL;
    frag_entry_t *e = &frag_table[frag_count++];
    memset(e, 0, sizeof(*e));
    e->id  = id;
    e->src = src;
    e->dst = dst;
    return e;
}

static void free_entry(frag_entry_t *e) {
    if (e->data) { free(e->data); e->data = NULL; }
    int idx = (int)(e - frag_table);
    if (idx < frag_count - 1)
        frag_table[idx] = frag_table[frag_count - 1];
    frag_count--;
}

int ipdefrag_add(uint16_t id, uint32_t src, uint32_t dst,
                 uint16_t offset, int more, const uint8_t *payload, uint16_t len) {
    frag_entry_t *e = find_entry(id, src, dst);
    if (!e) {
        e = alloc_entry(id, src, dst);
        if (!e) return IPDEFRAG_ERR;
    }

    uint16_t end = offset + len;
    if (end > e->buf_len) {
        uint8_t *nb = realloc(e->data, end);
        if (!nb) return IPDEFRAG_ERR;
        e->data    = nb;
        e->buf_len = end;
    }

    memcpy(e->data + offset, payload, len);
    e->received += len;

    if (!more) {
        e->total_len = end;
        e->last_seen = 1;
    }

    if (e->last_seen && e->received >= e->total_len)
        return IPDEFRAG_COMPLETE;

    return IPDEFRAG_PENDING;
}

int ipdefrag_get(uint16_t id, uint32_t src, uint32_t dst,
                 uint8_t *out, uint16_t *out_len, uint16_t max) {
    frag_entry_t *e = find_entry(id, src, dst);
    if (!e || !e->last_seen) return IPDEFRAG_ERR;
    if (e->total_len > max) return IPDEFRAG_ERR;
    memcpy(out, e->data, e->total_len);
    *out_len = e->total_len;
    free_entry(e);
    return IPDEFRAG_COMPLETE;
}

void ipdefrag_flush(void) {
    for (int i = 0; i < frag_count; i++)
        if (frag_table[i].data) free(frag_table[i].data);
    frag_count = 0;
    memset(frag_table, 0, sizeof(frag_table));
}
