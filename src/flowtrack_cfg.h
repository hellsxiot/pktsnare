#ifndef FLOWTRACK_CFG_H
#define FLOWTRACK_CFG_H

/* Default idle timeout before a flow is considered expired (seconds) */
#define FLOW_DEFAULT_TIMEOUT_SEC  60

/* Maximum simultaneous active flows (must be <= FLOW_TABLE_SIZE) */
#define FLOW_MAX_ACTIVE           128

/* Enable periodic flow expiry during capture loop */
#define FLOW_EXPIRY_INTERVAL_SEC  10

/* Print flow table to stdout on expiry if set to 1 */
#define FLOW_DUMP_ON_EXPIRY       0

#endif /* FLOWTRACK_CFG_H */
