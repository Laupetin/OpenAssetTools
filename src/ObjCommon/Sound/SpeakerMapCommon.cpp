#include "SpeakerMapCommon.h"

#include "SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace speaker_map
{
    std::string GetFileNameForSpeakerMapName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.spkrmap", assetName);
    }

    int GetSpeakerNameIndex(std::string speakerName, const char* speakerMapNames[], size_t speakerMapCount)
    {
        for (int i = 0; i < speakerMapCount; i++)
        {
            std::string currentName = speakerMapNames[i];
            if (currentName == speakerName)
            {
                return i;
            }
        }
        return -1;
    }

    std::vector<std::string> SplitWhitespace(const std::string& row)
    {
        std::istringstream iss(row);
        std::vector<std::string> out;
        std::string token;

        while (iss >> token)
        {
            out.push_back(token);
        }

        return out;
    }
} // namespace speaker_map
