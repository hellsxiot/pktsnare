/*
 * timestamp.c - High-resolution timestamp utilities for pktsnare
 */

#include "timestamp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

pkt_ts_t ts_now(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return 0;
    return ts_from_timespec(&ts);
}

pkt_ts_t ts_from_timespec(const struct timespec *ts)
{
    if (!ts)
        return 0;
    return (pkt_ts_t)ts->tv_sec * 1000000ULL +
           (pkt_ts_t)(ts->tv_nsec / 1000);
}

int64_t ts_delta_us(pkt_ts_t a, pkt_ts_t b)
{
    return (int64_t)a - (int64_t)b;
}

char *ts_format(pkt_ts_t ts, pkt_ts_t ref, int fmt, char *buf, size_t len)
{
    if (!buf || len < 2)
        return NULL;

    if (fmt & TS_FMT_EPOCH) {
        snprintf(buf, len, "%llu", (unsigned long long)ts);
        return buf;
    }

    if (fmt & TS_FMT_DELTA) {
        int64_t d = ts_delta_us(ts, ref);
        snprintf(buf, len, "%+lld us", (long long)d);
        return buf;
    }

    time_t sec  = (time_t)(ts / 1000000ULL);
    unsigned us = (unsigned)(ts % 1000000ULL);
    struct tm tm_buf;

    if (!gmtime_r(&sec, &tm_buf))
        return NULL;

    if (fmt & TS_FMT_ISO) {
        /* 2024-01-02T15:04:05.123456Z */
        snprintf(buf, len, "%04d-%02d-%02dT%02d:%02d:%02d.%06uZ",
                 tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
                 tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, us);
    } else {
        /* default: HH:MM:SS.uuuuuu */
        snprintf(buf, len, "%02d:%02d:%02d.%06u",
                 tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, us);
    }

    return buf;
}
