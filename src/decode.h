#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include <stddef.h>

/* Decoded field types */
typedef enum {
    FIELD_UINT8,
    FIELD_UINT16,
    FIELD_UINT32,
    FIELD_BYTES,
    FIELD_STRING
} field_type_t;

/* A single decoded field from a packet */
typedef struct {
    const char  *name;
    field_type_t type;
    union {
        uint8_t  u8;
        uint16_t u16;
        uint32_t u32;
        struct {
            const uint8_t *ptr;
            size_t         len;
        } bytes;
        const char *str;
    } value;
} decode_field_t;

/* Decoded packet result — up to 32 fields */
#define DECODE_MAX_FIELDS 32

typedef struct {
    const char    *proto_name;
    int            field_count;
    decode_field_t fields[DECODE_MAX_FIELDS];
} decode_result_t;

/* Decode raw bytes starting at offset for the given protocol layer.
 * Returns 0 on success, -1 on error. */
int decode_ethernet(const uint8_t *buf, size_t len, decode_result_t *out);
int decode_ipv4(const uint8_t *buf, size_t len, decode_result_t *out);
int decode_tcp(const uint8_t *buf, size_t len, decode_result_t *out);
int decode_udp(const uint8_t *buf, size_t len, decode_result_t *out);

/* Pretty-print a decode result to stdout */
void decode_print(const decode_result_t *result);

#endif /* DECODE_H */
