#ifndef CONFIG_H
#define CONFIG_H

#include "gd32vf103_gpio.h"

#define HSYNC_PORT GPIOB
#define HSYNC_PIN GPIO_PIN_9
#define VSYNC_PORT GPIOB
#define VSYNC_PIN GPIO_PIN_8

// 800x600@60Hz

/*
#define PIXEL_FREQ 40000000

#define H_FRONT_PORCH 40
#define H_SYNC_PULSE 128
#define H_BACK_PORCH 88
#define H_ACTIVE_VIDEO 800
#define H_LINE 1056

#define V_FRONT_PORCH 1
#define V_SYNC_PULSE 4
#define V_BACK_PORCH 23
#define V_ACTIVE_VIDEO 600
#define V_FRAME 628
*/

// 640x480@60Hz

#define PIXEL_FREQ 20000000

#define H_FRONT_PORCH 16
#define H_SYNC_PULSE 96
#define H_BACK_PORCH 48
#define H_ACTIVE_VIDEO 640
#define H_LINE 800

#define V_FRONT_PORCH 10
#define V_SYNC_PULSE 2
#define V_BACK_PORCH 33
#define V_ACTIVE_VIDEO 480
#define V_FRAME 525

#define CK_SYS 100000000
#define PIXEL_FREQ_MUL (CK_SYS / PIXEL_FREQ)

#endif /* CONFIG_H */
