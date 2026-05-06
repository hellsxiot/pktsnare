#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "ifstats.h"

#define PROC_NET_DEV "/proc/net/dev"

int ifstats_read(const char *iface, ifstats_t *out) {
    if (!iface || !out)
        return -1;

    FILE *fp = fopen(PROC_NET_DEV, "r");
    if (!fp) {
        perror("fopen /proc/net/dev");
        return -1;
    }

    char line[256];
    /* skip first two header lines */
    if (!fgets(line, sizeof(line), fp) || !fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return -1;
    }

    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char name[IF_NAMESIZE];
        ifstats_t s;
        /* format: iface: rx_bytes rx_pkts rx_err rx_drop ... tx_bytes tx_pkts tx_err tx_drop ... */
        int n = sscanf(line,
            " %15[^:]: %llu %llu %llu %llu %*u %*u %*u %*u "
            "%llu %llu %llu %llu",
            name,
            &s.rx_bytes, &s.rx_packets, &s.rx_errors, &s.rx_dropped,
            &s.tx_bytes, &s.tx_packets, &s.tx_errors, &s.tx_dropped);
        if (n == 9 && strcmp(name, iface) == 0) {
            strncpy(s.iface, iface, IF_NAMESIZE - 1);
            s.iface[IF_NAMESIZE - 1] = '\0';
            *out = s;
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found ? 0 : -1;
}

void ifstats_print(const ifstats_t *s, FILE *fp) {
    if (!s || !fp)
        return;
    fprintf(fp, "[ifstats] %s\n", s->iface);
    fprintf(fp, "  RX: pkts=%-10llu bytes=%-12llu err=%-6llu drop=%-6llu\n",
            s->rx_packets, s->rx_bytes, s->rx_errors, s->rx_dropped);
    fprintf(fp, "  TX: pkts=%-10llu bytes=%-12llu err=%-6llu drop=%-6llu\n",
            s->tx_packets, s->tx_bytes, s->tx_errors, s->tx_dropped);
}

void ifstats_delta(const ifstats_t *before, const ifstats_t *after, ifstats_t *delta) {
    if (!before || !after || !delta)
        return;
    strncpy(delta->iface, after->iface, IF_NAMESIZE);
    delta->rx_bytes    = after->rx_bytes    - before->rx_bytes;
    delta->rx_packets  = after->rx_packets  - before->rx_packets;
    delta->rx_errors   = after->rx_errors   - before->rx_errors;
    delta->rx_dropped  = after->rx_dropped  - before->rx_dropped;
    delta->tx_bytes    = after->tx_bytes    - before->tx_bytes;
    delta->tx_packets  = after->tx_packets  - before->tx_packets;
    delta->tx_errors   = after->tx_errors   - before->tx_errors;
    delta->tx_dropped  = after->tx_dropped  - before->tx_dropped;
}

/* Returns total error+drop count across RX and TX, useful for quick health checks. */
unsigned long long ifstats_total_errors(const ifstats_t *s) {
    if (!s)
        return 0;
    return s->rx_errors + s->rx_dropped + s->tx_errors + s->tx_dropped;
}
