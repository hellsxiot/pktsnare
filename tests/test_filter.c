#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "../src/filter.h"

/* Minimal Ethernet + IPv4 + TCP packet (no payload) */
static uint8_t make_tcp_pkt(uint8_t pkt[54],
                             uint32_t sip, uint32_t dip,
                             uint16_t sport, uint16_t dport)
{
    memset(pkt, 0, 54);
    /* Ethernet: dst/src MAC + EtherType 0x0800 */
    pkt[12] = 0x08; pkt[13] = 0x00;
    /* IPv4 header at offset 14 */
    pkt[14] = 0x45;          /* version+IHL */
    pkt[23] = 6;             /* protocol = TCP */
    uint32_t sip_n = htonl(sip), dip_n = htonl(dip);
    memcpy(pkt + 26, &sip_n, 4);
    memcpy(pkt + 30, &dip_n, 4);
    /* TCP header at offset 34 */
    uint16_t sp = htons(sport), dp = htons(dport);
    memcpy(pkt + 34, &sp, 2);
    memcpy(pkt + 36, &dp, 2);
    return 54;
}

static void test_parse_empty(void)
{
    filter_rule_t r;
    assert(filter_parse("", &r) == 0);
    assert(r.proto    == FILTER_PROTO_ANY);
    assert(r.src_ip   == 0);
    assert(r.dst_port == 0);
    printf("[PASS] parse empty\n");
}

static void test_parse_full(void)
{
    filter_rule_t r;
    assert(filter_parse("proto=tcp src=192.168.1.1 dst=10.0.0.2 sport=1234 dport=80", &r) == 0);
    assert(r.proto    == FILTER_PROTO_TCP);
    assert(r.src_ip   == 0xC0A80101u); /* 192.168.1.1 */
    assert(r.dst_ip   == 0x0A000002u); /* 10.0.0.2    */
    assert(r.src_port == 1234);
    assert(r.dst_port == 80);
    printf("[PASS] parse full expression\n");
}

static void test_match_tcp(void)
{
    filter_rule_t r;
    filter_parse("proto=tcp dport=80", &r);

    uint8_t pkt[54];
    make_tcp_pkt(pkt, 0xC0A80101, 0x0A000001, 54321, 80);
    assert(filter_match(&r, pkt, 54) == 1);

    /* wrong dest port */
    make_tcp_pkt(pkt, 0xC0A80101, 0x0A000001, 54321, 443);
    assert(filter_match(&r, pkt, 54) == 0);
    printf("[PASS] match tcp dport\n");
}

static void test_match_proto_mismatch(void)
{
    filter_rule_t r;
    filter_parse("proto=udp", &r);

    uint8_t pkt[54];
    make_tcp_pkt(pkt, 0, 0, 0, 0); /* TCP packet */
    assert(filter_match(&r, pkt, 54) == 0);
    printf("[PASS] proto mismatch drops packet\n");
}

int main(void)
{
    test_parse_empty();
    test_parse_full();
    test_match_tcp();
    test_match_proto_mismatch();
    printf("All filter tests passed.\n");
    return 0;
}
