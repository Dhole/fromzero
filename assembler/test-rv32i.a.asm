(.s 'a00)
    (add x1 x2 x3)
    (add ra sp gp)
(.s 'a04)
    (sub x2 x3 x4)
    (xor x4 x12 x15)

    (addi x1 x2 0x12)

(.s 'a16)
    (lb x1 x2 0x14)

    (sb x5 x3 0x23)

    (beq x4 x8 0x22)

; (.s arm-thumb)
;     (.equ GPIOC_CRH 0x40011004)
; 
;     (.section .text)
;     (.org 0)
; (.s vectors)
;     (.word STACKINIT)
;     (.word (+ _start 1))
;     (.word (+ _nmi_handler 1))
;     (.word (+ _hard_fault 1))
;     (.word (+ _memory_fault 1))
;     (.word (+ _bus_fault 1))
;     (.word (+ _usage_fault 1))
; (.s _start)
;     (ldr r6 RCC_APB2ENR)
;     (mov r0 0x10)
;     (str r0 r6)
;     (ldr r6 GPIOC_CRH)
;     (ldr r0 0x44434444)
;     (str r0 r6)
; 
; (.s loop)
;     (str r2 r6)           ; clear Port C, pin 12, turning on LED
;     (ldr r1 LEDDELAY)
; (.s delay1)
;     (subs r1 1)
;     (bne delay1)
; 
;     (str r3 r6)           ; set Port C, pin 12, turning off LED
;     (ldr r1 LEDDELAY)
; (.s delay2)
;     (subs r1 1)
;     (bne delay2)
; 
;     (b loop)                 ; continue forever
