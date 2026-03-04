# 1. Import the entire CubeMX-generated Makefile
include Makefile

# Src
C_SOURCES += Core/Src/nvmem.c
C_SOURCES += Core/Src/rtc_flag.c

# Re-evaluate the vpath directive
# This forces Make to re-scan our newly added directories.
vpath %.c $(sort $(dir $(C_SOURCES)))
