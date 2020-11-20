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
    KEY_RELEASE = 0xF0,
};

enum key_state {
    unpressed,
    pressed,
    release,
};

#endif /* KEYBOARD_H */
