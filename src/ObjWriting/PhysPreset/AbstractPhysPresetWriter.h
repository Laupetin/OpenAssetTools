#pragma once

#include "IPhysPresetWriter.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace physpreset
{
    class AbstractBaseWriter : public IWriter
    {
    protected:

    public:

    protected:
        explicit AbstractBaseWriter(std::ostream& stream);

        void WriteCString(const char* str);

        void WriteRawSingle(float val);
        void WriteRawDouble(float val);

        void WriteRawU64(uint64_t val);
        void WriteRawU32(uint32_t val);
        void WriteRawU16(uint16_t val);
        void WriteRawU8(uint8_t val);

        void WriteRawI64(uint64_t val);
        void WriteRawI32(uint32_t val);
        void WriteRawI16(uint16_t val);
        void WriteRawI8(uint8_t val);

        std::ostream& m_stream;
    };
} // namespace font
