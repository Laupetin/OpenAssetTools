#include "SndAliasListCommon.h"

#include <format>

namespace snd_alias_list
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.csv", assetName);
    }

    std::string GetFileNameForSpeakerMapName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.spkrmap", assetName);
    }

    void SetFlag(int& flags, int bit, bool enabled)
    {
        if (enabled)
        {
            flags |= bit;
        }
        else
        {
            flags &= ~bit;
        }
    }

    void SetReverbFlag(int& flags, const std::string& reverbString)
    {
        SetFlag(flags, 0x10, false);
        SetFlag(flags, 0x8, false);
        if (!reverbString.empty())
        {
            if (reverbString.contains("nowetlevel"))
            {
                SetFlag(flags, 0x10, true);
            }
            if (reverbString.contains("fulldrylevel"))
            {
                SetFlag(flags, 0x8, true);
            }
        }
    }

    void SetChannelIndex(int& rawFlags, unsigned channelIndex)
    {
        channelIndex &= 0x3F;

        rawFlags = (rawFlags & ~(0x3F << 8)) | ((static_cast<int>(channelIndex) & 0x3F) << 8);
    }

    size_t CountAliases(std::istream& in, unsigned minColCount)
    {
        std::istream::pos_type pos = in.tellg();
        size_t count = 0;
        const CsvInputStream csv(in);
        std::vector<std::string> currentRow;

        // Skip header
        csv.NextRow(currentRow);

        while (csv.NextRow(currentRow))
        {
            CsvInputStream::PreprocessRow(currentRow);

            if (CsvInputStream::RowIsEmpty(currentRow))
                continue;

            if (currentRow.size() < minColCount)
                continue;

            count++;
        }

        in.clear();
        in.seekg(pos);

        return count;
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

    float ReadColumnFloat(std::string cell, float defaultVal)
    {
        if (!cell.empty())
        {
            return std::stof(cell);
        }
        return defaultVal;
    }

    int ReadColumnInt(std::string cell, int defaultVal)
    {
        if (!cell.empty())
        {
            return std::stoi(cell);
        }
        return defaultVal;
    }

    void SetLoopFlag(int& flags, std::string loopStr)
    {
        SetFlag(flags, 0x1, false);
        SetFlag(flags, 0x20, false);
        if (!loopStr.empty())
        {
            SetFlag(flags, 0x1, true);
            if (loopStr == "rlooping")
            {
                SetFlag(flags, 0x20, true);
            }
        }
    }

    std::pair<std::string, std::string> SplitPathParts(const std::string& path)
    {
        if (path.empty())
        {
            return {"", ""};
        }

        const char* p = std::strrchr(path.c_str(), '/');
        if (!p)
        {
            return {"", path};
        }

        std::string dir(path.data(), p - path.c_str());
        std::string file(p + 1);
        return {dir, file};
    }

    int FindChannelIndex(const char* name, const char** channelNames, size_t channelCount)
    {
        if (!name)
        {
            return -1;
        }

        for (std::size_t i = 0; i < channelCount; ++i)
        {
            if (!std::strcmp(channelNames[i], name))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    float GetVolumeMod(std::string_view name, const std::unordered_map<std::string_view, float>& volModMap)
    {
        auto it = volModMap.find(name);
        if (it == volModMap.end())
        {
            return 1.0f;
        }
        return it->second;
    }

    float ReadColumnMasterAndSet(int& flags, std::string masterStr)
    {
        SetFlag(flags, 0x2, false);
        if (!masterStr.empty())
        {
            if (masterStr == "master")
            {
                SetFlag(flags, 0x2, true);
                return 0.0f;
            }
            else
            {
                return ReadColumnFloat(masterStr, 0.0f);
            }
        }
        else
        {
            return 1.0f;
        }
    }
} // namespace snd_alias_list
