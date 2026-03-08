#include "xmodem_uart.h"
#include "main.h"

extern UART_HandleTypeDef huart2;

bool uart_read_byte(uint8_t *data, uint32_t timeout_ms) {
  if (HAL_UART_Receive(&huart2, data, 1, timeout_ms) == HAL_OK) {
    return true;
  }
  return false;
}

void uart_write_byte(uint8_t data) {
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}

void uart_flush(void) {
  uint8_t dummy;
  while (HAL_UART_Receive(&huart2, &dummy, 1, 10) == HAL_OK) {
    // Drain the rx buffer until it is empty
  }
}
