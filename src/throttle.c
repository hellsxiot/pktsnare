#include <stdio.h>
#include <string.h>
#include <time.h>
#include "throttle.h"

static uint64_t elapsed_ms(const struct timespec *start, const struct timespec *end)
{
    int64_t sec_diff  = (int64_t)(end->tv_sec  - start->tv_sec);
    int64_t nsec_diff = (int64_t)(end->tv_nsec - start->tv_nsec);
    return (uint64_t)(sec_diff * 1000 + nsec_diff / 1000000);
}

int throttle_init(throttle_t *t, uint32_t max_pps, uint32_t max_bps)
{
    if (!t)
        return -1;
    memset(t, 0, sizeof(*t));
    t->max_pps = max_pps;
    t->max_bps = max_bps;
    clock_gettime(CLOCK_MONOTONIC, &t->window_start);
    return 0;
}

void throttle_reset(throttle_t *t)
{
    if (!t)
        return;
    t->pkt_count  = 0;
    t->byte_count = 0;
    clock_gettime(CLOCK_MONOTONIC, &t->window_start);
}

int throttle_check(throttle_t *t, uint32_t pkt_len)
{
    struct timespec now;
    uint64_t elapsed;

    if (!t)
        return 1;

    clock_gettime(CLOCK_MONOTONIC, &now);
    elapsed = elapsed_ms(&t->window_start, &now);

    /* Slide the window forward if enough time has passed */
    if (elapsed >= THROTTLE_WINDOW_MS) {
        t->pkt_count  = 0;
        t->byte_count = 0;
        t->window_start = now;
        elapsed = 0;
    }

    /* Scale limits to window size */
    uint64_t pps_limit = (t->max_pps == 0) ? UINT64_MAX
        : (uint64_t)t->max_pps * THROTTLE_WINDOW_MS / 1000;
    uint64_t bps_limit = (t->max_bps == 0) ? UINT64_MAX
        : (uint64_t)t->max_bps * THROTTLE_WINDOW_MS / 1000;

    if (t->pkt_count  >= pps_limit ||
        t->byte_count + pkt_len > bps_limit) {
        t->dropped_pkts++;
        return 0;
    }

    t->pkt_count++;
    t->byte_count += pkt_len;
    return 1;
}

uint32_t throttle_dropped(const throttle_t *t)
{
    return t ? t->dropped_pkts : 0;
}

void throttle_print_stats(const throttle_t *t)
{
    if (!t)
        return;
    printf("throttle: max_pps=%u max_bps=%u dropped=%u\n",
           t->max_pps, t->max_bps, t->dropped_pkts);
}
