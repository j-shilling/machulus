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

#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

struct _rsdp_descriptor
{
  char signature[8];
  uint8_t checksum;
  char oemid[6];
  uint8_t revision;
  uint32_t rsdt_addr;
} __attribute__ ((packed));

typedef struct _rsdp_descriptor rsdp_descriptor_t;

struct _rsdp_descriptor_20
{
  rsdp_descriptor_t rsdp_descriptor;
  uint32_t length;
  uint64_t xsdt_addr;
  uint8_t checksum;
  uint8_t reserved[3];
} __attribute__ ((packed));

typedef struct _rsdp_descriptor_20 rsdp_descriptor_20_t;

void acpi_init (void);

__END_DECLS