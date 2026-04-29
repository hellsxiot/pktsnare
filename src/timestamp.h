/*
 * timestamp.h - High-resolution timestamp utilities for pktsnare
 */

#ifndef PKTSNARE_TIMESTAMP_H
#define PKTSNARE_TIMESTAMP_H

#include <stdint.h>
#include <time.h>

/* Timestamp in microseconds since epoch */
typedef uint64_t pkt_ts_t;

/*
 * Format flags for timestamp string output
 */
#define TS_FMT_EPOCH   0x01   /* raw microseconds */
#define TS_FMT_HUMAN   0x02   /* HH:MM:SS.uuuuuu */
#define TS_FMT_ISO     0x04   /* ISO 8601 */
#define TS_FMT_DELTA   0x08   /* delta from reference */

#define TS_STR_MAX     64

/*
 * Get current time as pkt_ts_t (microseconds since epoch)
 */
pkt_ts_t ts_now(void);

/*
 * Convert struct timespec to pkt_ts_t
 */
pkt_ts_t ts_from_timespec(const struct timespec *ts);

/*
 * Format a timestamp into buf (at least TS_STR_MAX bytes).
 * ref is used only when fmt includes TS_FMT_DELTA.
 * Returns buf on success, NULL on error.
 */
char *ts_format(pkt_ts_t ts, pkt_ts_t ref, int fmt, char *buf, size_t len);

/*
 * Compute delta between two timestamps in microseconds.
 * Returns negative if a < b.
 */
int64_t ts_delta_us(pkt_ts_t a, pkt_ts_t b);

#endif /* PKTSNARE_TIMESTAMP_H */
