/*
 * pcap.h - PCAP file format read/write support for pktsnare
 *
 * Supports writing captured packets to standard .pcap files
 * for offline analysis with Wireshark and similar tools.
 */

#ifndef PKTSNARE_PCAP_H
#define PKTSNARE_PCAP_H

#include <stdint.h>
#include <stdio.h>
#include "capture.h"

/* PCAP magic numbers */
#define PCAP_MAGIC_NATIVE    0xa1b2c3d4  /* native byte order timestamps (us) */
#define PCAP_MAGIC_SWAPPED   0xd4c3b2a1  /* swapped byte order */
#define PCAP_MAGIC_NANO      0xa1b23c4d  /* nanosecond timestamps */

#define PCAP_VERSION_MAJOR   2
#define PCAP_VERSION_MINOR   4

/* Link-layer type constants (subset) */
#define PCAP_LINKTYPE_ETHERNET   1
#define PCAP_LINKTYPE_RAW_IP     101
#define PCAP_LINKTYPE_LINUX_SLL  113

/* Global file header (24 bytes) */
typedef struct {
    uint32_t magic_number;   /* magic number */
    uint16_t version_major;  /* major version number */
    uint16_t version_minor;  /* minor version number */
    int32_t  thiszone;       /* GMT to local correction (usually 0) */
    uint32_t sigfigs;        /* accuracy of timestamps (usually 0) */
    uint32_t snaplen;        /* max length of captured packets */
    uint32_t network;        /* data link type */
} pcap_file_header_t;

/* Per-packet header (16 bytes) */
typedef struct {
    uint32_t ts_sec;    /* timestamp seconds */
    uint32_t ts_usec;   /* timestamp microseconds */
    uint32_t incl_len;  /* number of octets of packet saved in file */
    uint32_t orig_len;  /* actual length of packet */
} pcap_packet_header_t;

/* PCAP writer context */
typedef struct {
    FILE    *fp;
    int      linktype;
    uint32_t snaplen;
    uint64_t packets_written;
    uint64_t bytes_written;
} pcap_writer_t;

/*
 * Open a new .pcap file for writing.
 * Returns 0 on success, -1 on error.
 */
int pcap_writer_open(pcap_writer_t *w, const char *path,
                     int linktype, uint32_t snaplen);

/*
 * Write a single captured packet to the pcap file.
 * Returns 0 on success, -1 on error.
 */
int pcap_writer_write(pcap_writer_t *w, const packet_t *pkt);

/*
 * Flush and close the pcap file.
 */
void pcap_writer_close(pcap_writer_t *w);

/*
 * Return a human-readable string for a link-layer type.
 */
const char *pcap_linktype_name(int linktype);

#endif /* PKTSNARE_PCAP_H */
