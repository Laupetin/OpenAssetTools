#pragma once
#include "Sound/WavTypes.h"

#include <istream>

class WavReader
{
public:
    explicit WavReader(std::istream& stream);

    bool ReadPcmHeader(WavMetaData& metaData, size_t& dataLen) const;
    bool ReadPcmData(void* outBuffer, size_t dataLen) const;

private:
    static bool ReadExact(std::istream& s, void* buf, std::streamsize n);
    static uint32_t MakeId(const char id[4]);

    std::istream& m_stream;
};
