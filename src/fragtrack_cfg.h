#ifndef FRAGTRACK_CFG_H
#define FRAGTRACK_CFG_H

/* Maximum number of concurrent fragment reassembly sessions.
 * Increase on high-traffic targets, lower on memory-constrained devices. */
#define FRAGTRACK_CFG_MAX_ENTRIES  64

/* Seconds before an incomplete fragment set is considered stale and evicted. */
#define FRAGTRACK_CFG_TIMEOUT_SEC  30

/* Maximum individual fragments tracked per IP datagram. */
#define FRAGTRACK_CFG_MAX_FRAGS    16

/* Enable verbose logging of fragment events (0 = off, 1 = on). */
#define FRAGTRACK_CFG_VERBOSE      0

#endif /* FRAGTRACK_CFG_H */
