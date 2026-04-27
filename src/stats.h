#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <time.h>

/* Per-protocol packet counters */
typedef struct {
    uint64_t total_packets;
    uint64_t total_bytes;
    uint64_t tcp_packets;
    uint64_t udp_packets;
    uint64_t icmp_packets;
    uint64_t arp_packets;
    uint64_t other_packets;
    uint64_t dropped_packets;
    time_t   start_time;
} pktsnare_stats_t;

/**
 * Initialize stats structure, zeroing counters and recording start time.
 */
void stats_init(pktsnare_stats_t *stats);

/**
 * Update stats given a protocol identifier and packet byte length.
 * proto: 6=TCP, 17=UDP, 1=ICMP, 0x0806=ARP (raw ethertype), else other.
 */
void stats_update(pktsnare_stats_t *stats, uint16_t proto, uint32_t pkt_len);

/**
 * Increment the dropped packet counter.
 */
void stats_drop(pktsnare_stats_t *stats);

/**
 * Print a summary of collected stats to stdout.
 */
void stats_print(const pktsnare_stats_t *stats);

/**
 * Return elapsed seconds since stats_init was called.
 */
double stats_elapsed(const pktsnare_stats_t *stats);

#endif /* STATS_H */
