#pragma once

#include <wchar.h>
#include <stdint.h>

char * wchar_to_utf_8(char * buffer, uint32_t n, wchar_t const * source);

wchar_t * utf_8_to_wchar(wchar_t * buffer, uint32_t n, char const * source);

