#include "decode.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

static void add_field(decode_result_t *r, const char *name,
                      field_type_t type, uint32_t uval,
                      const uint8_t *bptr, size_t blen)
{
    if (r->field_count >= DECODE_MAX_FIELDS) return;
    decode_field_t *f = &r->fields[r->field_count++];
    f->name = name;
    f->type = type;
    switch (type) {
        case FIELD_UINT8:  f->value.u8  = (uint8_t)uval;  break;
        case FIELD_UINT16: f->value.u16 = (uint16_t)uval; break;
        case FIELD_UINT32: f->value.u32 = uval;           break;
        case FIELD_BYTES:  f->value.bytes.ptr = bptr;
                           f->value.bytes.len = blen;     break;
        default: break;
    }
}

int decode_ethernet(const uint8_t *buf, size_t len, decode_result_t *out)
{
    if (!buf || !out || len < 14) return -1;
    memset(out, 0, sizeof(*out));
    out->proto_name = "Ethernet";
    add_field(out, "dst_mac",  FIELD_BYTES,  0, buf,     6);
    add_field(out, "src_mac",  FIELD_BYTES,  0, buf + 6, 6);
    add_field(out, "ethertype",FIELD_UINT16, (uint16_t)((buf[12] << 8) | buf[13]), NULL, 0);
    return 0;
}

int decode_ipv4(const uint8_t *buf, size_t len, decode_result_t *out)
{
    if (!buf || !out || len < 20) return -1;
    memset(out, 0, sizeof(*out));
    out->proto_name = "IPv4";
    add_field(out, "version",  FIELD_UINT8,  (buf[0] >> 4) & 0xf, NULL, 0);
    add_field(out, "ihl",      FIELD_UINT8,  buf[0] & 0xf,        NULL, 0);
    add_field(out, "tos",      FIELD_UINT8,  buf[1],              NULL, 0);
    add_field(out, "tot_len",  FIELD_UINT16, (uint16_t)((buf[2] << 8) | buf[3]), NULL, 0);
    add_field(out, "ttl",      FIELD_UINT8,  buf[8],              NULL, 0);
    add_field(out, "protocol", FIELD_UINT8,  buf[9],              NULL, 0);
    add_field(out, "src_ip",   FIELD_UINT32, ntohl(*(uint32_t *)(buf + 12)), NULL, 0);
    add_field(out, "dst_ip",   FIELD_UINT32, ntohl(*(uint32_t *)(buf + 16)), NULL, 0);
    return 0;
}

int decode_tcp(const uint8_t *buf, size_t len, decode_result_t *out)
{
    if (!buf || !out || len < 20) return -1;
    memset(out, 0, sizeof(*out));
    out->proto_name = "TCP";
    add_field(out, "src_port", FIELD_UINT16, (uint16_t)((buf[0] << 8) | buf[1]), NULL, 0);
    add_field(out, "dst_port", FIELD_UINT16, (uint16_t)((buf[2] << 8) | buf[3]), NULL, 0);
    add_field(out, "seq",      FIELD_UINT32, ntohl(*(uint32_t *)(buf + 4)),  NULL, 0);
    add_field(out, "ack",      FIELD_UINT32, ntohl(*(uint32_t *)(buf + 8)),  NULL, 0);
    add_field(out, "flags",    FIELD_UINT8,  buf[13],                         NULL, 0);
    add_field(out, "window",   FIELD_UINT16, (uint16_t)((buf[14] << 8) | buf[15]), NULL, 0);
    return 0;
}

int decode_udp(const uint8_t *buf, size_t len, decode_result_t *out)
{
    if (!buf || !out || len < 8) return -1;
    memset(out, 0, sizeof(*out));
    out->proto_name = "UDP";
    add_field(out, "src_port", FIELD_UINT16, (uint16_t)((buf[0] << 8) | buf[1]), NULL, 0);
    add_field(out, "dst_port", FIELD_UINT16, (uint16_t)((buf[2] << 8) | buf[3]), NULL, 0);
    add_field(out, "length",   FIELD_UINT16, (uint16_t)((buf[4] << 8) | buf[5]), NULL, 0);
    add_field(out, "checksum", FIELD_UINT16, (uint16_t)((buf[6] << 8) | buf[7]), NULL, 0);
    return 0;
}

void decode_print(const decode_result_t *result)
{
    if (!result) return;
    printf("[%s]\n", result->proto_name);
    for (int i = 0; i < result->field_count; i++) {
        const decode_field_t *f = &result->fields[i];
        switch (f->type) {
            case FIELD_UINT8:  printf("  %-12s: %u\n",   f->name, f->value.u8);  break;
            case FIELD_UINT16: printf("  %-12s: %u\n",   f->name, f->value.u16); break;
            case FIELD_UINT32: printf("  %-12s: %u\n",   f->name, f->value.u32); break;
            case FIELD_BYTES:
                printf("  %-12s: ", f->name);
                for (size_t j = 0; j < f->value.bytes.len; j++)
                    printf("%02x%s", f->value.bytes.ptr[j],
                           j + 1 < f->value.bytes.len ? ":" : "");
                printf("\n");
                break;
            default: break;
        }
    }
}
