/*
 * output.h - Output formatting and display for pktsnare
 *
 * Handles printing captured/dissected packets to stdout or a file
 * in various formats (plain text, hex dump, JSON-like).
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdint.h>
#include "dissect.h"

/* Output format modes */
typedef enum {
    OUTPUT_FMT_PLAIN  = 0,  /* Human-readable one-liner per packet */
    OUTPUT_FMT_HEX    = 1,  /* Full hex dump with ASCII sidebar */
    OUTPUT_FMT_JSON   = 2,  /* Minimal JSON object per packet */
} output_fmt_t;

/* Output configuration */
typedef struct {
    output_fmt_t  format;     /* Selected output format */
    FILE         *dest;       /* Output destination (stdout or file) */
    int           show_ts;    /* Whether to print timestamps */
    int           verbose;    /* Extra detail (TTL, checksum, etc.) */
    uint64_t      pkt_count;  /* Running packet counter */
} output_cfg_t;

/*
 * output_init - Initialise an output_cfg_t with sensible defaults.
 *
 * @cfg    : pointer to config struct to initialise
 * @fmt    : desired output format
 * @dest   : FILE* to write to (pass NULL to default to stdout)
 * @verbose: non-zero to enable verbose field output
 */
void output_init(output_cfg_t *cfg, output_fmt_t fmt, FILE *dest, int verbose);

/*
 * output_packet - Print a dissected packet according to cfg.
 *
 * @cfg  : output configuration
 * @pkt  : dissected packet to display
 */
void output_packet(output_cfg_t *cfg, const dissected_pkt_t *pkt);

/*
 * output_hex_dump - Write a raw hex + ASCII dump of a buffer.
 *
 * @dest : FILE* to write to
 * @buf  : raw bytes
 * @len  : number of bytes
 */
void output_hex_dump(FILE *dest, const uint8_t *buf, uint16_t len);

/*
 * output_stats - Print a brief capture-session summary line.
 *
 * @cfg       : output configuration (uses pkt_count)
 * @dropped   : packets dropped by the kernel ring buffer
 */
void output_stats(const output_cfg_t *cfg, uint32_t dropped);

#endif /* OUTPUT_H */
