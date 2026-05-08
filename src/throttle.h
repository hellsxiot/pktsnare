#ifndef THROTTLE_H
#define THROTTLE_H

#include <stdint.h>
#include <time.h>

/* Rate limiting / throttle control for packet capture output */

#define THROTTLE_DEFAULT_PPS   1000   /* default max packets per second */
#define THROTTLE_DEFAULT_BPS   (1024 * 1024)  /* default max bytes per second */
#define THROTTLE_WINDOW_MS     100    /* sliding window size in ms */

typedef struct {
    uint32_t max_pps;         /* max packets per second, 0 = unlimited */
    uint32_t max_bps;         /* max bytes per second, 0 = unlimited */
    uint64_t pkt_count;       /* packets in current window */
    uint64_t byte_count;      /* bytes in current window */
    struct timespec window_start;
    uint32_t dropped_pkts;    /* total dropped due to throttle */
} throttle_t;

/* Initialize throttle with given limits (0 = unlimited) */
int  throttle_init(throttle_t *t, uint32_t max_pps, uint32_t max_bps);

/* Reset counters and window */
void throttle_reset(throttle_t *t);

/*
 * Check if a packet of given size should be allowed through.
 * Returns 1 if allowed, 0 if should be dropped.
 */
int  throttle_check(throttle_t *t, uint32_t pkt_len);

/* Get number of packets dropped so far */
uint32_t throttle_dropped(const throttle_t *t);

/* Print current throttle stats to stdout */
void throttle_print_stats(const throttle_t *t);

/*
 * Update the rate limits on an already-initialized throttle.
 * Pass 0 for either limit to make it unlimited.
 * Does not reset counters or the current window.
 */
void throttle_set_limits(throttle_t *t, uint32_t max_pps, uint32_t max_bps);

#endif /* THROTTLE_H */
