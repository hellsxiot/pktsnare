#ifndef PORTMAP_CFG_H
#define PORTMAP_CFG_H

/*
 * portmap_cfg.h — compile-time tunables for the portmap module.
 */

/* Maximum number of entries in a portmap table (must be >= 14 for defaults). */
#ifndef PORTMAP_MAX_ENTRIES
#define PORTMAP_MAX_ENTRIES 64
#endif

/* Maximum length of a service name string (including NUL terminator). */
#ifndef PORTMAP_NAME_LEN
#define PORTMAP_NAME_LEN 16
#endif

/*
 * Set to 1 to pre-populate the table with well-known port assignments
 * on portmap_init(). Set to 0 to start with an empty table.
 */
#ifndef PORTMAP_LOAD_DEFAULTS
#define PORTMAP_LOAD_DEFAULTS 1
#endif

#endif /* PORTMAP_CFG_H */
