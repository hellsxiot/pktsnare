#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "pcap.h"

#define PCAP_MAGIC_LE     0xa1b2c3d4
#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4

static int write_global_header(FILE *fp, uint32_t snaplen, uint32_t linktype) {
    pcap_file_header_t hdr;
    hdr.magic_number  = PCAP_MAGIC_LE;
    hdr.version_major = PCAP_VERSION_MAJOR;
    hdr.version_minor = PCAP_VERSION_MINOR;
    hdr.thiszone      = 0;
    hdr.sigfigs       = 0;
    hdr.snaplen       = snaplen;
    hdr.network       = linktype;
    return fwrite(&hdr, sizeof(hdr), 1, fp) == 1 ? 0 : -1;
}

pcap_writer_t *pcap_writer_open(const char *path, uint32_t snaplen, uint32_t linktype) {
    if (!path) return NULL;
    FILE *fp = fopen(path, "wb");
    if (!fp) return NULL;

    pcap_writer_t *w = calloc(1, sizeof(pcap_writer_t));
    if (!w) { fclose(fp); return NULL; }

    w->fp       = fp;
    w->snaplen  = snaplen;
    w->linktype = linktype;
    w->pkt_count = 0;

    if (write_global_header(fp, snaplen, linktype) != 0) {
        fclose(fp);
        free(w);
        return NULL;
    }
    return w;
}

int pcap_writer_write(pcap_writer_t *w, const uint8_t *data, uint32_t len) {
    if (!w || !data || len == 0) return -1;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    pcap_rec_header_t rec;
    rec.ts_sec   = (uint32_t)ts.tv_sec;
    rec.ts_usec  = (uint32_t)(ts.tv_nsec / 1000);
    rec.incl_len = len > w->snaplen ? w->snaplen : len;
    rec.orig_len = len;

    if (fwrite(&rec, sizeof(rec), 1, w->fp) != 1) return -1;
    if (fwrite(data, 1, rec.incl_len, w->fp) != rec.incl_len) return -1;

    w->pkt_count++;
    return 0;
}

void pcap_writer_flush(pcap_writer_t *w) {
    if (w && w->fp) fflush(w->fp);
}

void pcap_writer_close(pcap_writer_t *w) {
    if (!w) return;
    if (w->fp) fclose(w->fp);
    free(w);
}
