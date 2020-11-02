# Copyright (c) 2019-2020, Emil Renner Berthing
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.

MAKEFLAGS    += rR
TARGET       ?= code

# override these settings for your board/chip
HXTAL        ?= 8000000
FLASH_SIZE   ?= 128*1024
RAM_SIZE     ?= 32*1024
# override these settings to your liking
# CORECLOCK    ?= 96000000
CORECLOCK    ?= 108000000
BOOTLOADER   ?= 4*1024

O          = build
HERE      := $(dir $(lastword $(MAKEFILE_LIST)))
OS        := $(if $(ComSpec),Windows,$(shell uname -s))

ARCH       = rv32imac
ABI        = ilp32
CODEMODEL  = medlow

OPT       = -Os -flto
ARCHFLAGS = -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie
DEPENDS   = -MMD -MP
WARNINGS  = -Wall -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Wformat-truncation=2 -Wundef -Wno-unused-parameter
CFLAGS    = $(OPT) $(ARCHFLAGS) -ggdb -pipe $(DEPENDS) $(WARNINGS)
CFLAGS   += -fno-common -ffunction-sections -fdata-sections
ASFLAGS   = $(OPT) $(ARCHFLAGS) -ggdb -pipe $(DEPENDS) $(WARNINGS)
CPPFLAGS  = $(if $(HXTAL),-DHXTAL=$(HXTAL) ,)-DCORECLOCK=$(CORECLOCK)
CPPFLAGS += -DBOOTLOADER=$(BOOTLOADER) -DFLASH_SIZE=$(FLASH_SIZE) -DRAM_SIZE=$(RAM_SIZE)
CPPFLAGS += -DHXTAL_VALUE=$(HXTAL) -DGD32VF103V_EVAL=1
LDFLAGS   = $(OPT) $(ARCHFLAGS) -static -Wl,-O1,--gc-sections,--relax,--build-id=none
LDSCRIPT  = $(HERE)gd32vf103.ld

libs = adc bkp can crc dac dbg dma eclic exmc exti fmc fwdgt gpio i2c pmu rcu rtc spi timer usart wwdgt

vpath %.S $(HERE)
vpath %.c $(HERE)

ifeq ($(SPECS),)
CFLAGS   += -ffreestanding -ftls-model=local-exec
CPPFLAGS += -I$(HERE)std -D_LIBC_LIMITS_H_
LDFLAGS  += -nostdlib
LIBS     += -lgcc
else ifeq ($(SPECS),picolibc)
else
LDFLAGS  += -nostartfiles
endif

GD32FIRM := $(HERE)../GD32VF103_Firmware_Library/Firmware/
GD32UTIL := $(HERE)../GD32VF103_Firmware_Library/Utilities/
GD32PATH := $(GD32FIRM)GD32VF103_standard_peripheral/
CPPFLAGS += -I$(HERE)include/
CPPFLAGS += -I$(GD32PATH)
CPPFLAGS += -I$(GD32PATH)Include/
CPPFLAGS += -I$(GD32UTIL)
CPPFLAGS += -I$(GD32UTIL)LCD_common/
CPPFLAGS += -I$(GD32FIRM)RISCV/drivers/

CROSS_COMPILE = riscv64-unknown-elf-
CC      = $(CROSS_COMPILE)gcc $(if $(SPECS),-specs=$(SPECS).specs ,)-std=gnu11
AS      = $(CROSS_COMPILE)gcc $(if $(SPECS),-specs=$(SPECS).specs ,)-x assembler-with-cpp
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
HEX     = $(OBJCOPY) -O ihex
BIN     = $(OBJCOPY) -O binary --strip-all

ifdef ComSpec
MKDIR_P    = mkdir
RM_RF      = rmdir /q /s
echo       = @echo $1
size       = @cmd /c 'for %I in ($1) do @echo.  %~zI bytes'
else
MKDIR_P    = mkdir -p
RM_RF      = rm -rf
echo       = @echo '$1'
ifeq ($(filter-out Linux MSYS%,$(OS)),)
size       = @stat --printf '  %s bytes\n' $1
else
size       = @stat -f '  %z bytes' $1
endif
endif

OPENOCD    = openocd
STM32FLASH = stm32flash
STTY       = stty
CAT        = cat
SERIAL     = /dev/ttyUSB0

DFU_UTIL   = dfu-util
DFU_DEVICE = 1d50:613e

asm-objs := $(patsubst %.S,%.o,$(filter-out start.S,$(wildcard *.S)))
c-objs   := $(patsubst %.c,%.o,$(filter-out start.c,$(wildcard *.c)))

objs = start.o
objs += $(patsubst %,lib-%.o,$(sort $(libs)))
objs += libutil-eval.o libutil-lcd_eval.o lcd_font.o n200_func.o system.o
objs += $(sort $(asm-objs) $(c-objs))
$(info    ojbs is $(objs))

objects = $(addprefix $O/,$(objs))#$(if $(SPECS),, $O/lib-std.o)
$(info    objects is $(objects))

# use make V=1 to see raw commands or make -s for silence
ifeq ($V$(findstring s,$(word 1,$(MAKEFLAGS))),)
Q := @
else
echo =
size =
endif

.SECONDEXPANSION:
.PHONY: all release clean dump dfu romdfu uart cat

all: $O/$$(TARGET).bin

release: CPPFLAGS += -DNDEBUG
release: $O/$$(TARGET).bin

# $O/lib-std.o: CFLAGS += -fno-builtin

$O/system.o: $(GD32PATH)system_gd32vf103.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/n200_func.o: $(GD32FIRM)RISCV/drivers/n200_func.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/lcd_font.o: $(GD32UTIL)LCD_common/lcd_font.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/libutil-%.o: $(GD32UTIL)gd32vf103v_%.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/lib-%.o: $(GD32PATH)Source/gd32vf103_%.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/%.o: %.S $(MAKEFILE_LIST) | $O
	$(call echo,  AS    $<)
	$Q$(AS) -o $@ $(ASFLAGS) $(CPPFLAGS) -c $<

$O/%.o: %.c $(MAKEFILE_LIST) | $O
	$(call echo,  CC    $<)
	$Q$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -c $<

$O/$(TARGET).elf: $$(objects) $$(LDSCRIPT)
	$(call echo,  CCLD  $@ $(objects))
	$Q$(CC) -o $@ $(LDFLAGS) -T$(LDSCRIPT) $(objects) $(LIBS)

$O/%.hex: $O/%.elf
	$(call echo,  HEX   $@)
	$Q$(HEX) $< $@

$O/%.bin: $O/%.elf
	$(call echo,  BIN   $@)
	$Q$(BIN) $< $@
	$(call size,$@)

$O:
	$(call echo,  MKDIR $@)
	$Q$(MKDIR_P) $@

clean:
	$(call echo,  RM    $O)
	$Q$(RM_RF) $O

dump: $O/$$(TARGET).elf
	$(OBJDUMP) -x -d $< | $(PAGER)

dfu: $O/$$(TARGET).bin
	$Q$(DFU_UTIL) -d $(DFU_DEVICE) -D $< -R

romdfu: $O/$$(TARGET).bin
	$Q$(DFU_UTIL) -d 28e9:0189 -a 0 --dfuse-address 0x08000000:leave -D $<

cat: | $$(SERIAL)
	$(STTY) -F$(SERIAL) raw -echo -hup cs8 -parenb -cstopb 115200
	$(CAT) $(SERIAL)

-include $O/*.d
