#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "stdbool.h"
#include <stdint.h>

/* Linker symbol defined in the linker script, marking the start of the
 * application code. The bootloader will jump to this address after it finishes
 * its tasks.
 */
extern uint32_t __app_start;

#define APP_START_ADDR ((uint32_t)&__app_start)

/* Physical address of the application start.
 * This is where the bootloader will jump to after it finishes its tasks.
 */
void bootloader_jump_to_app(void);

bool bootloader_check_force_update(void);

#endif
