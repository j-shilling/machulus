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

#include <stdint.h>

struct _mp_floating_pointer
{
  char signature[4]; /* _MP_ */
  uint32_t phys_addr; /* addr of mp config table */
  uint8_t length; /* length of this struct in 16 byte chunks */
  uint8_t rev; /* version of MP specification */
  uint8_t checksum; /* A checksum of the complete pointer structure.
                               All bytes specified by the length field, including
                               CHECKSUM and reserved bytes, must add up to
                               zero */
  uint8_t default_config; /* System default configuration type */
  uint8_t resv0 : 7;
  uint8_t imcr_present : 1;
  uint8_t resv1[3];
} __attribute__ ((packed));
typedef struct _mp_floating_pointer mp_floating_pointer_t;

struct _mp_config_table_header
{
  /* The ASCII string representation of “PCMP,” which confirms the presence of 
   * the table. */
  char signature[4];
  /* The length of the base configuration table in bytes, including the header, 
   * starting from offset 0.  This field aids in calculation of the checksum. */
  uint16_t length;
  /* The version number of the MP specification. A value of 01h indicates 
   * Version 1.1.  A value of 04h indicate Version 1.4 */
  uint8_t rev;
  /* A checksum of the entire base configuration table.  All bytes, including 
   * CHECKSUM and reserved bytes, must add up to zero. */
  uint8_t checksum;
  /* A string that identifies the manufacturer of the system hardware */
  char oem_id[8];
  /* A string that identifies the product family. */
  char product_id[12];
  /* A physical-address pointer to an OEM-defined configuration table.  This 
   * table is optional.  If not present, this field is zero. */
  uint32_t oem_addr;
  /* The size of the base OEM table in bytes.  If the table is not present, this
   * field is zero. */
  uint16_t oem_length;
  /* The number of entries in the variable portion of the base  table. This 
   * field helps the software identify the end of the table when stepping 
   * through the entries */
  uint16_t entry_count;
  /* The base address by which each processor accesses its local APIC. */
  uint32_t lapic_addr;
  /* Length in bytes of the extended entries that are located in memory at the 
   * end of the base configuration table.  A zero value in this field indicates 
   * that no extended entries are present. */
  uint16_t extended_length;
  /* Checksum for the set of extended table entries, including only extended 
   * entries starting from the end of the base configuration table.  When no 
   * extended table is present, this field is zero. */
  uint8_t extedend_checksum;
} __attribute__ ((packed));
typedef struct _mp_config_table_header mp_config_table_header_t;

/* The MP Configuration Table is followed by a variable number of entries, which
 * each start with a byte indicating the entry type. */

#define MP_ENTRY_PROCESSOR      (0)
#define MP_ENTRY_BUS            (1)
#define MP_ENTRY_IO_APIC        (2)
#define MP_ENTRY_IO_INT         (3)
#define MP_ENTRY_LOCAL_INT      (4)

struct _mp_proc_entry
{
  uint8_t entry_type;
  uint8_t lapic_id;             /* local apic id */
  uint8_t lapic_version;        /* bits 0-7 of local apic version register */
  uint8_t cpu_flags;
  uint32_t cpu_signature;
  uint32_t feature_flags;
  uint64_t reserved;
} __attribute__ ((packed));
typedef struct _mp_proc_entry mp_proc_entry_t;

struct _mp_bus_entry
{
  uint8_t entry_type;
  uint8_t bus_id;
  char type_string[6];
} __attribute__ ((packed));
typedef struct _mp_bus_entry mp_bus_entry_t;

struct _mp_io_apic_entry
{
  uint8_t entry_type;
  uint8_t io_apic_id;
  uint8_t io_apic_version;
  uint8_t io_apic_flags;
  uint32_t addr;
} __attribute__ ((packed));
typedef struct _mp_io_apic_entry mp_io_apic_entry_t;

struct _mp_io_int_entry
{
  uint8_t entry_type;
} __attribute__ ((packed));
typedef struct _mp_io_int_entry mp_io_int_entry_t;

struct _mp_local_int_entry
{
  uint8_t entry_type;
} __attribute__ ((packed));
typedef struct _mp_local_int_entry mp_local_int_entry_t;