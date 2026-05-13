#pragma once

#include "Csv/CsvStream.h"

#include <string>

namespace speaker_map
{
    std::string GetFileNameForSpeakerMapName(const std::string& assetName);
    int GetSpeakerNameIndex(std::string speakerName, const char* speakerMapNames[], size_t speakerMapCount);
    std::vector<std::string> SplitWhitespace(const std::string& row);
} // namespace sound_alias_list
