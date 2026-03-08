#ifndef XMODEM_UART_H
#define XMODEM_UART_H

#include <stdbool.h>
#include <stdint.h>

// The Timed Read
// Returns true if a byte was successfully received before timeout_ms elapsed.
// Drops the received byte into the memory address pointed to by 'data'.
bool uart_read_byte(uint8_t *data, uint32_t timeout_ms);

bool uart_read_bytes(uint8_t *data, uint16_t length, uint32_t timeout_ms);

// The Single Byte Transmit
// Blocks just long enough to push exactly one byte out of the TX pin.
void uart_write_byte(uint8_t data);

// The Hardware Flush
// Rapidly reads and discards incoming bytes until the UART RX line goes silent.
void uart_flush(void);

#endif /* XMODEM_UART_H */
