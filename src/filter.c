/*
 * filter.c - Packet filtering implementation for pktsnare
 *
 * Supports simple BPF-style filter expressions for protocol,
 * port, and host-based filtering on captured packets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>

#include "filter.h"
#include "dissect.h"

/* Initialize a filter context with default (pass-all) settings */
int filter_init(struct filter_ctx *ctx)
{
    if (!ctx)
        return -1;

    memset(ctx, 0, sizeof(*ctx));
    ctx->proto     = FILTER_PROTO_ANY;
    ctx->src_port  = 0;
    ctx->dst_port  = 0;
    ctx->src_ip    = 0;
    ctx->dst_ip    = 0;
    ctx->flags     = FILTER_FLAG_NONE;

    return 0;
}

/*
 * Parse a simple filter string into a filter_ctx.
 * Supported tokens:
 *   tcp | udp | icmp
 *   port <num>
 *   src port <num>
 *   dst port <num>
 *   host <ip>
 *   src host <ip>
 *   dst host <ip>
 */
int filter_parse(struct filter_ctx *ctx, const char *expr)
{
    if (!ctx || !expr)
        return -1;

    char buf[256];
    strncpy(buf, expr, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok = strtok(buf, " \t");
    while (tok) {
        if (strcmp(tok, "tcp") == 0) {
            ctx->proto = FILTER_PROTO_TCP;
        } else if (strcmp(tok, "udp") == 0) {
            ctx->proto = FILTER_PROTO_UDP;
        } else if (strcmp(tok, "icmp") == 0) {
            ctx->proto = FILTER_PROTO_ICMP;
        } else if (strcmp(tok, "port") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) return -1;
            int p = atoi(tok);
            ctx->src_port = (uint16_t)p;
            ctx->dst_port = (uint16_t)p;
            ctx->flags |= FILTER_FLAG_PORT;
        } else if (strcmp(tok, "src") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) return -1;
            if (strcmp(tok, "port") == 0) {
                tok = strtok(NULL, " \t");
                if (!tok) return -1;
                ctx->src_port = (uint16_t)atoi(tok);
                ctx->flags |= FILTER_FLAG_SRC_PORT;
            } else if (strcmp(tok, "host") == 0) {
                tok = strtok(NULL, " \t");
                if (!tok) return -1;
                ctx->src_ip = inet_addr(tok);
                ctx->flags |= FILTER_FLAG_SRC_HOST;
            }
        } else if (strcmp(tok, "dst") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) return -1;
            if (strcmp(tok, "port") == 0) {
                tok = strtok(NULL, " \t");
                if (!tok) return -1;
                ctx->dst_port = (uint16_t)atoi(tok);
                ctx->flags |= FILTER_FLAG_DST_PORT;
            } else if (strcmp(tok, "host") == 0) {
                tok = strtok(NULL, " \t");
                if (!tok) return -1;
                ctx->dst_ip = inet_addr(tok);
                ctx->flags |= FILTER_FLAG_DST_HOST;
            }
        } else if (strcmp(tok, "host") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) return -1;
            in_addr_t addr = inet_addr(tok);
            ctx->src_ip = addr;
            ctx->dst_ip = addr;
            ctx->flags |= FILTER_FLAG_SRC_HOST | FILTER_FLAG_DST_HOST;
        }
        tok = strtok(NULL, " \t");
    }

    return 0;
}

/*
 * Apply the filter to a dissected packet.
 * Returns 1 if the packet matches (should be kept), 0 otherwise.
 */
int filter_match(const struct filter_ctx *ctx, const struct dissected_pkt *pkt)
{
    if (!ctx || !pkt)
        return 0;

    /* Protocol check */
    if (ctx->proto != FILTER_PROTO_ANY && ctx->proto != pkt->proto)
        return 0;

    /* Source host check */
    if ((ctx->flags & FILTER_FLAG_SRC_HOST) &&
        ctx->src_ip != 0 && ctx->src_ip != pkt->src_ip)
        return 0;

    /* Destination host check */
    if ((ctx->flags & FILTER_FLAG_DST_HOST) &&
        ctx->dst_ip != 0 && ctx->dst_ip != pkt->dst_ip)
        return 0;

    /* Bidirectional port check */
    if (ctx->flags & FILTER_FLAG_PORT) {
        if (pkt->src_port != ctx->src_port && pkt->dst_port != ctx->dst_port)
            return 0;
    }

    /* Source port check */
    if ((ctx->flags & FILTER_FLAG_SRC_PORT) && ctx->src_port != 0)
        if (pkt->src_port != ctx->src_port)
            return 0;

    /* Destination port check */
    if ((ctx->flags & FILTER_FLAG_DST_PORT) && ctx->dst_port != 0)
        if (pkt->dst_port != ctx->dst_port)
            return 0;

    return 1;
}

/* Print a human-readable summary of the active filter */
void filter_print(const struct filter_ctx *ctx)
{
    if (!ctx) return;

    const char *proto_str[] = { "any", "tcp", "udp", "icmp" };
    fprintf(stderr, "[filter] proto=%s", proto_str[ctx->proto]);

    if (ctx->flags & FILTER_FLAG_SRC_HOST) {
        struct in_addr a = { .s_addr = ctx->src_ip };
        fprintf(stderr, " src_host=%s", inet_ntoa(a));
    }
    if (ctx->flags & FILTER_FLAG_DST_HOST) {
        struct in_addr a = { .s_addr = ctx->dst_ip };
        fprintf(stderr, " dst_host=%s", inet_ntoa(a));
    }
    if (ctx->flags & FILTER_FLAG_SRC_PORT)
        fprintf(stderr, " src_port=%u", ctx->src_port);
    if (ctx->flags & FILTER_FLAG_DST_PORT)
        fprintf(stderr, " dst_port=%u", ctx->dst_port);
    if (ctx->flags & FILTER_FLAG_PORT)
        fprintf(stderr, " port=%u", ctx->src_port);

    fprintf(stderr, "\n");
}
