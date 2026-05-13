#include "SndAliasListDumper.h"

#include "Sound/SndAliasListCommon.h"

#include <format>
#include <string>
#include <iostream>
#include <cassert>
#include <sstream>

using namespace snd_alias_list;

SndAliasListDumper::SndAliasListDumper(std::ostream& stream) 
    : CsvOutputStream(stream)
{
    
}

void SndAliasListDumper::WriteAliasFileHeader(const char* aliasHeaders[], size_t headerCount)
{
    for (int i = 0; i < headerCount; i++)
    {
        WriteColumn(aliasHeaders[i]);
    }

    NextRow();
}

void SndAliasListDumper::WriteColumnIntegral(const int& value, const int& defaultVal)
{
    if (value == defaultVal)
    {
        WriteColumn("");
    }
    else
    {
        WriteColumn(std::format("{}", value));
    }
}

void SndAliasListDumper::WriteColumnString(const char* stringValue)
{
    if (stringValue)
    {
        WriteColumn(FromReferencedString(stringValue));
    }
    else
    {
        WriteColumn("");
    }
}

void SndAliasListDumper::WriteColumnString(const std::string& stringValue)
{
    WriteColumnString(stringValue.c_str());
}

void SndAliasListDumper::WriteColumnEnumWithSize(const unsigned value, const char* const* enumValues, const size_t enumValueCount)
{
    assert(value < enumValueCount);
    WriteColumn(value < enumValueCount ? enumValues[value] : "");
}

void SndAliasListDumper::WriteColumnEnumFlagsWithSize(const unsigned value, const char* const* enumValues, const size_t enumValueCount)
{
    assert(enumValueCount <= 32u);
    std::ostringstream ss;
    auto first = true;
    for (auto i = 0u; i < enumValueCount; i++)
    {
        const auto flagValue = 1u << i;
        if (value & flagValue)
        {
            if (first)
                first = false;
            else
                ss << ' ';

            ss << enumValues[i];
        }
    }

    WriteColumn(ss.str());
}

void SndAliasListDumper::WriteColumnVolumeLinear(float value)
{
    WriteColumn(std::format("{:.2f}", value));
}

void SndAliasListDumper::WriteColumnFloat(float value, float defaultVal, int precision)
{
    if (value == defaultVal)
    {
        WriteColumn("");
        return;
    }

    std::string s = std::format("{:.{}f}", value, precision);
    s.erase(s.find_last_not_of('0') + 1);

    if (!s.empty() && s.back() == '.')
    {
        s.pop_back();
    }

    WriteColumn(s);
}

void SndAliasListDumper::WriteChannelEnum(int channel, const char* channelNames[], size_t channelCount)
{
    assert(channel < channelCount);

    WriteColumn(channelNames[channel]);
}

template<size_t Size> 
void SndAliasListDumper::WriteColumnEnum(const unsigned value, const char* const (&enumValues)[Size])
{
    WriteColumnEnumWithSize(value, enumValues, Size);
}

template<size_t Size> 
void SndAliasListDumper::WriteColumnEnumFlags(const unsigned value, const char* const (&enumValues)[Size])
{
    WriteColumnEnumFlagsWithSize(value, enumValues, Size);
}
