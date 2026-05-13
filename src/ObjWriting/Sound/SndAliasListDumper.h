#pragma once

#include "Csv/CsvStream.h"

namespace snd_alias_list
{
    class SndAliasListDumper : public CsvOutputStream
    {
    public:
        explicit SndAliasListDumper(std::ostream& stream);

        void WriteAliasFileHeader(const char* aliasHeaders[], size_t headerCount);
        void WriteColumnIntegral(const int& value, const int& defaultVal = 0);
        void WriteColumnString(const char* stringValue);
        void WriteColumnString(const std::string& stringValue);
        void WriteColumnEnumWithSize(const unsigned value, const char* const* enumValues, const size_t enumValueCount);
        void WriteColumnEnumFlagsWithSize(const unsigned value, const char* const* enumValues, const size_t enumValueCount);
        void WriteColumnVolumeLinear(float value);
        void WriteColumnFloat(float value, float defaultVal = -1.0f, int precision = 2);
        void WriteChannelEnum(int channel, const char* channelNames[], size_t channelCount);
        
        template<size_t Size> 
        void WriteColumnEnum(const unsigned value, const char* const (&enumValues)[Size]);
        template<size_t Size> 
        void WriteColumnEnumFlags(const unsigned value, const char* const (&enumValues)[Size]);
    };
} // namespace snd_alias_list
