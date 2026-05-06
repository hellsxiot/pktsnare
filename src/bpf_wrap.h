#ifndef PKTSNARE_BPF_WRAP_H
#define PKTSNARE_BPF_WRAP_H

#include <stdint.h>
#include <stddef.h>

/* Maximum BPF filter expression length */
#define BPF_EXPR_MAX 256

/* Opaque handle for a compiled BPF filter */
typedef struct bpf_wrap bpf_wrap_t;

/*
 * Compile a BPF filter expression string into an internal handle.
 * Returns NULL on failure; call bpf_wrap_error() for details.
 * snaplen is the capture snapshot length used during compilation.
 */
bpf_wrap_t *bpf_wrap_compile(const char *expr, int snaplen);

/*
 * Apply a compiled BPF filter to a raw packet buffer.
 * Returns 1 if the packet matches, 0 if not, -1 on error.
 */
int bpf_wrap_match(const bpf_wrap_t *bw, const uint8_t *pkt, size_t pktlen);

/*
 * Return the original filter expression string.
 */
const char *bpf_wrap_expr(const bpf_wrap_t *bw);

/*
 * Return a human-readable error string for the last failed operation.
 * Thread-local; valid until the next call on the same thread.
 */
const char *bpf_wrap_error(void);

/*
 * Free all resources associated with a compiled filter.
 */
void bpf_wrap_free(bpf_wrap_t *bw);

#endif /* PKTSNARE_BPF_WRAP_H */
