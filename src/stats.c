#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stats.h"

void stats_init(pktsnare_stats_t *stats) {
    if (!stats) return;
    memset(stats, 0, sizeof(*stats));
    stats->start_time = time(NULL);
}

void stats_update(pktsnare_stats_t *stats, uint16_t proto, uint32_t pkt_len) {
    if (!stats) return;

    stats->total_packets++;
    stats->total_bytes += pkt_len;

    switch (proto) {
        case 6:      stats->tcp_packets++;  break;
        case 17:     stats->udp_packets++;  break;
        case 1:      stats->icmp_packets++; break;
        case 0x0806: stats->arp_packets++;  break;
        default:     stats->other_packets++; break;
    }
}

void stats_drop(pktsnare_stats_t *stats) {
    if (!stats) return;
    stats->dropped_packets++;
}

double stats_elapsed(const pktsnare_stats_t *stats) {
    if (!stats) return 0.0;
    return difftime(time(NULL), stats->start_time);
}

void stats_print(const pktsnare_stats_t *stats) {
    if (!stats) return;

    double elapsed = stats_elapsed(stats);
    double pps = (elapsed > 0.0)
        ? (double)stats->total_packets / elapsed
        : 0.0;

    printf("\n--- pktsnare capture statistics ---\n");
    printf("  Duration      : %.2f seconds\n", elapsed);
    printf("  Total packets : %llu\n",  (unsigned long long)stats->total_packets);
    printf("  Total bytes   : %llu\n",  (unsigned long long)stats->total_bytes);
    printf("  Packets/sec   : %.2f\n",  pps);
    printf("  TCP           : %llu\n",  (unsigned long long)stats->tcp_packets);
    printf("  UDP           : %llu\n",  (unsigned long long)stats->udp_packets);
    printf("  ICMP          : %llu\n",  (unsigned long long)stats->icmp_packets);
    printf("  ARP           : %llu\n",  (unsigned long long)stats->arp_packets);
    printf("  Other         : %llu\n",  (unsigned long long)stats->other_packets);
    printf("  Dropped       : %llu\n",  (unsigned long long)stats->dropped_packets);
    printf("-----------------------------------\n");
}
