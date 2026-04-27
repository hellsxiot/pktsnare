#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "../src/output.h"
#include "../src/dissect.h"

static DissectedPacket make_pkt(void) {
    DissectedPacket p;
    memset(&p, 0, sizeof(p));
    p.timestamp.tv_sec  = 1700000000;
    p.timestamp.tv_usec = 123456;
    strncpy(p.proto_str, "TCP", sizeof(p.proto_str));
    p.has_ip        = 1;
    p.has_transport = 1;
    inet_pton(AF_INET, "192.168.1.10", &p.src_ip);
    inet_pton(AF_INET, "10.0.0.1",    &p.dst_ip);
    p.src_port   = 54321;
    p.dst_port   = 80;
    p.payload_len = 512;
    return p;
}

static void test_text_output_to_file(void) {
    const char *path = "/tmp/pktsnare_test_text.txt";
    output_init(OUTPUT_TEXT, path);
    DissectedPacket pkt = make_pkt();
    output_write(&pkt);
    output_close();

    FILE *f = fopen(path, "r");
    assert(f != NULL);
    char buf[256];
    assert(fgets(buf, sizeof(buf), f) != NULL);
    assert(strstr(buf, "TCP")     != NULL);
    assert(strstr(buf, "192.168.1.10") != NULL);
    assert(strstr(buf, "len=512") != NULL);
    fclose(f);
    printf("PASS: test_text_output_to_file\n");
}

static void test_json_output_to_file(void) {
    const char *path = "/tmp/pktsnare_test_json.txt";
    output_init(OUTPUT_JSON, path);
    DissectedPacket pkt = make_pkt();
    output_write(&pkt);
    output_close();

    FILE *f = fopen(path, "r");
    assert(f != NULL);
    char buf[512];
    assert(fgets(buf, sizeof(buf), f) != NULL);
    assert(buf[0] == '{');
    assert(strstr(buf, "\"proto\":\"TCP\"") != NULL);
    assert(strstr(buf, "\"dport\":80")      != NULL);
    assert(strstr(buf, "\"len\":512")       != NULL);
    fclose(f);
    printf("PASS: test_json_output_to_file\n");
}

static void test_null_packet_no_crash(void) {
    output_init(OUTPUT_TEXT, NULL);
    output_write(NULL);  /* must not crash */
    output_close();
    printf("PASS: test_null_packet_no_crash\n");
}

static void test_set_format_switch(void) {
    const char *path = "/tmp/pktsnare_test_switch.txt";
    output_init(OUTPUT_TEXT, path);
    output_set_format(OUTPUT_JSON);
    DissectedPacket pkt = make_pkt();
    output_write(&pkt);
    output_close();

    FILE *f = fopen(path, "r");
    assert(f != NULL);
    char buf[512];
    assert(fgets(buf, sizeof(buf), f) != NULL);
    assert(buf[0] == '{');  /* should be JSON after format switch */
    fclose(f);
    printf("PASS: test_set_format_switch\n");
}

int main(void) {
    test_text_output_to_file();
    test_json_output_to_file();
    test_null_packet_no_crash();
    test_set_format_switch();
    printf("All output tests passed.\n");
    return 0;
}
