@@@ led-stm32.asm
@@@ written by Frank Sergeant
@@@    frank@pygmy.utoh.org
@@@    http://pygmy.utoh.org/riscy
@@@ modified by Dhole to support the BluePill (STM32F103)
@@@ This program is in the public domain.  See http://pygmy.utoh.org/riscy/cortex/
@@@ for notes about the program and how to assemble, link, and burn to flash.

@@@ Blink the LED on the BluePill STM32F103 ARM Cortex M3 board.

@@@ Directives
        .thumb                  @ (same as saying '.code 16')
        .syntax unified

@@@ Equates
        .equ GPIOC_CRH,   0x40011004
        .equ GPIOC_ODR,   0x4001100C
        .equ RCC_APB2ENR, 0x40021018
        .equ STACKINIT,   0x20005000

        .equ LEDDELAY,    8000000

.section .text
        .org 0

@@@ Vectors
vectors:
        .word STACKINIT         @ stack pointer value when stack is empty
        .word _start + 1        @ reset vector (manually adjust to odd for thumb)
        .word _nmi_handler + 1  @
        .word _hard_fault  + 1  @
        .word _memory_fault + 1 @
        .word _bus_fault + 1    @
        .word _usage_fault + 1  @

_start:

        @@ Enable the Port C peripheral clock by setting bit 4
        ldr r6, = RCC_APB2ENR
        movs r0, 0x10
        str r0, [r6]

        @@ Set the config and mode bits for Port C bit 13 so it will
        @@ be a push-pull output (up to 50 MHz) by setting bits 23-20
        @@ to '0011'.

        ldr r6, = GPIOC_CRH
        ldr r0, = 0x44344444
        str r0, [r6]

        @@ Load R2 and R3 with the "on" and "off" constants
        movs r2, 0              @ value to turn on LED
        movs r3, 0x20 @ value to turn off LED
        lsls r3, r3, 8

        ldr r6, = GPIOC_ODR    @  point to Port C output data register

loop:
        str r2, [r6]           @ clear Port C, pin 13, turning on LED
        ldr r1, = LEDDELAY
delay1:
        subs r1, 1
        bne delay1

        str r3, [r6]           @ set Port C, pin 13, turning off LED
        ldr r1, = LEDDELAY
delay2:
        subs r1, 1
        bne delay2

        b loop                 @ continue forever

_dummy:                        @ if any int gets triggered, just hang in a loop
_nmi_handler:
_hard_fault:
_memory_fault:
_bus_fault:
_usage_fault:
        movs r0, 1
        add r1, r0
        add r2, r0
        b _dummy
