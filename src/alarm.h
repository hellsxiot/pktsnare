#ifndef PKTSNARE_ALARM_H
#define PKTSNARE_ALARM_H

#include <stdint.h>

/* Threshold-based packet rate alarm system */

typedef enum {
    ALARM_LEVEL_INFO = 0,
    ALARM_LEVEL_WARN,
    ALARM_LEVEL_CRIT
} alarm_level_t;

typedef struct {
    const char   *name;
    uint64_t      threshold;   /* packets per second */
    alarm_level_t level;
    int           triggered;   /* 1 if currently active */
    uint64_t      trigger_count;
} alarm_rule_t;

typedef struct {
    alarm_rule_t *rules;
    int           count;
    int           capacity;
    void        (*callback)(const alarm_rule_t *rule, uint64_t current_pps);
} alarm_ctx_t;

int  alarm_init(alarm_ctx_t *ctx, int capacity);
void alarm_free(alarm_ctx_t *ctx);
int  alarm_add_rule(alarm_ctx_t *ctx, const char *name,
                    uint64_t threshold, alarm_level_t level);
void alarm_evaluate(alarm_ctx_t *ctx, uint64_t current_pps);
void alarm_set_callback(alarm_ctx_t *ctx,
                        void (*cb)(const alarm_rule_t *, uint64_t));
int  alarm_any_triggered(const alarm_ctx_t *ctx);
const char *alarm_level_str(alarm_level_t level);

#endif /* PKTSNARE_ALARM_H */
