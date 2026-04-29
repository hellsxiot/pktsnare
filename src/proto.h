#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>
#include <stddef.h>

/* Known protocol identifiers */
typedef enum {
    PROTO_UNKNOWN = 0,
    PROTO_ARP,
    PROTO_ICMP,
    PROTO_TCP,
    PROTO_UDP,
    PROTO_DNS,
    PROTO_HTTP,
    PROTO_TLS,
    PROTO_DHCP,
    PROTO_NTP,
    PROTO_COUNT
} proto_id_t;

typedef struct {
    proto_id_t  id;
    const char *name;
    const char *description;
    uint16_t    default_port;   /* 0 if not port-based */
} proto_info_t;

typedef struct {
    proto_id_t  proto;
    uint16_t    src_port;
    uint16_t    dst_port;
    uint8_t     ip_proto;       /* IPPROTO_TCP, IPPROTO_UDP, etc. */
    const char *hint;           /* optional human-readable note */
} proto_match_t;

/* Identify protocol from IP protocol number and ports */
proto_id_t proto_identify(uint8_t ip_proto, uint16_t src_port, uint16_t dst_port);

/* Return static info struct for a given protocol id */
const proto_info_t *proto_info(proto_id_t id);

/* Fill out a proto_match_t given raw header fields */
void proto_match_fill(proto_match_t *m, uint8_t ip_proto,
                      uint16_t src_port, uint16_t dst_port);

/* Return printable name string (never NULL) */
const char *proto_name(proto_id_t id);

#endif /* PROTO_H */
