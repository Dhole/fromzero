#ifndef KEYBOARD_H
#define KEYBOARD_H

enum keys {
    KEY_A	= 0x1C,
    KEY_B	= 0x32,
    KEY_C	= 0x21,
    KEY_D	= 0x23,
    KEY_E	= 0x24,
    KEY_F	= 0x2B,
    KEY_G	= 0x34,
    KEY_H	= 0x33,
    KEY_I	= 0x43,
    KEY_J	= 0x3B,
    KEY_K	= 0x42,
    KEY_L	= 0x4B,
    KEY_M	= 0x3A,
    KEY_N	= 0x31,
    KEY_O	= 0x44,
    KEY_P	= 0x4D,
    KEY_Q	= 0x15,
    KEY_R	= 0x2D,
    KEY_S	= 0x1B,
    KEY_T	= 0x2C,
    KEY_U	= 0x3C,
    KEY_V	= 0x2A,
    KEY_W	= 0x1D,
    KEY_X	= 0x22,
    KEY_Y	= 0x35,
    KEY_Z	= 0x1A,
    KEY_SPACE	= 0x29,
    KEY_ENTER   = 0x5A,
    KEY_BKSP    = 0x66,
    MOD_RELEASE = 0xF0,
    MOD_MOVE    = 0xE0,
    KEY_SHIFT   = 0x12,
    KEY_TAB     = 0x0D,
    KEY_CTL     = 0x14,
    KEY_ALT     = 0x11,
    KEY_CAPS    = 0x58,
    KEY_1       = 0x16,
    KEY_2       = 0x1E,
    KEY_3       = 0x26,
    KEY_4       = 0x25,
    KEY_5       = 0x2E,
    KEY_6       = 0x36,
    KEY_7       = 0x3D,
    KEY_8       = 0x3E,
    KEY_9       = 0x46,
    KEY_0       = 0x45,
    KEY_BACKTCK = 0x0E,
    KEY_MINUS   = 0x4E,
    KEY_EQUAL   = 0x55,
    KEY_OPEN    = 0x54,
    KEY_CLOSE   = 0x5B,
    KEY_SCOLON  = 0x4C,
    KEY_QUOTE   = 0x52,
    KEY_BACKSL  = 0x5D,
    KEY_COMMA   = 0x41,
    KEY_DOT     = 0x49,
    KEY_SLASH   = 0x4A,
    KEY_F1      = 0x05,
    KEY_F2      = 0x06,
    KEY_F3      = 0x04,
    KEY_F4      = 0x0C,
    KEY_F5      = 0x03,
    KEY_F6      = 0x0B,
    KEY_F7      = 0x83,
    KEY_F8      = 0x0A,
    KEY_F9      = 0x01,
    KEY_F10     = 0x09,
    KEY_F11     = 0x78,
    KEY_F12     = 0x07,
    KEY_NP7     = 0x6C, //
    KEY_NP8     = 0x75, // UP
    KEY_NP9     = 0x7D, //
    KEY_NP4     = 0x6B, // LEFT
    KEY_NP5     = 0x73, //
    KEY_NP6     = 0x74, // RIGHT
    KEY_NP1     = 0x69, //
    KEY_NP2     = 0x72, // DOWN
    KEY_NP3     = 0x7A, //
    KEY_NP0     = 0x70, //
};
#define KEY_MASK      ((uint16_t)0x00ff)
#define KEY_TYPE_MASK ((uint16_t)0x0f00)
#define KEY_MOD_MASK  ((uint16_t)0xf000)

// enum key_type {
    // KEY_TYPE_NORMAL =  1 << (8 + 0),
#define KEY_TYPE_RELEASE  ((uint16_t)(1 << (8 + 0)))
#define KEY_TYPE_MOVE     ((uint16_t)(1 << (8 + 1)))
// };

// enum key_mod {
    // KEY_MOD_NONE =  1 << (12 + 0),
#define KEY_MOD_SHIFT  ((uint16_t)(1 << (12 + 0)))
#define KEY_MOD_CTL    ((uint16_t)(1 << (12 + 1)))
#define KEY_MOD_ALT    ((uint16_t)(1 << (12 + 2)))
// };

#define KEY_BUF_LEN 16
extern uint16_t key_buf[KEY_BUF_LEN];
extern volatile uint8_t key_buf_head;
extern volatile uint8_t key_buf_tail;

#endif /* KEYBOARD_H */
