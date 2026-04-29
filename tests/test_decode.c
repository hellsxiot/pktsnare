#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/decode.h"

static void test_decode_ethernet(void)
{
    /* minimal 14-byte ethernet frame header */
    uint8_t buf[14] = {
        0xff,0xff,0xff,0xff,0xff,0xff,  /* dst: broadcast */
        0xaa,0xbb,0xcc,0xdd,0xee,0xff,  /* src */
        0x08,0x00                        /* ethertype: IPv4 */
    };
    decode_result_t r;
    assert(decode_ethernet(buf, sizeof(buf), &r) == 0);
    assert(strcmp(r.proto_name, "Ethernet") == 0);
    assert(r.field_count == 3);
    assert(r.fields[2].value.u16 == 0x0800);
    printf("PASS: decode_ethernet\n");
}

static void test_decode_ipv4(void)
{
    uint8_t buf[20] = {
        0x45, 0x00, 0x00, 0x28,  /* ver/ihl, tos, tot_len=40 */
        0x00, 0x01, 0x00, 0x00,  /* id, flags/frag */
        0x40, 0x06, 0x00, 0x00,  /* ttl=64, proto=6(TCP), cksum */
        0x7f, 0x00, 0x00, 0x01,  /* src: 127.0.0.1 */
        0x7f, 0x00, 0x00, 0x01   /* dst: 127.0.0.1 */
    };
    decode_result_t r;
    assert(decode_ipv4(buf, sizeof(buf), &r) == 0);
    assert(strcmp(r.proto_name, "IPv4") == 0);
    /* version field */
    assert(r.fields[0].value.u8 == 4);
    /* protocol field (index 5) */
    assert(r.fields[5].value.u8 == 6);
    printf("PASS: decode_ipv4\n");
}

static void test_decode_tcp(void)
{
    uint8_t buf[20] = {
        0x1f, 0x90, 0x00, 0x50,  /* src=8080, dst=80 */
        0x00, 0x00, 0x00, 0x01,  /* seq=1 */
        0x00, 0x00, 0x00, 0x00,  /* ack=0 */
        0x50, 0x02, 0x20, 0x00,  /* offset, flags=SYN, window */
        0x00, 0x00, 0x00, 0x00   /* cksum, urg */
    };
    decode_result_t r;
    assert(decode_tcp(buf, sizeof(buf), &r) == 0);
    assert(strcmp(r.proto_name, "TCP") == 0);
    assert(r.fields[0].value.u16 == 8080);
    assert(r.fields[1].value.u16 == 80);
    assert(r.fields[4].value.u8  == 0x02);  /* SYN flag */
    printf("PASS: decode_tcp\n");
}

static void test_decode_udp(void)
{
    uint8_t buf[8] = {
        0x00, 0x35, 0xd8, 0xf0,  /* src=53, dst=55536 */
        0x00, 0x08, 0x00, 0x00   /* len=8, cksum=0 */
    };
    decode_result_t r;
    assert(decode_udp(buf, sizeof(buf), &r) == 0);
    assert(strcmp(r.proto_name, "UDP") == 0);
    assert(r.fields[0].value.u16 == 53);
    assert(r.fields[2].value.u16 == 8);
    printf("PASS: decode_udp\n");
}

static void test_decode_bad_input(void)
{
    decode_result_t r;
    assert(decode_ethernet(NULL, 14, &r) == -1);
    assert(decode_ipv4(NULL, 20, &r)     == -1);
    uint8_t tiny[4] = {0};
    assert(decode_tcp(tiny, sizeof(tiny), &r) == -1);
    assert(decode_udp(tiny, 4, &r)            == -1);
    printf("PASS: decode_bad_input\n");
}

int main(void)
{
    test_decode_ethernet();
    test_decode_ipv4();
    test_decode_tcp();
    test_decode_udp();
    test_decode_bad_input();
    printf("All decode tests passed.\n");
    return 0;
}
