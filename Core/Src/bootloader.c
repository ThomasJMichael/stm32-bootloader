#include "bootloader.h"
#include "main.h"
#include "usart.h"

void bootloader_jump_to_app(void) {
  // De-initialize peripherals used by the bootloader
  HAL_UART_MspDeInit(&huart2);

  // Tear down HAL
  HAL_DeInit();

  // Reset Clock configuration to default state
  HAL_RCC_DeInit();
  // Disable all interrupts
  __disable_irq();

  // Kill SysTick to prevent interrupts during the jump
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  uint32_t app_stack_ptr = *((volatile uint32_t *)APP_START_ADDR);
  uint32_t app_entry_point = *((volatile uint32_t *)(APP_START_ADDR) + 1);

  // Hardware stack ptr
  __set_MSP(app_stack_ptr);

  // Jump to application entry point
  ((void (*)(void))app_entry_point)();
}
