#!/bin/bash

# Define variables
BIN_FILE="build/bootloader.bin"
FLASH_ADDR="0x08000000"
BAUD_RATE="115200"
PORT="/dev/ttyACM0"

flash() {
  echo "==> Building project..."
  make -j$(nproc) # Compiles using all available CPU cores for speed

  if [ $? -ne 0 ]; then
    echo "==> Build failed! Aborting flash."
    exit 1
  fi

  echo "==> Flashing to $FLASH_ADDR..."
  st-flash write $BIN_FILE $FLASH_ADDR
}

monitor() {
  echo "==> Connecting to $PORT at $BAUD_RATE baud..."
  # Sleep for just a moment to let the USB re-enumerate if it just flashed
  sleep 0.5
  picocom -b $BAUD_RATE $PORT
}

# Parse command line arguments
case "$1" in
flash)
  flash
  ;;
monitor)
  monitor
  ;;
all)
  flash
  monitor
  ;;
*)
  echo "Usage: ./dev.sh {flash|monitor|all}"
  exit 1
  ;;
esac
