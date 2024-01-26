#pragma once
#include <Sound/WavTypes.h>
#include <ostream>

class WavWriter
{
public:
    explicit WavWriter(std::ostream& stream);

    void WritePcmHeader(const WavMetaData& metaData, size_t dataLen) const;
    void WritePcmData(const void* data, size_t dataLen) const;

private:
    std::ostream& m_stream;
};
