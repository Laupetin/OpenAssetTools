#pragma once

#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace cache
{
    class AbstractCacheReader
    {
    protected:
        AbstractCacheReader(std::istream& stream, MemoryManager& memory);

        void Read(void* data, size_t dataSize) const;
        const char* ReadString() const;

        _NODISCARD bool ReadBool() const;

        MemoryManager& m_memory;
        std::istream& m_stream;
    };
} // namespace cache
