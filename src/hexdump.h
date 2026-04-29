#ifndef PKTSNARE_HEXDUMP_H
#define PKTSNARE_HEXDUMP_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/*
 * hexdump_print - write formatted hex dump to a FILE stream.
 *
 * @out    : destination stream (e.g. stdout, stderr, or a log file)
 * @data   : raw bytes to dump
 * @len    : number of bytes
 * @offset : base address printed in the left-hand column
 */
void hexdump_print(FILE *out, const uint8_t *data, size_t len, size_t offset);

/*
 * hexdump_buf - render hex bytes into a caller-supplied string buffer.
 *
 * Produces a space-separated hex string with newlines every 16 bytes.
 * The result is always NUL-terminated.
 *
 * @data   : raw bytes to encode
 * @len    : number of bytes
 * @out_buf: destination character buffer
 * @out_sz : size of out_buf in bytes
 */
void hexdump_buf(const uint8_t *data, size_t len, char *out_buf, size_t out_sz);

#endif /* PKTSNARE_HEXDUMP_H */
