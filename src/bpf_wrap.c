#include "bpf_wrap.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pcap/pcap.h>

/* Thread-local error buffer */
static __thread char bpf_errbuf[256];

struct bpf_wrap {
    char             expr[BPF_EXPR_MAX];
    struct bpf_program prog;
    int              snaplen;
    int              compiled;
};

bpf_wrap_t *bpf_wrap_compile(const char *expr, int snaplen)
{
    if (!expr || snaplen <= 0) {
        snprintf(bpf_errbuf, sizeof(bpf_errbuf), "invalid arguments");
        return NULL;
    }

    if (strlen(expr) >= BPF_EXPR_MAX) {
        snprintf(bpf_errbuf, sizeof(bpf_errbuf),
                 "filter expression too long (max %d)", BPF_EXPR_MAX - 1);
        return NULL;
    }

    bpf_wrap_t *bw = calloc(1, sizeof(*bw));
    if (!bw) {
        snprintf(bpf_errbuf, sizeof(bpf_errbuf), "out of memory");
        return NULL;
    }

    strncpy(bw->expr, expr, BPF_EXPR_MAX - 1);
    bw->snaplen = snaplen;

    /* pcap_compile_nopcap: compile without an open device */
    if (pcap_compile_nopcap(snaplen, DLT_EN10MB, &bw->prog,
                            expr, 1, PCAP_NETMASK_UNKNOWN) < 0) {
        snprintf(bpf_errbuf, sizeof(bpf_errbuf),
                 "compile failed: %s", expr);
        free(bw);
        return NULL;
    }

    bw->compiled = 1;
    return bw;
}

int bpf_wrap_match(const bpf_wrap_t *bw, const uint8_t *pkt, size_t pktlen)
{
    if (!bw || !bw->compiled || !pkt) {
        snprintf(bpf_errbuf, sizeof(bpf_errbuf), "invalid handle or packet");
        return -1;
    }

    /* bpf_filter returns the number of bytes to keep; 0 means no match */
    uint32_t result = bpf_filter(bw->prog.bf_insns,
                                 (u_char *)pkt,
                                 (uint32_t)pktlen,
                                 (uint32_t)pktlen);
    return result > 0 ? 1 : 0;
}

const char *bpf_wrap_expr(const bpf_wrap_t *bw)
{
    if (!bw) return "";
    return bw->expr;
}

const char *bpf_wrap_error(void)
{
    return bpf_errbuf;
}

void bpf_wrap_free(bpf_wrap_t *bw)
{
    if (!bw) return;
    if (bw->compiled)
        pcap_freecode(&bw->prog);
    free(bw);
}
