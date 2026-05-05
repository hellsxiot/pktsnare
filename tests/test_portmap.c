#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>

#include "../src/portmap.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS: %s\n", msg); passed++; } \
    else      { printf("  FAIL: %s\n", msg); failed++; } \
} while (0)

static void test_init(void)
{
    portmap_t pm;
    printf("[test_init]\n");
    CHECK(portmap_init(&pm) == 0,       "init returns 0");
    CHECK(pm.count > 0,                 "defaults loaded");
    CHECK(portmap_init(NULL) == -1,     "null ptr returns -1");
}

static void test_lookup_defaults(void)
{
    portmap_t pm;
    portmap_init(&pm);
    printf("[test_lookup_defaults]\n");
    const char *n = portmap_lookup(&pm, 80, IPPROTO_TCP);
    CHECK(n != NULL,                    "port 80 found");
    CHECK(strcmp(n, "http") == 0,       "port 80 is http");
    CHECK(portmap_lookup(&pm, 53, IPPROTO_UDP) != NULL, "dns udp found");
    CHECK(portmap_lookup(&pm, 9999, IPPROTO_TCP) == NULL, "unknown returns NULL");
    CHECK(portmap_lookup(NULL, 80, IPPROTO_TCP) == NULL,  "null pm returns NULL");
}

static void test_register(void)
{
    portmap_t pm;
    portmap_init(&pm);
    printf("[test_register]\n");
    CHECK(portmap_register(&pm, 9200, IPPROTO_TCP, "elasticsearch") == 0,
          "register new port");
    const char *n = portmap_lookup(&pm, 9200, IPPROTO_TCP);
    CHECK(n != NULL && strcmp(n, "elasticsearch") == 0, "custom port found");
    /* overwrite existing */
    CHECK(portmap_register(&pm, 80, IPPROTO_TCP, "my-http") == 0,
          "overwrite existing");
    n = portmap_lookup(&pm, 80, IPPROTO_TCP);
    CHECK(n != NULL && strcmp(n, "my-http") == 0, "overwrite took effect");
    CHECK(portmap_register(NULL, 1, IPPROTO_TCP, "x") == -1, "null pm fails");
    CHECK(portmap_register(&pm, 1, IPPROTO_TCP, NULL) == -1, "null name fails");
}

static void test_remove(void)
{
    portmap_t pm;
    portmap_init(&pm);
    printf("[test_remove]\n");
    CHECK(portmap_remove(&pm, 22, IPPROTO_TCP) == 0,    "remove ssh ok");
    CHECK(portmap_lookup(&pm, 22, IPPROTO_TCP) == NULL, "ssh gone after remove");
    CHECK(portmap_remove(&pm, 22, IPPROTO_TCP) == -1,   "double remove fails");
    CHECK(portmap_remove(NULL, 80, IPPROTO_TCP) == -1,  "null pm fails");
}

static void test_clear(void)
{
    portmap_t pm;
    portmap_init(&pm);
    printf("[test_clear]\n");
    portmap_clear(&pm);
    CHECK(pm.count == 0,                                "count zero after clear");
    CHECK(portmap_lookup(&pm, 80, IPPROTO_TCP) == NULL, "no entries after clear");
}

int main(void)
{
    printf("=== test_portmap ===\n");
    test_init();
    test_lookup_defaults();
    test_register();
    test_remove();
    test_clear();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
