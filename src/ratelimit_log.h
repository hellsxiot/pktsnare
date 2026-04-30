#ifndef PKTSNARE_RATELIMIT_LOG_H
#define PKTSNARE_RATELIMIT_LOG_H

#include <stdio.h>
#include "ratelimit.h"

/*
 * Print a one-line stats summary for the rate limiter to `out`.
 * `label` is a human-readable name for the limiter (may be NULL).
 */
void ratelimit_log_stats(const ratelimit_t *rl, const char *label, FILE *out);

/*
 * Emit a warning line to `out` when the drop ratio exceeds `threshold`
 * (0.0 – 1.0).  Useful for periodic health checks.
 */
void ratelimit_log_warn_if_high(const ratelimit_t *rl, const char *label,
                                double threshold, FILE *out);

#endif /* PKTSNARE_RATELIMIT_LOG_H */
