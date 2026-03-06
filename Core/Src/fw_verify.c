#include "fw_verify.h"
#include "main.h"
#include <stdint.h>

extern const PartitionInfo Partitions[];

static bool is_magic_valid(const fw_header_t *header) {
  return header->magic == FW_MAGIC;
}

static uint32_t calculate_payload_crc(NVMEM_SLOT slot, uint32_t payload_size) {
  uint32_t *start_addr =
      (uint32_t *)(Partitions[slot].base_address + sizeof(fw_header_t));

  uint32_t word_count = payload_size / 4;

  __HAL_RCC_CRC_CLK_ENABLE();
  CRC->CR |= CRC_CR_RESET; // Reset CRC calculation unit

  for (int i = 0; i < word_count; i++) {
    CRC->DR = *start_addr;
    start_addr++;
  }

  return CRC->DR;
}

bool fw_is_valid(NVMEM_SLOT slot) {
  fw_header_t *header = (fw_header_t *)Partitions[slot].base_address;
  if (!is_magic_valid(header)) {
    return false;
  }

  if (header->size % 4 != 0) {
    return false;
  }

  uint32_t calculated_crc = calculate_payload_crc(slot, header->size);

  if (calculated_crc != header->checksum) {
    return false;
  }

  return true;
}
