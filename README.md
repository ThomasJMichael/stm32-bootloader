# STM32F4 Fail-Safe Bootloader

A bare-metal, dual-slot bootloader for STM32F401 microcontrollers featuring A/B style staging, hardware CRC32 validation, and XMODEM-1K over-the-air (OTA) firmware updates.

## Memory Map

The STM32F401xE internal flash (512 KB) is partitioned into three operational sectors:

| Partition | Start Address | Size | Description |
| :--- | :--- | :--- | :--- |
| **Bootloader** | `0x08000000` | 32 KB | Bootloader executable. Executes on hardware reset. |
| **App Slot** | `0x08008000` | 224 KB | Active, validated application firmware. |
| **Staging Slot** | `0x08040000` | 224 KB | Temporary holding area for OTA updates. |

---

## Building a Compatible Application

To ensure the bootloader can successfully validate and execute a target firmware image, the application must adhere to the following hardware and compiler constraints. Failure to implement these will result in a hardware fault upon branching.

### 1. Linker Script Configuration
The bootloader expects a 512-byte (`0x200`) metadata header prepended to the raw application binary. The application's linker script must be offset to account for both the bootloader partition and this header padding.

Update the `FLASH` origin in `STM32F401XX_FLASH.ld`:
```ld
MEMORY
{
  RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 96K
  /* App slot base (0x08008000) + 512-byte header (0x200) */
  FLASH (rx)     : ORIGIN = 0x08008200, LENGTH = 224K - 0x200
}

```

### 2. VTOR Realignment

The ARM Cortex-M4 requires the Vector Table Offset Register (VTOR) to be naturally aligned to a 512-byte boundary. The bootloader leaves the CPU executing from the application's starting address, but VTOR must be explicitly remapped by the application code to redirect hardware interrupts.

### 3. Interrupt Re-enabling

The bootloader sets the `PRIMASK` register (`__disable_irq()`) prior to branching to ensure a sterile execution environment. The application must re-enable global interrupts early in its initialization sequence to restore functionality to the SysTick timer and other peripherals.

In the application's `main.c`:

```c
int main(void) {
    /* 1. Shift the interrupt vector table to the application space */
    SCB->VTOR = 0x08008200; 
    
    /* 2. Clear the bootloader's global interrupt mask */
    __enable_irq(); 
    
    /* MCU Configuration */
    HAL_Init();
    // ...
}

```

---

## Firmware Header Specification

Firmware updates must be encapsulated with a 512-byte header before transmission. This header is validated by the bootloader prior to booting the app or migrating data from the staging slot.

**Header Structure (Little-Endian):**

* **Magic Word:** `0xBADC0FFE` (uint32)
* **Version:** `uint32`
* **Payload Size:** `uint32` (Must be a multiple of 4 bytes)
* **CRC32 Checksum:** `uint32`
* **Padding:** 496 bytes of `0xFF`

**CRC Algorithm:**
The checksum must be calculated using the native **STM32 Hardware CRC32** algorithm (Ethernet Polynomial `0x04C11DB7`), processing the raw application binary in 32-bit (4-byte) chunks. If the raw binary byte length is not a perfect multiple of 4, it must be padded with `0xFF` prior to CRC calculation.

---

## Boot Sequence & Operation

Communication occurs over `USART2` at **115200 baud (8-N-1)**.

### Standard Boot

1. The MCU initializes and probes the RTC Backup Registers for a pending update flag.
2. If flagged and the Staging slot is valid, the MCU securely copies the Staging slot to the App slot.
3. The MCU validates the App slot's Magic Word and CRC32.
4. If valid, tears down peripherals and branches to `0x08008200`.

### Forced Update Mode (XMODEM)

To manually force the bootloader into serial receive mode (e.g., to recover a corrupted application):

1. Press and hold the **User Button** (PC13).
2. Assert and release the **Reset Button**.
3. Continue holding the User Button for > 2 seconds.

The bootloader will begin emitting `C` characters over UART, indicating it is ready to receive an XMODEM-1K payload.

*(Note: A companion host-side CLI tool for automated header packing and serial flashing is currently in development and will be linked here upon release).*

