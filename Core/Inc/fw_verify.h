#ifndef FW_VERIFY_H
#define FW_VERIFY_H

#include "nvmem.h"
#include <stdbool.h>
#include <stdint.h>

#define FW_MAGIC 0xBADC0FFE
#define HEADER_SIZE 0x200 // 512 byte packed header

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t size;
  uint32_t checksum;
} fw_header_t;

bool fw_is_valid(NVMEM_SLOT);

#endif /* FW_VERIFY_H */
