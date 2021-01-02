#ifndef VIDEO_H
#define VIDEO_H

#define BLANK 0
#define TEXT_W (320/8/2)
#define TEXT_H (240/8/2)
#define LINE_LEN (TEXT_W + BLANK)
#define SYNC (160/8/2/2)

extern volatile char text[TEXT_H][TEXT_W];
extern volatile uint8_t lines[2][SYNC + LINE_LEN];
extern volatile uint8_t red_lines[2][SYNC + LINE_LEN];
extern volatile uint8_t h_sync[TEXT_W];

#endif /* VIDEO_H */
