#include <stdint.h>
#include <string.h>
#include "ethtype.h"

/* Map of common EtherType values to human-readable names */
static const ethtype_entry_t ethtype_table[] = {
    { 0x0800, "IPv4"       },
    { 0x0806, "ARP"        },
    { 0x0842, "WakeOnLAN"  },
    { 0x86DD, "IPv6"       },
    { 0x8100, "802.1Q"     },
    { 0x88A8, "802.1ad"    },
    { 0x8847, "MPLS-UC"    },
    { 0x8848, "MPLS-MC"    },
    { 0x8863, "PPPoE-Disc" },
    { 0x8864, "PPPoE-Sess" },
    { 0x88CC, "LLDP"       },
    { 0x9100, "QinQ"       },
    { 0x0000, NULL         }  /* sentinel */
};

const char *ethtype_name(uint16_t ethertype)
{
    const ethtype_entry_t *e = ethtype_table;
    while (e->name != NULL) {
        if (e->ethertype == ethertype)
            return e->name;
        e++;
    }
    return "Unknown";
}

int ethtype_is_vlan(uint16_t ethertype)
{
    return (ethertype == 0x8100 ||
            ethertype == 0x88A8 ||
            ethertype == 0x9100);
}

int ethtype_is_ip(uint16_t ethertype)
{
    return (ethertype == 0x0800 || ethertype == 0x86DD);
}

int ethtype_lookup(const char *name, uint16_t *out)
{
    if (!name || !out)
        return -1;
    const ethtype_entry_t *e = ethtype_table;
    while (e->name != NULL) {
        if (strcasecmp(e->name, name) == 0) {
            *out = e->ethertype;
            return 0;
        }
        e++;
    }
    return -1;
}
