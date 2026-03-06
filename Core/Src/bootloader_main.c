#include "bootloader_main.h"
#include "bootloader.h"
#include "main.h"
#include "usart.h"
#include <string.h>

static void print_msg(const char *msg) {
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void boot_main(void) {
  print_msg("\r\n\r\n=== STM32 Fail-Safe Bootloader ===\r\n");
  print_msg("[INFO] RTC flag checking bypassed for now.\r\n");

  print_msg("[INFO] Jumping to Slot A...\r\n");

  // Give the UART hardware a few milliseconds to finish shifting out
  // the last characters before we rip the vector table out from under it.
  HAL_Delay(10);

  bootloader_jump_to_app();

  // We should never reach this loop unless the jump function fails or Slot A is
  // empty
  while (1) {
    print_msg("[FATAL] Jump to application failed!\r\n");
    HAL_Delay(1000);
  }
}
