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

bool bootloader_check_force_update(void) {
  uint32_t start_tick = HAL_GetTick();
  const uint32_t required_hold_time = 2000; // 2 seconds

  // If button isn't even pressed at start, skip immediately
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) {
    return false; // Proceed to normal boot
  }

  // Button is held, wait to see if held
  while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
    if ((HAL_GetTick() - start_tick) >= required_hold_time) {
      return true; // Special status: Enter Update Mode
    }
  }

  return false; // Button released too early, boot normally
}
