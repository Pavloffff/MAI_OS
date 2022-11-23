#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kptools.h"

char *getString()
{
    size_t sz = 0;
    char *_str = (char *) calloc(1, sizeof(char));
    char c;
    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            break;
        }
        _str[sz] = c;
        _str = (char *) realloc(_str, (++sz + 1) * sizeof(char));
    }
    _str[sz++] = '\0';
    return _str;
}
