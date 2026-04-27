#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

/* Filter rule types */
typedef enum {
    FILTER_PROTO_ANY  = 0,
    FILTER_PROTO_TCP  = 6,
    FILTER_PROTO_UDP  = 17,
    FILTER_PROTO_ICMP = 1
} filter_proto_t;

typedef struct {
    filter_proto_t proto;     /* protocol to match, or ANY */
    uint32_t       src_ip;    /* source IP in host byte order, 0 = wildcard */
    uint32_t       dst_ip;    /* dest   IP in host byte order, 0 = wildcard */
    uint16_t       src_port;  /* source port, 0 = wildcard */
    uint16_t       dst_port;  /* dest   port, 0 = wildcard */
} filter_rule_t;

/*
 * Parse a human-readable filter string into a filter_rule_t.
 * Accepted format (all fields optional):
 *   "proto=tcp src=192.168.1.1 dst=10.0.0.1 sport=80 dport=443"
 * Returns 0 on success, -1 on parse error.
 */
int filter_parse(const char *expr, filter_rule_t *rule);

/*
 * Apply the rule to raw packet bytes (starting at the Ethernet header).
 * Returns 1 if the packet matches, 0 if it should be dropped.
 */
int filter_match(const filter_rule_t *rule, const uint8_t *pkt, int pkt_len);

/*
 * Print a human-readable representation of the rule to stdout.
 */
void filter_print(const filter_rule_t *rule);

/*
 * Reset all fields in a filter_rule_t to their wildcard/default values.
 * Equivalent to zeroing the struct: proto=ANY, all IPs and ports = 0.
 * Useful to ensure a rule is fully initialised before selectively setting
 * individual fields.
 */
void filter_rule_init(filter_rule_t *rule);

#endif /* FILTER_H */
