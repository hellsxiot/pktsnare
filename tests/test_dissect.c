#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "../src/dissect.h"

/* Minimal Ethernet + IPv4 + UDP frame */
static const uint8_t udp_frame[] = {
    /* Ethernet */
    0xff,0xff,0xff,0xff,0xff,0xff,   /* dst mac */
    0x00,0x11,0x22,0x33,0x44,0x55,   /* src mac */
    0x08,0x00,                        /* ethertype: IPv4 */
    /* IPv4 (IHL=5, proto=UDP=17) */
    0x45,0x00,0x00,0x1d,             /* ver/ihl, tos, total len=29 */
    0x00,0x00,0x40,0x00,             /* id, flags, frag */
    0x40,0x11,0x00,0x00,             /* ttl=64, proto=17, cksum */
    0xc0,0xa8,0x01,0x01,             /* src: 192.168.1.1 */
    0xc0,0xa8,0x01,0x02,             /* dst: 192.168.1.2 */
    /* UDP */
    0x00,0x35,0x04,0xd2,             /* src=53, dst=1234 */
    0x00,0x09,0x00,0x00,             /* len=9, cksum */
    /* payload */
    0xde,0xad,0xbe
};

/* Minimal Ethernet + IPv4 + TCP frame */
static const uint8_t tcp_frame[] = {
    /* Ethernet */
    0xaa,0xbb,0xcc,0xdd,0xee,0xff,
    0x11,0x22,0x33,0x44,0x55,0x66,
    0x08,0x00,
    /* IPv4 */
    0x45,0x00,0x00,0x28,
    0x00,0x01,0x40,0x00,
    0x80,0x06,0x00,0x00,             /* ttl=128, proto=6 (TCP) */
    0x0a,0x00,0x00,0x01,             /* src: 10.0.0.1 */
    0x0a,0x00,0x00,0x02,             /* dst: 10.0.0.2 */
    /* TCP (data offset=5, SYN flag) */
    0x1f,0x90,0x00,0x50,             /* src=8080, dst=80 */
    0x00,0x00,0x00,0x01,             /* seq=1 */
    0x00,0x00,0x00,0x00,             /* ack=0 */
    0x50,0x02,0x20,0x00,             /* doff=5, SYN, window */
    0x00,0x00,0x00,0x00              /* cksum, urg */
};

static void test_udp_dissect(void)
{
    packet_info_t info;
    int ret = dissect_packet(udp_frame, sizeof(udp_frame), &info);
    assert(ret == 0);
    assert(info.top_proto == PROTO_UDP);
    assert(info.transport.src_port == 53);
    assert(info.transport.dst_port == 1234);
    assert(info.ip.ttl == 64);
    assert(info.payload_len == 3);
    printf("PASS: test_udp_dissect\n");
}

static void test_tcp_dissect(void)
{
    packet_info_t info;
    int ret = dissect_packet(tcp_frame, sizeof(tcp_frame), &info);
    assert(ret == 0);
    assert(info.top_proto == PROTO_TCP);
    assert(info.transport.src_port == 8080);
    assert(info.transport.dst_port == 80);
    assert(info.transport.flags == 0x02);  /* SYN */
    assert(info.ip.ttl == 128);
    printf("PASS: test_tcp_dissect\n");
}

static void test_null_input(void)
{
    packet_info_t info;
    assert(dissect_packet(NULL, 0, &info) == -1);
    assert(dissect_packet(udp_frame, 0, &info) == -1);
    assert(dissect_packet(udp_frame, sizeof(udp_frame), NULL) == -1);
    printf("PASS: test_null_input\n");
}

static void test_truncated_frame(void)
{
    packet_info_t info;
    /* Only 10 bytes — too short for Ethernet header */
    assert(dissect_packet(udp_frame, 10, &info) == -1);
    printf("PASS: test_truncated_frame\n");
}

int main(void)
{
    test_null_input();
    test_truncated_frame();
    test_udp_dissect();
    test_tcp_dissect();
    printf("All dissect tests passed.\n");
    return 0;
}
