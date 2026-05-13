#pragma once

#include "Csv/CsvStream.h"

#include <unordered_map>
#include <string>

namespace snd_alias_list
{
class SndAliasListLoader : public CsvInputStream
{
public:
    explicit SndAliasListLoader(std::istream& stream);

    float ReadColumnFloat(std::string cell, float defaultVal = -1.0f);
    char* ReadColumnString(std::string cell, MemoryManager& memory);
    int ReadColumnInt(std::string cell, int defaultVal = -1);
    float ReadColumnMasterAndSet(int& flags, std::string masterStr);
    void SetFlag(int& flags, int bit, bool enabled) const;
    void SetReverbFlag(int& flags, const std::string& reverbString);
    void SetLoopFlag(int& flags, std::string loopStr);
    int FindChannelIndex(const char* name, const char** channelNames, size_t channelCount);
    float GetVolumeMod(std::string_view name, const std::unordered_map<std::string_view, float>& volModMap);
    void SetChannelFlag(int& flags, std::string channelName, const char* channelNames[], size_t channelCount);
    void SetChannelIndex(int& rawFlags, unsigned channelIndex);
    int CountAliases(unsigned minColCount) const;
};
} // namespace snd_alias_list
