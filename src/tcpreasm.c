#include <string.h>
#include <stdlib.h>
#include "tcpreasm.h"

void tcpreasm_init(tcpreasm_ctx_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

tcpreasm_stream_t *tcpreasm_get_stream(tcpreasm_ctx_t *ctx,
    uint32_t src_ip, uint32_t dst_ip,
    uint16_t src_port, uint16_t dst_port)
{
    for (int i = 0; i < TCPREASM_MAX_STREAMS; i++) {
        tcpreasm_stream_t *s = &ctx->streams[i];
        if (!s->in_use) continue;
        if (s->src_ip == src_ip && s->dst_ip == dst_ip &&
            s->src_port == src_port && s->dst_port == dst_port)
            return s;
    }
    /* allocate new */
    for (int i = 0; i < TCPREASM_MAX_STREAMS; i++) {
        tcpreasm_stream_t *s = &ctx->streams[i];
        if (s->in_use) continue;
        memset(s, 0, sizeof(*s));
        s->src_ip   = src_ip;
        s->dst_ip   = dst_ip;
        s->src_port = src_port;
        s->dst_port = dst_port;
        s->in_use   = 1;
        ctx->stream_count++;
        return s;
    }
    return NULL;
}

int tcpreasm_add_segment(tcpreasm_ctx_t *ctx,
    tcpreasm_stream_t *stream,
    uint32_t seq, const uint8_t *data, uint16_t len)
{
    if (!stream || !data || len == 0) return -1;
    if (stream->seg_count >= TCPREASM_MAX_SEG_BUF) {
        ctx->dropped_ooo++;
        return -1;
    }
    tcpreasm_seg_t *seg = malloc(sizeof(tcpreasm_seg_t));
    if (!seg) return -1;
    seg->seq  = seq;
    seg->len  = len > 1500 ? 1500 : len;
    memcpy(seg->data, data, seg->len);
    seg->next = NULL;
    /* insert sorted by seq */
    tcpreasm_seg_t **cur = &stream->seg_head;
    while (*cur && (*cur)->seq < seq)
        cur = &(*cur)->next;
    seg->next = *cur;
    *cur = seg;
    stream->seg_count++;
    return 0;
}

int tcpreasm_get_data(tcpreasm_stream_t *stream,
    uint8_t *buf, size_t buf_len, size_t *out_len)
{
    if (!stream || !buf || !out_len) return -1;
    *out_len = 0;
    tcpreasm_seg_t *seg = stream->seg_head;
    while (seg) {
        if (*out_len + seg->len > buf_len) break;
        memcpy(buf + *out_len, seg->data, seg->len);
        *out_len += seg->len;
        seg = seg->next;
    }
    return (*out_len > 0) ? 0 : -1;
}

void tcpreasm_free_stream(tcpreasm_stream_t *stream) {
    tcpreasm_seg_t *seg = stream->seg_head;
    while (seg) {
        tcpreasm_seg_t *next = seg->next;
        free(seg);
        seg = next;
    }
    stream->seg_head  = NULL;
    stream->seg_count = 0;
    stream->in_use    = 0;
}

void tcpreasm_expire(tcpreasm_ctx_t *ctx, uint32_t now) {
    for (int i = 0; i < TCPREASM_MAX_STREAMS; i++) {
        tcpreasm_stream_t *s = &ctx->streams[i];
        if (!s->in_use) continue;
        if (now - s->last_seen >= TCPREASM_TIMEOUT_SEC) {
            tcpreasm_free_stream(s);
            ctx->stream_count--;
            ctx->expired++;
        }
    }
}

void tcpreasm_stats(const tcpreasm_ctx_t *ctx,
    uint64_t *reassembled, uint64_t *dropped, uint64_t *expired)
{
    if (reassembled) *reassembled = ctx->reassembled;
    if (dropped)     *dropped     = ctx->dropped_ooo;
    if (expired)     *expired     = ctx->expired;
}
