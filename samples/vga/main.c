#include "gd32vf103.h"
#include "gd32vf103_timer.h"
#include "gd32vf103_rcu.h"
#include "gd32vf103_gpio.h"

#include "config.h"
#include "keyboard.h"
#include "video.h"

const int32_t pad = 0;
volatile uint8_t h_sync[TEXT_W];

int32_t cursor_x = pad;
int32_t cursor_y = pad;

void putc(char c)
{
    text[cursor_y][cursor_x] = c;
    cursor_x++;
    if (cursor_x >= TEXT_W-pad) {
        cursor_x = pad;
        cursor_y++;
        if (cursor_y >= TEXT_H-pad) {
            cursor_y = pad;
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

char key2char(uint8_t key, bool shift)
{
    switch (key) {
        case KEY_A      : return shift ? 'A' : 'a';
        case KEY_B      : return shift ? 'B' : 'b';
        case KEY_C      : return shift ? 'C' : 'c';
        case KEY_D      : return shift ? 'D' : 'd';
        case KEY_E      : return shift ? 'E' : 'e';
        case KEY_F      : return shift ? 'F' : 'f';
        case KEY_G      : return shift ? 'G' : 'g';
        case KEY_H      : return shift ? 'H' : 'h';
        case KEY_I      : return shift ? 'I' : 'i';
        case KEY_J      : return shift ? 'J' : 'j';
        case KEY_K      : return shift ? 'K' : 'k';
        case KEY_L      : return shift ? 'L' : 'l';
        case KEY_M      : return shift ? 'M' : 'm';
        case KEY_N      : return shift ? 'N' : 'n';
        case KEY_O      : return shift ? 'O' : 'o';
        case KEY_P      : return shift ? 'P' : 'p';
        case KEY_Q      : return shift ? 'Q' : 'q';
        case KEY_R      : return shift ? 'R' : 'r';
        case KEY_S      : return shift ? 'S' : 's';
        case KEY_T      : return shift ? 'T' : 't';
        case KEY_U      : return shift ? 'U' : 'u';
        case KEY_V      : return shift ? 'V' : 'v';
        case KEY_W      : return shift ? 'W' : 'w';
        case KEY_X      : return shift ? 'X' : 'x';
        case KEY_Y      : return shift ? 'Y' : 'y';
        case KEY_Z      : return shift ? 'Z' : 'z';
        case KEY_SPACE  : return shift ? ' ' : ' ';
        case KEY_1      : return shift ? '!' : '1';
        case KEY_2      : return shift ? '@' : '2';
        case KEY_3      : return shift ? '#' : '3';
        case KEY_4      : return shift ? '$' : '4';
        case KEY_5      : return shift ? '%' : '5';
        case KEY_6      : return shift ? '^' : '6';
        case KEY_7      : return shift ? '&' : '7';
        case KEY_8      : return shift ? '*' : '8';
        case KEY_9      : return shift ? '(' : '9';
        case KEY_0      : return shift ? ')' : '0';
        case KEY_BACKTCK: return shift ? '~' : '`';
        case KEY_MINUS  : return shift ? '_' : '-';
        case KEY_EQUAL  : return shift ? '+' : '=';
        case KEY_OPEN   : return shift ? '{' : '[';
        case KEY_CLOSE  : return shift ? '}' : ']';
        case KEY_SCOLON : return shift ? ':' : ';';
        case KEY_QUOTE  : return shift ? '"' : '\'';
        case KEY_BACKSL : return shift ? '|' : '\\';
        case KEY_COMMA  : return shift ? '<' : ',';
        case KEY_DOT    : return shift ? '>' : '.';
        case KEY_SLASH  : return shift ? '?' : '/';
        default: return '?';
    }
}

// enum key_mod key_state = none;

void key_handler(uint16_t code)
{
    if (code & KEY_TYPE_RELEASE) {
    } else if (code & KEY_TYPE_MOVE) {
        switch (code & KEY_MASK) {
            case KEY_NP8: // UP
                putc(1 * 16 + 8);
                break;
            case KEY_NP2: // DOWN
                putc(1 * 16 + 9);
                break;
            case KEY_NP4: // LEFT
                putc(1 * 16 + 11);
                break;
            case KEY_NP6: // RIGHT
                putc(1 * 16 + 10);
                break;

        }
    } else {
        if (code & (KEY_MOD_CTL | KEY_MOD_ALT)) {

        } else {
            switch (code & KEY_MASK) {
            case KEY_ENTER:
                cursor_y++;
                cursor_x = pad;
                if (cursor_y >= TEXT_H-pad) {
                    cursor_y = pad;
                }
                break;
            case KEY_BKSP:
                cursor_x--;
                if (cursor_x < pad) {
                    cursor_x = pad;
                }
                text[cursor_y][cursor_x] = ' ';
                break;
            default:
                putc(key2char((uint8_t) (code & KEY_MASK), code & KEY_MOD_SHIFT));
                break;
            }
        }
    }
}

void key_handler2(uint8_t code)
{
    putc(hexchar((code & 0xf0) >> 4));
    putc(hexchar((code & 0x0f) >> 0));
    putc(' ');
}


void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_USART2);
    // rcu_periph_clock_enable(RCU_SPI1);
}

void gpio_config(void)
{
    gpio_bit_reset(HSYNC_PORT, HSYNC_PIN);
    gpio_bit_reset(VSYNC_PORT, HSYNC_PIN);
    // gpio_init(HSYNC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HSYNC_PIN);
    gpio_init(VSYNC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, VSYNC_PIN);

    // SPI0 is used for generating the green signal of VGA as master output (master mode)
    /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GREEN_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GREEN_PIN);
    // SPI is used for generating the H_SYNC signal of VGA as master output (master mode)
    /* SPI2 GPIO config: MISO/PB5 */
    gpio_init(HSYNC_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, HSYNC_PIN);

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
    spi0.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi0.nss                  = SPI_NSS_SOFT;
    spi0.prescale             = SPI_PSC_16;
    spi0.endian               = SPI_ENDIAN_LSB;
    spi_init(SPI0, &spi0);

    //
    // SPI2
    //
    spi_parameter_struct spi2;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI2);
    spi_struct_para_init(&spi2);

    /* SPI2 parameter config */
    spi2.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;
    spi2.device_mode          = SPI_MASTER;
    spi2.frame_size           = SPI_FRAMESIZE_8BIT;
    spi2.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi2.nss                  = SPI_NSS_SOFT;
    spi2.prescale             = SPI_PSC_8;
    spi2.endian               = SPI_ENDIAN_LSB;
    spi_init(SPI2, &spi2);
}

void usart_config(void)
{
    /* USART configure */
    usart_deinit(USART2);
    //usart_baudrate_set(USART2, 12000U);
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
    dma_init_struct.number       = SYNC + LINE_LEN;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);

    /* SPI2 transmit dma config:DMA1-DMA_CH1 */
    dma_deinit(DMA1, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    dma_init_struct.memory_addr  = (uint32_t)h_sync;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.number       = TEXT_W;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA1, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH1);
    dma_memory_to_memory_disable(DMA1, DMA_CH1);
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
    // lines[0][TEXT_W] = 0xf0;
    // lines[1][TEXT_W] = 0xf0;
    // lines[0][TEXT_W+1] = 0xc0;
    // lines[1][TEXT_W+1] = 0xc0;

    system_init();

    //ECLIC init
    eclic_init(ECLIC_NUM_INTERRUPTS);
    eclic_mode_enable();

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
    // spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_RBNE);

    spi_enable(SPI0);
    spi_enable(SPI2);

    int i;
    int j;
    for (i = 0; i < TEXT_H; i++) {
        for (j = 0; j < TEXT_W; j++) {
            text[i][j] = 11 * 16 + 1;
        }
    }
    int b;
    for (i = 0; i < TEXT_W * 8; i++) {
        if ((H_FRONT_PORCH/2/2) <= i && i < ((H_FRONT_PORCH + H_SYNC_PULSE)/2/2)) {
            b = 0;
        } else {
            b = 1;
        }
        h_sync[i / 8] |= b << (i % 8);
    }
    // h_sync[0] = 0x01;
    // for (i = 0; i < TEXT_W; i++) {
    //     text[0][i] = 11 * 16 + 1;
    //     text[TEXT_H-1][i] = 11 * 16 + 1;
    // }
    // for (i = 0; i < TEXT_H; i++) {
    //     // text[i][0] = 11 * 16 + 1;
    //     // text[i][320/8-1] = 11 * 16 + 1;
    //     text[i][0] = 11 * 16 + 1;
    //     text[i][TEXT_W-1] = 11 * 16 + 1;
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
    uint16_t key;
    while (1) {
        if (key_buf_tail != key_buf_head) {
            // gpio_bit_reset(GPIOA, GPIO_PIN_1);
            key = key_buf[key_buf_tail];
            key_buf_tail = (key_buf_tail + 1) % KEY_BUF_LEN;
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
