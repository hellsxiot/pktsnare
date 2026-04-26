#ifndef DISSECT_H
#define DISSECT_H

#include <stdint.h>
#include <stddef.h>

/* Supported protocol layers */
typedef enum {
    PROTO_UNKNOWN = 0,
    PROTO_ETHERNET,
    PROTO_ARP,
    PROTO_IPV4,
    PROTO_IPV6,
    PROTO_TCP,
    PROTO_UDP,
    PROTO_ICMP,
} proto_t;

typedef struct {
    proto_t  proto;
    uint8_t  src_mac[6];
    uint8_t  dst_mac[6];
    uint16_t ethertype;
} eth_info_t;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t  ttl;
    uint8_t  proto;   /* next layer protocol */
    uint16_t total_len;
} ipv4_info_t;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;     /* TCP only */
    uint32_t ack;     /* TCP only */
    uint8_t  flags;   /* TCP only */
    uint16_t length;  /* UDP payload length */
} transport_info_t;

typedef struct {
    eth_info_t       eth;
    ipv4_info_t      ip;
    transport_info_t transport;
    proto_t          top_proto;
    const uint8_t   *payload;
    size_t           payload_len;
} packet_info_t;

int  dissect_packet(const uint8_t *data, size_t len, packet_info_t *out);
void dissect_print(const packet_info_t *info);

#endif /* DISSECT_H */
