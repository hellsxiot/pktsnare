#ifndef PROMISC_H
#define PROMISC_H

#include <stdbool.h>

/*
 * promisc.h - Promiscuous mode management for network interfaces
 *
 * Enables/disables promiscuous mode on a given interface and
 * tracks whether we toggled it so we can restore state on exit.
 */

typedef struct {
    char iface[64];
    bool was_promisc;   /* original state before we touched it */
    bool enabled;       /* current state managed by us */
    int  sockfd;        /* raw socket used for ioctl */
} promisc_ctx_t;

/* Initialize context for the given interface name.
 * Returns 0 on success, -1 on error (sets errno). */
int promisc_init(promisc_ctx_t *ctx, const char *iface);

/* Enable promiscuous mode. No-op if already enabled.
 * Returns 0 on success, -1 on error. */
int promisc_enable(promisc_ctx_t *ctx);

/* Disable promiscuous mode. Restores original state if it was
 * not promiscuous before promisc_init was called.
 * Returns 0 on success, -1 on error. */
int promisc_disable(promisc_ctx_t *ctx);

/* Query current kernel promisc state for the interface.
 * Returns 1 if promisc, 0 if not, -1 on error. */
int promisc_query(const char *iface);

/* Release resources held by ctx (closes sockfd). */
void promisc_free(promisc_ctx_t *ctx);

#endif /* PROMISC_H */
