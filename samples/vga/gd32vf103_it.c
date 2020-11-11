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
#include "gd32vf103_spi.h"

#include "config.h"
#include "font.h"

// #define  ARRAYSIZE         10
// uint8_t array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};

// #define SIZE 9
// uint8_t data[9][SIZE] = {
//   {0x88,0x6,0x18,0x0,0x8,0x80,0x1,0x80,0x80,},
//   {0x88,0x2,0x8,0x0,0x8,0x80,0x0,0x80,0x80,},
//   {0x89,0xc2,0x8,0x70,0x8,0x9c,0xb8,0x87,0x80,},
//   {0xfa,0x22,0x8,0x88,0x8,0xa2,0xc0,0x88,0x80,},
//   {0x8b,0xe2,0x8,0x88,0xa,0xa2,0x80,0x88,0x80,},
//   {0x8a,0x2,0x8,0x88,0xa,0xa2,0x80,0x88,0x80,},
//   {0x8a,0x2,0x8,0x88,0xd,0xa2,0x80,0x88,0x80,},
//   {0x89,0xe7,0x1c,0x70,0x8,0x9c,0x81,0xc7,0x80,},
//   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},
// };

// uint8_t *next_line[2] = {data[0], data[0]};

#define LINE_LEN (320/8)
#define TEXT_W (320/8)
#define TEXT_H (240/8)
uint8_t lines[2][320/8] = {
  {
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
  },
  {
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
  },
};
char text[TEXT_H][TEXT_W];
// uint8_t next_line_index = 0;
uint8_t *next_line = lines[0];

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
    int y;
    if ((TIMER_INTF(TIMER1) & TIMER_INT_FLAG_UP)) {
        TIMER_INTF(TIMER1) = (~(uint32_t)TIMER_INT_FLAG_UP);

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

        // gpio_bit_reset(GREEN_PORT, GREEN_PIN);

        if (line < V_ACTIVE_VIDEO) {
            // V_ACTIVE_VIDEO
            // gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else if (line < V_ACTIVE_VIDEO + V_FRONT_PORCH) {
            // V_FRONT_PORCH
            // gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else if (line < V_ACTIVE_VIDEO + V_FRONT_PORCH + V_SYNC_PULSE) {
            // V_SYNC_PULSE
            gpio_bit_reset(VSYNC_PORT, VSYNC_PIN);
        } else {
            // V_BACK_PORCH
            gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        }

        // timer_counter_value_config(TIMER1, 0);

        // // H_FRONT_PORCH
        // gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < H_FRONT_PORCH * PIXEL_FREQ_MUL);

        // H_SYNC_PULSE
        gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE) * PIXEL_FREQ_MUL);

        // // H_BACK_PORCH
        gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH - 11) * PIXEL_FREQ_MUL);

        int i;
        // H_ACTIVE_VIDEO
        // gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        if (line < V_ACTIVE_VIDEO) {
        // if (line == 10) {

            // DMA_CHMADDR(DMA0, DMA_CH2) = next_line[line % 2];
            // DMA_CHCNT(DMA0, DMA_CH2) = (SIZE & DMA_CHANNEL_CNT_MASK);
            // dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
            // dma_channel_disable(DMA0, DMA_CH2);
            // dma_channel_enable(DMA0, DMA_CH2);
            // spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
            // next_line[(line+1)%2] = data[(line/2) % 9];

            DMA_CHMADDR(DMA0, DMA_CH2) = lines[(line / 2) % 2];
            // DMA_CHMADDR(DMA0, DMA_CH2) = lines[0];
            DMA_CHCNT(DMA0, DMA_CH2) = (LINE_LEN & DMA_CHANNEL_CNT_MASK);
            dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
            dma_channel_disable(DMA0, DMA_CH2);
            dma_channel_enable(DMA0, DMA_CH2);
            spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
            if (line % 2 == 1) {
              y = ((line + 1) / 2) % 240;
              next_line = lines[y % 2];
              for (i = 0; i < TEXT_W; i++) {
                next_line[i] = font_8x8[(int)(text[y/8][i]) * 8 + (y % 8)];
              }
              // if (line / 2 / 8 < 16) {
              //   for (i = 0; i < 16; i++) {
              //     // next_line[i] = font[(line / 16) * 16 + i][i % 8];
              //     next_line[i] = font_8x8[((line / 16) * 16 + i) * 8 + (line/2) % 8];
              //   }
              // }
              // else {
              //   for (i = 0; i < 16; i++) {
              //     next_line[i] = 0;
              //   }
              // }
            }

            // next_line[(line+1)%2] = data[(line/2) % 9];

            // if (line % 16 == 0 || (line + 1) % 16 == 0) {
            //     gpio_bit_set(GREEN_PORT, GREEN_PIN);
            // } else {
            //     gpio_bit_reset(GREEN_PORT, GREEN_PIN);
            // }
        }

        line++;
        if (line == V_FRAME) {
            line = 0;
        }
    }
}
