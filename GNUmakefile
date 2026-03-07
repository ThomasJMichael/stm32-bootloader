include Makefile

CUSTOM_SOURCES = \
Core/Src/nvmem.c \
Core/Src/bootloader_main.c \
Core/Src/bootloader.c \
Core/Src/update_flag.c \
Core/Src/fw_verify.c \
Core/Src/xmodem.c \
Core/Src/xmodem_uart.c

# Append to C_SOURCES so the linker command includes them in the final string
C_SOURCES += $(CUSTOM_SOURCES)

# Calculate the .o file paths for our custom files
CUSTOM_OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(CUSTOM_SOURCES:.c=.o)))

# Force the ELF target to explicitly depend on our new objects
# This catches the prerequisites that the base Makefile missed during Pass 1
$(BUILD_DIR)/$(TARGET).elf: $(CUSTOM_OBJECTS)
