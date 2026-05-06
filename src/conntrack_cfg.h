#ifndef CONNTRACK_CFG_H
#define CONNTRACK_CFG_H

/* Maximum simultaneous tracked connections */
#define CONNTRACK_CFG_MAX_ENTRIES   4096

/* Seconds before an idle connection is expired */
#define CONNTRACK_CFG_TIMEOUT_SEC   120

/* Run expiry sweep every N packets */
#define CONNTRACK_CFG_SWEEP_INTERVAL 256

/* Enable verbose dump on SIGUSR1 (1 = yes, 0 = no) */
#define CONNTRACK_CFG_DUMP_ON_SIGNAL 1

#endif /* CONNTRACK_CFG_H */
