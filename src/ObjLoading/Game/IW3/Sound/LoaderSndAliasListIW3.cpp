#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <Game/IW3/Sound/SndAliasListConstantsIW3.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <set>

using namespace IW3;
using namespace snd_alias_list;

namespace fs = std::filesystem;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

    void SetChannelFlag(int& flags, std::string channelName)
    {
        if (!channelName.empty())
        {
            int channelIndex = FindChannelIndex(channelName.c_str(), snd_alias_channel_names, std::extent_v<decltype(snd_alias_channel_names)>);
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

    char* ReadColumnString(std::string cell, MemoryManager& memory)
    {
        if (!cell.empty())
        {
            return memory.Dup(cell.c_str());
        }
        return nullptr;
    }

    void PopulateSoundFile(std::string type, std::string soundName, int& flags, SoundFile& soundFile)
    {
        SetFlag(flags, 0x40, false);
        SetFlag(flags, 0x80, false);
        if (!type.empty())
        {
            if (type == "streamed")
            {
                SetFlag(flags, 0x80, true);
                soundFile.type = SAT_STREAMED;
            }
            else if (type == "primed")
            {
                SetFlag(flags, 0x40, true);
                SetFlag(flags, 0x80, true);
            }
            else
            {
                soundFile.type = SAT_UNKNOWN;
            }
        }
        else
        {
            SetFlag(flags, 0x40, true);
            soundFile.type = SAT_LOADED;
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
            const auto fileName = GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            size_t entryCount = CountAliases(*file.m_stream, COL_COUNT_MIN);
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

                // Grab volMod to calcualte volMin and volMax
                float volMod = 1.0f;
                if (!currentRow[COL_VOL_MOD].empty())
                {
                    volMod = GetVolumeMod(currentRow[COL_VOL_MOD], volume_mod_groups_table);
                }

                // Read static fields
                aliasList->head[dataRowIndex].aliasName = ReadColumnString(currentRow[COL_NAME], m_memory);
                aliasList->head[dataRowIndex].sequence = ReadColumnInt(currentRow[COL_SEQUENCE], 0);
                aliasList->head[dataRowIndex].soundFile = m_memory.Alloc<SoundFile>();
                aliasList->head[dataRowIndex].volMin = ReadColumnFloat(currentRow[COL_VOL_MIN], 1) * volMod;
                aliasList->head[dataRowIndex].volMax = ReadColumnFloat(currentRow[COL_VOL_MAX], 1) * volMod;
                aliasList->head[dataRowIndex].pitchMin = ReadColumnFloat(currentRow[COL_PITCH_MIN], 1);
                aliasList->head[dataRowIndex].pitchMax = ReadColumnFloat(currentRow[COL_PITCH_MAX], 1);
                aliasList->head[dataRowIndex].distMin = ReadColumnFloat(currentRow[COL_DIST_MIN], 120);
                aliasList->head[dataRowIndex].distMax = ReadColumnFloat(currentRow[COL_DIST_MAX], 500000);
                aliasList->head[dataRowIndex].probability = ReadColumnFloat(currentRow[COL_PROBABILITY], 1.0f);
                aliasList->head[dataRowIndex].slavePercentage = ReadColumnMasterAndSet(aliasList->head[dataRowIndex].flags, currentRow[COL_MASTER_SLAVE]);
                aliasList->head[dataRowIndex].subtitle = ReadColumnString(currentRow[COL_SUBTITLE], m_memory);
                aliasList->head[dataRowIndex].secondaryAliasName = ReadColumnString(currentRow[COL_SECONDARY], m_memory);
                aliasList->head[dataRowIndex].startDelay = ReadColumnInt(currentRow[COL_START_DELAY], 0);
                aliasList->head[dataRowIndex].lfePercentage = ReadColumnFloat(currentRow[COL_LFE], 0.0f);
                aliasList->head[dataRowIndex].centerPercentage = ReadColumnFloat(currentRow[COL_CENTER], 0.0f);
                aliasList->head[dataRowIndex].envelopMin = ReadColumnFloat(currentRow[COL_ENV_MIN], 0.0f);
                aliasList->head[dataRowIndex].envelopMax = ReadColumnFloat(currentRow[COL_ENV_MAX], 0.0f);
                aliasList->head[dataRowIndex].envelopPercentage = ReadColumnFloat(currentRow[COL_ENV_PERCENT], 0.0f);

                // Populate preliminary sound file fields
                std::string soundFileName = currentRow[COL_FILE];
                PopulateSoundFile(currentRow[COL_TYPE], soundFileName, aliasList->head[dataRowIndex].flags, *aliasList->head[dataRowIndex].soundFile);

                // Load sound file contents based on type
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
                        aliasList->head[dataRowIndex].soundFile->u.loadSnd->name = ReadColumnString(soundFileName, m_memory);
                    }
                    else if (aliasList->head[dataRowIndex].soundFile->type == SAT_STREAMED)
                    {
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.dir = ReadColumnString(SplitPathParts(soundFileName).first, m_memory);
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.name = ReadColumnString(SplitPathParts(soundFileName).second, m_memory);
                    }
                }

                // Allocate sound curve and make dependency
                if (!currentRow[COL_FALLOFF].empty())
                {
                    aliasList->head[dataRowIndex].volumeFalloffCurve = m_memory.Alloc<SndCurve>();
                    AssetRegistration<AssetSoundCurve> registration(currentRow[COL_FALLOFF], aliasList->head[dataRowIndex].volumeFalloffCurve);
                    context.AddAsset(std::move(registration));

                    aliasList->head[dataRowIndex].volumeFalloffCurve->filename = ReadColumnString(currentRow[19], m_memory);
                }

                // Allocate and read speaker map from disk
                if (!currentRow[COL_SPEAKERMAP].empty())
                {
                    const auto speakerMapFileName = GetFileNameForSpeakerMapName(currentRow[COL_SPEAKERMAP]);
                    const auto speakerMapFile = m_search_path.Open(speakerMapFileName);
                    if (!speakerMapFile.IsOpen())
                        return AssetCreationResult::NoAction();

                    aliasList->head[dataRowIndex].speakerMap = m_memory.Alloc<SpeakerMap>();
                    aliasList->head[dataRowIndex].speakerMap->name = ReadColumnString(currentRow[COL_SPEAKERMAP], m_memory);

                    ReadSpeakerMap(*speakerMapFile.m_stream, *aliasList->head[dataRowIndex].speakerMap);
                }

                // Populate more flags
                SetChannelFlag(aliasList->head[dataRowIndex].flags, currentRow[COL_CHANNEL]);
                SetLoopFlag(aliasList->head[dataRowIndex].flags, currentRow[COL_LOOP]);
                SetReverbFlag(aliasList->head[dataRowIndex].flags, currentRow[COL_REVERB]);

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
