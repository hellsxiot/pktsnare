#ifndef MACTRACK_CFG_H
#define MACTRACK_CFG_H

/* Default idle timeout before an entry is expired (seconds) */
#define MACTRACK_DEFAULT_EXPIRY_SEC  300

/* Warn when table utilisation exceeds this fraction (0.0 - 1.0) */
#define MACTRACK_WARN_THRESHOLD      0.85

/* Enable per-entry byte accounting (set 0 to save a few bytes) */
#define MACTRACK_BYTE_ACCOUNTING     1

#endif /* MACTRACK_CFG_H */
