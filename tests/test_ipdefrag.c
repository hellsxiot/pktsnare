#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/ipdefrag.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) printf("  %-40s", name)
#define PASS() do { puts("PASS"); passed++; } while(0)
#define FAIL(msg) do { printf("FAIL (%s)\n", msg); failed++; } while(0)

static void test_single_fragment(void) {
    TEST("single fragment (no more bit)");
    ipdefrag_init();
    uint8_t payload[] = {0xde,0xad,0xbe,0xef};
    int r = ipdefrag_add(1, 0xC0A80001, 0xC0A80002, 0, 0, payload, 4);
    if (r == IPDEFRAG_COMPLETE) PASS(); else FAIL("expected COMPLETE");
}

static void test_two_fragments_reassemble(void) {
    TEST("two fragments reassemble");
    ipdefrag_init();
    uint8_t p1[] = {0x01,0x02,0x03,0x04};
    uint8_t p2[] = {0x05,0x06,0x07,0x08};
    int r1 = ipdefrag_add(2, 0x0A000001, 0x0A000002, 0,  1, p1, 4);
    int r2 = ipdefrag_add(2, 0x0A000001, 0x0A000002, 4,  0, p2, 4);
    if (r1 == IPDEFRAG_PENDING && r2 == IPDEFRAG_COMPLETE) PASS();
    else FAIL("unexpected status");
}

static void test_get_reassembled(void) {
    TEST("get reassembled buffer");
    ipdefrag_init();
    uint8_t p1[] = {0xAA,0xBB};
    uint8_t p2[] = {0xCC,0xDD};
    ipdefrag_add(3, 1, 2, 0, 1, p1, 2);
    ipdefrag_add(3, 1, 2, 2, 0, p2, 2);
    uint8_t out[16];
    uint16_t olen = 0;
    int r = ipdefrag_get(3, 1, 2, out, &olen, sizeof(out));
    if (r == IPDEFRAG_COMPLETE && olen == 4 &&
        out[0]==0xAA && out[1]==0xBB && out[2]==0xCC && out[3]==0xDD)
        PASS();
    else FAIL("buffer mismatch");
}

static void test_missing_last_fragment(void) {
    TEST("pending when last frag not received");
    ipdefrag_init();
    uint8_t p[] = {0x01,0x02};
    int r = ipdefrag_add(4, 1, 2, 0, 1, p, 2);
    if (r == IPDEFRAG_PENDING) PASS(); else FAIL("expected PENDING");
}

static void test_get_before_complete(void) {
    TEST("get returns err when incomplete");
    ipdefrag_init();
    uint8_t p[] = {0xFF};
    ipdefrag_add(5, 1, 2, 0, 1, p, 1);
    uint8_t out[16]; uint16_t olen = 0;
    int r = ipdefrag_get(5, 1, 2, out, &olen, sizeof(out));
    if (r == IPDEFRAG_ERR) PASS(); else FAIL("expected ERR");
}

static void test_flush_clears_state(void) {
    TEST("flush clears all entries");
    ipdefrag_init();
    uint8_t p[] = {0x01};
    ipdefrag_add(6, 1, 2, 0, 1, p, 1);
    ipdefrag_flush();
    uint8_t out[16]; uint16_t olen = 0;
    int r = ipdefrag_get(6, 1, 2, out, &olen, sizeof(out));
    if (r == IPDEFRAG_ERR) PASS(); else FAIL("expected ERR after flush");
}

static void test_different_flows_isolated(void) {
    TEST("different flow IDs are isolated");
    ipdefrag_init();
    uint8_t pa[] = {0x01,0x02};
    uint8_t pb[] = {0xAA,0xBB};
    ipdefrag_add(10, 1, 2, 0, 0, pa, 2);
    ipdefrag_add(11, 1, 2, 0, 0, pb, 2);
    uint8_t out[16]; uint16_t olen = 0;
    ipdefrag_get(10, 1, 2, out, &olen, sizeof(out));
    int r = ipdefrag_get(11, 1, 2, out, &olen, sizeof(out));
    if (r == IPDEFRAG_COMPLETE && out[0] == 0xAA) PASS();
    else FAIL("flow isolation broken");
}

int main(void) {
    printf("=== test_ipdefrag ===\n");
    test_single_fragment();
    test_two_fragments_reassemble();
    test_get_reassembled();
    test_missing_last_fragment();
    test_get_before_complete();
    test_flush_clears_state();
    test_different_flows_isolated();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
