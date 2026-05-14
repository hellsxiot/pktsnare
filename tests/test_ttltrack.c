#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/ttltrack.h"

static ttltrack_table_t tbl;

static uint32_t mkip(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | d;
}

static void test_init(void) {
    ttltrack_init(&tbl);
    assert(tbl.count == 0);
    assert(tbl.anomalies_detected == 0);
    printf("PASS test_init\n");
}

static void test_update_new_entry(void) {
    ttltrack_init(&tbl);
    uint32_t ip = mkip(10, 0, 0, 1);
    int ret = ttltrack_update(&tbl, ip, 60);
    assert(ret == 0);
    assert(tbl.count == 1);
    ttltrack_entry_t *e = ttltrack_lookup(&tbl, ip);
    assert(e != NULL);
    assert(e->observed_ttl == 60);
    assert(e->baseline_ttl == 64);
    assert(e->pkt_count == 1);
    printf("PASS test_update_new_entry\n");
}

static void test_no_anomaly_normal_ttl(void) {
    ttltrack_init(&tbl);
    uint32_t ip = mkip(192, 168, 1, 1);
    /* ttl=120 -> baseline 128, delta=8 <= threshold */
    int ret = ttltrack_update(&tbl, ip, 120);
    assert(ret == 0);
    assert(tbl.anomalies_detected == 0);
    printf("PASS test_no_anomaly_normal_ttl\n");
}

static void test_anomaly_detected(void) {
    ttltrack_init(&tbl);
    uint32_t ip = mkip(172, 16, 0, 5);
    /* baseline will be 128, ttl=50 -> delta=78 > threshold */
    int ret = ttltrack_update(&tbl, ip, 50);
    assert(ret == 1);
    assert(tbl.anomalies_detected == 1);
    ttltrack_entry_t *e = ttltrack_lookup(&tbl, ip);
    assert(e->anomaly_flagged == 1);
    printf("PASS test_anomaly_detected\n");
}

static void test_multiple_ips(void) {
    ttltrack_init(&tbl);
    for (int i = 1; i <= 5; i++)
        ttltrack_update(&tbl, mkip(10, 0, 0, (uint8_t)i), 60);
    assert(tbl.count == 5);
    printf("PASS test_multiple_ips\n");
}

static void test_reset(void) {
    ttltrack_init(&tbl);
    ttltrack_update(&tbl, mkip(1, 2, 3, 4), 55);
    ttltrack_reset(&tbl);
    assert(tbl.count == 0);
    assert(tbl.anomalies_detected == 0);
    printf("PASS test_reset\n");
}

static void test_lookup_miss(void) {
    ttltrack_init(&tbl);
    ttltrack_entry_t *e = ttltrack_lookup(&tbl, mkip(9, 9, 9, 9));
    assert(e == NULL);
    printf("PASS test_lookup_miss\n");
}

int main(void) {
    test_init();
    test_update_new_entry();
    test_no_anomaly_normal_ttl();
    test_anomaly_detected();
    test_multiple_ips();
    test_reset();
    test_lookup_miss();
    printf("All ttltrack tests passed.\n");
    return 0;
}
