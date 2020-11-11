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

#define  ARRAYSIZE         10

extern char text[240/8][320/8];

uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
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
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_SPI0);
    // rcu_periph_clock_enable(RCU_SPI2);
}

void gpio_config(void)
{
    gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
    gpio_bit_reset(VSYNC_PORT, HSYNC_PIN);
    gpio_init(HSYNC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HSYNC_PIN | VSYNC_PIN);

    /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
    // /* SPI2 GPIO config:SCK/PC10, MISO/PC11, MOSI/PC12 */
    // gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);
    // gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_12);
    // gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /*Configure PA0(TIMER1 CH0) as alternate function*/
    // gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    // spi_i2s_deinit(SPI2);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_LSB;
    spi_init(SPI0, &spi_init_struct);

    // /* SPI2 parameter config */
    // spi_init_struct.device_mode = SPI_SLAVE;
    // spi_init_struct.nss         = SPI_NSS_SOFT;
    // spi_init(SPI2, &spi_init_struct);

}

void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    /* SPI0 transmit dma config:DMA0-DMA_CH2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);
    
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)spi0_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);

    /* SPI0 receive dma config:DMA0-DMA_CH1 */
    // dma_deinit(DMA0, DMA_CH1);
    // dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    // dma_init_struct.memory_addr  = (uint32_t)spi0_receive_array;
    // dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    // dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    // dma_init(DMA0, DMA_CH1, &dma_init_struct);
    // /* configure DMA mode */
    // dma_circulation_disable(DMA0, DMA_CH1);
    // dma_memory_to_memory_disable(DMA0, DMA_CH1);

    // /* SPI2 transmit dma config:DMA1,DMA_CH1 */
    // dma_deinit(DMA1, DMA_CH1);
    // dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    // dma_init_struct.memory_addr  = (uint32_t)spi2_send_array;
    // dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    // dma_init_struct.priority     = DMA_PRIORITY_MEDIUM;
    // dma_init(DMA1, DMA_CH1, &dma_init_struct);
    // /* configure DMA mode */
    // dma_circulation_disable(DMA1, DMA_CH1);
    // dma_memory_to_memory_disable(DMA1, DMA_CH1);

    // /* SPI2 receive dma config:DMA1,DMA_CH0 */
    // dma_deinit(DMA1, DMA_CH0);
    // dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    // dma_init_struct.memory_addr  = (uint32_t)spi2_receive_array;
    // dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    // dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    // dma_init(DMA1, DMA_CH0, &dma_init_struct);
    // /* configure DMA mode */
    // dma_circulation_disable(DMA1, DMA_CH0);
    // dma_memory_to_memory_disable(DMA1, DMA_CH0);
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

    // spi_enable(SPI2);
    spi_enable(SPI0);

    // gpio_bit_set(GPIOA, GPIO_PIN_1);
    // gpio_bit_set(GPIOA, GPIO_PIN_2);
    // gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);

    eclic_global_interrupt_enable();
    eclic_set_nlbits(ECLIC_GROUP_LEVEL3_PRIO1);
    eclic_irq_enable(TIMER1_IRQn,1,0);
    timer_config();

    text[4][4 + 0] = 'H';
    text[4][4 + 1] = 'e';
    text[4][4 + 2] = 'l';
    text[4][4 + 3] = 'l';
    text[4][4 + 4] = 'o';
    text[4][4 + 5] = ' ';
    text[4][4 + 6] = 'w';
    text[4][4 + 7] = 'o';
    text[4][4 + 8] = 'r';
    text[4][4 + 9] = 'l';
    text[4][4 +10] = 'd';
    text[4][4 +11] = '!';
    while (1) {
        // current = gpio_output_bit_get(GPIOA, GPIO_PIN_2);
        // if (current == SET) {
        //     gpio_bit_reset(GPIOA, GPIO_PIN_2);
        // } else {
        //     gpio_bit_set(GPIOA, GPIO_PIN_2);
        // }
        // delay_1ms(1000);
    }
}
