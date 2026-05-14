#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "../src/mactrack.h"

static const uint8_t MAC_A[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const uint8_t MAC_B[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
static const uint8_t MAC_C[6] = {0xde, 0xad, 0xbe, 0xef, 0x00, 0x01};

static void test_init(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    assert(tbl.count == 0);
    printf("PASS test_init\n");
}

static void test_update_new(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    int r = mactrack_update(&tbl, MAC_A, 0xc0a80101, 100);
    assert(r == 0);
    assert(tbl.count == 1);
    printf("PASS test_update_new\n");
}

static void test_update_existing(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    mactrack_update(&tbl, MAC_A, 0xc0a80101, 100);
    mactrack_update(&tbl, MAC_A, 0xc0a80101, 200);
    assert(tbl.count == 1);
    mactrack_entry_t *e = mactrack_lookup(&tbl, MAC_A);
    assert(e != NULL);
    assert(e->pkt_count == 2);
    assert(e->byte_count == 300);
    printf("PASS test_update_existing\n");
}

static void test_lookup_miss(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    mactrack_update(&tbl, MAC_A, 0xc0a80101, 64);
    mactrack_entry_t *e = mactrack_lookup(&tbl, MAC_B);
    assert(e == NULL);
    printf("PASS test_lookup_miss\n");
}

static void test_multiple_macs(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    mactrack_update(&tbl, MAC_A, 0x01010101, 50);
    mactrack_update(&tbl, MAC_B, 0x02020202, 60);
    mactrack_update(&tbl, MAC_C, 0x03030303, 70);
    assert(tbl.count == 3);
    assert(mactrack_lookup(&tbl, MAC_A) != NULL);
    assert(mactrack_lookup(&tbl, MAC_B) != NULL);
    assert(mactrack_lookup(&tbl, MAC_C) != NULL);
    printf("PASS test_multiple_macs\n");
}

static void test_expire(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    mactrack_update(&tbl, MAC_A, 0xc0a80101, 64);
    mactrack_entry_t *e = mactrack_lookup(&tbl, MAC_A);
    assert(e != NULL);
    /* force last_seen into the past */
    e->last_seen -= 400;
    mactrack_expire(&tbl, 300);
    assert(tbl.count == 0);
    assert(mactrack_lookup(&tbl, MAC_A) == NULL);
    printf("PASS test_expire\n");
}

static void test_reset(void) {
    mactrack_table_t tbl;
    mactrack_init(&tbl);
    mactrack_update(&tbl, MAC_A, 0xc0a80101, 100);
    mactrack_reset(&tbl);
    assert(tbl.count == 0);
    assert(mactrack_lookup(&tbl, MAC_A) == NULL);
    printf("PASS test_reset\n");
}

static void test_null_safety(void) {
    mactrack_init(NULL);
    assert(mactrack_update(NULL, MAC_A, 0, 0) == -1);
    assert(mactrack_lookup(NULL, MAC_A) == NULL);
    mactrack_expire(NULL, 60);
    printf("PASS test_null_safety\n");
}

int main(void) {
    test_init();
    test_update_new();
    test_update_existing();
    test_lookup_miss();
    test_multiple_macs();
    test_expire();
    test_reset();
    test_null_safety();
    printf("All mactrack tests passed.\n");
    return 0;
}
