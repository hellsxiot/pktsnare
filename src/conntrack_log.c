#include "conntrack_log.h"
#include "conntrack.h"
#include <stdio.h>
#include <time.h>

static const char *state_str(conn_state_t s) {
    switch (s) {
    case CONN_STATE_NEW:         return "NEW";
    case CONN_STATE_ESTABLISHED: return "ESTABLISHED";
    case CONN_STATE_CLOSING:     return "CLOSING";
    case CONN_STATE_CLOSED:      return "CLOSED";
    default:                     return "UNKNOWN";
    }
}

void conntrack_log_entry(FILE *fp, const conn_entry_t *e) {
    if (!fp || !e) return;
    char tbuf[32];
    struct tm *tm = localtime(&e->last_seen);
    strftime(tbuf, sizeof(tbuf), "%H:%M:%S", tm);
    fprintf(fp,
            "%s conntrack %08x:%-5u -> %08x:%-5u proto=%-3u "
            "state=%-11s fwd=%llu/%u rev=%llu/%u\n",
            tbuf,
            e->key.src_ip, e->key.src_port,
            e->key.dst_ip, e->key.dst_port,
            e->key.proto,
            state_str(e->state),
            (unsigned long long)e->bytes_fwd, e->pkts_fwd,
            (unsigned long long)e->bytes_rev, e->pkts_rev);
}

void conntrack_log_expired(FILE *fp, const conn_entry_t *e) {
    if (!fp || !e) return;
    fprintf(fp, "conntrack expire %08x:%u -> %08x:%u proto=%u "
                "total_bytes=%llu\n",
            e->key.src_ip, e->key.src_port,
            e->key.dst_ip, e->key.dst_port,
            e->key.proto,
            (unsigned long long)(e->bytes_fwd + e->bytes_rev));
}
