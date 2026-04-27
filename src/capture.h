#ifndef CAPTURE_H
#define CAPTURE_H

#include <stdint.h>
#include <sys/time.h>

#define CAPTURE_SNAPLEN     65535
#define CAPTURE_TIMEOUT_MS  1000
#define IFACE_NAME_MAX      16

typedef struct {
    char        iface[IFACE_NAME_MAX];
    int         sockfd;
    int         promiscuous;
    uint32_t    snaplen;
    uint64_t    pkt_count;
    uint64_t    byte_count;
} capture_ctx_t;

typedef struct {
    uint8_t        *data;
    uint32_t        len;
    uint32_t        caplen;
    struct timeval  ts;
} raw_packet_t;

typedef void (*packet_handler_t)(const raw_packet_t *pkt, void *user);

/*
 * capture_open  - open a raw socket on `iface` for packet capture.
 *                 if promiscuous != 0, the interface is put into
 *                 promiscuous mode.  Returns 0 on success, -1 on error.
 *
 * capture_close - release resources and restore interface flags.
 *
 * capture_loop  - read packets in a loop, calling `handler` for each one.
 *                 returns when *stop_flag becomes non-zero, or on error.
 *                 Returns 0 on clean stop, -1 on error.
 *
 * capture_stats - print packet/byte counters for the given context to stdout.
 *
 * capture_reset_stats - zero out the packet and byte counters in `ctx`.
 */
int  capture_open(capture_ctx_t *ctx, const char *iface, int promiscuous);
void capture_close(capture_ctx_t *ctx);
int  capture_loop(capture_ctx_t *ctx, packet_handler_t handler, void *user, volatile int *stop_flag);
void capture_stats(const capture_ctx_t *ctx);
void capture_reset_stats(capture_ctx_t *ctx);

#endif /* CAPTURE_H */
