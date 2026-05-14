#ifndef MACTRACK_LOG_H
#define MACTRACK_LOG_H

#include "mactrack.h"

void mactrack_log_new(const mactrack_entry_t *e);
void mactrack_log_expired(const mactrack_entry_t *e);

#endif /* MACTRACK_LOG_H */
