#ifndef VIDEO_H
#define VIDEO_H

#include "config.h"

#define BLANK 0
#define TEXT_W (640/8/DIV)
#define TEXT_H (480/8/DIV)
#define LINE_LEN (TEXT_W + BLANK)
#define SYNC (160/8/DIV)

extern volatile char text[TEXT_H][TEXT_W];
extern volatile uint8_t lines[2][SYNC + LINE_LEN];
extern volatile uint8_t h_sync[TEXT_W];

#endif /* VIDEO_H */
