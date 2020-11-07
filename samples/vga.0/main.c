/*
 * Copyright (c) 2019, Emil Renner Berthing
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */
#include "gd32vf103/mtimer.h"
#include "lib/mtimer.h"
#include "lib/eclic.h"
#include "lib/rcu.h"
#include "lib/gpio.h"

// #define BLINK MTIMER_FREQ /* 1 second */
volatile uint32_t blink;

#define LED GPIO_PA1

void MTIMER_IRQHandler(void)
{
	uint64_t next;

	gpio_pin_toggle(LED);

	next = mtimer_mtimecmp() + blink;
	MTIMER->mtimecmp_hi = next >> 32;
	MTIMER->mtimecmp_lo = next;
}

/* if the compiler can't generate functions suitable
 * for interrupt handlers, we can't implement this
 * function directly in C
 */
#ifdef __interrupt
void trap_entry(void)
{
	gpio_pin_clear(LED);

	while (1)
		/* forever */;
}
#endif

static void mtimer_enable(void)
{
	uint64_t next = mtimer_mtime() + blink;

	MTIMER->mtimecmp_hi = next >> 32;
	MTIMER->mtimecmp_lo = next;

	eclic_config(MTIMER_IRQn, ECLIC_ATTR_TRIG_LEVEL, 1);
	eclic_enable(MTIMER_IRQn);
}

int main(void)
{
	blink = MTIMER_FREQ/4;
	/* initialize system clock */
	// rcu_sysclk_init();

	// HXTAL == 8_000_000
	// By running at 80/100 MHz we lose the ability to use the USB
	// peripherial, which needs 48 Mhz, but that's OK because we don't use
	// USB.
	/*
	// CK_SYS = 100_000_000
	// CK_APB1 = 50_000_000
	// HXTAL -> [PREDV0 /2] -> [PLLMF x25] -> 100 MHz
	rcu_sysclk_hxtal(RCU_CFG0_PLLMF_MUL25 |
			RCU_CFG0_PLLSEL |
			RCU_CFG0_APB1PSC_DIV2,
			RCU_CFG1_PREDV0_DIV2);
	*/
	// CK_SYS =  80_000_000
	// CK_APB1 = 40_000_000
	// HXTAL -> [PREDV0 /2] -> [PLLMF x20] -> 80 MHz
	rcu_sysclk_hxtal(RCU_CFG0_PLLMF_MUL20 |
			RCU_CFG0_PLLSEL |
			RCU_CFG0_APB1PSC_DIV2,
			RCU_CFG1_PREDV0_DIV2);

	/* initialize eclic */
	eclic_init();
	/* enable global interrupts */
	eclic_global_interrupt_enable();

	/* turn on power to GPIOA */
	RCU->APB2EN |= RCU_APB2EN_PAEN;

	gpio_pin_set(LED);
	gpio_pin_config(LED, GPIO_MODE_PP_50MHZ);

	mtimer_enable();

	while (1)
		wait_for_interrupt();
}
