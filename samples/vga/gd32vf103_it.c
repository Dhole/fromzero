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
// #include "gd32vf103v_eval.h"
#include "gd32vf103_timer.h"
#include "gd32vf103_eclic.h"
#include "gd32vf103_spi.h"

#include "config.h"
#include "font.h"
#include "keyboard.h"

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
uint8_t lines[2][320/8];
uint8_t *cur_line = lines[0];
char text[TEXT_H][TEXT_W];
int32_t cursor_x = 1;
int32_t cursor_y = 1;
// uint8_t next_line_index = 0;
uint8_t *next_line = lines[0];

enum sync_state {
    FRONT_PORCH,
    SYNC_PULSE,
    BACK_PORCH,
    ACTIVE_VIDEO,
};

// static enum sync_state v_state = FRONT_PORCH;

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

        /*if (line < V_ACTIVE_VIDEO) {
            // V_ACTIVE_VIDEO
            // gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else if (line < V_ACTIVE_VIDEO + V_FRONT_PORCH) {
            // V_FRONT_PORCH
            // gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        } else*/ if (line < V_ACTIVE_VIDEO + V_FRONT_PORCH + V_SYNC_PULSE) {
            // V_SYNC_PULSE
            gpio_bit_reset(VSYNC_PORT, VSYNC_PIN);
        } else {
            // V_BACK_PORCH
            gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
        }

        // timer_counter_value_config(TIMER1, 0);

        // // H_FRONT_PORCH
        // gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        // cur_line = lines[(line / 2) % 2];
        cur_line = lines[(line / 2) % 2];
        DMA_CHMADDR(DMA0, DMA_CH2) = (uint32_t) cur_line;
        while (timer_counter_read(TIMER1) < H_FRONT_PORCH * PIXEL_FREQ_MUL - 3);

        // H_SYNC_PULSE
        gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);

        // DMA_CHMADDR(DMA0, DMA_CH2) = lines[0];
        DMA_CHCNT(DMA0, DMA_CH2) = (LINE_LEN & DMA_CHANNEL_CNT_MASK);
        dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
        dma_channel_disable(DMA0, DMA_CH2);
        spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE) * PIXEL_FREQ_MUL - 3);

        // // H_BACK_PORCH
        gpio_bit_set(HSYNC_PORT, HSYNC_PIN);
        while (timer_counter_read(TIMER1) < (H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH) * PIXEL_FREQ_MUL - 23);

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

            // DMA_CHMADDR(DMA0, DMA_CH2) = (uint32_t) lines[(line / 2) % 2];
            dma_channel_enable(DMA0, DMA_CH2);
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

void putc(char c)
{
  text[cursor_y][cursor_x] = c;
  cursor_x++;
  if (cursor_x >= 320/8-1) {
    cursor_x = 1;
    cursor_y++;
    if (cursor_y >= 240/8-1) {
      cursor_y = 1;
    }
  }
}

char hexchar(uint8_t v)
{
  if (v < 10) {
    return '0' + v;
  } else {
    return 'a' + (v - 10);
  }
  return '*';
}

// void SPI1_IRQHandler(void)
// {
//     uint8_t data;
//     /* receive data */
//     if(RESET != spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_INT_FLAG_RBNE)){
//         data = spi_i2s_data_receive(SPI1);
//         spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_RBNE);
//         spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_RBNE);
//         putc(hexchar((data & 0xf0) >> 4));
//         putc(hexchar((data & 0x0f) >> 0));
//         putc(' ');
//     }
// }

char key2char(uint8_t key)
{
  switch (key) {
    case KEY_A    : return 'a';
    case KEY_B    : return 'b';
    case KEY_C    : return 'c';
    case KEY_D    : return 'd';
    case KEY_E    : return 'e';
    case KEY_F    : return 'f';
    case KEY_G    : return 'g';
    case KEY_H    : return 'h';
    case KEY_I    : return 'i';
    case KEY_J    : return 'j';
    case KEY_K    : return 'k';
    case KEY_L    : return 'l';
    case KEY_M    : return 'm';
    case KEY_N    : return 'n';
    case KEY_O    : return 'o';
    case KEY_P    : return 'p';
    case KEY_Q    : return 'q';
    case KEY_R    : return 'r';
    case KEY_S    : return 's';
    case KEY_T    : return 't';
    case KEY_U    : return 'u';
    case KEY_V    : return 'v';
    case KEY_W    : return 'w';
    case KEY_X    : return 'x';
    case KEY_Y    : return 'y';
    case KEY_Z    : return 'z';
    case KEY_SPACE: return ' ';
    default: return '?';
  }
}

enum key_state key_state = unpressed;

void USART2_IRQHandler(void)
{
    uint8_t code;
    gpio_bit_reset(GPIOA, GPIO_PIN_1);
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)){
        /* receive data */
        code = usart_data_receive(USART2);
        usart_interrupt_disable(USART2, USART_INT_RBNE);
        usart_interrupt_enable(USART2, USART_INT_RBNE);
        switch (key_state) {
          case unpressed:
            key_state = pressed;
            switch (code) {
              case KEY_ENTER:
                cursor_y++;
                cursor_x = 1;
                if (cursor_y >= 240/8-1) {
                  cursor_y = 1;
                }
                break;
              case KEY_BKSP:
                cursor_x--;
                if (cursor_x < 1) {
                  cursor_x = 1;
                }
                break;
              default:
                putc(key2char(code));
            }
            break;
          case pressed:
            if (code == KEY_RELEASE) {
              key_state = release;
            }
            break;
          case release:
            key_state = unpressed;
            break;
        }
        // putc(hexchar((data & 0xf0) >> 4));
        // putc(hexchar((data & 0x0f) >> 0));
        // putc(' ');
    }
}
