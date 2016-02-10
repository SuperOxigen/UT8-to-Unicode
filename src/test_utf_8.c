#include <stdio.h>
#include <locale.h>
#include "utf_8.h"

int main(void)
{
    wchar_t wstr[] = L"Привет!";
    char str[20];

    wchar_to_utf_8(str, 20, wstr);

    char    cbuf[20];
    wchar_t wbuf[20];

    utf_8_to_wchar(wbuf, 20, str);
    for (volatile int i = 0; i < 100000000; i++)
    {
        wchar_to_utf_8(cbuf, 20, wbuf);
        utf_8_to_wchar(wbuf, 20, cbuf);
    }

    setlocale(LC_ALL, "");
    printf("UNI:   %ls\n", wbuf);

    return 0;
}

