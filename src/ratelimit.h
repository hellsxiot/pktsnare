#ifndef PKTSNARE_RATELIMIT_H
#define PKTSNARE_RATELIMIT_H

#include <stdint.h>
#include <time.h>

/* Token bucket rate limiter for packet processing */

typedef struct {
    double      tokens;         /* current token count */
    double      capacity;       /* max tokens (burst size) */
    double      rate;           /* tokens per second */
    struct timespec last_refill;
    uint64_t    dropped;        /* total packets dropped */
    uint64_t    allowed;        /* total packets allowed */
} ratelimit_t;

/*
 * Initialize a rate limiter.
 * rate_pps  - allowed packets per second
 * burst     - maximum burst size in packets
 */
int  ratelimit_init(ratelimit_t *rl, double rate_pps, double burst);

/*
 * Check whether a packet should be allowed through.
 * Returns 1 if allowed, 0 if dropped.
 */
int  ratelimit_check(ratelimit_t *rl);

/* Reset counters and refill tokens to capacity */
void ratelimit_reset(ratelimit_t *rl);

/* Return drop ratio as a value between 0.0 and 1.0 */
double ratelimit_drop_ratio(const ratelimit_t *rl);

void ratelimit_stats(const ratelimit_t *rl, uint64_t *allowed, uint64_t *dropped);

#endif /* PKTSNARE_RATELIMIT_H */
