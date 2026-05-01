#include "WordSize.h"

#include <cassert>

unsigned GetPointerSizeForWordSize(const WordSize wordSize)
{
    switch (wordSize)
    {
    case WordSize::BITS_32:
        return sizeof(uint32_t);

    case WordSize::BITS_64:
        return sizeof(uint64_t);

    default:
        assert(false);
        return sizeof(uint32_t);
    }
}
