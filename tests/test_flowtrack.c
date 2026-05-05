#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "../src/flowtrack.h"

static void make_key(flow_key_t *k, uint16_t sport, uint16_t dport, uint8_t proto) {
    memset(k, 0, sizeof(*k));
    k->src_ip[0] = 192; k->src_ip[1] = 168; k->src_ip[2] = 1; k->src_ip[3] = 1;
    k->dst_ip[0] = 10;  k->dst_ip[1] = 0;   k->dst_ip[2] = 0; k->dst_ip[3] = 1;
    k->src_port = sport;
    k->dst_port = dport;
    k->proto    = proto;
}

static void test_init(void) {
    flow_table_t ft;
    flow_table_init(&ft);
    assert(flow_count(&ft) == 0);
    printf("PASS test_init\n");
}

static void test_insert_lookup(void) {
    flow_table_t ft;
    flow_table_init(&ft);
    flow_key_t k;
    make_key(&k, 1234, 80, 6);

    flow_entry_t *fe = flow_insert(&ft, &k);
    assert(fe != NULL);
    assert(flow_count(&ft) == 1);

    flow_entry_t *found = flow_lookup(&ft, &k);
    assert(found == fe);
    printf("PASS test_insert_lookup\n");
}

static void test_update(void) {
    flow_table_t ft;
    flow_table_init(&ft);
    flow_key_t k;
    make_key(&k, 5000, 443, 6);

    flow_entry_t *fe = flow_insert(&ft, &k);
    assert(fe);
    flow_update(fe, 1500);
    flow_update(fe, 200);
    assert(fe->pkt_count == 2);
    assert(fe->byte_count == 1700);
    printf("PASS test_update\n");
}

static void test_lookup_miss(void) {
    flow_table_t ft;
    flow_table_init(&ft);
    flow_key_t k;
    make_key(&k, 9999, 22, 17);
    assert(flow_lookup(&ft, &k) == NULL);
    printf("PASS test_lookup_miss\n");
}

static void test_expire(void) {
    flow_table_t ft;
    flow_table_init(&ft);
    flow_key_t k;
    make_key(&k, 3000, 53, 17);
    flow_entry_t *fe = flow_insert(&ft, &k);
    assert(fe);
    /* force last_seen into the past */
    fe->last_seen.tv_sec -= 120;
    flow_expire(&ft, 60);
    assert(flow_count(&ft) == 0);
    assert(flow_lookup(&ft, &k) == NULL);
    printf("PASS test_expire\n");
}

int main(void) {
    test_init();
    test_insert_lookup();
    test_update();
    test_lookup_miss();
    test_expire();
    printf("All flowtrack tests passed.\n");
    return 0;
}
