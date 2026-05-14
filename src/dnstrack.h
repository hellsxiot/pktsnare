#ifndef DNSTRACK_H
#define DNSTRACK_H

#include <stdint.h>
#include <time.h>

#define DNSTRACK_MAX_ENTRIES  256
#define DNSTRACK_MAX_NAME_LEN 128
#define DNSTRACK_TTL_DEFAULT  60

typedef struct dns_entry {
    uint16_t    txid;
    char        qname[DNSTRACK_MAX_NAME_LEN];
    uint16_t    qtype;
    uint32_t    src_ip;
    time_t      timestamp;
    int         answered;
    uint32_t    resolved_ip;
} dns_entry_t;

typedef struct dnstrack_ctx {
    dns_entry_t entries[DNSTRACK_MAX_ENTRIES];
    int         count;
    int         ttl_seconds;
    uint64_t    total_queries;
    uint64_t    total_responses;
    uint64_t    expired;
} dnstrack_ctx_t;

void     dnstrack_init(dnstrack_ctx_t *ctx, int ttl_seconds);
void     dnstrack_reset(dnstrack_ctx_t *ctx);
int      dnstrack_add_query(dnstrack_ctx_t *ctx, uint16_t txid,
                            const char *qname, uint16_t qtype,
                            uint32_t src_ip);
int      dnstrack_match_response(dnstrack_ctx_t *ctx, uint16_t txid,
                                 uint32_t resolved_ip);
void     dnstrack_expire(dnstrack_ctx_t *ctx);
int      dnstrack_lookup(const dnstrack_ctx_t *ctx, uint16_t txid,
                         dns_entry_t *out);
void     dnstrack_print_stats(const dnstrack_ctx_t *ctx);

#endif /* DNSTRACK_H */
