#ifndef VIDEO_H
#define VIDEO_H

#define TEXT_W (320/8/2)
#define TEXT_H (240/8/2)
#define LINE_LEN TEXT_W

extern volatile char text[TEXT_H][TEXT_W];
extern volatile uint8_t lines[2][TEXT_W];

#endif /* VIDEO_H */
