#ifndef PKTSNARE_IFACE_H
#define PKTSNARE_IFACE_H

#include <stdint.h>
#include <net/if.h>

#define IFACE_MAX_COUNT 16
#define IFACE_FLAG_UP       0x01
#define IFACE_FLAG_PROMISC  0x02
#define IFACE_FLAG_LOOPBACK 0x04

typedef struct {
    char     name[IF_NAMESIZE];
    uint32_t index;
    uint32_t flags;
    uint8_t  hwaddr[6];
    uint32_t mtu;
} iface_info_t;

typedef struct {
    iface_info_t entries[IFACE_MAX_COUNT];
    int          count;
} iface_list_t;

/* Enumerate all available network interfaces */
int iface_list(iface_list_t *out);

/* Look up a single interface by name, returns 0 on success */
int iface_get(const char *name, iface_info_t *out);

/* Set or clear promiscuous mode on an interface */
int iface_set_promisc(const char *name, int enable);

/* Check whether interface is up and usable */
int iface_is_up(const char *name);

/* Print interface info to stdout (human-readable) */
void iface_print(const iface_info_t *info);

#endif /* PKTSNARE_IFACE_H */
