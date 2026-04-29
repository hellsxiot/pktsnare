#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "hexdump.h"

#define BYTES_PER_ROW 16

void hexdump_print(FILE *out, const uint8_t *data, size_t len, size_t offset)
{
    if (!out || !data || len == 0)
        return;

    for (size_t i = 0; i < len; i += BYTES_PER_ROW) {
        fprintf(out, "%08zx  ", offset + i);

        /* hex bytes */
        for (size_t j = 0; j < BYTES_PER_ROW; j++) {
            if (i + j < len)
                fprintf(out, "%02x ", data[i + j]);
            else
                fprintf(out, "   ");
            if (j == 7)
                fputc(' ', out);
        }

        fprintf(out, " |");

        /* printable chars */
        for (size_t j = 0; j < BYTES_PER_ROW && i + j < len; j++) {
            unsigned char c = data[i + j];
            fputc(isprint(c) ? c : '.', out);
        }

        fprintf(out, "|\n");
    }
}

void hexdump_buf(const uint8_t *data, size_t len, char *out_buf, size_t out_sz)
{
    if (!data || !out_buf || out_sz == 0)
        return;

    out_buf[0] = '\0';
    size_t pos = 0;

    for (size_t i = 0; i < len && pos + 4 < out_sz; i++) {
        int n = snprintf(out_buf + pos, out_sz - pos,
                         (i + 1) % BYTES_PER_ROW == 0 ? "%02x\n" : "%02x ",
                         data[i]);
        if (n < 0)
            break;
        pos += (size_t)n;
    }
}
