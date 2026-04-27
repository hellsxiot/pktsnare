#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "../src/session.h"

static session_key_t make_key(uint32_t sip, uint32_t dip,
                               uint16_t sp, uint16_t dp, uint8_t proto) {
    session_key_t k;
    k.src_ip   = sip;
    k.dst_ip   = dip;
    k.src_port = sp;
    k.dst_port = dp;
    k.proto    = proto;
    return k;
}

static void test_init_empty(void) {
    session_table_t tbl;
    session_table_init(&tbl);
    assert(tbl.count == 0);
    printf("[PASS] test_init_empty\n");
}

static void test_insert_and_lookup(void) {
    session_table_t tbl;
    session_table_init(&tbl);

    session_key_t k = make_key(0x0a000001, 0x0a000002, 1234, 80, PROTO_TCP);
    session_entry_t *e = session_insert(&tbl, &k);
    assert(e != NULL);
    assert(tbl.count == 1);

    session_entry_t *found = session_lookup(&tbl, &k);
    assert(found != NULL);
    assert(found->key.src_port == 1234);
    printf("[PASS] test_insert_and_lookup\n");
}

static void test_lookup_missing(void) {
    session_table_t tbl;
    session_table_init(&tbl);

    session_key_t k = make_key(0x01010101, 0x02020202, 9999, 53, PROTO_UDP);
    session_entry_t *found = session_lookup(&tbl, &k);
    assert(found == NULL);
    printf("[PASS] test_lookup_missing\n");
}

static void test_update_counts(void) {
    session_table_t tbl;
    session_table_init(&tbl);

    session_key_t k = make_key(0xc0a80001, 0xc0a80002, 4321, 443, PROTO_TCP);
    session_entry_t *e = session_insert(&tbl, &k);
    assert(e != NULL);

    session_update(e, 512, 1);
    session_update(e, 256, 0);
    session_update(e, 128, 1);

    assert(e->pkts_fwd == 2);
    assert(e->pkts_rev == 1);
    assert(e->bytes_fwd == 640);
    assert(e->bytes_rev == 256);
    printf("[PASS] test_update_counts\n");
}

static void test_expire(void) {
    session_table_t tbl;
    session_table_init(&tbl);

    session_key_t k1 = make_key(0x01000001, 0x01000002, 100, 200, PROTO_TCP);
    session_key_t k2 = make_key(0x01000003, 0x01000004, 300, 400, PROTO_UDP);

    session_entry_t *e1 = session_insert(&tbl, &k1);
    session_entry_t *e2 = session_insert(&tbl, &k2);
    assert(e1 && e2);

    time_t old_time = time(NULL) - (SESSION_TIMEOUT_SEC + 10);
    e1->last_seen = old_time;

    int expired = session_expire(&tbl, time(NULL));
    assert(expired == 1);
    assert(tbl.count == 1);
    printf("[PASS] test_expire\n");
}

int main(void) {
    test_init_empty();
    test_insert_and_lookup();
    test_lookup_missing();
    test_update_counts();
    test_expire();
    printf("All session tests passed.\n");
    return 0;
}
