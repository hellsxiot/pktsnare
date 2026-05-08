#ifndef PKTSNARE_ETHTYPE_H
#define PKTSNARE_ETHTYPE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ethtype_entry_t - table entry mapping EtherType value to a name string.
 */
typedef struct {
    uint16_t    ethertype;
    const char *name;
} ethtype_entry_t;

/**
 * ethtype_name - return a human-readable string for an EtherType value.
 * @ethertype: 16-bit EtherType in host byte order.
 *
 * Returns a static string (never NULL). Unknown types return "Unknown".
 */
const char *ethtype_name(uint16_t ethertype);

/**
 * ethtype_is_vlan - check whether the EtherType indicates a VLAN tag.
 * Returns non-zero if true.
 */
int ethtype_is_vlan(uint16_t ethertype);

/**
 * ethtype_is_ip - check whether the EtherType is IPv4 or IPv6.
 * Returns non-zero if true.
 */
int ethtype_is_ip(uint16_t ethertype);

/**
 * ethtype_lookup - resolve a name string back to its EtherType value.
 * @name: case-insensitive protocol name (e.g. "IPv4", "ARP").
 * @out:  pointer to store the result.
 *
 * Returns 0 on success, -1 if the name is not found.
 */
int ethtype_lookup(const char *name, uint16_t *out);

#ifdef __cplusplus
}
#endif

#endif /* PKTSNARE_ETHTYPE_H */
