#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "../src/promisc.h"

/* Use loopback for tests — promisc on lo is harmless and always available */
#define TEST_IFACE "lo"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN(name, expr) do { \
    tests_run++; \
    if (expr) { tests_passed++; printf("  PASS  %s\n", name); } \
    else { printf("  FAIL  %s  (line %d)\n", name, __LINE__); } \
} while (0)

static void test_init_free(void)
{
    promisc_ctx_t ctx;
    int r = promisc_init(&ctx, TEST_IFACE);
    if (r < 0) {
        printf("  SKIP  init (need raw socket capability)\n");
        return;
    }
    RUN("init sets iface", strcmp(ctx.iface, TEST_IFACE) == 0);
    RUN("init opens sockfd", ctx.sockfd >= 0);
    promisc_free(&ctx);
    RUN("free closes sockfd", ctx.sockfd == -1);
}

static void test_query(void)
{
    int r = promisc_query(TEST_IFACE);
    /* lo may or may not be promisc; just check we get a valid answer */
    RUN("query returns 0 or 1", r == 0 || r == 1);
}

static void test_query_bad_iface(void)
{
    int r = promisc_query("__no_such_iface__");
    RUN("query bad iface returns -1", r == -1);
}

static void test_enable_disable(void)
{
    promisc_ctx_t ctx;
    if (promisc_init(&ctx, TEST_IFACE) < 0) {
        printf("  SKIP  enable/disable (need capability)\n");
        return;
    }

    /* Enable — may fail if we lack CAP_NET_ADMIN; that's acceptable */
    int en = promisc_enable(&ctx);
    if (en == 0) {
        RUN("enable sets ctx.enabled", ctx.enabled == true);
        /* Calling enable again should be a no-op */
        RUN("double enable ok", promisc_enable(&ctx) == 0);

        int dis = promisc_disable(&ctx);
        RUN("disable succeeds", dis == 0);
        /* If we weren't promisc before, enabled should now be false */
        if (!ctx.was_promisc)
            RUN("disable clears ctx.enabled", ctx.enabled == false);
    } else {
        printf("  SKIP  enable/disable (CAP_NET_ADMIN not granted)\n");
    }

    promisc_free(&ctx);
}

static void test_iface_name_truncation(void)
{
    promisc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    /* Provide a name longer than the buffer — should not overflow */
    const char *long_name =
        "averylonginterfacenamethatexceedssixtyfourbyteseasilyxxxxxxxxxxxxxxxxxx";
    promisc_init(&ctx, long_name);
    RUN("iface name fits in buffer", ctx.iface[63] == '\0');
    if (ctx.sockfd >= 0)
        promisc_free(&ctx);
}

int main(void)
{
    printf("=== test_promisc ===\n");
    test_init_free();
    test_query();
    test_query_bad_iface();
    test_enable_disable();
    test_iface_name_truncation();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
