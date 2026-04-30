#include "ratelimit.h"
#include <string.h>
#include <errno.h>

static double timespec_diff_sec(const struct timespec *a, const struct timespec *b)
{
    return (double)(a->tv_sec  - b->tv_sec)
         + (double)(a->tv_nsec - b->tv_nsec) * 1e-9;
}

int ratelimit_init(ratelimit_t *rl, double rate_pps, double burst)
{
    if (!rl || rate_pps <= 0.0 || burst <= 0.0)
        return -EINVAL;

    memset(rl, 0, sizeof(*rl));
    rl->rate     = rate_pps;
    rl->capacity = burst;
    rl->tokens   = burst;   /* start full */
    clock_gettime(CLOCK_MONOTONIC, &rl->last_refill);
    return 0;
}

int ratelimit_check(ratelimit_t *rl)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    double elapsed = timespec_diff_sec(&now, &rl->last_refill);
    if (elapsed > 0.0) {
        rl->tokens += elapsed * rl->rate;
        if (rl->tokens > rl->capacity)
            rl->tokens = rl->capacity;
        rl->last_refill = now;
    }

    if (rl->tokens >= 1.0) {
        rl->tokens -= 1.0;
        rl->allowed++;
        return 1;
    }

    rl->dropped++;
    return 0;
}

void ratelimit_reset(ratelimit_t *rl)
{
    if (!rl) return;
    rl->tokens   = rl->capacity;
    rl->dropped  = 0;
    rl->allowed  = 0;
    clock_gettime(CLOCK_MONOTONIC, &rl->last_refill);
}

double ratelimit_drop_ratio(const ratelimit_t *rl)
{
    uint64_t total = rl->allowed + rl->dropped;
    if (total == 0) return 0.0;
    return (double)rl->dropped / (double)total;
}

void ratelimit_stats(const ratelimit_t *rl, uint64_t *allowed, uint64_t *dropped)
{
    if (allowed) *allowed = rl->allowed;
    if (dropped) *dropped = rl->dropped;
}
