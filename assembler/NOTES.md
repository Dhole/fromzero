# Parameters

MAX_LABEL_LEN = 16
MAX_LINE_LEN = 100

## String with max length = 16

- Fixed (16 + 1 = 17 bytes)
    - char [16]value
    - uint8_t length
- Dynamic (4 + 1 + 0..16 = 5..21)
    - char *value
    - uint8_t length
