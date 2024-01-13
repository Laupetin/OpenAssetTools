#pragma once

#include <ostream>
#include <string>

namespace cache
{
    class AbstractCacheWriter
    {
    protected:
        explicit AbstractCacheWriter(std::ostream& stream);

        void WriteFileDependency(const std::string& dependencyName, uint64_t timestamp) const;
        void WriteGdtDependency(const std::string& dependencyName, uint32_t crc32) const;

        void Write(const void* data, size_t dataSize) const;
        void WriteString(const char* str) const;

        void WriteBool(bool value) const;
        void WriteTrue() const;
        void WriteFalse() const;

        std::ostream& m_stream;
    };
} // namespace cache
