/*
 * pktsnare - Lightweight packet capture and protocol dissection CLI
 * Main entry point: argument parsing and capture loop orchestration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

#include "capture.h"
#include "dissect.h"

#define DEFAULT_SNAPLEN   1514
#define DEFAULT_COUNT     0      /* 0 = unlimited */
#define DEFAULT_IFACE     "eth0"

static volatile int running = 1;

static void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "  -i <iface>    Network interface to capture on (default: %s)\n"
        "  -c <count>    Stop after capturing N packets (default: unlimited)\n"
        "  -s <snaplen>  Snapshot length in bytes (default: %d)\n"
        "  -f <filter>   BPF filter expression\n"
        "  -v            Verbose dissection output\n"
        "  -h            Show this help message\n",
        prog, DEFAULT_IFACE, DEFAULT_SNAPLEN);
}

static void packet_handler(const uint8_t *data, uint32_t len,
                           uint64_t ts_usec, void *user) {
    dissect_opts_t *opts = (dissect_opts_t *)user;

    printf("[%lu.%06lu] len=%u\n",
           (unsigned long)(ts_usec / 1000000),
           (unsigned long)(ts_usec % 1000000),
           len);

    dissect_packet(data, len, opts);
    putchar('\n');
}

int main(int argc, char *argv[]) {
    const char *iface  = DEFAULT_IFACE;
    const char *filter = NULL;
    int         count    = DEFAULT_COUNT;
    int         snaplen  = DEFAULT_SNAPLEN;
    int         verbose  = 0;
    int         opt;

    while ((opt = getopt(argc, argv, "i:c:s:f:vh")) != -1) {
        switch (opt) {
        case 'i': iface   = optarg;          break;
        case 'c': count   = atoi(optarg);    break;
        case 's': snaplen = atoi(optarg);    break;
        case 'f': filter  = optarg;          break;
        case 'v': verbose = 1;               break;
        case 'h': print_usage(argv[0]);      return EXIT_SUCCESS;
        default:  print_usage(argv[0]);      return EXIT_FAILURE;
        }
    }

    if (snaplen < 64 || snaplen > 65535) {
        fprintf(stderr, "error: snaplen must be between 64 and 65535\n");
        return EXIT_FAILURE;
    }

    /* Set up dissection options */
    dissect_opts_t dopts = {
        .verbose  = verbose,
        .link_type = LINKTYPE_ETHERNET,   /* assume Ethernet for now */
    };

    /* Open capture handle */
    capture_handle_t *cap = capture_open(iface, snaplen, filter);
    if (!cap) {
        fprintf(stderr, "error: failed to open capture on %s\n", iface);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Capturing on %s (snaplen=%d)%s%s ...\n",
            iface, snaplen,
            filter ? ", filter: " : "",
            filter ? filter       : "");

    signal(SIGINT,  handle_sigint);
    signal(SIGTERM, handle_sigint);

    int captured = 0;
    while (running && (count == 0 || captured < count)) {
        int ret = capture_next(cap, packet_handler, &dopts);
        if (ret < 0) {
            fprintf(stderr, "error: capture_next failed\n");
            break;
        }
        if (ret > 0)
            captured++;
    }

    fprintf(stderr, "\nCaptured %d packet%s.\n",
            captured, captured == 1 ? "" : "s");

    capture_close(cap);
    return EXIT_SUCCESS;
}
