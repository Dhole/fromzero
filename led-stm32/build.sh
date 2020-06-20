#!/bin/sh

set -e

# Using cortex-m1 to force thumb1 instructions
arm-none-eabi-as -mcpu=cortex-m1 -mthumb -mapcs-32 -gstabs -ahls=led-stm32.lst -o led-stm32.o led-stm32.s
arm-none-eabi-ld -v -T stm32.ld -nostartfiles -o led-stm32.elf led-stm32.o
arm-none-eabi-objcopy -O binary led-stm32.elf  led-stm32.bin
