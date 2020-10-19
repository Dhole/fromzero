#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>

#include "error.h"
#include "string.h"

error_t hex2num(string_t *s, uint32_t *n);
error_t bin2num(string_t *s, uint32_t *n);
error_t dec2num(string_t *s, uint32_t *n);
error_t str2num(string_t *s, int32_t *n);

#endif // INSTRUCTIONS_H
