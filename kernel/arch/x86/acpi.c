/*
 * Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>

#include "acpi.h"
#include "paging.h"

/* Root system description pointer for ACPI 1.0
 *
 * 8-byte signature
 * 1-byte checksum
 * 6-byte oemid
 * 1-byte revision
 * 4-byte rsdt address
 */
struct rsdp_v1 {
    char bytes[20];
};

/* Root system descriptor pointer for ACPI 2.0
 *
 * 20-byte rsdp_v1
 * 4-byte  length
 * 8-byte  xsdt address
 * 1-byte  extended checksum
 * 3-byte  reserved
 */
struct rsdp_v2 {
    char bytes[36];
};

void* rsdp = NULL;

/** return the revision field of an RSDP */
static inline int acpi_rsdp_revision(rsdp_v1_t* rsdp)
{
    return rsdp->bytes[15];
}

/** Return true if addr points to a valid RSDP */
static inline bool acpi_is_valid_rsdp(void* addr)
{
    /* Check signature */
    {
        char* signature = (char*)addr;

        if(!((signature[0] == 'R') && (signature[1] == 'S') && (signature[2] == 'D') && (signature[3] == ' ') &&
               (signature[4] == 'P') && (signature[5] == 'T') && (signature[6] == 'R') && (signature[7] == ' ')))
            return false;
    }

    /* Check ACPI 1.0 Checksum */
    {
        rsdp_v1_t* rsdp = (rsdp_v1_t*)addr;
        int sum = 0;

        for(int i = 0; i < sizeof(rsdp_v1_t); i++)
            sum += rsdp->bytes[i];

        if(((char*)(&sum))[0] != 0)
            return false;
    }

    /* Check ACPI version before continuing */
    if(acpi_rsdp_revision((rsdp_v1_t*)addr) == 0)
        return true;

    /* Check ACPI 2.0 checksum */
    {
        rsdp_v2_t* rsdp = (rsdp_v2_t*)addr;
        int sum = 0;

        for(int i = 20; i < sizeof(rsdp_v2_t); i++)
            sum += rsdp->bytes[i];

        if(((char*)(&sum))[0] != 0)
            return false;
    }

    return true;
}

void acpi_init(void)
{
    /* RSDP might be in the first KB of EBDA */
    {
        uint16_t ebda_base_ptr = *((uint16_t*)phys_to_virt((void*)0x40E));
        void* ebda = (void*)(((uintptr_t)ebda_base_ptr) << 4);

        uintptr_t cur = ((uintptr_t)ebda);
        uintptr_t end = cur + 1024;

        cur += cur % 16;
        while(cur < end) {
            if(acpi_is_valid_rsdp((void*)cur)) {
                rsdp = (void*)cur;
                return;
            }

            cur += 16;
        }
    }

    /* RSDP might be in block */
    {
        uintptr_t cur = (uintptr_t)phys_to_virt((void*)0x000E0000);
        uintptr_t end = (uintptr_t)phys_to_virt((void*)0x00100000);

        while(cur < end) {
            if(acpi_is_valid_rsdp((void*)cur)) {
                rsdp = (void*)cur;
                return;
            }

            cur += 16;
        }
    }
}