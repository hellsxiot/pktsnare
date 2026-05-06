#ifndef PKTSNARE_BPF_WRAP_CFG_H
#define PKTSNARE_BPF_WRAP_CFG_H

/*
 * bpf_wrap_cfg.h — compile-time defaults for the BPF wrapper module.
 *
 * Override any of these with -D<NAME>=<value> in CFLAGS before including
 * bpf_wrap.h if the defaults don't suit your target.
 */

/* Default snapshot length passed to pcap_compile_nopcap when the caller
 * does not have a specific value.  65535 captures full Ethernet frames. */
#ifndef BPF_WRAP_DEFAULT_SNAPLEN
#define BPF_WRAP_DEFAULT_SNAPLEN 65535
#endif

/* Link-layer type assumed during filter compilation.
 * DLT_EN10MB (1) covers standard Ethernet and is appropriate for most
 * embedded targets that use eth0/eth1 style interfaces. */
#ifndef BPF_WRAP_DEFAULT_DLT
#define BPF_WRAP_DEFAULT_DLT 1   /* DLT_EN10MB */
#endif

/* When non-zero, bpf_wrap_compile() will emit a warning to stderr if the
 * compiled BPF program exceeds this many instructions.  0 disables the
 * check.  Complex filters on low-end CPUs can be surprisingly slow. */
#ifndef BPF_WRAP_INSN_WARN_THRESHOLD
#define BPF_WRAP_INSN_WARN_THRESHOLD 64
#endif

#endif /* PKTSNARE_BPF_WRAP_CFG_H */
