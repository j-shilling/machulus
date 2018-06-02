#pragma once

#include <sys/cdefs.h>

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
