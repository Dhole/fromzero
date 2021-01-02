#include "io.h"
#include "video.h"

const int32_t pad = 0;

uint16_t input_buf[INPUT_BUF_LEN];
volatile uint8_t input_buf_head = 0;
volatile uint8_t input_buf_tail = 0;

int32_t cursor_x = pad;
int32_t cursor_y = pad;

void input_push(int c)
{
    input_buf[input_buf_head] = (uint16_t) c;
    input_buf_head = (input_buf_head + 1) % INPUT_BUF_LEN;
}

int input_pop()
{
    int c = 0;
    if (input_buf_tail != input_buf_head) {
        c = input_buf[input_buf_tail];
        input_buf_tail = (input_buf_tail + 1) % INPUT_BUF_LEN;
    }
    return c;
}

int input_peek()
{
    int c = 0;
    if (input_buf_tail != input_buf_head) {
        c = input_buf[input_buf_tail];
    }
    return c;
}

int getchar()
{
    int c;
    while (1) {
        c = input_pop();
        if (c != 0) {
            return c;
        }
    }
}

int peek()
{
    int c;
    while (1) {
        c = input_peek();
        if (c != 0) {
            return c;
        }
    }
}

void delc()
{
   if (cursor_x > 0) {
       cursor_x--;
       text[cursor_y][cursor_x] = ' ';
   }
}

void putc(char c)
{
    if (c == '\n') {
        cursor_y++;
        cursor_x = pad;
        if (cursor_y >= TEXT_H-pad) {
            cursor_y = pad;
        }
        return;
    }
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

void _putchar(char c)
{
    putc(c);
}
