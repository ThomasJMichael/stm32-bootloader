#include "bootloader_main.h"
#include "bootloader.h"
#include "fw_verify.h"
#include "main.h"
#include "nvmem.h"
#include "update_flag.h"
#include "usart.h"
#include "xmodem.h"

static bool quiet_mode = false;

static void print_msg(const char *msg) {
  if (quiet_mode) {
    return;
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

void boot_main(void) {
  print_msg("\r\n\r\n=== STM32 Fail-Safe Bootloader ===\r\n");
  bool force_update = false;

  print_msg("[INFO] Probing for pending firmware update...\r\n");
  if (rtc_is_update_pending()) {
    print_msg("[INFO] Pending update flag detected. Attempting to copy "
              "firmware from staging to app slot...\r\n");
    if (fw_is_valid(STAGING)) {
      if (nvmem_copy(STAGING, APP) == RESULT_OK) {
        print_msg(
            "[INFO] Firmware update successful. Removing update flag.\r\n");
        rtc_clear_update_pending();
        NVIC_SystemReset();
      } else {
        print_msg("[ERROR] Firmware copy failed! Staying in bootloader.\r\n");
        while (1) {
          HAL_Delay(1000);
        }
      }
    }
  }

  print_msg("[INFO] Checking physical override button...\r\n");
  if (bootloader_check_force_update()) {
    print_msg("[INFO] Force update button held. Staying in bootloader.\r\n");
    force_update = true;
  }

  print_msg("[INFO] Validating application firmware...\r\n");
  if (force_update || !fw_is_valid(APP)) {
    print_msg("[INFO] Waiting for firmware update over XMODEM...\r\n");

    // FIX 2: Restore the vow of silence before XMODEM starts!
    quiet_mode = true;

    if (xmodem_receive_firmware()) {
      rtc_write_update_pending();
      NVIC_SystemReset();
    } else {
      // FIX 3: Turn prints back on so we can see the fatal error
      quiet_mode = false;
      print_msg("[FATAL] XMODEM timeout and APP slot is invalid. Halting.\r\n");
      while (1) {
        HAL_Delay(1000);
      }
    }
  }

  print_msg("[INFO] Jumping to App...\r\n");
  HAL_Delay(10);
  bootloader_jump_to_app();

  // We should never reach this loop unless the jump function fails
  while (1) {
    print_msg("[FATAL] Jump to application failed!\r\n");
    HAL_Delay(1000);
  }
}
