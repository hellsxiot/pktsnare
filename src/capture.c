#include "capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

int capture_open(capture_ctx_t *ctx, const char *iface, int promiscuous) {
    struct ifreq        ifr;
    struct packet_mreq  mreq;
    struct sockaddr_ll  sa;

    memset(ctx, 0, sizeof(*ctx));
    strncpy(ctx->iface, iface, IFACE_NAME_MAX - 1);
    ctx->snaplen     = CAPTURE_SNAPLEN;
    ctx->promiscuous = promiscuous;

    ctx->sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ctx->sockfd < 0) {
        perror("socket");
        return -1;
    }

    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(ctx->sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX");
        close(ctx->sockfd);
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sll_family   = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_ALL);
    sa.sll_ifindex  = ifr.ifr_ifindex;
    if (bind(ctx->sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind");
        close(ctx->sockfd);
        return -1;
    }

    if (promiscuous) {
        memset(&mreq, 0, sizeof(mreq));
        mreq.mr_ifindex = ifr.ifr_ifindex;
        mreq.mr_type    = PACKET_MR_PROMISC;
        if (setsockopt(ctx->sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
                       &mreq, sizeof(mreq)) < 0) {
            perror("setsockopt PROMISC");
        }
    }

    return 0;
}

void capture_close(capture_ctx_t *ctx) {
    if (ctx->sockfd >= 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
    }
}

int capture_loop(capture_ctx_t *ctx, packet_handler_t handler, void *user, volatile int *stop_flag) {
    uint8_t     buf[CAPTURE_SNAPLEN];
    raw_packet_t pkt;
    ssize_t      n;

    while (!(*stop_flag)) {
        n = recv(ctx->sockfd, buf, sizeof(buf), 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("recv");
            return -1;
        }

        gettimeofday(&pkt.ts, NULL);
        pkt.data   = buf;
        pkt.caplen = (uint32_t)(n < ctx->snaplen ? n : ctx->snaplen);
        pkt.len    = (uint32_t)n;

        ctx->pkt_count++;
        ctx->byte_count += pkt.len;

        handler(&pkt, user);
    }
    return 0;
}

void capture_stats(const capture_ctx_t *ctx) {
    printf("[stats] iface=%s packets=%lu bytes=%lu\n",
           ctx->iface, ctx->pkt_count, ctx->byte_count);
}
