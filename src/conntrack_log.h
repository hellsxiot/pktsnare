#ifndef CONNTRACK_LOG_H
#define CONNTRACK_LOG_H

#include <stdio.h>
#include "conntrack.h"

void conntrack_log_entry(FILE *fp, const conn_entry_t *e);
void conntrack_log_expired(FILE *fp, const conn_entry_t *e);

#endif /* CONNTRACK_LOG_H */
