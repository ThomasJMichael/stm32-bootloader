#include "nvmem.h"
#include "main.h"
#include "result.h"
#include <stdint.h>

#define IS_ALIGNED(ptr, size)                                                  \
  (((size) % 4 == 0) && (((uintptr_t)(ptr)) % 4 == 0))

const PartitionInfo Partitions[] = {
    [APP] =
        {
            .base_address = 0x08008000,
            .size = 0x00038000, // 224KB
            .sector_number = FLASH_SECTOR_2,
            .number_of_sectors = 4,
        },
    [STAGING] =
        {
            .base_address = 0x08040000,
            .size = 0x00038000, // 224KB
            .sector_number = FLASH_SECTOR_6,
            .number_of_sectors = 2,
        },
};

result_t nvmem_write(NVMEM_SLOT slot, uint32_t offset, const uint8_t *data,
                     uint32_t size) {
  if (!IS_ALIGNED(data, size) || (offset % 4 != 0)) {
    return RESULT_ERROR_ALIGNMENT;
  }

  HAL_FLASH_Unlock();
  uint32_t *word_data = (uint32_t *)data;
  for (unsigned int i = 0; i < size; i += 4) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          Partitions[slot].base_address + offset + i,
                          word_data[i / 4]) != HAL_OK) {
      HAL_FLASH_Lock();
      return RESULT_ERROR;
    }
  }
  HAL_FLASH_Lock();
  return RESULT_OK;
}

result_t nvmem_read(NVMEM_SLOT slot, uint32_t offset, uint8_t *data,
                    uint32_t size) {
  if (!IS_ALIGNED(data, size)) {
    return RESULT_ERROR_ALIGNMENT;
  }

  if (data == NULL)
    return RESULT_ERROR_NULL_POINTER;
  if (size == 0)
    return RESULT_ERROR_SIZE_ZERO;

  uint32_t absolute_address = Partitions[slot].base_address + offset;
  uint32_t *address_ptr = (uint32_t *)absolute_address;
  while (size >= 4) {
    data[0] = (uint8_t)(*address_ptr & 0xFF);
    data[1] = (uint8_t)((*address_ptr >> 8) & 0xFF);
    data[2] = (uint8_t)((*address_ptr >> 16) & 0xFF);
    data[3] = (uint8_t)((*address_ptr >> 24) & 0xFF);
    address_ptr++;
    size -= 4;
    data += 4;
  }
  while (size > 0) {
    data[0] = (uint8_t)(*address_ptr & 0xFF);
    size--;
    data++;
  }
  return RESULT_OK;
}

result_t nvmem_erase(NVMEM_SLOT slot) {
  uint32_t sectorError = 0;
  FLASH_EraseInitTypeDef eraseInitStruct = {
      FLASH_TYPEERASE_SECTORS, FLASH_BANK_1, Partitions[slot].sector_number,
      Partitions[slot].number_of_sectors, FLASH_VOLTAGE_RANGE_3};
  HAL_FLASH_Unlock();
  if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK) {
    HAL_FLASH_Lock();
    return RESULT_ERROR;
  }
  HAL_FLASH_Lock();
  return RESULT_OK;
}

result_t nvmem_copy(NVMEM_SLOT src_slot, NVMEM_SLOT dst_slot) {

  uint8_t data[256];

  RESULT_OK_OR_RETURN(nvmem_erase(dst_slot));

  for (unsigned int chunk_offset = 0; chunk_offset < Partitions[src_slot].size;
       chunk_offset += 256) {
    uint32_t chunk_size = (Partitions[src_slot].size - chunk_offset) >= 256
                              ? 256
                              : (Partitions[src_slot].size - chunk_offset);

    RESULT_OK_OR_RETURN(nvmem_read(src_slot, chunk_offset, data, chunk_size));

    if (nvmem_write(dst_slot, chunk_offset, data, chunk_size) != RESULT_OK) {
      return RESULT_ERROR_WRITE_FAILURE;
    }
  }
  return RESULT_OK;
}
