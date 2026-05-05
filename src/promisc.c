#include "promisc.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

static int get_flags(int sockfd, const char *iface, short *flags)
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
        return -1;
    *flags = ifr.ifr_flags;
    return 0;
}

static int set_flags(int sockfd, const char *iface, short flags)
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    ifr.ifr_flags = flags;
    return ioctl(sockfd, SIOCSIFFLAGS, &ifr);
}

int promisc_init(promisc_ctx_t *ctx, const char *iface)
{
    memset(ctx, 0, sizeof(*ctx));
    strncpy(ctx->iface, iface, sizeof(ctx->iface) - 1);

    ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctx->sockfd < 0)
        return -1;

    int r = promisc_query(iface);
    if (r < 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
        return -1;
    }
    ctx->was_promisc = (r == 1);
    ctx->enabled = ctx->was_promisc;
    return 0;
}

int promisc_enable(promisc_ctx_t *ctx)
{
    if (ctx->enabled)
        return 0;
    short flags;
    if (get_flags(ctx->sockfd, ctx->iface, &flags) < 0)
        return -1;
    flags |= IFF_PROMISC;
    if (set_flags(ctx->sockfd, ctx->iface, flags) < 0)
        return -1;
    ctx->enabled = true;
    return 0;
}

int promisc_disable(promisc_ctx_t *ctx)
{
    if (!ctx->enabled)
        return 0;
    /* Only clear promisc if we were the ones who set it */
    if (ctx->was_promisc)
        return 0;
    short flags;
    if (get_flags(ctx->sockfd, ctx->iface, &flags) < 0)
        return -1;
    flags &= ~IFF_PROMISC;
    if (set_flags(ctx->sockfd, ctx->iface, flags) < 0)
        return -1;
    ctx->enabled = false;
    return 0;
}

int promisc_query(const char *iface)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return -1;
    short flags;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    int ret = -1;
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0)
        ret = (ifr.ifr_flags & IFF_PROMISC) ? 1 : 0;
    close(fd);
    return ret;
}

void promisc_free(promisc_ctx_t *ctx)
{
    if (ctx->sockfd >= 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
    }
}
