#ifndef RING_H
#define RING_H

#include <stdint.h>
#include <stddef.h>

#define RING_DEFAULT_CAPACITY 64
#define RING_MAX_PKT_SIZE     2048

typedef struct {
    uint8_t  data[RING_MAX_PKT_SIZE];
    size_t   len;
    uint64_t timestamp_us;
} ring_entry_t;

typedef struct {
    ring_entry_t *entries;
    size_t        capacity;
    size_t        head;
    size_t        tail;
    size_t        count;
    uint64_t      dropped;
} ring_buf_t;

/* Allocate and initialise a ring buffer with given capacity.
 * Returns NULL on failure. */
ring_buf_t *ring_create(size_t capacity);

/* Free all resources associated with the ring buffer. */
void ring_destroy(ring_buf_t *rb);

/* Push a packet into the ring. Overwrites oldest entry when full.
 * Returns 0 on success, -1 on bad args. */
int ring_push(ring_buf_t *rb, const uint8_t *data, size_t len, uint64_t ts_us);

/* Pop the oldest packet from the ring into dst.
 * Returns 0 on success, 1 if empty, -1 on bad args. */
int ring_pop(ring_buf_t *rb, ring_entry_t *dst);

/* Peek at the oldest entry without removing it.
 * Returns pointer to internal entry or NULL if empty. */
const ring_entry_t *ring_peek(const ring_buf_t *rb);

/* Returns number of entries currently held. */
size_t ring_count(const ring_buf_t *rb);

/* Returns number of packets dropped due to overflow. */
uint64_t ring_dropped(const ring_buf_t *rb);

#endif /* RING_H */
