#include "alarm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int alarm_init(alarm_ctx_t *ctx, int capacity)
{
    if (!ctx || capacity <= 0)
        return -1;
    ctx->rules = calloc(capacity, sizeof(alarm_rule_t));
    if (!ctx->rules)
        return -1;
    ctx->count    = 0;
    ctx->capacity = capacity;
    ctx->callback = NULL;
    return 0;
}

void alarm_free(alarm_ctx_t *ctx)
{
    if (!ctx)
        return;
    free(ctx->rules);
    ctx->rules    = NULL;
    ctx->count    = 0;
    ctx->capacity = 0;
}

int alarm_add_rule(alarm_ctx_t *ctx, const char *name,
                   uint64_t threshold, alarm_level_t level)
{
    if (!ctx || !name || ctx->count >= ctx->capacity)
        return -1;
    alarm_rule_t *r = &ctx->rules[ctx->count];
    r->name          = name;
    r->threshold     = threshold;
    r->level         = level;
    r->triggered     = 0;
    r->trigger_count = 0;
    ctx->count++;
    return 0;
}

void alarm_set_callback(alarm_ctx_t *ctx,
                        void (*cb)(const alarm_rule_t *, uint64_t))
{
    if (ctx)
        ctx->callback = cb;
}

void alarm_evaluate(alarm_ctx_t *ctx, uint64_t current_pps)
{
    if (!ctx)
        return;
    for (int i = 0; i < ctx->count; i++) {
        alarm_rule_t *r = &ctx->rules[i];
        int was_triggered = r->triggered;
        r->triggered = (current_pps >= r->threshold) ? 1 : 0;
        if (r->triggered && !was_triggered) {
            r->trigger_count++;
            if (ctx->callback)
                ctx->callback(r, current_pps);
        }
    }
}

int alarm_any_triggered(const alarm_ctx_t *ctx)
{
    if (!ctx)
        return 0;
    for (int i = 0; i < ctx->count; i++)
        if (ctx->rules[i].triggered)
            return 1;
    return 0;
}

const char *alarm_level_str(alarm_level_t level)
{
    switch (level) {
    case ALARM_LEVEL_INFO: return "INFO";
    case ALARM_LEVEL_WARN: return "WARN";
    case ALARM_LEVEL_CRIT: return "CRIT";
    default:               return "UNKNOWN";
    }
}
