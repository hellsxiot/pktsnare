#include "ring.h"

#include <stdlib.h>
#include <string.h>

ring_buf_t *ring_create(size_t capacity) {
    if (capacity == 0)
        capacity = RING_DEFAULT_CAPACITY;

    ring_buf_t *rb = calloc(1, sizeof(ring_buf_t));
    if (!rb)
        return NULL;

    rb->entries = calloc(capacity, sizeof(ring_entry_t));
    if (!rb->entries) {
        free(rb);
        return NULL;
    }

    rb->capacity = capacity;
    rb->head     = 0;
    rb->tail     = 0;
    rb->count    = 0;
    rb->dropped  = 0;
    return rb;
}

void ring_destroy(ring_buf_t *rb) {
    if (!rb)
        return;
    free(rb->entries);
    free(rb);
}

int ring_push(ring_buf_t *rb, const uint8_t *data, size_t len, uint64_t ts_us) {
    if (!rb || !data || len == 0 || len > RING_MAX_PKT_SIZE)
        return -1;

    if (rb->count == rb->capacity) {
        /* Overwrite oldest — advance tail */
        rb->tail = (rb->tail + 1) % rb->capacity;
        rb->dropped++;
    } else {
        rb->count++;
    }

    ring_entry_t *slot = &rb->entries[rb->head];
    memcpy(slot->data, data, len);
    slot->len          = len;
    slot->timestamp_us = ts_us;

    rb->head = (rb->head + 1) % rb->capacity;
    return 0;
}

int ring_pop(ring_buf_t *rb, ring_entry_t *dst) {
    if (!rb || !dst)
        return -1;
    if (rb->count == 0)
        return 1;

    *dst     = rb->entries[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count--;
    return 0;
}

const ring_entry_t *ring_peek(const ring_buf_t *rb) {
    if (!rb || rb->count == 0)
        return NULL;
    return &rb->entries[rb->tail];
}

size_t ring_count(const ring_buf_t *rb) {
    return rb ? rb->count : 0;
}

uint64_t ring_dropped(const ring_buf_t *rb) {
    return rb ? rb->dropped : 0;
}
