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
// #include "lib/mtimer.h"
// #include "lib/eclic.h"
// #include "lib/rcu.h"
// #include "lib/gpio.h"
#include <stdint.h>

// const uint32_t RCU_APB2EN_PAEN = 1 << 2;
// const uint32_t GPIO_MODE_PP_50MHZ = 0x3;
// 
// const uint32_t LED_GPIO_PORT = 0;
// const uint32_t LED_GPIO_NUM = 1;
// const uint32_t GPIOA_BASE = 0x40010800;
// 
// struct gd32vf103_gpio {
// 	volatile uint32_t CTL0; // 0
// 	volatile uint32_t CTL1; // 1
// 	volatile uint32_t ISTAT; // 2
// 	volatile uint32_t OCTL; // 3
// 	volatile uint32_t BOP; // 4
// 	volatile uint32_t BC; // 5
// 	volatile uint32_t LOCK; // 6
// };
// 
// static inline struct gd32vf103_gpio *
// gpio_pin_port(uint32_t port)
// {
// 	return (struct gd32vf103_gpio *)(GPIOA_BASE + port*0x400UL);
// }
// 
// void _gpio_pin_config(uint32_t port, uint32_t num, uint32_t emode)
// {
// 	uint32_t mode = emode;
// 	volatile uint32_t *reg = &gpio_pin_port(port)->CTL0;
// 	unsigned int nr = num;
// 
// 	if (nr >= 8) {
// 		nr -= 8;
// 		reg++;
// 	}
// 	nr <<= 2;
// 	// nr = 0;
// 	mode = 0x3;
// 	*reg = (*reg & ~(0xfU << nr)) | (mode << nr);
// }

int main(void)
{
	/* initialize system clock */
	// rcu_sysclk_init();

	/* initialize eclic */
	// eclic_init();
	/* enable global interrupts */
	// eclic_global_interrupt_enable();

	/* turn on power to GPIOA */
	volatile uint32_t *RCU_APB2EN = (uint32_t *) (0x40021000 + 0x18);
	*RCU_APB2EN |= 1 << 2;

	volatile uint32_t *GPIOA_CRL = (uint32_t *) (0x40010800 + 0*4);
	*GPIOA_CRL = 0x44444434;
	// _gpio_pin_config(LED_GPIO_PORT, LED_GPIO_NUM, GPIO_MODE_PP_50MHZ);

	uint32_t lo = 0x00000000;
	uint32_t hi = 0x00000002;
	volatile uint32_t *GPIOA_ODR = (uint32_t *) (0x40010800 + 3*4);

	int LEDDELAY = 400000;
	volatile int i;

loop:
	// set
	// gpio_pin_port(LED_GPIO_PORT)->BOP = 1U << LED_GPIO_NUM;
	*GPIOA_ODR = lo;
	i = LEDDELAY;

delay1:
	i -= 1;
	if (i != 0) {
		goto delay1;
	}
	// clear
	// gpio_pin_port(LED_GPIO_PORT)->BC = 1U << LED_GPIO_NUM;
	*GPIOA_ODR = hi;
	i = LEDDELAY;

delay2:
	i -= 1;
	if (i != 0) {
		goto delay2;
	}
	goto loop;
}
