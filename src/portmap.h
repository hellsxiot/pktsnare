#ifndef PORTMAP_H
#define PORTMAP_H

#include <stdint.h>

/* Maximum number of port-to-service mappings */
#define PORTMAP_MAX_ENTRIES 64

typedef struct {
    uint16_t port;
    uint8_t  proto;   /* IPPROTO_TCP or IPPROTO_UDP */
    char     name[16];
} portmap_entry_t;

typedef struct {
    portmap_entry_t entries[PORTMAP_MAX_ENTRIES];
    int             count;
} portmap_t;

/*
 * Initialize portmap with well-known defaults.
 * Returns 0 on success, -1 on error.
 */
int portmap_init(portmap_t *pm);

/*
 * Register a custom port -> name mapping.
 * Returns 0 on success, -1 if table is full or args invalid.
 */
int portmap_register(portmap_t *pm, uint16_t port, uint8_t proto,
                     const char *name);

/*
 * Look up the service name for a given port/proto.
 * Returns pointer to static name string, or NULL if not found.
 */
const char *portmap_lookup(const portmap_t *pm, uint16_t port, uint8_t proto);

/*
 * Remove a mapping by port and proto.
 * Returns 0 if removed, -1 if not found.
 */
int portmap_remove(portmap_t *pm, uint16_t port, uint8_t proto);

/*
 * Reset table to empty (no defaults).
 */
void portmap_clear(portmap_t *pm);

#endif /* PORTMAP_H */
