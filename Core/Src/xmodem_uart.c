#include "xmodem_uart.h"
#include "main.h"

extern UART_HandleTypeDef huart2;

// The Timed Read
// Returns true if a byte was successfully received before timeout_ms elapsed.
// Drops the received byte into the memory address pointed to by 'data'.
bool uart_read_byte(uint8_t *data, uint32_t timeout_ms) {
  if (HAL_UART_Receive(&huart2, data, 1, timeout_ms) == HAL_OK) {
    return true; // Byte received successfully
  }
  return false; // Timeout or error occurred
}

// The Single Byte Transmit
// Blocks just long enough to push exactly one byte out of the TX pin.
void uart_write_byte(uint8_t data) {
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);
}

// The Hardware Flush
// Rapidly reads and discards incoming bytes until the UART RX line goes silent.
void uart_flush(void) {
  uint8_t dummy;
  while (HAL_UART_Receive(&huart2, &dummy, 1, 10) == HAL_OK) {
    // Keep reading until no more bytes are received (timeout)
  }
}
