#include "proto.h"
#include <netinet/in.h>
#include <string.h>

static const proto_info_t proto_table[PROTO_COUNT] = {
    [PROTO_UNKNOWN] = { PROTO_UNKNOWN, "unknown",  "Unidentified protocol",  0    },
    [PROTO_ARP]     = { PROTO_ARP,     "arp",      "Address Resolution",     0    },
    [PROTO_ICMP]    = { PROTO_ICMP,    "icmp",     "Internet Control Msg",   0    },
    [PROTO_TCP]     = { PROTO_TCP,     "tcp",      "Transmission Control",   0    },
    [PROTO_UDP]     = { PROTO_UDP,     "udp",      "User Datagram",          0    },
    [PROTO_DNS]     = { PROTO_DNS,     "dns",      "Domain Name System",     53   },
    [PROTO_HTTP]    = { PROTO_HTTP,    "http",     "Hypertext Transfer",     80   },
    [PROTO_TLS]     = { PROTO_TLS,     "tls",      "Transport Layer Sec",    443  },
    [PROTO_DHCP]    = { PROTO_DHCP,    "dhcp",     "Dynamic Host Config",    67   },
    [PROTO_NTP]     = { PROTO_NTP,     "ntp",      "Network Time Protocol",  123  },
};

/* port-based heuristics for UDP/TCP */
static proto_id_t port_lookup(uint16_t port) {
    switch (port) {
        case 53:  return PROTO_DNS;
        case 80:  return PROTO_HTTP;
        case 443: return PROTO_TLS;
        case 67:
        case 68:  return PROTO_DHCP;
        case 123: return PROTO_NTP;
        default:  return PROTO_UNKNOWN;
    }
}

proto_id_t proto_identify(uint8_t ip_proto, uint16_t src_port, uint16_t dst_port) {
    switch (ip_proto) {
        case IPPROTO_ICMP:
            return PROTO_ICMP;
        case IPPROTO_TCP: {
            proto_id_t p = port_lookup(dst_port);
            if (p == PROTO_UNKNOWN) p = port_lookup(src_port);
            return (p != PROTO_UNKNOWN) ? p : PROTO_TCP;
        }
        case IPPROTO_UDP: {
            proto_id_t p = port_lookup(dst_port);
            if (p == PROTO_UNKNOWN) p = port_lookup(src_port);
            return (p != PROTO_UNKNOWN) ? p : PROTO_UDP;
        }
        default:
            return PROTO_UNKNOWN;
    }
}

const proto_info_t *proto_info(proto_id_t id) {
    if (id < PROTO_COUNT)
        return &proto_table[id];
    return &proto_table[PROTO_UNKNOWN];
}

void proto_match_fill(proto_match_t *m, uint8_t ip_proto,
                      uint16_t src_port, uint16_t dst_port) {
    if (!m) return;
    m->ip_proto = ip_proto;
    m->src_port = src_port;
    m->dst_port = dst_port;
    m->proto    = proto_identify(ip_proto, src_port, dst_port);
    m->hint     = proto_table[m->proto].description;
}

const char *proto_name(proto_id_t id) {
    return proto_info(id)->name;
}
