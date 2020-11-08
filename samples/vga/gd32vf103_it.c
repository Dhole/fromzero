/*!
    \file  gd32vf103_it.c
    \brief interrupt service routines

    \version 2019-06-05, V1.0.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/
#include "gd32vf103.h"
#include "gd32vf103v_eval.h"
#include "gd32vf103_timer.h"
#include "gd32vf103_eclic.h"

#include "config.h"

enum sync_state {
    FRONT_PORCH,
    SYNC_PULSE,
    BACK_PORCH,
    ACTIVE_VIDEO,
};

static enum sync_state v_state = FRONT_PORCH;

// static int current = 0;
// static volatile int inside = 0;
static uint32_t line = 0;

/**
  * @brief  This function handles TIMER1 interrupt request.
  * @param  None
  * @retval None
  */
void TIMER1_IRQHandler(void)
{
    if ((TIMER_INTF(TIMER1) & TIMER_INT_FLAG_UP)) {
        // eclic_clear_pending(TIMER1_IRQn);
        // eclic_disable_interrupt(TIMER1_IRQn);
        // if (inside) {
        //     return;
        // }
        // inside = 1;
        // FlagStatus current;
        // if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)){
        TIMER_INTF(TIMER1) = (~(uint32_t)TIMER_INT_FLAG_UP);
            /* clear channel 0 interrupt bit */
            // timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
            // current = gpio_output_bit_get(GPIOA, GPIO_PIN_1);
            // current = gpio_output_bit_get(HSYNC_PORT, HSYNC_PIN);
            // if (current) {
            //     // gpio_bit_reset(GPIOA, GPIO_PIN_1);
            //     gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
            //     current = 0;
            // } else {
            //     // gpio_bit_set(GPIOA, GPIO_PIN_1);
            //     gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
            //     current = 1;
            // }
            // TIMER_CNT(TIMER1) = 700 * 5;
            // TIMER_INTF(TIMER1) = (~(uint32_t)TIMER_INT_FLAG_UP);
        // }
        // inside = 0;
        // switch (h_state) {
        // case FRONT_PORCH:
        //     gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        //     TIMER_CAR(TIMER1) = (uint32_t) H_FRONT_PORCH * PIXEL_FREQ_MUL - 1 - 0;
        //     h_state = SYNC_PULSE;
        //     break;
        // case SYNC_PULSE:
        //     gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
        //     TIMER_CAR(TIMER1) = (uint32_t) H_SYNC_PULSE * PIXEL_FREQ_MUL - 1 - 0;
        //     h_state = BACK_PORCH;
        //     break;
        // case BACK_PORCH:
        //     gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        //     TIMER_CAR(TIMER1) = (uint32_t) H_BACK_PORCH * PIXEL_FREQ_MUL - 1 - 0;
        //     h_state = ACTIVE_VIDEO;
        //     break;
        // case ACTIVE_VIDEO:
        //     // gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        //     gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
        //     TIMER_CAR(TIMER1) = (uint32_t) H_ACTIVE_VIDEO * PIXEL_FREQ_MUL - 1 - 0;
        //     h_state = FRONT_PORCH;
        //     break;
        // }

        // volatile uint32_t cnt;
        // do {
        //     cnt = timer_counter_read(TIMER1);
        // } while (cnt > 10);

        // volatile uint32_t cnt0, cnt1, cnt2, cnt3;
        // cnt0 = timer_counter_read(TIMER1);
        // cnt1 = timer_counter_read(TIMER1);
        // cnt2 = timer_counter_read(TIMER1);
        // cnt3 = timer_counter_read(TIMER1);
        // if (cnt0 < 10) {
        //         gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
        // } else {
        //         gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        // }
        // if (timer_counter_read(TIMER1) == 0) {
        //     return;
        // }

        gpio_bit_reset(GREEN_PORT, GREEN_PIN);

        if (line < V_FRONT_PORCH) {
            // V_FRONT_PORCH
            gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else if (line < V_FRONT_PORCH + V_SYNC_PULSE) {
            // V_SYNC_PULSE
            gpio_bit_reset(VSYNC_PORT, VSYNC_PIN);
        } else if (line < V_FRONT_PORCH + V_SYNC_PULSE + V_BACK_PORCH) {
            // V_BACK_PORCH
            gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else {
            // V_ACTIVE_VIDEO
            gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        }

        timer_counter_value_config(TIMER1, 0);

        // // H_FRONT_PORCH
        gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < H_FRONT_PORCH * PIXEL_FREQ_MUL - 10);

        // H_SYNC_PULSE
        gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE) * PIXEL_FREQ_MUL - 10);

        // // H_BACK_PORCH
        gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH) * PIXEL_FREQ_MUL - 16);

        // H_ACTIVE_VIDEO
        gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        if (line >= V_FRONT_PORCH + V_SYNC_PULSE + V_BACK_PORCH) {
            if (line % 16 == 0) {
                gpio_bit_set(GREEN_PORT, GREEN_PIN);
            } else {
                gpio_bit_reset(GREEN_PORT, GREEN_PIN);
            }
        }

        line++;
        if (line == V_FRAME) {
            line = 0;
        }
    }
}
