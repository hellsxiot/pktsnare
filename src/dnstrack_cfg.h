#ifndef DNSTRACK_CFG_H
#define DNSTRACK_CFG_H

/* Default TTL for tracked DNS entries (seconds) */
#define DNSTRACK_CFG_TTL        30

/* Max simultaneous in-flight DNS queries to track */
#define DNSTRACK_CFG_MAX        256

/* Enable verbose logging of matched responses */
#define DNSTRACK_CFG_VERBOSE    0

#endif /* DNSTRACK_CFG_H */
