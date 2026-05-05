#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/session.h"

#define SRC_IP   0xC0A80101u  /* 192.168.1.1 */
#define DST_IP   0xC0A80102u  /* 192.168.1.2 */
#define SRC_PORT 12345
#define DST_PORT 80
#define PROTO_TCP 6

static void test_init(void) {
    session_init();
    assert(session_get_count() == 0);
    printf("PASS test_init\n");
}

static void test_create_and_lookup(void) {
    session_init();
    session_entry_t *e = session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    assert(e != NULL);
    assert(e->active == 1);
    assert(e->src_ip == SRC_IP);
    assert(e->dst_ip == DST_IP);
    assert(e->proto == PROTO_TCP);
    assert(session_get_count() == 1);

    session_entry_t *found = session_lookup(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    assert(found == e);
    printf("PASS test_create_and_lookup\n");
}

static void test_reverse_lookup(void) {
    session_init();
    session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    /* lookup with reversed src/dst should still find the session */
    session_entry_t *found = session_lookup(DST_IP, SRC_IP, DST_PORT, SRC_PORT, PROTO_TCP);
    assert(found != NULL);
    printf("PASS test_reverse_lookup\n");
}

static void test_update(void) {
    session_init();
    session_entry_t *e = session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    session_update(e, 100);
    session_update(e, 200);
    assert(e->pkt_count == 2);
    assert(e->byte_count == 300);
    printf("PASS test_update\n");
}

static void test_lookup_miss(void) {
    session_init();
    session_entry_t *found = session_lookup(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    assert(found == NULL);
    printf("PASS test_lookup_miss\n");
}

static void test_expire(void) {
    session_init();
    session_entry_t *e = session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    /* force last_seen to the past */
    e->last_seen -= 120;
    session_expire(60);
    assert(session_get_count() == 0);
    session_entry_t *found = session_lookup(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    assert(found == NULL);
    printf("PASS test_expire\n");
}

/* Ensure a session that hasn't timed out is NOT removed by session_expire */
static void test_expire_keeps_active(void) {
    session_init();
    session_entry_t *e = session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    /* last_seen is current; only age it by less than the timeout */
    e->last_seen -= 30;
    session_expire(60);
    assert(session_get_count() == 1);
    session_entry_t *found = session_lookup(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    assert(found != NULL);
    printf("PASS test_expire_keeps_active\n");
}

static void test_dump_no_crash(void) {
    session_init();
    session_create(SRC_IP, DST_IP, SRC_PORT, DST_PORT, PROTO_TCP);
    session_dump(stdout);
    printf("PASS test_dump_no_crash\n");
}

int main(void) {
    test_init();
    test_create_and_lookup();
    test_reverse_lookup();
    test_update();
    test_lookup_miss();
    test_expire();
    test_expire_keeps_active();
    test_dump_no_crash();
    printf("All session tests passed.\n");
    return 0;
}
