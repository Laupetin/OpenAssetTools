#include "CommonT6.h"

#include <cctype>

int CommonT6::Com_HashKey(const char* str, const int maxLen)
{
    if (str == nullptr)
        return 0;

    int hash = 0;
    for (int i = 0; i < maxLen; i++)
    {
        if (str[i] == '\0')
            break;

        hash += str[i] * (0x77 + i);
    }

    return hash ^ ((hash ^ (hash >> 10)) >> 10);
}

int CommonT6::Com_HashString(const char* str, const int len)
{
    if (!str)
        return 0;

    int result = 0x1505;
    int offset = 0;
    while(str[offset])
    {
        if (len > 0 && offset >= len)
            break;

        const int c = tolower(str[offset++]);
        result = c + 33 * result;
    }

    return result;
}