#include <stddef.h>
#include <stdint.h>

#include <gdt.h>

typedef struct {
  uint16_t limit0to15;
  uint16_t base0to15;
  uint8_t base16to23;
  uint8_t access;
  uint8_t flags;
  uint8_t base24to31;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
  uint16_t len;
  uint64_t base;
} __attribute__((packed)) gdtr_t;

_Static_assert(sizeof(gdt_entry_t) == 8, "GDT Entry should be 8 bytes");
_Static_assert(offsetof(gdt_entry_t, limit0to15) == 0, "Bad offset");
_Static_assert(offsetof(gdt_entry_t, base0to15) == 2, "Bad offset");
_Static_assert(offsetof(gdt_entry_t, base16to23) == 4, "Bad offset");
_Static_assert(offsetof(gdt_entry_t, access) == 5, "Bad offset");
_Static_assert(offsetof(gdt_entry_t, flags) == 6, "Bad offset");
_Static_assert(offsetof(gdt_entry_t, base24to31) == 7, "Bad offset");

static uint8_t const GDT_FLAG_PRESENT = (1 << 7);
static uint8_t const GDT_FLAG_KERNEL = (0);
static uint8_t const GDT_FLAG_USER = (3 << 5);
static uint8_t const GDT_FLAG_CODE = (1 << 4) | (1 << 3) | (1 << 1);
static uint8_t const GDT_FLAG_DATA = (1 << 4) | (1 << 1);

static uint8_t const GDT_FLAG_64_BIT = (1 << 5);
static uint8_t const GDT_FLAGS_PAGE_GRANULARITY = (1 << 7);

static int const GDT_KERNEL_CODE_INDEX = 1;
static int const GDT_KERNEL_DATA_INDEX = 2;
static int const GDT_USER_CODE_INDEX = 3;
static int const GDT_USER_DATA_INDEX = 4;

static uint16_t const GDT_KERNEL_CODE_OFFSET =
    GDT_KERNEL_CODE_INDEX * sizeof(gdt_entry_t);
static uint16_t const GDT_KERNEL_DATA_OFFSET =
    GDT_KERNEL_DATA_INDEX * sizeof(gdt_entry_t);

static void gdt_entry_init(gdt_entry_t *const entry, uint32_t base,
                           uint32_t limit, uint8_t access, uint8_t flags) {
  entry->base0to15 = (uint16_t)(base & 0x0000FFFF);
  entry->base16to23 = (uint8_t)((base & 0x00FF0000) >> 16);
  entry->base24to31 = (uint8_t)((base & 0xFF000000) >> 24);

  entry->limit0to15 = (uint16_t)(limit & 0x0000FFFF);
  entry->flags = (uint8_t)((limit & 0x000F0000) >> 16);

  entry->flags |= flags;
  entry->access = access | GDT_FLAG_PRESENT;
}

static gdt_entry_t gdt[6] __attribute__((aligned(8))) = {{0}};
static gdtr_t const gdtr = {.len = sizeof(gdt), .base = (uint64_t)gdt};

void gdt_init(void) {
  gdt_entry_init(gdt + GDT_KERNEL_CODE_INDEX, 0, 0x000FFFFF,
                 GDT_FLAG_KERNEL | GDT_FLAG_CODE,
                 GDT_FLAGS_PAGE_GRANULARITY | GDT_FLAG_64_BIT);
  gdt_entry_init(gdt + GDT_KERNEL_DATA_INDEX, 0, 0x000FFFFF,
                 GDT_FLAG_KERNEL | GDT_FLAG_DATA,
                 GDT_FLAGS_PAGE_GRANULARITY | GDT_FLAG_64_BIT);
  gdt_entry_init(gdt + GDT_USER_CODE_INDEX, 0, 0x000FFFFF,
                 GDT_FLAG_USER | GDT_FLAG_CODE,
                 GDT_FLAGS_PAGE_GRANULARITY | GDT_FLAG_64_BIT);
  gdt_entry_init(gdt + GDT_USER_DATA_INDEX, 0, 0x000FFFFF,
                 GDT_FLAG_USER | GDT_FLAG_DATA,
                 GDT_FLAGS_PAGE_GRANULARITY | GDT_FLAG_64_BIT);

  gdt_install((void *)(&gdtr), GDT_KERNEL_CODE_OFFSET, GDT_KERNEL_DATA_OFFSET);
}
