#include "gd32vf103.h"
#include "gd32vf103_timer.h"
#include "gd32vf103_eclic.h"
#include "gd32vf103_spi.h"
#include "gd32vf103_usart.h"

#include "config.h"
#include "video.h"
#include "keyboard.h"
#include "font.h"

uint8_t volatile lines[2][SYNC + LINE_LEN];
uint8_t volatile red_lines[2][SYNC + LINE_LEN];
// uint32_t cur_line_offset = 0;
// uint8_t volatile *cur_line = lines[0];
char volatile text[TEXT_H][TEXT_W];
// char volatile *text_line = text[0];
// uint8_t next_line_index = 0;
uint8_t volatile *cur_line = lines[0];
uint8_t volatile *next_line = lines[0];
uint8_t volatile *cur_red_line = red_lines[0];
uint8_t volatile *next_red_line = red_lines[0];

enum sync_state {
    FRONT_PORCH,
    SYNC_PULSE,
    BACK_PORCH,
    ACTIVE_VIDEO,
};

uint16_t key_buf[16];
volatile uint8_t key_buf_head = 0;
volatile uint8_t key_buf_tail = 0;

static uint32_t line = 0;
static uint16_t key_type_next = 0;
static uint16_t key_mod = 0;
static int y;

// static int i_from = 0; //, i_to = 0;

void TIMER1_IRQHandler(void)
{
    uint16_t code;
    int i;
    int ydiv8, ymod8;
    // No need for if, because we only have a single TIMER1 interrupt enabled
    // if ((TIMER_INTF(TIMER1) & TIMER_INT_FLAG_UP)) {
    //
    // Horizontal state: H_FRONT_PORCH
    //

    /*if (line < V_ACTIVE_VIDEO) {
    // Vertical state: V_ACTIVE_VIDEO
    gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
    } else if (line < V_ACTIVE_VIDEO + V_FRONT_PORCH) {
    // Vertical state:  V_FRONT_PORCH
    gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
    } else*/ if (line == V_ACTIVE_VIDEO + V_FRONT_PORCH) {
        // Vertical state:  V_SYNC_PULSE
        // while (TIMER_CNT(TIMER1) < 0 * PIXEL_FREQ_MUL);
        gpio_bit_reset(VSYNC_PORT, VSYNC_PIN);
    } else if (line == V_ACTIVE_VIDEO + V_FRONT_PORCH + V_SYNC_PULSE) {
        // Vertical state:  V_BACK_PORCH
        // while (TIMER_CNT(TIMER1) < 0 * PIXEL_FREQ_MUL);
        gpio_bit_set(VSYNC_PORT, VSYNC_PIN);
    }

    // dma_channel_disable(DMA0, DMA_CH2);
    DMA_CHCTL(DMA0, DMA_CH2) &= ~DMA_CHXCTL_CHEN;
    // spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    SPI_CTL1(SPI0) |= (uint32_t)SPI_CTL1_DMATEN;

    dma_channel_disable(DMA0, DMA_CH3);
    // DMA_CHCTL(DMA0, DMA_CH3) &= ~DMA_CHXCTL_CHEN;
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    // uint32_t ctl = 0U;
    // ctl = USART_CTL2(USART0);
    // ctl &= ~USART_CTL2_DENT;
    // ctl |= USART_DENT_ENABLE;
    // USART_CTL2(USART0) |= ctl;

    // dma_channel_disable(DMA1, DMA_CH1);
    DMA_CHCTL(DMA1, DMA_CH1) &= ~DMA_CHXCTL_CHEN;
    // spi_dma_enable(SPI2, SPI_DMA_TRANSMIT);
    SPI_CTL1(SPI2) |= (uint32_t)SPI_CTL1_DMATEN;

    DMA_CHMADDR(DMA0, DMA_CH2) = (uint32_t) (cur_line);
    DMA_CHMADDR(DMA0, DMA_CH3) = (uint32_t) (cur_red_line);
    DMA_CHMADDR(DMA1, DMA_CH1) = (uint32_t) (h_sync);

    // Horizontal state:  H_ACTIVE_VIDEO
    DMA_CHCTL(DMA1, DMA_CH1) |= DMA_CHXCTL_CHEN;
    if (line < V_ACTIVE_VIDEO) {
        DMA_CHCTL(DMA0, DMA_CH2) |= DMA_CHXCTL_CHEN;
        DMA_CHCTL(DMA0, DMA_CH3) |= DMA_CHXCTL_CHEN;
    }
    TIMER_INTF(TIMER1) = (~(uint32_t)TIMER_INT_FLAG_UP);

    line++;
    if (line == V_FRAME) {
        line = 0;
    }

    if (line < V_ACTIVE_VIDEO && (line & 0x3)) {
        y = (line / 4);
        cur_line = lines[y % 2];
        next_line = lines[(y+1) % 2];
        cur_red_line = red_lines[y % 2];
        next_red_line = red_lines[(y+1) % 2];
        ydiv8 = y >> 3; // 7 / 8
        ymod8 = y & 0x7; // 7 % 8
        for (i = 0; i < TEXT_W; i++) {
            cur_line[SYNC + i] = font_8x8[(int)(text[ydiv8][i]) * 8 + ymod8];
            cur_red_line[SYNC + i] = font_8x8[(int)(text[ydiv8][i]) * 8 + ymod8];
        }
    }

    uint16_t key_type;
    if ((line & 0x1f) == 0) {
        if (RESET != usart_flag_get(USART2, USART_FLAG_RBNE)) {
            code = usart_data_receive(USART2);
            //code &= KEY_MASK;
            key_type = key_type_next;
            if (key_type_next & KEY_TYPE_RELEASE) {
                key_type_next &= ~KEY_TYPE_RELEASE;
                switch (code) {
                case KEY_SHIFT:
                    key_mod &= ~KEY_MOD_SHIFT;
                    return;
                case KEY_ALT:
                    key_mod &= ~KEY_MOD_ALT;
                    return;
                case KEY_CTL:
                    key_mod &= ~KEY_MOD_CTL;
                    return;
                default:
                    break;
                }
            }
            if (key_type_next & KEY_TYPE_MOVE) {
                key_type_next &= ~KEY_TYPE_MOVE;
            }

            switch (code) {
            case MOD_MOVE:
                key_type_next |= KEY_TYPE_MOVE;
                break;
            case MOD_RELEASE:
                key_type_next |= KEY_TYPE_RELEASE;
                break;
            case KEY_SHIFT:
                key_mod |= KEY_MOD_SHIFT;
                break;
            case KEY_ALT:
                key_mod |= KEY_MOD_ALT;
                break;
            case KEY_CTL:
                key_mod |= KEY_MOD_CTL;
                break;
            default:
                key_buf[key_buf_head] = code | key_mod | key_type;
                key_buf_head = (key_buf_head + 1) % KEY_BUF_LEN;
                break;
            }
        }
    }
}
