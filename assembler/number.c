#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "error.h"
#include "string.h"

error_t
hex2num(string_t *s, uint32_t *n)
{
    int i = 0, count = 0;
    char c;
    *n = 0;
    while (true) {
	c = s->data[s->length - 1 - i];
	if (c >= 0x30 && c <= 0x39) {
	    c -= 0x30;
	} else if (c >= 0x41 && c <= 0x46) {
	    c -= (0x41 - 10);
	} else if (c >= 0x61 && c <= 0x66) {
	    c -= (0x61 - 10);
	} else if (c != '_') {
	    return ERR_NUM_DEC;
	}
	if (c != '_') {
	    *n += ((uint32_t) c) << (4 * count);
	    count++;
	}

	i++;
	if (i >= s->length) {
	    break;
	}
	if (count >= 4*2) {
	    return ERR_NUM_LONG;
	}
    }
    return OK;
}

error_t
bin2num(string_t *s, uint32_t *n)
{
    int i = 0, count = 0;
    char c;
    *n = 0;
    while (true) {
	c = s->data[s->length - 1 - i];
	if (c >= 0x30 && c <= 0x31) {
	    c -= 0x30;
	} else if (c != '_') {
	    return ERR_NUM_DEC;
	}
	if (c != '_') {
	    *n += ((uint32_t) c) << (1 * count);
	    count++;
	}

	i++;
	if (i >= s->length) {
	    break;
	}
	if (count >= 32) {
	    return ERR_NUM_LONG;
	}
    }
    return OK;
}

error_t
dec2num(string_t *s, uint32_t *n)
{
    int i = 0, count = 0;
    uint32_t pow = 1;
    char c;
    *n = 0;
    while (true) {
	c = s->data[s->length - 1 - i];
	if (c >= 0x30 && c <= 0x39) {
	    c -= 0x30;
	} else if (c != '_') {
	    return ERR_NUM_DEC;
	}
	if (c != '_') {
	    *n += ((uint32_t) c) * pow;
	    pow *= 10;
	    count++;
	}

	i++;
	if (i >= s->length) {
	    break;
	}
	if (count >= 10) {
	    return ERR_NUM_LONG;
	}
    }
    return OK;
}

error_t
str2num(string_t *s, uint32_t *n)
{
    if (s->data[0] < 0x30 || s->data[0] > 0x39) {
	return ERR_NUM_DEC;
    }
    string_t slice;
    if (string_has_prefix(s, "0x")) {
	string_slice(s, &slice, 2, 0);
	return hex2num(&slice, n);
    } else if (string_has_prefix(s, "0b")) {
	string_slice(s, &slice, 2, 0);
	return bin2num(&slice, n);
    } else {
	return dec2num(s, n);
    }
}

#ifdef TEST

#include "assertion-macros.h"

int
main()
{
    string_t s;
    uint32_t n;

    //
    // hex
    //

    string_set_ref_c(&s, "12abCD");
    assert_equal(hex2num(&s, &n), OK);
    assert_equal(n, 0x12abCD);

    string_set_ref_c(&s, "12_ab_CD");
    assert_equal(hex2num(&s, &n), OK);
    assert_equal(n, 0x12abCD);

    string_set_ref_c(&s, "0");
    assert_equal(hex2num(&s, &n), OK);
    assert_equal(n, 0x0);

    string_set_ref_c(&s, "1122334455");
    assert_equal(hex2num(&s, &n), ERR_NUM_LONG);

    string_set_ref_c(&s, "0xqwerty");
    assert_equal(hex2num(&s, &n), ERR_NUM_DEC);

    //
    // bin
    //

    string_set_ref_c(&s, "1011001110");
    assert_equal(bin2num(&s, &n), OK);
    assert_equal(n, 0x2ce);

    string_set_ref_c(&s, "10_1100_1110");
    assert_equal(bin2num(&s, &n), OK);
    assert_equal(n, 0x2ce);

    string_set_ref_c(&s, "0");
    assert_equal(bin2num(&s, &n), OK);
    assert_equal(n, 0x0);

    string_set_ref_c(&s, "11111111111111111111111111111111");
    assert_equal(bin2num(&s, &n), OK);
    assert_equal(n, 0xffffffff);

    string_set_ref_c(&s, "111111111111111111111111111111110");
    assert_equal(bin2num(&s, &n), ERR_NUM_LONG);

    string_set_ref_c(&s, "2");
    assert_equal(bin2num(&s, &n), ERR_NUM_DEC);

    //
    // dec
    //

    string_set_ref_c(&s, "123456");
    assert_equal(dec2num(&s, &n), OK);
    assert_equal(n, 123456);

    string_set_ref_c(&s, "123_456");
    assert_equal(dec2num(&s, &n), OK);
    assert_equal(n, 123456);

    string_set_ref_c(&s, "0");
    assert_equal(dec2num(&s, &n), OK);
    assert_equal(n, 0);

    string_set_ref_c(&s, "12345678901");
    assert_equal(dec2num(&s, &n), ERR_NUM_LONG);

    string_set_ref_c(&s, "0a");
    assert_equal(dec2num(&s, &n), ERR_NUM_DEC);

    //
    // str
    //

    string_set_ref_c(&s, "123456");
    assert_equal(str2num(&s, &n), OK);
    assert_equal(n, 123456);

    string_set_ref_c(&s, "0x12abCD");
    assert_equal(str2num(&s, &n), OK);
    assert_equal(n, 0x12abCD);

    string_set_ref_c(&s, "0b1011001110");
    assert_equal(str2num(&s, &n), OK);
    assert_equal(n, 0x2ce);

    string_set_ref_c(&s, "a");
    assert_equal(str2num(&s, &n), ERR_NUM_DEC);

    string_set_ref_c(&s, "0xy");
    assert_equal(str2num(&s, &n), ERR_NUM_DEC);

    return 0;
}

#endif
