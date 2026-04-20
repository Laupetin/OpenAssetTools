#include "AbstractWeaponWriter.h"

#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <Dumping/AssetDumpingContext.h>

namespace weapon
{
    AbstractBaseWriter::AbstractBaseWriter(std::ostream& stream, AssetDumpingContext& context)
        : m_stream(stream)
        , m_context(context)
    {
    }

    void AbstractBaseWriter::WriteCString(const char* str)
    {
        if (!str)
        {
            char null = '\0';
            m_stream.write(&null, 1);
            return;
        }

        const size_t len = std::strlen(str) + 1; // include null terminator
        m_stream.write(str, len);
    }

    void AbstractBaseWriter::WriteRawSingle(float val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawDouble(float val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawU64(uint64_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawU32(uint32_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawU16(uint16_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawU8(uint8_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawI64(uint64_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawI32(uint32_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawI16(uint16_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    void AbstractBaseWriter::WriteRawI8(uint8_t val)
    {
        m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
} // namespace weapon
