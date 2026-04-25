# pktsnare

Lightweight packet capture and protocol dissection CLI for embedded Linux targets.

---

## Features

- Capture live traffic on any network interface
- Dissect common protocols (Ethernet, IP, TCP, UDP, ICMP, ARP)
- Minimal dependencies — designed for resource-constrained embedded systems
- Human-readable output with optional hex dump

---

## Requirements

- Linux kernel ≥ 3.x with raw socket support
- libpcap (optional, for `.pcap` file output)
- GCC or Clang

---

## Installation

```bash
git clone https://github.com/youruser/pktsnare.git && cd pktsnare
make && sudo make install
```

---

## Usage

```bash
# Capture packets on eth0
sudo pktsnare -i eth0

# Capture 100 packets and display hex dump
sudo pktsnare -i eth0 -c 100 --hex

# Filter by protocol and write to file
sudo pktsnare -i eth0 -p tcp -w capture.pcap

# Read and dissect an existing pcap file
pktsnare -r capture.pcap
```

**Options**

| Flag | Description |
|------|-------------|
| `-i <iface>` | Network interface to capture on |
| `-c <count>` | Stop after capturing N packets |
| `-p <proto>` | Filter by protocol (tcp, udp, icmp, arp) |
| `-w <file>` | Write capture to a `.pcap` file |
| `-r <file>` | Read and dissect a `.pcap` file |
| `--hex` | Print raw hex dump for each packet |

---

## Building from Source

```bash
make          # Build release binary
make debug    # Build with debug symbols
make clean    # Remove build artifacts
```

---

## License

MIT © 2024 — see [LICENSE](LICENSE) for details.