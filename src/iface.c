#include "iface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <errno.h>

int iface_exists(const char *name) {
    if (!name || strlen(name) == 0)
        return 0;
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
    int ret = ioctl(sock, SIOCGIFINDEX, &ifr);
    close(sock);
    return (ret == 0) ? 1 : 0;
}

int iface_is_up(const char *name) {
    if (!name) return 0;
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
    int ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
    close(sock);
    if (ret < 0) return 0;
    return (ifr.ifr_flags & IFF_UP) ? 1 : 0;
}

int iface_get_mtu(const char *name) {
    if (!name) return -1;
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
    int ret = ioctl(sock, SIOCGIFMTU, &ifr);
    close(sock);
    if (ret < 0) return -1;
    return ifr.ifr_mtu;
}

int iface_list(char out[][IFNAMSIZ], int max_count) {
    struct if_nameindex *ifaces = if_nameindex();
    if (!ifaces) return 0;
    int count = 0;
    for (struct if_nameindex *p = ifaces; p->if_index != 0 && count < max_count; p++) {
        strncpy(out[count], p->if_name, IFNAMSIZ - 1);
        out[count][IFNAMSIZ - 1] = '\0';
        count++;
    }
    if_freenameindex(ifaces);
    return count;
}

void iface_print_info(const char *name) {
    if (!name) return;
    printf("iface: %s  up=%d  mtu=%d\n",
        name, iface_is_up(name), iface_get_mtu(name));
}
