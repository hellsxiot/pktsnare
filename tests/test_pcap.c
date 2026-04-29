#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include "../src/pcap.h"

#define TMP_FILE "/tmp/test_pktsnare.pcap"

static void test_open_close(void) {
    pcap_writer_t *w = pcap_writer_open(TMP_FILE, PCAP_DEFAULT_SNAPLEN, PCAP_LINKTYPE_ETHERNET);
    assert(w != NULL);
    assert(w->pkt_count == 0);
    assert(w->linktype == PCAP_LINKTYPE_ETHERNET);
    pcap_writer_close(w);
    unlink(TMP_FILE);
    printf("[PASS] test_open_close\n");
}

static void test_write_packet(void) {
    uint8_t pkt[64];
    memset(pkt, 0xAB, sizeof(pkt));

    pcap_writer_t *w = pcap_writer_open(TMP_FILE, PCAP_DEFAULT_SNAPLEN, PCAP_LINKTYPE_ETHERNET);
    assert(w != NULL);

    int rc = pcap_writer_write(w, pkt, sizeof(pkt));
    assert(rc == 0);
    assert(w->pkt_count == 1);

    rc = pcap_writer_write(w, pkt, sizeof(pkt));
    assert(rc == 0);
    assert(w->pkt_count == 2);

    pcap_writer_close(w);
    unlink(TMP_FILE);
    printf("[PASS] test_write_packet\n");
}

static void test_snaplen_truncation(void) {
    uint8_t big_pkt[256];
    memset(big_pkt, 0xFF, sizeof(big_pkt));
    uint32_t snaplen = 100;

    pcap_writer_t *w = pcap_writer_open(TMP_FILE, snaplen, PCAP_LINKTYPE_RAW);
    assert(w != NULL);

    int rc = pcap_writer_write(w, big_pkt, sizeof(big_pkt));
    assert(rc == 0);

    /* Verify file size: global hdr + rec hdr + snaplen bytes */
    pcap_writer_flush(w);
    FILE *fp = fopen(TMP_FILE, "rb");
    assert(fp != NULL);
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);
    long expected = sizeof(pcap_file_header_t) + sizeof(pcap_rec_header_t) + snaplen;
    assert(sz == expected);

    pcap_writer_close(w);
    unlink(TMP_FILE);
    printf("[PASS] test_snaplen_truncation\n");
}

static void test_null_inputs(void) {
    assert(pcap_writer_open(NULL, 65535, PCAP_LINKTYPE_ETHERNET) == NULL);
    pcap_writer_write(NULL, NULL, 0);
    pcap_writer_close(NULL);
    printf("[PASS] test_null_inputs\n");
}

int main(void) {
    test_open_close();
    test_write_packet();
    test_snaplen_truncation();
    test_null_inputs();
    printf("All pcap tests passed.\n");
    return 0;
}
