#ifndef NVMEM_H
#define NVMEM_H

#include "result.h"
#include <stdint.h>

typedef enum {
  APP = 0,
  STAGING = 1,
} NVMEM_SLOT;

typedef struct {
  uint32_t base_address;
  uint32_t size;
  uint32_t sector_number;
  uint8_t number_of_sectors;
} PartitionInfo;

extern const PartitionInfo Partitions[];

result_t nvmem_write(NVMEM_SLOT slot, uint32_t offset, const uint8_t *data,
                     uint32_t size);
result_t nvmem_read(NVMEM_SLOT slot, uint32_t offset, uint8_t *data,
                    uint32_t size);
result_t nvmem_erase(NVMEM_SLOT slot);
result_t nvmem_copy(NVMEM_SLOT src_slot, NVMEM_SLOT dst_slot);

#endif
