#include "dissect.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define ETH_HDR_LEN   14
#define IPV4_HDR_MIN  20
#define TCP_HDR_MIN   20
#define UDP_HDR_LEN    8

#define ETHERTYPE_IPV4  0x0800
#define ETHERTYPE_ARP   0x0806
#define ETHERTYPE_IPV6  0x86DD

#define IP_PROTO_ICMP   1
#define IP_PROTO_TCP    6
#define IP_PROTO_UDP   17

static int parse_ethernet(const uint8_t *data, size_t len, packet_info_t *out)
{
    if (len < ETH_HDR_LEN)
        return -1;

    memcpy(out->eth.dst_mac, data,     6);
    memcpy(out->eth.src_mac, data + 6, 6);
    out->eth.ethertype = (data[12] << 8) | data[13];
    out->eth.proto     = PROTO_ETHERNET;
    return 0;
}

static int parse_ipv4(const uint8_t *data, size_t len, packet_info_t *out)
{
    if (len < IPV4_HDR_MIN)
        return -1;

    uint8_t ihl = (data[0] & 0x0F) * 4;
    if (len < ihl)
        return -1;

    out->ip.ttl       = data[8];
    out->ip.proto     = data[9];
    out->ip.total_len = (data[2] << 8) | data[3];
    memcpy(&out->ip.src_ip, data + 12, 4);
    memcpy(&out->ip.dst_ip, data + 16, 4);
    return ihl;  /* return header length so caller can advance */
}

static void parse_transport(const uint8_t *data, size_t len,
                            uint8_t ip_proto, packet_info_t *out)
{
    if (ip_proto == IP_PROTO_TCP && len >= TCP_HDR_MIN) {
        out->transport.src_port = (data[0] << 8) | data[1];
        out->transport.dst_port = (data[2] << 8) | data[3];
        out->transport.seq      = (data[4] << 24) | (data[5] << 16) |
                                  (data[6] << 8)  |  data[7];
        out->transport.ack      = (data[8] << 24) | (data[9] << 16) |
                                  (data[10] << 8) |  data[11];
        out->transport.flags    = data[13];
        uint8_t doff = (data[12] >> 4) * 4;
        out->payload     = data + doff;
        out->payload_len = (len > doff) ? len - doff : 0;
        out->top_proto   = PROTO_TCP;
    } else if (ip_proto == IP_PROTO_UDP && len >= UDP_HDR_LEN) {
        out->transport.src_port = (data[0] << 8) | data[1];
        out->transport.dst_port = (data[2] << 8) | data[3];
        out->transport.length   = (data[4] << 8) | data[5];
        out->payload     = data + UDP_HDR_LEN;
        out->payload_len = (len > UDP_HDR_LEN) ? len - UDP_HDR_LEN : 0;
        out->top_proto   = PROTO_UDP;
    } else if (ip_proto == IP_PROTO_ICMP) {
        out->top_proto = PROTO_ICMP;
    } else {
        out->top_proto = PROTO_UNKNOWN;
    }
}

int dissect_packet(const uint8_t *data, size_t len, packet_info_t *out)
{
    if (!data || !out || len < ETH_HDR_LEN)
        return -1;

    memset(out, 0, sizeof(*out));

    if (parse_ethernet(data, len, out) < 0)
        return -1;

    if (out->eth.ethertype == ETHERTYPE_IPV4) {
        const uint8_t *ip_data = data + ETH_HDR_LEN;
        size_t         ip_len  = len  - ETH_HDR_LEN;
        int ihl = parse_ipv4(ip_data, ip_len, out);
        if (ihl < 0)
            return -1;
        parse_transport(ip_data + ihl, ip_len - ihl, out->ip.proto, out);
    } else if (out->eth.ethertype == ETHERTYPE_ARP) {
        out->top_proto = PROTO_ARP;
    } else if (out->eth.ethertype == ETHERTYPE_IPV6) {
        out->top_proto = PROTO_IPV6;
    } else {
        out->top_proto = PROTO_UNKNOWN;
    }

    return 0;
}

void dissect_print(const packet_info_t *info)
{
    if (!info) return;

    char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &info->ip.src_ip, src, sizeof(src));
    inet_ntop(AF_INET, &info->ip.dst_ip, dst, sizeof(dst));

    const char *proto_str = "UNKNOWN";
    switch (info->top_proto) {
        case PROTO_TCP:  proto_str = "TCP";  break;
        case PROTO_UDP:  proto_str = "UDP";  break;
        case PROTO_ICMP: proto_str = "ICMP"; break;
        case PROTO_ARP:  proto_str = "ARP";  break;
        case PROTO_IPV6: proto_str = "IPv6"; break;
        default: break;
    }

    if (info->top_proto == PROTO_TCP || info->top_proto == PROTO_UDP) {
        printf("[%s] %s:%u -> %s:%u  ttl=%u payload=%zu\n",
               proto_str, src, info->transport.src_port,
               dst, info->transport.dst_port,
               info->ip.ttl, info->payload_len);
    } else {
        printf("[%s] %s -> %s\n", proto_str, src, dst);
    }
}
