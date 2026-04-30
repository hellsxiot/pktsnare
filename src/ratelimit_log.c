#include "ratelimit.h"
#include "ratelimit_log.h"
#include <stdio.h>
#include <time.h>

void ratelimit_log_stats(const ratelimit_t *rl, const char *label, FILE *out)
{
    if (!rl || !out) return;

    uint64_t allowed, dropped;
    ratelimit_stats(rl, &allowed, &dropped);
    double ratio = ratelimit_drop_ratio(rl);

    /* ISO-8601 timestamp */
    time_t now = time(NULL);
    struct tm tm_buf;
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime_r(&now, &tm_buf));

    fprintf(out,
        "[%s] ratelimit(%s): rate=%.1f pps burst=%.1f "
        "allowed=%llu dropped=%llu drop_ratio=%.2f%%\n",
        ts,
        label ? label : "default",
        rl->rate,
        rl->capacity,
        (unsigned long long)allowed,
        (unsigned long long)dropped,
        ratio * 100.0);
}

void ratelimit_log_warn_if_high(const ratelimit_t *rl, const char *label,
                                double threshold, FILE *out)
{
    if (!rl || !out) return;
    double ratio = ratelimit_drop_ratio(rl);
    if (ratio >= threshold) {
        fprintf(out,
            "WARN ratelimit(%s): drop ratio %.1f%% exceeds threshold %.1f%%\n",
            label ? label : "default",
            ratio * 100.0,
            threshold * 100.0);
    }
}
