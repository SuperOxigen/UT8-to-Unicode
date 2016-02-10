#include "utf_8.h"

#define MAX_1_BYTE 0x7F
#define MAX_2_BYTE 0x7FF
#define MAX_3_BYTE 0xFFFF
#define MAX_4_BYTE 0x1FFFFF
#define MAX_5_BYTE 0x3FFFFFF
#define MAX_6_BYTE 0x7FFFFFFF

#define BAD_WC -1

#define BAD_CH -1
#define CONT_CH -2

#define START_BYTE_1 '\x00'
#define START_MASK_1 '\x7F'
#define START_BYTE_2 '\xC0'
#define START_MASK_2 '\x1F'
#define START_BYTE_3 '\xE0'
#define START_MASK_3 '\x0F'
#define START_BYTE_4 '\xF0'
#define START_MASK_4 '\x07'
#define START_BYTE_5 '\xF8'
#define START_MASK_5 '\x03'
#define START_BYTE_6 '\xFC'
#define START_MASK_6 '\x01'
#define CONT_BYTE '\x80'
#define CONT_MASK '\x3F'

#define INVALID_CHAR L'\xFFFD'

static int wchar_byte_size(wchar_t wc)
{
    uint32_t ui32 = (uint32_t) wc;
    if (ui32 <= MAX_1_BYTE)
        return 1;
    if (ui32 <= MAX_2_BYTE)
        return 2;
    if (ui32 <= MAX_3_BYTE)
        return 3;
    if (ui32 <= MAX_4_BYTE)
        return 4;
    if (ui32 <= MAX_5_BYTE)
        return 5;
    if (ui32 <= MAX_6_BYTE)
        return 6;
    return BAD_WC;
}

static int utf_8_expected_byte_length(char const * utf)
{
    if (!*utf) return 0;
    char utfc = *utf;
    if ((utfc & ~START_MASK_1) == START_BYTE_1)
        return 1;
    if ((utfc & ~START_MASK_2) == START_BYTE_2)
        return 2;
    if ((utfc & ~START_MASK_3) == START_BYTE_3)
        return 3;
    if ((utfc & ~START_MASK_4) == START_BYTE_4)
        return 4;
    if ((utfc & ~START_MASK_5) == START_BYTE_5)
        return 5;
    if ((utfc & ~START_MASK_6) == START_BYTE_6)
        return 6;
    if ((utfc & ~CONT_MASK) == CONT_BYTE)
        return CONT_CH;
    else 
        return BAD_CH;
}

static int utf_8_actual_byte_length(char const * utf)
{
    if (!*utf) return 0;
    if ((*utf & ~CONT_MASK) != CONT_BYTE)
    {
        int bc = 1;
        utf++;
        while ((*utf & ~CONT_MASK) == CONT_BYTE)
        {
            bc++;
            utf++;
        }
        return bc;
    }
    else
    {
        return BAD_CH;
    }
}

static int utf_8_next_non_cont(char const * utf)
{
    if (!*utf || !*++utf) return 0;
    int bc = 1;
    while ((*utf & ~CONT_MASK) == CONT_BYTE)
    {
        bc++;
        utf++;
    }
    return 1;
}

static inline char wchar_get_char_bits(wchar_t wc, uint32_t i)
{
    return (wc >> i);
}

static inline wchar_t wchar_put_char_bits(char cc, uint32_t i)
{
    wchar_t wc = ((wchar_t) cc) & 0xFF; // Only char bits
    return (wc << i);
}

static char * put_wchar_to_utf_8(char * buffer, uint32_t n, wchar_t const wc)
{
    uint32_t bytes = wchar_byte_size(wc);
    if (bytes > n) return buffer;
    if (bytes == 1)
    {
        buffer[0] = START_BYTE_1;
        buffer[0] |= wchar_get_char_bits(wc, 0) & START_MASK_1;
    }
    else if (bytes == 2)
    {
        buffer[0] = START_BYTE_2;
        buffer[0] |= wchar_get_char_bits(wc, 6) & START_MASK_2;
        buffer[1] = CONT_BYTE;
        buffer[1] |= wchar_get_char_bits(wc, 0) & CONT_MASK;
    }
    else if (bytes == 3)
    {
        buffer[0] = START_BYTE_3;
        buffer[0] |= wchar_get_char_bits(wc, 12) & START_MASK_3;
        buffer[1] = CONT_BYTE;
        buffer[1] |= wchar_get_char_bits(wc, 6) & CONT_MASK;
        buffer[2] = CONT_BYTE;
        buffer[2] |= wchar_get_char_bits(wc, 0) & CONT_MASK;
    }
    else if (bytes == 4)
    {
        buffer[0] = START_BYTE_4;
        buffer[0] |= wchar_get_char_bits(wc, 18) & START_MASK_4;
        buffer[1] = CONT_BYTE;
        buffer[1] |= wchar_get_char_bits(wc, 12) & CONT_MASK;
        buffer[2] = CONT_BYTE;
        buffer[2] |= wchar_get_char_bits(wc, 6) & CONT_MASK;
        buffer[3] = CONT_BYTE;
        buffer[3] |= wchar_get_char_bits(wc, 0) & CONT_MASK;
    }
    else if (bytes == 5)
    {
        buffer[0] = START_BYTE_5;
        buffer[0] |= wchar_get_char_bits(wc, 24) & START_MASK_5;
        buffer[1] = CONT_BYTE;
        buffer[1] |= wchar_get_char_bits(wc, 18) & CONT_MASK;
        buffer[2] = CONT_BYTE;
        buffer[2] |= wchar_get_char_bits(wc, 12) & CONT_MASK;
        buffer[3] = CONT_BYTE;
        buffer[3] |= wchar_get_char_bits(wc, 6) & CONT_MASK;
        buffer[4] = CONT_BYTE;
        buffer[4] |= wchar_get_char_bits(wc, 0) & CONT_MASK;
    }
    else if (bytes == 6)
    {
        buffer[0] = START_BYTE_6;
        buffer[0] |= wchar_get_char_bits(wc, 30) & START_MASK_6;
        buffer[1] = CONT_BYTE;
        buffer[1] |= wchar_get_char_bits(wc, 24) & CONT_MASK;
        buffer[2] = CONT_BYTE;
        buffer[2] |= wchar_get_char_bits(wc, 18) & CONT_MASK;
        buffer[3] = CONT_BYTE;
        buffer[3] |= wchar_get_char_bits(wc, 12) & CONT_MASK;
        buffer[4] = CONT_BYTE;
        buffer[4] |= wchar_get_char_bits(wc, 6) & CONT_MASK;
        buffer[5] = CONT_BYTE;
        buffer[5] |= wchar_get_char_bits(wc, 0) & CONT_MASK;
    }
    else
    {
        return 0;
    }

    return buffer + bytes;
}

static char const * put_utf_8_to_wchar(wchar_t * buffer, char const * source)
{
    int el = utf_8_expected_byte_length(source);
    int al = utf_8_actual_byte_length(source);
    if (al == el && al != 0)
    {
        if (al == 1)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_1, 0);
        }
        else if (al == 2)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_2, 6);
            *buffer |= wchar_put_char_bits(source[1] & CONT_MASK, 0);
        }
        else if (al == 3)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_3, 12);
            *buffer |= wchar_put_char_bits(source[1] & CONT_MASK, 6);
            *buffer |= wchar_put_char_bits(source[2] & CONT_MASK, 0);
        }
        else if (al == 4)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_4, 18);
            *buffer |= wchar_put_char_bits(source[1] & CONT_MASK, 12);
            *buffer |= wchar_put_char_bits(source[2] & CONT_MASK, 6);
            *buffer |= wchar_put_char_bits(source[3] & CONT_MASK, 0);
        }
        else if (al == 5)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_5, 24);
            *buffer |= wchar_put_char_bits(source[1] & CONT_MASK, 18);
            *buffer |= wchar_put_char_bits(source[2] & CONT_MASK, 12);
            *buffer |= wchar_put_char_bits(source[3] & CONT_MASK, 6);
            *buffer |= wchar_put_char_bits(source[4] & CONT_MASK, 0);
        }
        else if (al == 6)
        {
            *buffer = wchar_put_char_bits(source[0] & START_MASK_6, 30);
            *buffer |= wchar_put_char_bits(source[1] & CONT_MASK, 24);
            *buffer |= wchar_put_char_bits(source[2] & CONT_MASK, 18);
            *buffer |= wchar_put_char_bits(source[3] & CONT_MASK, 12);
            *buffer |= wchar_put_char_bits(source[4] & CONT_MASK, 6);
            *buffer |= wchar_put_char_bits(source[5] & CONT_MASK, 0);
        }
    }
    else if (el == CONT_CH)
    {
        *buffer = INVALID_CHAR;
        al = utf_8_next_non_cont(source);
    }
    else if (al == BAD_CH || el == BAD_CH)
    {
        *buffer = INVALID_CHAR;
    }
    else if (al == 0 || el == 0)
    {
        *buffer = L'\00';
        al = 1;
    }
    else
    {
        *buffer = INVALID_CHAR;
        al = utf_8_next_non_cont(source);
    }

    return source + al;
}

char * wchar_to_utf_8(char * buffer, uint32_t n, wchar_t const * source)
{
    char * next, * curr = buffer;
    wchar_t const * curr_source = source;
    uint32_t i;
    int wc_size;

    for (i = 0; i < n && *curr_source != L'\0';)
    {
        wc_size = wchar_byte_size(*curr_source);
        if (wc_size > (n-i))
        {
            break;
        }
        else if (wc_size == BAD_WC)
        {
            wc_size = wchar_byte_size(INVALID_CHAR);
            next = put_wchar_to_utf_8(curr, n-i, INVALID_CHAR);
        }
        else
        {
            next = put_wchar_to_utf_8(curr, n-i, *curr_source);
        }
        i += wc_size;
        curr_source++;
        curr = next;
    }
    for (; i < n; i++)
    {
        buffer[i] = '\0';
    }

    return buffer;
}

wchar_t * utf_8_to_wchar(wchar_t * buffer, uint32_t n, char const * source)
{
    wchar_t * curr = buffer;
    char const * next_source;
    char const * curr_source = source;
    uint32_t i;
    int utf_e_size, utf_a_size, utf_next;

    for (i = 0; i < n && *curr_source != '\0';)
    {
        utf_e_size = utf_8_expected_byte_length(curr_source); // size || Cont Ch || Bad Char
        utf_a_size = utf_8_actual_byte_length(curr_source);   // size || Bad Char
        if (utf_e_size == CONT_CH)
        {
            utf_next = utf_8_next_non_cont(curr_source);
            if (!utf_next) break;
            curr_source += utf_next;
        }
        else if (utf_e_size == BAD_CH || utf_a_size == BAD_CH || utf_e_size > utf_a_size)
        {
            utf_next = utf_8_next_non_cont(curr_source);
            *curr = INVALID_CHAR;
            curr++;
            i++;
            curr_source += utf_next;
        }
        else if (utf_e_size <= utf_a_size)
        {
            next_source = put_utf_8_to_wchar(curr, curr_source);
            curr++;
            i++;
            curr_source = next_source;
        }
        else
        {
            i++;
        }
    }
    for (;i < n; i++)
    {
        buffer[i] = L'\0';
    }

    return buffer;
}
