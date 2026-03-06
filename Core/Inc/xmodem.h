#ifndef XMODEM_H
#define XMODEM_H

#include <stdbool.h>

#define XMODEM_SOH 0x01 // Start of Header (128 bytes)
#define XMODEM_STX 0x02 // Start of Text (1024 bytes)
#define XMODEM_EOT 0x04 // End of Transmission
#define XMODEM_ACK 0x06 // Acknowledge
#define XMODEM_NAK 0x15 // Negative Acknowledge
#define XMODEM_CAN 0x18 // Cancel
#define XMODEM_C 0x43   // 'C' - Request start in CRC mode

#define XMODEM_BUFFER_SIZE 1028
#define SYNC_TIMEOUT_MS 3000 // 3 seconds to wait for initial sync
//
#define MAX_ERROR_COUNT 10
#define ERROR_ABORT_IF_MAX_REACHED()                                           \
  do {                                                                         \
    error_count++;                                                             \
    if (error_count >= MAX_ERROR_COUNT) {                                      \
      uart_write_byte(XMODEM_CAN);                                             \
      return STATE_ABORT;                                                      \
    }                                                                          \
  } while (0)

typedef enum {
  STATE_SYNC = 0,
  STATE_RECEIVE,
  STATE_VALIDATE,
  STATE_PROCESS,
  STATE_COMPLETE,
  STATE_ABORT
} xmodem_state_t;

// Every state must return the next state to transition to
typedef xmodem_state_t (*xmodem_state_func_t)(void);

bool xmodem_receive_firmware(void);
#endif /* XMODEM_H */
