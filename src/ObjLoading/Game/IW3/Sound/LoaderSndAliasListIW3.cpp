#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <Csv/CsvStream.h>
#include <format>
#include <iostream>
#include <Game/IW3/Sound/SndAliasListFields.h>

using namespace IW3;

namespace
{
    constexpr unsigned CHANNEL_MASK = 0x1Fu;
    constexpr unsigned CHANNEL_SHIFT = 8u;
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
            return 1.0f;
        return it->second;
    }

    int FindChannelIndex(const char* name)
    {
        if (name)
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

    void SetChannelIndex(int& rawFlags, unsigned channelIndex)
    {
        channelIndex &= 0x1Fu;

        rawFlags = (rawFlags & ~(0x1Fu << 8u)) 
            | ((static_cast<int>(channelIndex) & 0x1Fu) << 8u);
    }

    void SetLooping(int& rawFlags, bool looping)
    {
        if (looping)
        {
            rawFlags &= 0x1;
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

            auto* aliasList = m_memory.Alloc<snd_alias_list_t>();
            aliasList->aliasName = m_memory.Dup(assetName.c_str());
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
                snd_alias_t alias;

                currentRowIndex++;
                CsvInputStream::PreprocessRow(currentRow);

                if (CsvInputStream::RowIsEmpty(currentRow))
                    continue;

                if (currentRow.size() < COL_COUNT_MIN)
                {
                    con::error("{} Column count lower than min column count ({})", ErrorPrefix(assetName, currentRowIndex), COL_COUNT_MIN);
                    return AssetCreationResult::Failure();
                }

                if (!currentRow[0].empty())
                {
                    alias.aliasName = currentRow[0].c_str();
                }

                if (!currentRow[1].empty())
                {
                    alias.sequence = std::stoi(currentRow[1]);
                }
                else
                {
                    alias.sequence = 0;
                }

                alias.soundFile = m_memory.Alloc<SoundFile>();
                if (!currentRow[11].empty())
                {
                    alias.soundFile->type = SAT_STREAMED;
                }
                else
                {
                    alias.soundFile->type = SAT_LOADED;
                }

                const std::string soundFileName = currentRow[2];
                if (!soundFileName.empty())
                {
                    if (alias.soundFile->type == SAT_LOADED)
                    {
                        alias.soundFile->u.loadSnd = m_memory.Alloc<LoadedSound>();
                        alias.soundFile->u.loadSnd->name = m_memory.Dup(soundFileName.c_str());
                        AssetRegistration<AssetLoadedSound> registration(soundFileName, alias.soundFile->u.loadSnd);
                        context.AddAsset(std::move(registration));
                    }
                    else if (alias.soundFile->type == SAT_STREAMED)
                    {
                        alias.soundFile->u.streamSnd.dir = m_memory.Dup(SplitPathParts(soundFileName).first.c_str());
                        alias.soundFile->u.streamSnd.name = m_memory.Dup(SplitPathParts(soundFileName).second.c_str());
                    }
                }

                float volMod = 1.0f;
                if (!currentRow[5].empty())
                {
                    volMod = GetVolumeMod(currentRow[5]);
                }

                if (!currentRow[3].empty())
                {
                    alias.volMin = std::stof(currentRow[3]) * volMod;
                }
                else
                {
                    alias.volMin = 1;
                }

                if (!currentRow[4].empty())
                {
                    alias.volMax = std::stof(currentRow[4]) * volMod;
                }
                else
                {
                    alias.volMax = 1;
                }

                if (!currentRow[6].empty())
                {
                    alias.pitchMin = std::stof(currentRow[6]);
                }
                else
                {
                    alias.pitchMin = 1;
                }

                if (!currentRow[7].empty())
                {
                    alias.pitchMax = std::stof(currentRow[7]);
                }
                else
                {
                    alias.pitchMax = 1;
                }

                if (!currentRow[8].empty())
                {
                    alias.distMin = std::stof(currentRow[8]);
                }
                else
                {
                    alias.distMin = 120;
                }

                if (!currentRow[9].empty())
                {
                    alias.subtitle = m_memory.Dup(currentRow[9].c_str());
                    //alias.distMax = std::stof(currentRow[9]);
                }
                else
                {
                    //alias.distMax = 500000;
                }

                std::string channelName = currentRow[10];
                if (!channelName.empty())
                {
                    int channelIndex = FindChannelIndex(channelName.c_str());
                    if (channelIndex == -1)
                    {
                        con::warn(std::format("Found invalid channel '{}', defaulting to 'auto'", channelName));
                        SetChannelIndex(alias.flags, 0);
                        continue;
                    }

                    SetChannelIndex(alias.flags, channelIndex);
                }
                else
                {
                    SetChannelIndex(alias.flags, 0);
                }

                if (!currentRow[12].empty())
                {
                    alias.volumeFalloffCurve = m_memory.Alloc<SndCurve>();
                    AssetRegistration<AssetSoundCurve> registration(currentRow[12], alias.volumeFalloffCurve);
                    context.AddAsset(std::move(registration));

                    alias.volumeFalloffCurve->filename = m_memory.Dup(currentRow[12].c_str());
                }

                if (!currentRow[12].empty())
                {
                    //alias.probability = std::stof(currentRow[12]);
                }
                else
                {
                    //alias.probability = 1.0f;
                }

                if (!currentRow[13].empty())
                {
                    SetLooping(alias.flags, true);
                }
                else
                {
                    SetLooping(alias.flags, false);
                }

                if (!currentRow[14].empty())
                {
                    alias.speakerMap = m_memory.Alloc<SpeakerMap>();
                    alias.speakerMap->name = m_memory.Dup(currentRow[14].c_str());
                }

                if (!currentRow[14].empty())
                {
                    //alias.slavePercentage = std::stof(currentRow[14].c_str());
                }
                else
                {
                    //alias.slavePercentage = 1.0f;
                }

                if (!currentRow[17].empty() && currentRow[17] != "0")
                {
                    alias.secondaryAliasName = m_memory.Dup(currentRow[17].c_str());
                }

                if (!currentRow[19].empty())
                {
                    alias.startDelay = std::stoi(currentRow[19].c_str());
                }
                else
                {
                    alias.startDelay = 0;
                }

                if (!currentRow[22].empty())
                {
                    alias.lfePercentage = std::stof(currentRow[22].c_str());
                }
                else
                {
                    alias.lfePercentage = 0.0f;
                }

                if (!currentRow[23].empty())
                {
                    alias.centerPercentage = std::stof(currentRow[23].c_str());
                }
                else
                {
                    alias.centerPercentage = 0.0f;
                }

                if (!currentRow[25].empty())
                {
                    alias.envelopMin = std::stof(currentRow[25].c_str());
                }
                else
                {
                    alias.envelopMin = 0.0f;
                }

                if (!currentRow[26].empty())
                {
                    alias.envelopMax = std::stof(currentRow[26].c_str());
                }
                else
                {
                    alias.envelopMax = 0.0f;
                }

                if (!currentRow[27].empty())
                {
                    alias.envelopPercentage = std::stof(currentRow[27].c_str());
                }
                else
                {
                    alias.envelopPercentage = 0.0f;
                }

                aliases.push_back(alias);
            }

            aliasList->head = m_memory.Alloc<snd_alias_t>(aliases.size());
            for (unsigned int i = 0; i < aliases.size(); i++)
            {
                snd_alias_t alias = aliases[i];
                *(aliasList->head + i) = alias;
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
