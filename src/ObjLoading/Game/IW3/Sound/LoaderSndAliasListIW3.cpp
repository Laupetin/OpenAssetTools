#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <Csv/CsvStream.h>
#include <Game/IW3/Sound/SndAliasListConstantsIW3.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <set>

using namespace IW3;
namespace fs = std::filesystem;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

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

    float ReadColumnFloat(std::string cell, float defaultVal = -1.0f)
    {
        if (!cell.empty())
        {
            return std::stof(cell);
        }
        return defaultVal;
    }

    int ReadColumnInt(std::string cell, int defaultVal = -1)
    {
        if (!cell.empty())
        {
            return std::stoi(cell);
        }
        return defaultVal;
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

    void SetChannelFlag(int& flags, std::string channelName)
    {
        if (!channelName.empty())
        {
            int channelIndex = FindChannelIndex(channelName.c_str());
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

                // Grab volMod to calcualte volMin and volMax
                float volMod = 1.0f;
                if (!currentRow[5].empty())
                {
                    volMod = GetVolumeMod(currentRow[5]);
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
                if (!currentRow[21].empty())
                {
                    const auto speakerMapFileName = snd_alias::GetFileNameForSpeakerMapName(currentRow[21]);
                    const auto speakerMapFile = m_search_path.Open(speakerMapFileName);
                    if (!speakerMapFile.IsOpen())
                        return AssetCreationResult::NoAction();

                    aliasList->head[dataRowIndex].speakerMap = m_memory.Alloc<SpeakerMap>();
                    aliasList->head[dataRowIndex].speakerMap->name = ReadColumnString(currentRow[21], m_memory);

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
