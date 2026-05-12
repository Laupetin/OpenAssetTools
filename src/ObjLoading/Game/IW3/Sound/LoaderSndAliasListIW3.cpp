#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <Csv/CsvStream.h>
#include <Game/IW3/Sound/SndAliasListConstantsIW3.h>
#include <format>
#include <iostream>
#include <filesystem>
#include <map>
#include <set>

using namespace IW3;
namespace fs = std::filesystem;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

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

    float GetVolumeMod(std::string_view name)
    {
        auto it = volume_mod_groups_table.find(name);
        if (it == volume_mod_groups_table.end())
        {
            return 1.0f;
        }
        return it->second;
    }

    int FindChannelIndex(const char* name)
    {
        if (!name)
        {
            return -1;
        }

        for (std::size_t i = 0; i < std::extent_v<decltype(snd_alias_channel_names)>; ++i)
        {
            if (!std::strcmp(snd_alias_channel_names[i], name))
            {
                return static_cast<int>(i);
            }
        }
        return -1;
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

    void SetChannelIndex(int& rawFlags, unsigned channelIndex)
    {
        channelIndex &= 0x3F;

        rawFlags = (rawFlags & ~(0x3F << 8)) | ((static_cast<int>(channelIndex) & 0x3F) << 8);
    }

    size_t CountAliases(std::istream& in)
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

            if (currentRow.size() < COL_COUNT_MIN)
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

    int GetSpeakerNameIndex(std::string speakerName)
    {
        for (int i = 0; i < std::extent_v<decltype(speaker_map_names)>; i++)
        {
            std::string currentName = speaker_map_names[i];
            if (currentName == speakerName)
            {
                return i;
            }
        }
        return -1;
    }

    int GetSourceNameIndex(std::string sourceName)
    {
        if (sourceName == "MONOSOURCE")
        {
            return 0;
        }
        else if (sourceName == "LEFTSOURCE")
        {
            return 1;
        }
        else if (sourceName == "RIGHTSOURCE")
        {
            return 2;
        }
        return -1;
    }

    struct TempSpeaker
    {
        int speaker = 0;
        std::vector<float> levels;
    };

    struct TempChannelMap
    {
        std::map<int, TempSpeaker> speakers;
    };

    void FinalizeChannelMap(const TempChannelMap& temp, MSSChannelMap& out)
    {
        out.speakerCount = (int)temp.speakers.size();

        int i = 0;

        for (const auto& [speakerIndex, tempSpeaker] : temp.speakers)
        {
            MSSSpeakerLevels& dst = out.speakers[i];

            dst.speaker = speakerIndex;
            dst.numLevels = (int)tempSpeaker.levels.size();

            for (int j = 0; j < dst.numLevels && j < 2; j++)
            {
                dst.levels[j] = tempSpeaker.levels[j];
            }

            i++;
        }
    }

    void ReadSpeakerMap(std::istream& in, SpeakerMap& out)
    {
        std::string line;

        // Skip header
        std::getline(in, line);

        std::vector<TempChannelMap> tempMaps;

        TempChannelMap current;

        while (std::getline(in, line))
        {
            // Blank line = next channel map
            if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            {
                if (!current.speakers.empty())
                {
                    tempMaps.push_back(current);
                    current = TempChannelMap();
                }

                continue;
            }

            auto tokens = SplitWhitespace(line);

            if (tokens.size() != 3)
                continue;

            int sourceIndex = GetSourceNameIndex(tokens[0]);
            int speakerIndex = GetSpeakerNameIndex(tokens[1]);
            float level = std::stof(tokens[2]);

            if (speakerIndex < 0)
                continue;

            auto& speaker = current.speakers[speakerIndex];

            speaker.speaker = speakerIndex;
            speaker.levels.push_back(level);
        }

        // Final map
        if (!current.speakers.empty())
        {
            tempMaps.push_back(current);
        }

        // Copy into final structure
        int mapIndex = 0;

        for (const auto& temp : tempMaps)
        {
            int x = mapIndex % 2;
            int y = mapIndex / 2;

            FinalizeChannelMap(temp, out.channelMaps[x][y]);

            mapIndex++;
        }
    }


    class SndAliasListLoader : public AssetCreator<AssetSound>
    {
    public:
        SndAliasListLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = snd_alias::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            size_t entryCount = CountAliases(*file.m_stream);
            auto* aliasList = m_memory.Alloc<snd_alias_list_t>();
            aliasList->aliasName = m_memory.Dup(assetName.c_str());
            aliasList->count = entryCount;
            aliasList->head = m_memory.Alloc<snd_alias_t>(entryCount);
            AssetRegistration<AssetSound> registration(assetName, aliasList);

            const CsvInputStream csv(*file.m_stream);
            std::vector<std::string> currentRow;
            std::vector<snd_alias_t> aliases;

            // Skip header
            csv.NextRow(currentRow);

            auto currentRowIndex = 0u;
            auto dataRowIndex = 0u;
            while (csv.NextRow(currentRow))
            {
                currentRowIndex++;
                CsvInputStream::PreprocessRow(currentRow);

                if (CsvInputStream::RowIsEmpty(currentRow))
                    continue;

                if (currentRow.size() < COL_COUNT_MIN)
                {
                    con::error("{} Column count lower than min column count ({})", ErrorPrefix(assetName, currentRowIndex), COL_COUNT_MIN);
                    return AssetCreationResult::Failure();
                }

                // aliasName
                if (!currentRow[0].empty())
                {
                    aliasList->head[dataRowIndex].aliasName = m_memory.Dup(currentRow[0].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].aliasName = nullptr;
                }

                // sequence
                if (!currentRow[1].empty())
                {
                    aliasList->head[dataRowIndex].sequence = std::stoi(currentRow[1]);
                }
                else
                {
                    aliasList->head[dataRowIndex].sequence = 0;
                }

                // soundFile
                SetFlag(aliasList->head[dataRowIndex].flags, 0x40, false);
                SetFlag(aliasList->head[dataRowIndex].flags, 0x80, false);
                aliasList->head[dataRowIndex].soundFile = m_memory.Alloc<SoundFile>();
                if (!currentRow[11].empty())
                {
                    if (currentRow[11] == "streamed")
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x80, true);
                        aliasList->head[dataRowIndex].soundFile->type = SAT_STREAMED;
                    }
                    else if (currentRow[11] == "primed")
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x40, true);
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x80, true);
                    }
                    else if (!currentRow[11].empty())
                    {
                        aliasList->head[dataRowIndex].soundFile->type = SAT_UNKNOWN;
                    }
                }
                else
                {
                    SetFlag(aliasList->head[dataRowIndex].flags, 0x40, true);
                    aliasList->head[dataRowIndex].soundFile->type = SAT_LOADED;
                }

                const std::string soundFileName = currentRow[2];
                if (!soundFileName.empty())
                {
                    if (aliasList->head[dataRowIndex].soundFile->type == SAT_LOADED)
                    {
                        auto* loadedSoundDependency = context.LoadDependency<AssetLoadedSound>(soundFileName);
                        if (!loadedSoundDependency)
                        {
                            con::error("Could not load Sound \"{}\" due to missing Loaded Sound \"{}\"", assetName, soundFileName);
                            return AssetCreationResult::Failure();
                        }
                        registration.AddDependency(loadedSoundDependency);
                        aliasList->head[dataRowIndex].soundFile->u.loadSnd = loadedSoundDependency->Asset();
                        aliasList->head[dataRowIndex].soundFile->u.loadSnd->name = m_memory.Dup(soundFileName.c_str());
                    }
                    else if (aliasList->head[dataRowIndex].soundFile->type == SAT_STREAMED)
                    {
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.dir = m_memory.Dup(SplitPathParts(soundFileName).first.c_str());
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.name = m_memory.Dup(SplitPathParts(soundFileName).second.c_str());
                    }
                }

                // volMod
                float volMod = 1.0f;
                if (!currentRow[5].empty())
                {
                    volMod = GetVolumeMod(currentRow[5]);
                }

                // volMin
                if (!currentRow[3].empty())
                {
                    aliasList->head[dataRowIndex].volMin = std::stof(currentRow[3]) * volMod;
                }
                else
                {
                    aliasList->head[dataRowIndex].volMin = 1;
                }

                // volMax
                if (!currentRow[4].empty())
                {
                    aliasList->head[dataRowIndex].volMax = std::stof(currentRow[4]) * volMod;
                }
                else
                {
                    aliasList->head[dataRowIndex].volMax = 1;
                }

                // pitchMin
                if (!currentRow[6].empty())
                {
                    aliasList->head[dataRowIndex].pitchMin = std::stof(currentRow[6]);
                }
                else
                {
                    aliasList->head[dataRowIndex].pitchMin = 1;
                }

                // pitchMax
                if (!currentRow[7].empty())
                {
                    aliasList->head[dataRowIndex].pitchMax = std::stof(currentRow[7]);
                }
                else
                {
                    aliasList->head[dataRowIndex].pitchMax = 1;
                }

                // distMin
                if (!currentRow[8].empty())
                {
                    aliasList->head[dataRowIndex].distMin = std::stof(currentRow[8]);
                }
                else
                {
                    aliasList->head[dataRowIndex].distMin = 120;
                }

                // distMax
                if (!currentRow[9].empty())
                {
                    aliasList->head[dataRowIndex].distMax = std::stof(currentRow[9]);
                }
                else
                {
                    aliasList->head[dataRowIndex].distMax = 500000;
                }

                // channel
                std::string channelName = currentRow[10];
                if (!channelName.empty())
                {
                    int channelIndex = FindChannelIndex(channelName.c_str());
                    if (channelIndex == -1)
                    {
                        con::warn(std::format("Found invalid channel '{}', defaulting to 'auto'", channelName));
                        SetChannelIndex(aliasList->head[dataRowIndex].flags, 0);
                        continue;
                    }

                    SetChannelIndex(aliasList->head[dataRowIndex].flags, channelIndex);
                }
                else
                {
                    SetChannelIndex(aliasList->head[dataRowIndex].flags, 0);
                }

                // probability
                if (!currentRow[12].empty())
                {
                    aliasList->head[dataRowIndex].probability = std::stof(currentRow[12]);
                }
                else
                {
                    aliasList->head[dataRowIndex].probability = 1.0f;
                }

                // looping
                SetFlag(aliasList->head[dataRowIndex].flags, 0x1, false);
                SetFlag(aliasList->head[dataRowIndex].flags, 0x20, false);
                if (!currentRow[13].empty())
                {
                    if (currentRow[13] == "rlooping")
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x20, true);
                    }
                    SetFlag(aliasList->head[dataRowIndex].flags, 0x1, true);
                }

                // slavePercentage
                SetFlag(aliasList->head[dataRowIndex].flags, 0x2, false);
                if (!currentRow[14].empty())
                {
                    if (currentRow[14] == "master")
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x2, true);
                        aliasList->head[dataRowIndex].slavePercentage = 0.0f;
                    }
                    else
                    {
                        aliasList->head[dataRowIndex].slavePercentage = std::stof(currentRow[14].c_str());
                    }
                }
                else
                {
                    aliasList->head[dataRowIndex].slavePercentage = 1.0f;
                }

                // Skip loadspec / index 15

                // subtitle
                if (!currentRow[16].empty())
                {
                    aliasList->head[dataRowIndex].subtitle = m_memory.Dup(currentRow[16].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].subtitle = nullptr;
                }


                // Skip compression / index 17

                // secondaryAliasName
                if (!currentRow[18].empty() && currentRow[18] != "0")
                {
                    aliasList->head[dataRowIndex].secondaryAliasName = m_memory.Dup(currentRow[18].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].secondaryAliasName = nullptr;
                }

                // volumeFalloffCurve
                if (!currentRow[19].empty())
                {
                    aliasList->head[dataRowIndex].volumeFalloffCurve = m_memory.Alloc<SndCurve>();
                    AssetRegistration<AssetSoundCurve> registration(currentRow[19], aliasList->head[dataRowIndex].volumeFalloffCurve);
                    context.AddAsset(std::move(registration));

                    aliasList->head[dataRowIndex].volumeFalloffCurve->filename = m_memory.Dup(currentRow[19].c_str());
                }

                // startDelay
                if (!currentRow[20].empty())
                {
                    aliasList->head[dataRowIndex].startDelay = std::stoi(currentRow[20].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].startDelay = 0;
                }

                // speakerMap
                if (!currentRow[21].empty())
                {
                    const auto speakerMapFileName = snd_alias::GetFileNameForSpeakerMapName(currentRow[21]);
                    const auto speakerMapFile = m_search_path.Open(speakerMapFileName);
                    if (!speakerMapFile.IsOpen())
                        return AssetCreationResult::NoAction();

                    aliasList->head[dataRowIndex].speakerMap = m_memory.Alloc<SpeakerMap>();
                    aliasList->head[dataRowIndex].speakerMap->name = m_memory.Dup(currentRow[21].c_str());

                    ReadSpeakerMap(*speakerMapFile.m_stream, *aliasList->head[dataRowIndex].speakerMap);
                }

                // reverb
                SetFlag(aliasList->head[dataRowIndex].flags, 0x10, false);
                SetFlag(aliasList->head[dataRowIndex].flags, 0x8, false);
                if (!currentRow[22].empty())
                {
                    if (currentRow[22].contains("nowetlevel"))
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x10, true);
                    }
                    if (currentRow[22].contains("fulldrylevel"))
                    {
                        SetFlag(aliasList->head[dataRowIndex].flags, 0x8, true);
                    }
                }

                // lfePercentage
                if (!currentRow[23].empty())
                {
                    aliasList->head[dataRowIndex].lfePercentage = std::stof(currentRow[23].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].lfePercentage = 0.0f;
                }

                // centerPercentage
                if (!currentRow[24].empty())
                {
                    aliasList->head[dataRowIndex].centerPercentage = std::stof(currentRow[24].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].centerPercentage = 0.0f;
                }

                // Skip compression / index 25

                // envelopMin
                if (!currentRow[26].empty())
                {
                    aliasList->head[dataRowIndex].envelopMin = std::stof(currentRow[26].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].envelopMin = 0.0f;
                }

                // envelopMax
                if (!currentRow[27].empty())
                {
                    aliasList->head[dataRowIndex].envelopMax = std::stof(currentRow[27].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].envelopMax = 0.0f;
                }

                // envelopPercentage
                if (!currentRow[28].empty())
                {
                    aliasList->head[dataRowIndex].envelopPercentage = std::stof(currentRow[28].c_str());
                }
                else
                {
                    aliasList->head[dataRowIndex].envelopPercentage = 0.0f;
                }

                // Skip conversion / index 29

                dataRowIndex++;
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static std::string ErrorPrefix(const std::string& assetName, const unsigned rowIndex)
        {
            return std::format("SoundAliasList \"{}\" Row {}:", assetName, rowIndex);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace snd_alias_list
{
    std::unique_ptr<AssetCreator<AssetSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<SndAliasListLoader>(memory, searchPath, zone);
    }
} // namespace snd_alias_list
