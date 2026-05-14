#ifndef DNSTRACK_LOG_H
#define DNSTRACK_LOG_H

#include "dnstrack.h"

void dnstrack_log_query(const dns_entry_t *e);
void dnstrack_log_response(const dns_entry_t *e);

#endif /* DNSTRACK_LOG_H */
