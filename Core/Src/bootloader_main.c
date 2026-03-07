#include "bootloader_main.h"
#include "bootloader.h"
#include "fw_verify.h"
#include "main.h"
#include "nvmem.h"
#include "update_flag.h"
#include "usart.h"
#include "xmodem.h"
#include <string.h>

static void print_msg(const char *msg) {
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void boot_main(void) {
  print_msg("\r\n\r\n=== STM32 Fail-Safe Bootloader ===\r\n");
  bool force_update = false;

  if (rtc_is_update_pending()) {
    if (fw_is_valid(STAGING)) {
      if (nvmem_copy(STAGING, APP) == RESULT_OK) {
        print_msg(
            "[INFO] Firmware update successful. Clearing update flag.\r\n");
        rtc_clear_update_pending();
        NVIC_SystemReset();
      } else {
        print_msg("[ERROR] Firmware copy failed! Staying in bootloader.\r\n");
        while (1) {
          HAL_Delay(1000);
        }
      }
    }

    if (bootloader_check_force_update()) {
      print_msg("[INFO] Force update button held. Staying in bootloader.\r\n");
      force_update = true;
    }

    if (force_update || !fw_is_valid(APP)) {
      if (xmodem_receive_firmware()) {
        rtc_write_update_pending();
        NVIC_SystemReset();
      }
    }

    print_msg("[INFO] Jumping to App...\r\n");

    // Give the UART hardware a few milliseconds to finish shifting out
    // the last characters before we rip the vector table out from under it.
    HAL_Delay(10);

    bootloader_jump_to_app();

    // We should never reach this loop unless the jump function fails or Slot A
    // is empty
    while (1) {
      print_msg("[FATAL] Jump to application failed!\r\n");
      HAL_Delay(1000);
    }
  }
}
