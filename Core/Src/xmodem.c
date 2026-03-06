#include "xmodem.h"
#include "main.h"
#include "nvmem.h"
#include "stm32f4xx_hal.h"
#include "xmodem_uart.h"

extern const PartitionInfo Partitions[];

static uint8_t packet_number = 1;
static uint32_t flash_write_offset = 0;
static uint8_t packet_buffer[XMODEM_BUFFER_SIZE];
static uint16_t current_payload_size = 0;
static uint8_t error_count = 0;

uint16_t calculate_xmodem_crc16(const uint8_t *data, uint16_t length) {
  uint16_t crc = 0;
  for (uint16_t i = 0; i < length; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

static xmodem_state_t handle_sync_state(void) {
  uart_write_byte(XMODEM_C); // Request CRC mode
  return STATE_RECEIVE;
}

static xmodem_state_t handle_recieve_state(void) {
  uint8_t steering_byte;
  if (!uart_read_byte(&steering_byte, SYNC_TIMEOUT_MS)) {
    // Timeout. If we are on first packet, we send sync byte again. Otherwise,
    // we attempt a retry with NAK
    uart_write_byte((packet_number == 1) ? XMODEM_C : XMODEM_NAK);
    ERROR_ABORT_IF_MAX_REACHED();

    return STATE_RECEIVE;
  }

  switch (steering_byte) {
  case XMODEM_SOH:
    current_payload_size = 128;
    break;
  case XMODEM_STX:
    current_payload_size = 1024;
    break;
  case XMODEM_EOT:
    uart_write_byte(XMODEM_ACK);
    return STATE_COMPLETE;
  default:
    // We received something unexpected. Flush the UART and attempt to resync.
    uart_flush();
    uart_write_byte((packet_number == 1) ? XMODEM_C : XMODEM_NAK);
    ERROR_ABORT_IF_MAX_REACHED();
    return STATE_RECEIVE;
  }

  uint16_t expected_packet_size =
      current_payload_size + 4; // Header(2) + Payload + CRC(2)
  for (int i = 0; i < expected_packet_size; i++) {
    if (!uart_read_byte(&packet_buffer[i], 1000)) {
      uart_flush(); // Something went wrong during packet reception. Flush and
                    // attempt retry.
      uart_write_byte(XMODEM_NAK);
      ERROR_ABORT_IF_MAX_REACHED();
      return STATE_RECEIVE;
    }
  }
  return STATE_VALIDATE;
}
static xmodem_state_t handle_validate_state(void) {
  // Byte 0 and Byte 1 must always be exact logical opposites.
  if ((packet_buffer[0] + packet_buffer[1]) != 0xFF) {
    uart_flush();
    uart_write_byte(XMODEM_NAK);
    ERROR_ABORT_IF_MAX_REACHED();
    return STATE_RECEIVE;
  }

  // Lost ACK?
  if (packet_buffer[0] == (uint8_t)(packet_number - 1)) {
    uart_flush();
    uart_write_byte(XMODEM_ACK);
    return STATE_RECEIVE;
  }

  // Fatal Sequence Mismatch (Did we skip a packet entirely?)
  if (packet_buffer[0] != packet_number) {
    uart_flush();
    uart_write_byte(XMODEM_CAN);
    return STATE_ABORT;
  }

  uint16_t expected_crc = (packet_buffer[current_payload_size + 2] << 8) |
                          packet_buffer[current_payload_size + 3];

  uint16_t calculated_crc =
      calculate_xmodem_crc16(&packet_buffer[2], current_payload_size);

  if (calculated_crc != expected_crc) {
    uart_flush();
    uart_write_byte(XMODEM_NAK);
    ERROR_ABORT_IF_MAX_REACHED();
    return STATE_RECEIVE;
  }

  return STATE_PROCESS;
}
static xmodem_state_t handle_process_state(void) {
  if (nvmem_write(STAGING, flash_write_offset, &packet_buffer[2],
                  current_payload_size) != RESULT_OK) {
    uart_flush();
    uart_write_byte(XMODEM_CAN);
    return STATE_ABORT;
  }

  flash_write_offset += current_payload_size;
  error_count = 0;
  packet_number++;

  uart_write_byte(XMODEM_ACK);
  return STATE_RECEIVE;
}

static const xmodem_state_func_t state_table[] = {
    [STATE_SYNC] = handle_sync_state,
    [STATE_RECEIVE] = handle_recieve_state,
    [STATE_VALIDATE] = handle_validate_state,
    [STATE_PROCESS] = handle_process_state};

bool xmodem_receive_firmware(void) {
  packet_number = 1;
  flash_write_offset = 0;
  error_count = 0;
  current_payload_size = 0;

  xmodem_state_t current_state = STATE_SYNC;

  while (current_state != STATE_COMPLETE && current_state != STATE_ABORT) {
    current_state = state_table[current_state]();
  }

  return (current_state == STATE_COMPLETE);
}
