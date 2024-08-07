(.org 0x0800_0000)
    (lui t0 (.hi reset))
    (jalr zero t0 (.lo reset))
(.org 0x0800_015c)

($ 'reset)
    (csrrs a0 zero 0x346)              ; start.S:60  jalr zero, a0, %lo(_start) ; [02] -r-x section size 244 named .text
    (beq a0 zero (@ start))          ; start.S:258  beqz a0, 0f
    (lui a1 (.hi 0xe0042000))             ; start.S:260  lui a1, DBG_BASE >> 12
    (lui a0 (.hi 0x4b5a7000))             ; start.S:261  li a0, DBG_KEY_UNLOCK
    (addi a0 a0 -1672)
    (sw a0 a1 12)               ; start.S:262  sw a0, DBG_KEY(a1)
    (addi a0 zero 1)                    ; start.S:263  li a0, DBG_CMD_RESET
    (sw a0 a1 8)                ; start.S:264  sw a0, DBG_CMD(a1)

($ 'start)
    ; [RCU_APB2EN] |= 1 << 2
    (lui t0 (.hi RCU_APB2EN))
    (lw t0 t0 (.lo RCU_APB2EN))
    (lw t1 t0 0)
    (ori t1 t1 0x4)
    (sw t1 t0 0)

    ; [GPIOA_CRL] = GPIOA_CONF
    (lui t0 (.hi GPIOA_CRL))
    (lw t0 t0 (.lo GPIOA_CRL))
    (lui t1 (.hi GPIOA_CONF))
    (lw t1 t1 (.lo GPIOA_CONF))
    (sw t1 t0 0)

    (addi t0 zero 0x0) ; t0 = lo
    (addi t1 zero 0x2) ; t1 = hi
    (lui t2 (.hi GPIOA_ODR)) ; t2 = GPIOA_ODR
    (lw t2 t2 (.lo GPIOA_ODR)) ; t2 = GPIOA_ODR

($ 'loop)
    (sw t0 t2 0) ; [GPIOA_ODR] = lo
    (lui t3 (.hi LEDDELAY)) ; for (t3 = LEDDELAY; t3 != 0; t3--) {}
    (lw t3 t3 (.lo LEDDELAY)) ; for (t3 = LEDDELAY; t3 != 0; t3--) {}
($ 'delay1)
    (addi t3 t3 -1)
    (bne t3 zero (@ delay1))

    (sw t1 t2 0) ; [GPIOA_ODR] = hi
    (lui t3 (.hi LEDDELAY)) ; for (t3 = LEDDELAY; t3 != 0; t3--) {}
    (lw t3 t3 (.lo LEDDELAY)) ; for (t3 = LEDDELAY; t3 != 0; t3--) {}
($ 'delay2)
    (addi t3 t3 -1)
    (bne t3 zero (@ delay2))

    (beq t3 zero (@ loop))

($ 'GPIOA_CRL)
    (.word 0x40010800)
($ 'GPIOA_ODR)
    (.word 0x4001080c)
($ 'RCU_APB2EN)
    (.word 0x40021018)
($ 'LEDDELAY)
    (.word 200000)
($ 'GPIOA_CONF)
    (.word 0x44444434)
