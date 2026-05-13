#include "LoaderSndAliasListIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Sound/SndAliasListConstantsIW4.h"
#include "Sound/SndAliasListCommon.h"
#include "Sound/SpeakerMapCommon.h"
#include "Utils/Logging/Log.h"

#include <Sound/SndAliasListLoader.h>
#include <format>
#include <map>

using namespace IW4;
using namespace snd_alias_list;
using namespace speaker_map;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

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
            int speakerIndex = GetSpeakerNameIndex(tokens[1], speaker_map_names, std::extent_v<decltype(speaker_map_names)>);
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

    void PopulateSoundFile(const SndAliasListLoader& loader, std::string type, std::string soundName, int& flags, SoundFile& soundFile)
    {
        loader.SetFlag(flags, 0x40, false);
        loader.SetFlag(flags, 0x80, false);
        if (!type.empty())
        {
            if (type == "streamed")
            {
                loader.SetFlag(flags, 0x80, true);
                soundFile.type = SAT_STREAMED;
            }
            else if (type == "primed")
            {
                loader.SetFlag(flags, 0x40, true);
                loader.SetFlag(flags, 0x80, true);
            }
            else
            {
                soundFile.type = SAT_UNKNOWN;
            }
        }
        else
        {
            loader.SetFlag(flags, 0x40, true);
            soundFile.type = SAT_LOADED;
        }
    }

    class SndAliasListLoaderIW4 : public AssetCreator<AssetSound>
    {
    public:
        SndAliasListLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
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

            SndAliasListLoader loader(*file.m_stream);
            int entryCount = loader.CountAliases(COL_COUNT_MIN);
            auto* aliasList = m_memory.Alloc<snd_alias_list_t>();
            aliasList->aliasName = m_memory.Dup(assetName.c_str());
            aliasList->count = entryCount;
            aliasList->head = m_memory.Alloc<snd_alias_t>(entryCount);
            AssetRegistration<AssetSound> registration(assetName, aliasList);

            std::vector<std::string> currentRow;

            // Skip header
            loader.NextRow(currentRow);

            auto currentRowIndex = 0u;
            auto dataRowIndex = 0u;
            while (loader.NextRow(currentRow))
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
                    volMod = loader.GetVolumeMod(currentRow[COL_VOL_MOD], volume_mod_groups_table);
                }

                // Read static fields
                aliasList->head[dataRowIndex].aliasName = loader.ReadColumnString(currentRow[COL_NAME], m_memory);
                aliasList->head[dataRowIndex].sequence = loader.ReadColumnInt(currentRow[COL_SEQUENCE], 0);
                aliasList->head[dataRowIndex].soundFile = m_memory.Alloc<SoundFile>();
                aliasList->head[dataRowIndex].volMin = loader.ReadColumnFloat(currentRow[COL_VOL_MIN], 1) * volMod;
                aliasList->head[dataRowIndex].volMax = loader.ReadColumnFloat(currentRow[COL_VOL_MAX], 1) * volMod;
                aliasList->head[dataRowIndex].pitchMin = loader.ReadColumnFloat(currentRow[COL_PITCH_MIN], 1);
                aliasList->head[dataRowIndex].pitchMax = loader.ReadColumnFloat(currentRow[COL_PITCH_MAX], 1);
                aliasList->head[dataRowIndex].distMin = loader.ReadColumnFloat(currentRow[COL_DIST_MIN], 120);
                aliasList->head[dataRowIndex].distMax = loader.ReadColumnFloat(currentRow[COL_DIST_MAX], 500000);
                aliasList->head[dataRowIndex].velocityMin = loader.ReadColumnFloat(currentRow[COL_VELOCITY_MIN], 1);
                aliasList->head[dataRowIndex].probability = loader.ReadColumnFloat(currentRow[COL_PROBABILITY], 1.0f);
                aliasList->head[dataRowIndex].slavePercentage =
                    loader.ReadColumnMasterAndSet(aliasList->head[dataRowIndex].flags, currentRow[COL_MASTER_SLAVE]);
                aliasList->head[dataRowIndex].subtitle = loader.ReadColumnString(currentRow[COL_SUBTITLE], m_memory);
                aliasList->head[dataRowIndex].secondaryAliasName = loader.ReadColumnString(currentRow[COL_SECONDARY], m_memory);
                aliasList->head[dataRowIndex].mixerGroup = loader.ReadColumnString(currentRow[COL_MIXER_GROUP], m_memory);
                aliasList->head[dataRowIndex].startDelay = loader.ReadColumnInt(currentRow[COL_START_DELAY], 0);
                aliasList->head[dataRowIndex].lfePercentage = loader.ReadColumnFloat(currentRow[COL_LFE], 0.0f);
                aliasList->head[dataRowIndex].centerPercentage = loader.ReadColumnFloat(currentRow[COL_CENTER], 0.0f);
                aliasList->head[dataRowIndex].envelopMin = loader.ReadColumnFloat(currentRow[COL_ENV_MIN], 0.0f);
                aliasList->head[dataRowIndex].envelopMax = loader.ReadColumnFloat(currentRow[COL_ENV_MAX], 0.0f);
                aliasList->head[dataRowIndex].envelopPercentage = loader.ReadColumnFloat(currentRow[COL_ENV_PERCENT], 0.0f);

                // Populate preliminary sound file fields
                std::string soundFileName = currentRow[COL_FILE];
                PopulateSoundFile(loader, currentRow[COL_TYPE], soundFileName, aliasList->head[dataRowIndex].flags, *aliasList->head[dataRowIndex].soundFile);

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
                        aliasList->head[dataRowIndex].soundFile->u.loadSnd->name = loader.ReadColumnString(soundFileName, m_memory);
                    }
                    else if (aliasList->head[dataRowIndex].soundFile->type == SAT_STREAMED)
                    {
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.dir = loader.ReadColumnString(SplitPathParts(soundFileName).first, m_memory);
                        aliasList->head[dataRowIndex].soundFile->u.streamSnd.name = loader.ReadColumnString(SplitPathParts(soundFileName).second, m_memory);
                    }
                }

                // Allocate sound curve and make dependency
                if (!currentRow[COL_FALLOFF].empty())
                {
                    aliasList->head[dataRowIndex].volumeFalloffCurve = m_memory.Alloc<SndCurve>();
                    AssetRegistration<AssetSoundCurve> registration(currentRow[COL_FALLOFF], aliasList->head[dataRowIndex].volumeFalloffCurve);
                    context.AddAsset(std::move(registration));

                    aliasList->head[dataRowIndex].volumeFalloffCurve->filename = loader.ReadColumnString(currentRow[19], m_memory);
                }

                // Allocate and read speaker map from disk
                if (!currentRow[COL_SPEAKERMAP].empty())
                {
                    const auto speakerMapFileName = GetFileNameForSpeakerMapName(currentRow[COL_SPEAKERMAP]);
                    const auto speakerMapFile = m_search_path.Open(speakerMapFileName);
                    if (!speakerMapFile.IsOpen())
                        return AssetCreationResult::NoAction();

                    aliasList->head[dataRowIndex].speakerMap = m_memory.Alloc<SpeakerMap>();
                    aliasList->head[dataRowIndex].speakerMap->name = loader.ReadColumnString(currentRow[COL_SPEAKERMAP], m_memory);

                    ReadSpeakerMap(*speakerMapFile.m_stream, *aliasList->head[dataRowIndex].speakerMap);
                }

                // Populate more flags
                loader.SetChannelFlag(
                    aliasList->head[dataRowIndex].flags, currentRow[COL_CHANNEL], snd_alias_channel_names, std::extent_v<decltype(snd_alias_channel_names)>);
                loader.SetLoopFlag(aliasList->head[dataRowIndex].flags, currentRow[COL_LOOP]);
                loader.SetReverbFlag(aliasList->head[dataRowIndex].flags, currentRow[COL_REVERB]);

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
    std::unique_ptr<AssetCreator<AssetSound>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<SndAliasListLoaderIW4>(memory, searchPath, zone);
    }
} // namespace snd_alias_list
