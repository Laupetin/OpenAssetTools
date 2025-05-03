#include "Architecture.h"

#include <cassert>

unsigned GetPointerSizeForArchitecture(const Architecture architecture)
{
    switch (architecture)
    {
    case Architecture::X86:
        return sizeof(uint32_t);

    case Architecture::X64:
        return sizeof(uint64_t);

    default:
        assert(false);
        return sizeof(uint32_t);
    }
}
