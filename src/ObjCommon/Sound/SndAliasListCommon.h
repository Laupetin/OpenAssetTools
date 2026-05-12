#pragma once

#include "Csv/CsvStream.h"

#include <string>
#include <iostream>
#include <filesystem>

namespace snd_alias_list
{
    std::string GetFileNameForAssetName(const std::string& assetName);
    std::string GetFileNameForSpeakerMapName(const std::string& assetName);

    float ReadColumnFloat(std::string cell, float defaultVal = -1.0f);
    int ReadColumnInt(std::string cell, int defaultVal = -1);
    float ReadColumnMasterAndSet(int& flags, std::string masterStr);

    void SetFlag(int& flags, int bit, bool enabled);
    void SetReverbFlag(int& flags, const std::string& reverbString);
    void SetLoopFlag(int& flags, std::string loopStr);

    int FindChannelIndex(const char* name, const char** channelNames, size_t channelCount);
    float GetVolumeMod(std::string_view name, const std::unordered_map<std::string_view, float>& volModMap);

    void SetChannelIndex(int& rawFlags, unsigned channelIndex);
    size_t CountAliases(std::istream& in, unsigned minColCount);

    std::pair<std::string, std::string> SplitPathParts(const std::string& path);
    std::vector<std::string> SplitWhitespace(const std::string& row);

} // namespace sound_alias_list
