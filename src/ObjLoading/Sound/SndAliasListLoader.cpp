#include "SndAliasListLoader.h"

#include "Utils/Logging/Log.h"
#include "Csv/CsvStream.h"

#include <format>
#include <string>
#include <sstream>

using namespace snd_alias_list;

SndAliasListLoader::SndAliasListLoader(std::istream& stream)
    : CsvInputStream(stream)
{}

void SndAliasListLoader::SetFlag(int& flags, int bit, bool enabled) const
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

void SndAliasListLoader::SetReverbFlag(int& flags, const std::string& reverbString)
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

void SndAliasListLoader::SetChannelIndex(int& rawFlags, unsigned channelIndex)
{
    channelIndex &= 0x3F;

    rawFlags = (rawFlags & ~(0x3F << 8)) | ((static_cast<int>(channelIndex) & 0x3F) << 8);
}

int SndAliasListLoader::CountAliases(unsigned minColCount) const
{
    std::istream::pos_type pos = m_stream.tellg();
    int count = 0;
    const CsvInputStream csv(m_stream);
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

    m_stream.clear();
    m_stream.seekg(pos);

    return count;
}


float SndAliasListLoader::ReadColumnFloat(std::string cell, float defaultVal)
{
    if (!cell.empty())
    {
        return std::stof(cell);
    }
    return defaultVal;
}

char* SndAliasListLoader::ReadColumnString(std::string cell, MemoryManager& memory)
{
    if (!cell.empty())
    {
        return memory.Dup(cell.c_str());
    }
    return nullptr;
}

int SndAliasListLoader::ReadColumnInt(std::string cell, int defaultVal)
{
    if (!cell.empty())
    {
        return std::stoi(cell);
    }
    return defaultVal;
}

void SndAliasListLoader::SetLoopFlag(int& flags, std::string loopStr)
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

int SndAliasListLoader::FindChannelIndex(const char* name, const char** channelNames, size_t channelCount)
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

float SndAliasListLoader::GetVolumeMod(std::string_view name, const std::unordered_map<std::string_view, float>& volModMap)
{
    auto it = volModMap.find(name);
    if (it == volModMap.end())
    {
        return 1.0f;
    }
    return it->second;
}

void SndAliasListLoader::SetChannelFlag(int& flags, std::string channelName, const char* channelNames[], size_t channelCount)
{
    if (!channelName.empty())
    {
        int channelIndex = FindChannelIndex(channelName.c_str(), channelNames, channelCount);
        if (channelIndex == -1)
        {
            con::warn(std::format("Found invalid channel '{}', defaulting to 'auto'", channelName));
            SetChannelIndex(flags, 0);
        }
        else
        {
            SetChannelIndex(flags, channelIndex);
        }
    }
    else
    {
        SetChannelIndex(flags, 0);
    }
}

float SndAliasListLoader::ReadColumnMasterAndSet(int& flags, std::string masterStr)
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
