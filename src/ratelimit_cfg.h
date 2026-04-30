#ifndef PKTSNARE_RATELIMIT_CFG_H
#define PKTSNARE_RATELIMIT_CFG_H

/*
 * Default rate-limiter configuration constants.
 * Override at compile time with -DPKTSNARE_DEFAULT_RATE_PPS=<n> etc.
 */

#ifndef PKTSNARE_DEFAULT_RATE_PPS
#  define PKTSNARE_DEFAULT_RATE_PPS   1000.0   /* packets per second */
#endif

#ifndef PKTSNARE_DEFAULT_BURST
#  define PKTSNARE_DEFAULT_BURST       200.0   /* burst in packets   */
#endif

/*
 * Minimum and maximum values accepted from CLI / config file.
 */
#define RATELIMIT_MIN_RATE_PPS    1.0
#define RATELIMIT_MAX_RATE_PPS    1000000.0

#define RATELIMIT_MIN_BURST       1.0
#define RATELIMIT_MAX_BURST       100000.0

/* Validate a rate value; returns 1 if ok */
static inline int ratelimit_cfg_valid(double rate_pps, double burst)
{
    return (rate_pps >= RATELIMIT_MIN_RATE_PPS &&
            rate_pps <= RATELIMIT_MAX_RATE_PPS &&
            burst    >= RATELIMIT_MIN_BURST    &&
            burst    <= RATELIMIT_MAX_BURST);
}

#endif /* PKTSNARE_RATELIMIT_CFG_H */
