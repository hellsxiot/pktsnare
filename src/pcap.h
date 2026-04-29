#ifndef PCAP_H
#define PCAP_H

#include <stdint.h>
#include <stdio.h>

/* Standard pcap file global header (24 bytes) */
typedef struct {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t  thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
} pcap_file_header_t;

/* Per-packet record header (16 bytes) */
typedef struct {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
} pcap_rec_header_t;

/* Common link-layer type constants */
#define PCAP_LINKTYPE_ETHERNET  1
#define PCAP_LINKTYPE_RAW       101
#define PCAP_LINKTYPE_LINUX_SLL 113

#define PCAP_DEFAULT_SNAPLEN    65535

typedef struct {
    FILE    *fp;
    uint32_t snaplen;
    uint32_t linktype;
    uint64_t pkt_count;
} pcap_writer_t;

/**
 * Open a new .pcap file for writing.
 * Returns NULL on failure.
 */
pcap_writer_t *pcap_writer_open(const char *path, uint32_t snaplen, uint32_t linktype);

/**
 * Write a single packet record.
 * Returns 0 on success, -1 on error.
 */
int pcap_writer_write(pcap_writer_t *w, const uint8_t *data, uint32_t len);

/** Flush buffered data to disk. */
void pcap_writer_flush(pcap_writer_t *w);

/** Close writer and free resources. */
void pcap_writer_close(pcap_writer_t *w);

#endif /* PCAP_H */
