#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/ipdefrag.h"

/* Build a minimal fake IPv4 fragment header + payload */
static void make_fragment(uint8_t *buf, size_t *len,
                          uint32_t src, uint32_t dst,
                          uint16_t id, uint8_t proto,
                          uint16_t frag_off, int more_frags,
                          const uint8_t *payload, size_t plen)
{
    memset(buf, 0, 20 + plen);
    buf[0]  = 0x45;                        /* version=4, ihl=5 */
    buf[9]  = proto;
    buf[10] = (uint8_t)(src >> 24); buf[11] = (uint8_t)(src >> 16);
    buf[12] = (uint8_t)(src >> 8);  buf[13] = (uint8_t)src;
    buf[14] = (uint8_t)(dst >> 24); buf[15] = (uint8_t)(dst >> 16);
    buf[16] = (uint8_t)(dst >> 8);  buf[17] = (uint8_t)dst;
    /* id */
    buf[4] = (uint8_t)(id >> 8); buf[5] = (uint8_t)id;
    /* frag offset in units of 8 bytes; MF flag in bit 13 */
    uint16_t fo = (uint16_t)((frag_off / 8) & 0x1FFF);
    if (more_frags) fo |= 0x2000;
    buf[6] = (uint8_t)(fo >> 8); buf[7] = (uint8_t)fo;
    /* total length */
    uint16_t tlen = (uint16_t)(20 + plen);
    buf[2] = (uint8_t)(tlen >> 8); buf[3] = (uint8_t)tlen;
    memcpy(buf + 20, payload, plen);
    *len = 20 + plen;
}

static void test_create_destroy(void)
{
    ipdefrag_ctx_t *ctx = ipdefrag_create();
    assert(ctx != NULL);
    assert(ipdefrag_active(ctx) == 0);
    ipdefrag_destroy(ctx);
    printf("  [PASS] create/destroy\n");
}

static void test_single_unfragmented(void)
{
    ipdefrag_ctx_t *ctx = ipdefrag_create();
    uint8_t pkt[64], payload[8] = {1,2,3,4,5,6,7,8};
    size_t plen;
    /* offset=0, MF=0 => complete datagram */
    make_fragment(pkt, &plen, 0xC0A80001, 0xC0A80002,
                  0x1234, 17, 0, 0, payload, 8);
    ipdefrag_result_t out;
    int r = ipdefrag_feed(ctx, pkt, plen, &out);
    assert(r == 1);
    assert(out.len == 8);
    assert(memcmp(out.data, payload, 8) == 0);
    free(out.data);
    ipdefrag_destroy(ctx);
    printf("  [PASS] single unfragmented\n");
}

static void test_two_fragments(void)
{
    ipdefrag_ctx_t *ctx = ipdefrag_create();
    uint8_t pkt[64], p1[8] = {0xAA,0xBB,0xCC,0xDD,0,0,0,0};
    uint8_t p2[4] = {0x11,0x22,0x33,0x44};
    size_t plen;
    ipdefrag_result_t out;

    make_fragment(pkt, &plen, 0x0A000001, 0x0A000002,
                  0xABCD, 6, 0, 1, p1, 8);
    assert(ipdefrag_feed(ctx, pkt, plen, &out) == 0);
    assert(ipdefrag_active(ctx) == 1);

    make_fragment(pkt, &plen, 0x0A000001, 0x0A000002,
                  0xABCD, 6, 8, 0, p2, 4);
    int r = ipdefrag_feed(ctx, pkt, plen, &out);
    assert(r == 1);
    assert(out.len == 12);
    assert(out.data[0] == 0xAA);
    assert(out.data[8] == 0x11);
    free(out.data);
    assert(ipdefrag_active(ctx) == 0);
    ipdefrag_destroy(ctx);
    printf("  [PASS] two-fragment reassembly\n");
}

static void test_expire(void)
{
    ipdefrag_ctx_t *ctx = ipdefrag_create();
    uint8_t pkt[64], payload[8] = {0};
    size_t plen;
    ipdefrag_result_t out;

    make_fragment(pkt, &plen, 0x01010101, 0x02020202,
                  0x0001, 17, 0, 1, payload, 8);
    ipdefrag_feed(ctx, pkt, plen, &out);
    assert(ipdefrag_active(ctx) == 1);

    int evicted = ipdefrag_expire(ctx, (uint32_t)(IPDEFRAG_TIMEOUT_SEC + 9999));
    assert(evicted >= 1);
    assert(ipdefrag_active(ctx) == 0);
    ipdefrag_destroy(ctx);
    printf("  [PASS] expire stale flows\n");
}

int main(void)
{
    printf("test_ipdefrag\n");
    test_create_destroy();
    test_single_unfragmented();
    test_two_fragments();
    test_expire();
    printf("All ipdefrag tests passed.\n");
    return 0;
}
