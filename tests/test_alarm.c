#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/alarm.h"

static int callback_fired = 0;
static const char *last_rule_name = NULL;

static void test_callback(const alarm_rule_t *rule, uint64_t pps)
{
    (void)pps;
    callback_fired++;
    last_rule_name = rule->name;
}

static void test_init_free(void)
{
    alarm_ctx_t ctx;
    assert(alarm_init(&ctx, 4) == 0);
    assert(ctx.count    == 0);
    assert(ctx.capacity == 4);
    alarm_free(&ctx);
    assert(ctx.rules == NULL);
    printf("[PASS] test_init_free\n");
}

static void test_add_rule(void)
{
    alarm_ctx_t ctx;
    alarm_init(&ctx, 2);
    assert(alarm_add_rule(&ctx, "high-pps", 1000, ALARM_LEVEL_WARN) == 0);
    assert(alarm_add_rule(&ctx, "flood",    5000, ALARM_LEVEL_CRIT) == 0);
    assert(ctx.count == 2);
    /* capacity exceeded */
    assert(alarm_add_rule(&ctx, "extra", 9999, ALARM_LEVEL_INFO) == -1);
    alarm_free(&ctx);
    printf("[PASS] test_add_rule\n");
}

static void test_evaluate_triggers(void)
{
    alarm_ctx_t ctx;
    alarm_init(&ctx, 4);
    alarm_set_callback(&ctx, test_callback);
    alarm_add_rule(&ctx, "warn-rule", 500,  ALARM_LEVEL_WARN);
    alarm_add_rule(&ctx, "crit-rule", 2000, ALARM_LEVEL_CRIT);

    callback_fired  = 0;
    last_rule_name  = NULL;

    /* below all thresholds */
    alarm_evaluate(&ctx, 100);
    assert(alarm_any_triggered(&ctx) == 0);
    assert(callback_fired == 0);

    /* crosses warn threshold */
    alarm_evaluate(&ctx, 600);
    assert(ctx.rules[0].triggered == 1);
    assert(ctx.rules[1].triggered == 0);
    assert(callback_fired == 1);
    assert(strcmp(last_rule_name, "warn-rule") == 0);

    /* callback not re-fired if still triggered */
    alarm_evaluate(&ctx, 700);
    assert(callback_fired == 1);

    /* crosses crit threshold */
    alarm_evaluate(&ctx, 3000);
    assert(alarm_any_triggered(&ctx) == 1);
    assert(callback_fired == 2);

    /* drops below warn — no longer triggered */
    alarm_evaluate(&ctx, 50);
    assert(alarm_any_triggered(&ctx) == 0);

    alarm_free(&ctx);
    printf("[PASS] test_evaluate_triggers\n");
}

static void test_level_str(void)
{
    assert(strcmp(alarm_level_str(ALARM_LEVEL_INFO), "INFO") == 0);
    assert(strcmp(alarm_level_str(ALARM_LEVEL_WARN), "WARN") == 0);
    assert(strcmp(alarm_level_str(ALARM_LEVEL_CRIT), "CRIT") == 0);
    printf("[PASS] test_level_str\n");
}

int main(void)
{
    test_init_free();
    test_add_rule();
    test_evaluate_triggers();
    test_level_str();
    printf("All alarm tests passed.\n");
    return 0;
}
