#ifndef TCPREASM_H
#define TCPREASM_H

#include <stdint.h>
#include <stddef.h>

#define TCPREASM_MAX_STREAMS  64
#define TCPREASM_MAX_SEG_BUF  16
#define TCPREASM_TIMEOUT_SEC  30

typedef struct tcpreasm_seg {
    uint32_t seq;
    uint16_t len;
    uint8_t  data[1500];
    struct tcpreasm_seg *next;
} tcpreasm_seg_t;

typedef struct tcpreasm_stream {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t next_seq;
    uint32_t last_seen;
    int      seg_count;
    tcpreasm_seg_t *seg_head;
    int      in_use;
} tcpreasm_stream_t;

typedef struct tcpreasm_ctx {
    tcpreasm_stream_t streams[TCPREASM_MAX_STREAMS];
    int stream_count;
    uint64_t reassembled;
    uint64_t dropped_ooo;
    uint64_t expired;
} tcpreasm_ctx_t;

void tcpreasm_init(tcpreasm_ctx_t *ctx);

tcpreasm_stream_t *tcpreasm_get_stream(tcpreasm_ctx_t *ctx,
    uint32_t src_ip, uint32_t dst_ip,
    uint16_t src_port, uint16_t dst_port);

int tcpreasm_add_segment(tcpreasm_ctx_t *ctx,
    tcpreasm_stream_t *stream,
    uint32_t seq, const uint8_t *data, uint16_t len);

int tcpreasm_get_data(tcpreasm_stream_t *stream,
    uint8_t *buf, size_t buf_len, size_t *out_len);

void tcpreasm_expire(tcpreasm_ctx_t *ctx, uint32_t now);
void tcpreasm_free_stream(tcpreasm_stream_t *stream);
void tcpreasm_stats(const tcpreasm_ctx_t *ctx,
    uint64_t *reassembled, uint64_t *dropped, uint64_t *expired);

#endif /* TCPREASM_H */
