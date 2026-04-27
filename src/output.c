#include "output.h"
#include "dissect.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

static OutputFormat current_format = OUTPUT_TEXT;
static FILE *out_fp = NULL;

void output_init(OutputFormat fmt, const char *filepath) {
    current_format = fmt;
    if (filepath) {
        out_fp = fopen(filepath, "w");
        if (!out_fp) {
            perror("output_init: fopen");
            out_fp = stdout;
        }
    } else {
        out_fp = stdout;
    }
}

void output_close(void) {
    if (out_fp && out_fp != stdout) {
        fclose(out_fp);
        out_fp = NULL;
    }
}

static void print_text(const DissectedPacket *pkt) {
    char ts[32];
    struct tm *tm_info = localtime(&pkt->timestamp.tv_sec);
    strftime(ts, sizeof(ts), "%H:%M:%S", tm_info);

    fprintf(out_fp, "[%s.%06ld] %s", ts, pkt->timestamp.tv_usec, pkt->proto_str);

    if (pkt->has_ip) {
        char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &pkt->src_ip, src, sizeof(src));
        inet_ntop(AF_INET, &pkt->dst_ip, dst, sizeof(dst));
        fprintf(out_fp, " %s -> %s", src, dst);
    }

    if (pkt->has_transport) {
        fprintf(out_fp, " port %u -> %u", pkt->src_port, pkt->dst_port);
    }

    fprintf(out_fp, " len=%u\n", pkt->payload_len);
}

static void print_json(const DissectedPacket *pkt) {
    char src[INET_ADDRSTRLEN] = "-", dst[INET_ADDRSTRLEN] = "-";
    if (pkt->has_ip) {
        inet_ntop(AF_INET, &pkt->src_ip, src, sizeof(src));
        inet_ntop(AF_INET, &pkt->dst_ip, dst, sizeof(dst));
    }
    fprintf(out_fp,
        "{\"ts\":%ld,\"proto\":\"%s\",\"src\":\"%s\",\"dst\":\"%s\","
        "\"sport\":%u,\"dport\":%u,\"len\":%u}\n",
        pkt->timestamp.tv_sec, pkt->proto_str, src, dst,
        pkt->src_port, pkt->dst_port, pkt->payload_len);
}

void output_write(const DissectedPacket *pkt) {
    if (!pkt || !out_fp) return;
    switch (current_format) {
        case OUTPUT_JSON: print_json(pkt); break;
        case OUTPUT_TEXT: /* fall through */
        default:          print_text(pkt); break;
    }
    fflush(out_fp);
}

void output_set_format(OutputFormat fmt) {
    current_format = fmt;
}
