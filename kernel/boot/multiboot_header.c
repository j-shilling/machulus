#include <multiboot2.h>
#include <stdint.h>

struct multiboot2_header {
  uint32_t magic;
  uint32_t architecture;
  uint32_t header_length;
  uint32_t checksum;

  // TAGS
  struct multiboot_header_tag end_tag;
} __attribute__((packed));

static struct multiboot2_header const MULTIBOOT2_HEADER
    __attribute__((section(".multiboot_header"))) __attribute__((aligned(8)))
    __attribute__((unused)) = {
        .magic = MULTIBOOT2_HEADER_MAGIC,
        .architecture = MULTIBOOT_ARCHITECTURE_I386,
        .header_length = sizeof(struct multiboot2_header),
        .checksum = -((uint32_t)MULTIBOOT2_HEADER_MAGIC +
                      (uint32_t)MULTIBOOT_ARCHITECTURE_I386 +
                      (uint32_t)sizeof(struct multiboot2_header)),
        .end_tag = {.type = MULTIBOOT_HEADER_TAG_END,
                    .flags = 0,
                    .size = sizeof(struct multiboot_header_tag)}};
