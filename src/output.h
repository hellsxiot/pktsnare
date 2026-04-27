#ifndef OUTPUT_H
#define OUTPUT_H

#include "dissect.h"
#include <stdio.h>

/**
 * Supported output formats for packet display.
 */
typedef enum {
    OUTPUT_TEXT = 0,  /* Human-readable one-liner per packet */
    OUTPUT_JSON = 1   /* Newline-delimited JSON records       */
} OutputFormat;

/**
 * Initialize the output subsystem.
 * @param fmt      Desired output format.
 * @param filepath Path to output file, or NULL for stdout.
 */
void output_init(OutputFormat fmt, const char *filepath);

/**
 * Flush and close the output file (if not stdout).
 */
void output_close(void);

/**
 * Write a dissected packet to the configured output.
 * @param pkt  Pointer to a fully populated DissectedPacket.
 */
void output_write(const DissectedPacket *pkt);

/**
 * Change the output format at runtime (before the next write).
 * @param fmt  New format to use.
 */
void output_set_format(OutputFormat fmt);

#endif /* OUTPUT_H */
