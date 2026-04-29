#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <netinet/in.h>
#include "../src/proto.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_icmp_identification(void) {
    proto_id_t id = proto_identify(IPPROTO_ICMP, 0, 0);
    CHECK(id == PROTO_ICMP, "ICMP identified by ip_proto");
    CHECK(strcmp(proto_name(id), "icmp") == 0, "ICMP name is 'icmp'");
}

static void test_dns_udp(void) {
    proto_id_t id = proto_identify(IPPROTO_UDP, 12345, 53);
    CHECK(id == PROTO_DNS, "DNS identified via dst port 53/udp");

    id = proto_identify(IPPROTO_UDP, 53, 12345);
    CHECK(id == PROTO_DNS, "DNS identified via src port 53/udp");
}

static void test_http_tcp(void) {
    proto_id_t id = proto_identify(IPPROTO_TCP, 54321, 80);
    CHECK(id == PROTO_HTTP, "HTTP identified via dst port 80/tcp");
}

static void test_tls_tcp(void) {
    proto_id_t id = proto_identify(IPPROTO_TCP, 54321, 443);
    CHECK(id == PROTO_TLS, "TLS identified via dst port 443/tcp");
}

static void test_generic_tcp_udp(void) {
    proto_id_t id = proto_identify(IPPROTO_TCP, 1234, 5678);
    CHECK(id == PROTO_TCP, "Unknown TCP ports fall back to PROTO_TCP");

    id = proto_identify(IPPROTO_UDP, 1234, 5678);
    CHECK(id == PROTO_UDP, "Unknown UDP ports fall back to PROTO_UDP");
}

static void test_unknown_ip_proto(void) {
    proto_id_t id = proto_identify(0xFF, 0, 0);
    CHECK(id == PROTO_UNKNOWN, "Unknown ip_proto returns PROTO_UNKNOWN");
    CHECK(strcmp(proto_name(PROTO_UNKNOWN), "unknown") == 0, "PROTO_UNKNOWN name ok");
}

static void test_proto_match_fill(void) {
    proto_match_t m;
    proto_match_fill(&m, IPPROTO_UDP, 9999, 53);
    CHECK(m.proto == PROTO_DNS,   "proto_match_fill sets proto correctly");
    CHECK(m.src_port == 9999,     "proto_match_fill sets src_port");
    CHECK(m.dst_port == 53,       "proto_match_fill sets dst_port");
    CHECK(m.hint != NULL,         "proto_match_fill sets hint");
}

static void test_proto_info(void) {
    const proto_info_t *info = proto_info(PROTO_NTP);
    CHECK(info != NULL,                      "proto_info returns non-NULL");
    CHECK(info->default_port == 123,         "NTP default port is 123");
    CHECK(strcmp(info->name, "ntp") == 0,   "NTP name matches");
}

int main(void) {
    printf("=== test_proto ===\n");
    test_icmp_identification();
    test_dns_udp();
    test_http_tcp();
    test_tls_tcp();
    test_generic_tcp_udp();
    test_unknown_ip_proto();
    test_proto_match_fill();
    test_proto_info();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
