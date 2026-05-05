#include "portmap.h"

#include <string.h>
#include <netinet/in.h>

/* Built-in well-known port mappings */
static const portmap_entry_t defaults[] = {
    {  20, IPPROTO_TCP, "ftp-data" },
    {  21, IPPROTO_TCP, "ftp"      },
    {  22, IPPROTO_TCP, "ssh"      },
    {  23, IPPROTO_TCP, "telnet"   },
    {  25, IPPROTO_TCP, "smtp"     },
    {  53, IPPROTO_TCP, "dns"      },
    {  53, IPPROTO_UDP, "dns"      },
    {  67, IPPROTO_UDP, "dhcp-srv" },
    {  68, IPPROTO_UDP, "dhcp-cli" },
    {  80, IPPROTO_TCP, "http"     },
    { 123, IPPROTO_UDP, "ntp"      },
    { 443, IPPROTO_TCP, "https"    },
    { 514, IPPROTO_UDP, "syslog"   },
    { 8080, IPPROTO_TCP, "http-alt"},
};

#define NDEFAULTS ((int)(sizeof(defaults) / sizeof(defaults[0])))

int portmap_init(portmap_t *pm)
{
    if (!pm)
        return -1;
    memset(pm, 0, sizeof(*pm));
    if (NDEFAULTS > PORTMAP_MAX_ENTRIES)
        return -1;
    memcpy(pm->entries, defaults, sizeof(defaults));
    pm->count = NDEFAULTS;
    return 0;
}

int portmap_register(portmap_t *pm, uint16_t port, uint8_t proto,
                     const char *name)
{
    if (!pm || !name || pm->count >= PORTMAP_MAX_ENTRIES)
        return -1;
    /* overwrite if already exists */
    for (int i = 0; i < pm->count; i++) {
        if (pm->entries[i].port == port && pm->entries[i].proto == proto) {
            strncpy(pm->entries[i].name, name,
                    sizeof(pm->entries[i].name) - 1);
            return 0;
        }
    }
    pm->entries[pm->count].port  = port;
    pm->entries[pm->count].proto = proto;
    strncpy(pm->entries[pm->count].name, name,
            sizeof(pm->entries[pm->count].name) - 1);
    pm->count++;
    return 0;
}

const char *portmap_lookup(const portmap_t *pm, uint16_t port, uint8_t proto)
{
    if (!pm)
        return NULL;
    for (int i = 0; i < pm->count; i++) {
        if (pm->entries[i].port == port && pm->entries[i].proto == proto)
            return pm->entries[i].name;
    }
    return NULL;
}

int portmap_remove(portmap_t *pm, uint16_t port, uint8_t proto)
{
    if (!pm)
        return -1;
    for (int i = 0; i < pm->count; i++) {
        if (pm->entries[i].port == port && pm->entries[i].proto == proto) {
            pm->entries[i] = pm->entries[pm->count - 1];
            pm->count--;
            return 0;
        }
    }
    return -1;
}

void portmap_clear(portmap_t *pm)
{
    if (pm) {
        memset(pm, 0, sizeof(*pm));
    }
}
