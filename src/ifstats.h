#ifndef IFSTATS_H
#define IFSTATS_H

#include <stdio.h>
#include <net/if.h>

/*
 * ifstats — read per-interface packet/byte counters from /proc/net/dev.
 * Useful for lightweight monitoring on embedded targets without netlink.
 */

typedef struct {
    char iface[IF_NAMESIZE];

    unsigned long long rx_bytes;
    unsigned long long rx_packets;
    unsigned long long rx_errors;
    unsigned long long rx_dropped;

    unsigned long long tx_bytes;
    unsigned long long tx_packets;
    unsigned long long tx_errors;
    unsigned long long tx_dropped;
} ifstats_t;

/*
 * ifstats_read - populate @out with current counters for @iface.
 * Returns 0 on success, -1 if the interface was not found or on error.
 */
int ifstats_read(const char *iface, ifstats_t *out);

/*
 * ifstats_print - pretty-print stats to @fp (e.g. stdout).
 */
void ifstats_print(const ifstats_t *s, FILE *fp);

/*
 * ifstats_delta - compute counter difference between two snapshots.
 * Useful for rate calculations over a polling interval.
 */
void ifstats_delta(const ifstats_t *before, const ifstats_t *after, ifstats_t *delta);

#endif /* IFSTATS_H */
