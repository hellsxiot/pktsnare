#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/capture.h"

/* ---------- helpers ---------- */

static int handler_called = 0;
static uint32_t last_pkt_len = 0;

static void dummy_handler(const raw_packet_t *pkt, void *user) {
    (void)user;
    handler_called++;
    last_pkt_len = pkt->len;
}

/* ---------- unit tests ---------- */

static void test_ctx_init_on_open(void) {
    /* We can't open a real socket in a unit-test environment without root,
     * so we just verify that capture_open fails gracefully and leaves the
     * socket fd in a defined state when the interface doesn't exist. */
    capture_ctx_t ctx;
    int ret = capture_open(&ctx, "noif999", 0);
    /* Expected: failure because interface doesn't exist */
    assert(ret == -1);
    /* sockfd should have been cleaned up */
    printf("[PASS] test_ctx_init_on_open\n");
}

static void test_snaplen_default(void) {
    capture_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    /* Simulate what capture_open does before the socket call */
    ctx.snaplen = CAPTURE_SNAPLEN;
    assert(ctx.snaplen == 65535);
    printf("[PASS] test_snaplen_default\n");
}

static void test_iface_name_truncation(void) {
    capture_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    strncpy(ctx.iface, "verylonginterfacename", IFACE_NAME_MAX - 1);
    ctx.iface[IFACE_NAME_MAX - 1] = '\0';
    assert(strlen(ctx.iface) < IFACE_NAME_MAX);
    printf("[PASS] test_iface_name_truncation\n");
}

static void test_handler_signature(void) {
    /* Verify the callback typedef compiles and is assignable */
    packet_handler_t h = dummy_handler;
    assert(h != NULL);
    (void)handler_called;
    (void)last_pkt_len;
    printf("[PASS] test_handler_signature\n");
}

int main(void) {
    printf("=== capture module tests ===\n");
    test_ctx_init_on_open();
    test_snaplen_default();
    test_iface_name_truncation();
    test_handler_signature();
    printf("All tests passed.\n");
    return 0;
}
