#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "../src/bpf_wrap.h"

/* Minimal Ethernet + IPv4 + UDP frame targeting port 53 */
static const uint8_t udp_dns_pkt[] = {
    /* Ethernet */
    0xff,0xff,0xff,0xff,0xff,0xff,  /* dst MAC */
    0x00,0x11,0x22,0x33,0x44,0x55,  /* src MAC */
    0x08,0x00,                       /* EtherType IPv4 */
    /* IPv4 */
    0x45,0x00,0x00,0x1c,             /* ver/ihl/tos/totlen */
    0x00,0x01,0x00,0x00,             /* id/frag */
    0x40,0x11,0x00,0x00,             /* ttl/proto UDP/checksum */
    0x7f,0x00,0x00,0x01,             /* src 127.0.0.1 */
    0x7f,0x00,0x00,0x01,             /* dst 127.0.0.1 */
    /* UDP */
    0x04,0xd2,                       /* src port 1234 */
    0x00,0x35,                       /* dst port 53 */
    0x00,0x08,0x00,0x00              /* len/checksum */
};

static void test_compile_valid(void)
{
    bpf_wrap_t *bw = bpf_wrap_compile("udp port 53", 65535);
    assert(bw != NULL);
    assert(strcmp(bpf_wrap_expr(bw), "udp port 53") == 0);
    bpf_wrap_free(bw);
    printf("  [PASS] compile valid expression\n");
}

static void test_compile_invalid(void)
{
    bpf_wrap_t *bw = bpf_wrap_compile("this is not valid bpf!!!", 65535);
    assert(bw == NULL);
    printf("  [PASS] compile invalid expression returns NULL\n");
}

static void test_compile_null(void)
{
    bpf_wrap_t *bw = bpf_wrap_compile(NULL, 65535);
    assert(bw == NULL);
    printf("  [PASS] compile NULL expression returns NULL\n");
}

static void test_match_udp_dns(void)
{
    bpf_wrap_t *bw = bpf_wrap_compile("udp port 53", 65535);
    assert(bw != NULL);
    int r = bpf_wrap_match(bw, udp_dns_pkt, sizeof(udp_dns_pkt));
    assert(r == 1);
    bpf_wrap_free(bw);
    printf("  [PASS] udp port 53 matches DNS packet\n");
}

static void test_no_match(void)
{
    bpf_wrap_t *bw = bpf_wrap_compile("tcp port 80", 65535);
    assert(bw != NULL);
    int r = bpf_wrap_match(bw, udp_dns_pkt, sizeof(udp_dns_pkt));
    assert(r == 0);
    bpf_wrap_free(bw);
    printf("  [PASS] tcp port 80 does not match UDP packet\n");
}

static void test_match_null_handle(void)
{
    int r = bpf_wrap_match(NULL, udp_dns_pkt, sizeof(udp_dns_pkt));
    assert(r == -1);
    printf("  [PASS] NULL handle returns -1\n");
}

static void test_expr_too_long(void)
{
    char big[BPF_EXPR_MAX + 10];
    memset(big, 'a', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';
    bpf_wrap_t *bw = bpf_wrap_compile(big, 65535);
    assert(bw == NULL);
    printf("  [PASS] oversized expression rejected\n");
}

int main(void)
{
    printf("=== test_bpf_wrap ===\n");
    test_compile_valid();
    test_compile_invalid();
    test_compile_null();
    test_match_udp_dns();
    test_no_match();
    test_match_null_handle();
    test_expr_too_long();
    printf("All tests passed.\n");
    return 0;
}
