#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/dnstrack.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_init(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    assert(ctx.count == 0);
    assert(ctx.ttl_seconds == 30);
    assert(ctx.total_queries == 0);
    assert(ctx.total_responses == 0);
    printf("  [PASS] test_init\n");
}

static void test_add_query(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    int r = dnstrack_add_query(&ctx, 0x1234, "example.com", 1, 0xC0A80001);
    assert(r == 0);
    assert(ctx.count == 1);
    assert(ctx.total_queries == 1);
    assert(strcmp(ctx.entries[0].qname, "example.com") == 0);
    assert(ctx.entries[0].txid == 0x1234);
    printf("  [PASS] test_add_query\n");
}

static void test_match_response(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    dnstrack_add_query(&ctx, 0xABCD, "test.local", 1, 0x0A000001);
    int r = dnstrack_match_response(&ctx, 0xABCD, 0x08080808);
    assert(r == 0);
    assert(ctx.entries[0].answered == 1);
    assert(ctx.entries[0].resolved_ip == 0x08080808);
    assert(ctx.total_responses == 1);
    printf("  [PASS] test_match_response\n");
}

static void test_match_response_no_entry(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    int r = dnstrack_match_response(&ctx, 0xDEAD, 0x01010101);
    assert(r == -1);
    assert(ctx.total_responses == 0);
    printf("  [PASS] test_match_response_no_entry\n");
}

static void test_lookup(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    dnstrack_add_query(&ctx, 0x0001, "lookup.test", 28, 0x7F000001);
    dns_entry_t out;
    int r = dnstrack_lookup(&ctx, 0x0001, &out);
    assert(r == 0);
    assert(out.txid == 0x0001);
    assert(strcmp(out.qname, "lookup.test") == 0);
    assert(out.qtype == 28);
    printf("  [PASS] test_lookup\n");
}

static void test_lookup_missing(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    dns_entry_t out;
    int r = dnstrack_lookup(&ctx, 0xFFFF, &out);
    assert(r == -1);
    printf("  [PASS] test_lookup_missing\n");
}

static void test_reset(void)
{
    dnstrack_ctx_t ctx;
    dnstrack_init(&ctx, 30);
    dnstrack_add_query(&ctx, 0x0010, "reset.test", 1, 0);
    dnstrack_reset(&ctx);
    assert(ctx.count == 0);
    assert(ctx.total_queries == 0);
    printf("  [PASS] test_reset\n");
}

int main(void)
{
    printf("=== test_dnstrack ===\n");
    RUN_TEST(test_init);
    RUN_TEST(test_add_query);
    RUN_TEST(test_match_response);
    RUN_TEST(test_match_response_no_entry);
    RUN_TEST(test_lookup);
    RUN_TEST(test_lookup_missing);
    RUN_TEST(test_reset);
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
