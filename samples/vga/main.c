/*!
    \file  main.c
    \brief TIMER1 oc timebase demo
    
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
// #include <stdio.h>
// #include "gd32vf103v_eval.h"
#include "gd32vf103_timer.h"
#include "gd32vf103_rcu.h"
#include "gd32vf103_gpio.h"

#include "config.h"
#include "keyboard.h"

// #define  ARRAYSIZE         10

extern char text[240/8][320/8];
extern uint8_t lines[2][320/8];

extern uint32_t key_buf_len;
extern uint8_t key_buf[16];
extern volatile uint8_t key_buf_head;
extern volatile uint8_t key_buf_tail;

int32_t cursor_x = 1;
int32_t cursor_y = 1;

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

enum key_mod key_state = none;

void key_handler(uint8_t code)
{
  switch (key_state) {
    case none:
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
          text[cursor_y][cursor_x] = ' ';
          break;
        case KEY_RELEASE:
          key_state = release;
          break;
        default:
          putc(key2char(code));
      }
      break;
    case release:
      key_state = none;
      break;
  }
}

void key_handler2(uint8_t code)
{
        putc(hexchar((code & 0xf0) >> 4));
        putc(hexchar((code & 0x0f) >> 0));
        putc(' ');
}


// uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
// uint8_t spi2_send_array[ARRAYSIZE] = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA};
// uint8_t spi0_receive_array[ARRAYSIZE]; 
// uint8_t spi2_receive_array[ARRAYSIZE];

void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_USART2);
    // rcu_periph_clock_enable(RCU_SPI1);
}

void gpio_config(void)
{
    gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
    gpio_bit_reset(VSYNC_PORT, HSYNC_PIN);
    gpio_init(HSYNC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HSYNC_PIN | VSYNC_PIN);

    // SPI0 is used for generating the green signal of VGA as master output (master mode)
    /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    // SPI1 is used to receive PS/2 data as master input (slave mode)
    /* SPI1 GPIO config:SCK/PB13, MISO/PB14, MOSI/PB15 */
    // gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    // gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14);

    // USART2 is PS/2
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
}

void spi_config(void)
{
    //
    // SPI0
    //
    spi_parameter_struct spi0;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi0);

    /* SPI0 parameter config */
    spi0.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi0.device_mode          = SPI_MASTER;
    spi0.frame_size           = SPI_FRAMESIZE_8BIT;
    spi0.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi0.nss                  = SPI_NSS_SOFT;
    spi0.prescale             = SPI_PSC_8;
    spi0.endian               = SPI_ENDIAN_LSB;
    spi_init(SPI0, &spi0);

    //
    // SPI1
    //
    // spi_parameter_struct spi1;
    // /* deinitilize SPI and the parameters */
    // spi_i2s_deinit(SPI1);
    // spi_struct_para_init(&spi1);

    // /* configure SPI1 parameter */
    // spi1.frame_size           = SPI_FRAMESIZE_8BIT;
    // spi1.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    // spi1.prescale             = SPI_PSC_8;
    // spi1.endian               = SPI_ENDIAN_LSB;
    // spi1.trans_mode           = SPI_TRANSMODE_BDRECEIVE;
    // spi1.device_mode          = SPI_SLAVE;
    // spi1.nss                  = SPI_NSS_SOFT;
    // spi_init(SPI1, &spi1);
}

void usart_config(void)
{
    /* USART configure */
    usart_deinit(USART2);
    //usart_baudrate_set(USART2, 12000U);
    //
    uint32_t uclk = 50000000;
    uint32_t baudval = 12000;
    uint32_t usart_periph = USART2;
    uint32_t udiv = (uclk+baudval/2U)/baudval;
    uint32_t intdiv = udiv & (0x0000fff0U);
    uint32_t fradiv = udiv & (0x0000000fU);
    USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
    //
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_ODD);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    // usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_enable(USART2);
}

void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    /* SPI0 transmit dma config:DMA0-DMA_CH2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);
    
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)lines[0];
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.number       = 320/8;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/1 = SystemCoreClock.
    TIMER1 configuration is timing mode, and the timing is 0.2s(4000/20000 = 0.2s).
    CH0 update rate = TIMER1 counter clock/CH0CV = 20000/4000 = 5Hz.
    ---------------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 1 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    // timer_initpara.period            = (H_FRONT_PORCH * PIXEL_FREQ_MUL) / 5 - 1;
    // timer_initpara.period            = 80 - 1;
    timer_initpara.period            = H_LINE * PIXEL_FREQ_MUL - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    // timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);


    TIMER_INTF(TIMER1) = (~(uint32_t)TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    timer_interrupt_disable(TIMER1, TIMER_INT_CH0);
    timer_interrupt_disable(TIMER1, TIMER_INT_CH1);
    timer_interrupt_disable(TIMER1, TIMER_INT_CH2);
    timer_interrupt_disable(TIMER1, TIMER_INT_CH3);
    timer_interrupt_disable(TIMER1, TIMER_INT_CMT);
    timer_interrupt_disable(TIMER1, TIMER_INT_TRG);
    timer_interrupt_disable(TIMER1, TIMER_INT_BRK);

    timer_enable(TIMER1);
}

static void system_clock_init(void)
{
    uint32_t timeout   = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while ((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if (0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while (1) {
        }
    }

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    // /* CK_PLL = (CK_PREDIV0) * 27 = 108 MHz */
    // RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
    // RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL27);

    // /* CK_PLL = (CK_PREDIV0) * 20 = 80 MHz */
    // RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
    // RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL20);

    /* CK_PLL = (CK_PREDIV0) * 20 = 100 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL25);

    SystemCoreClock = CK_SYS;

    // HXTAL_VALUE==8000000
    RCU_CFG1 &= ~(RCU_CFG1_PREDV0SEL | RCU_CFG1_PREDV1 | RCU_CFG1_PLL1MF | RCU_CFG1_PREDV0);
    RCU_CFG1 |= (RCU_PREDV0SRC_HXTAL | RCU_PREDV0_DIV2 | RCU_PREDV1_DIV2 | RCU_PLL1_MUL20 | RCU_PLL2_MUL20);

    /* enable PLL1 */
    RCU_CTL |= RCU_CTL_PLL1EN;
    /* wait till PLL1 is ready */
    while (0U == (RCU_CTL & RCU_CTL_PLL1STB)) {
    }

    /* enable PLL2 */
    RCU_CTL |= RCU_CTL_PLL2EN;
    /* wait till PLL1 is ready */
    while (0U == (RCU_CTL & RCU_CTL_PLL2STB)) {
    }

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while (0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while (0U == (RCU_CFG0 & RCU_SCSS_PLL)) {
    }
}

static void
system_init(void)
{
    /* reset the RCC clock configuration to the default reset state */
    /* enable IRC8M */
    RCU_CTL |= RCU_CTL_IRC8MEN;

    /* reset SCS, AHBPSC, APB1PSC, APB2PSC, ADCPSC, CKOUT0SEL bits */
    RCU_CFG0 &= ~(RCU_CFG0_SCS | RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC |
                  RCU_CFG0_ADCPSC | RCU_CFG0_ADCPSC_2 | RCU_CFG0_CKOUT0SEL);

    /* reset HXTALEN, CKMEN, PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_HXTALEN | RCU_CTL_CKMEN | RCU_CTL_PLLEN);

    /* Reset HXTALBPS bit */
    RCU_CTL &= ~(RCU_CTL_HXTALBPS);

    /* reset PLLSEL, PREDV0_LSB, PLLMF, USBFSPSC bits */

    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PREDV0_LSB | RCU_CFG0_PLLMF |
                  RCU_CFG0_USBFSPSC | RCU_CFG0_PLLMF_4);
    RCU_CFG1 = 0x00000000U;

    /* Reset HXTALEN, CKMEN, PLLEN, PLL1EN and PLL2EN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_PLL1EN | RCU_CTL_PLL2EN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);
    /* disable all interrupts */
    RCU_INT = 0x00FF0000U;

    /* Configure the System clock source, PLL Multiplier, AHB/APBx prescalers and Flash settings */
    system_clock_init();
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    // FlagStatus current;
    // gd_eval_led_init(LED2);

    //////
    // SystemInit();
    system_init();

    //ECLIC init
    eclic_init(ECLIC_NUM_INTERRUPTS);
    eclic_mode_enable();

    //////

    rcu_config();
    gpio_config();
    dma_config();
    spi_config();
    usart_config();

    gpio_bit_set(GPIOA, GPIO_PIN_1);
    gpio_bit_set(GPIOA, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);

    eclic_global_interrupt_enable();
    eclic_set_nlbits(ECLIC_GROUP_LEVEL2_PRIO2);
    eclic_irq_enable(TIMER1_IRQn,3, 3);
    // eclic_irq_enable(SPI1_IRQn,2,0);
    // eclic_irq_enable(USART2_IRQn, 0, 0);
    timer_config();
    // usart_interrupt_disable(USART2, USART_INT_RBNE);
    // usart_interrupt_enable(USART2, USART_INT_RBNE);
    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_RBNE);

    spi_enable(SPI0);
    // spi_enable(SPI1);

    // int i;
    // for (i = 0; i < 320/8; i++) {
    //     text[0][i] = 11 * 16 + 1;
    //     text[240/8-1][i] = 11 * 16 + 1;
    // }
    // for (i = 1; i < 240/8 -1; i++) {
    //     // text[i][0] = 11 * 16 + 1;
    //     // text[i][320/8-1] = 11 * 16 + 1;
    //     text[i][1] = 11 * 16 + 1;
    //     text[i][320/8-2] = 11 * 16 + 1;
    // }


    // text[4][4 + 0] = 'H';
    // text[4][4 + 1] = 'e';
    // text[4][4 + 2] = 'l';
    // text[4][4 + 3] = 'l';
    // text[4][4 + 4] = 'o';
    // text[4][4 + 5] = ' ';
    // text[4][4 + 6] = 'w';
    // text[4][4 + 7] = 'o';
    // text[4][4 + 8] = 'r';
    // text[4][4 + 9] = 'l';
    // text[4][4 +10] = 'd';
    // text[4][4 +11] = '!';
    // char c = 0;
    // volatile int j = 0;
    uint8_t key;
    while (1) {
        if (key_buf_tail != key_buf_head) {
            // gpio_bit_reset(GPIOA, GPIO_PIN_1);
            key = key_buf[key_buf_tail];
            key_buf_tail = (key_buf_tail + 1) % key_buf_len;
            key_handler(key);
        }
        // for (j = 0; j < 0xffffff/50; j++) {

        // }
        // putc(c++);
        // current = gpio_output_bit_get(GPIOA, GPIO_PIN_2);
        // if (current == SET) {
        //     gpio_bit_reset(GPIOA, GPIO_PIN_2);
        // } else {
        //     gpio_bit_set(GPIOA, GPIO_PIN_2);
        // }
        // delay_1ms(1000);
    }
}

// void *memcpy(void *restrict dest, const void *restrict src, size_t n)
// {
// 	if (n > 0) {
// 		char *cdest = dest;
// 		const char *csrc = src;
// 		const char *cend = csrc + n;
// 
// 		do {
// 			*cdest++ = *csrc++;
// 		} while (csrc < cend);
// 	}
// 
// 	return dest;
// }
