#include "SpeakerMapLoaderIW3.h"

#include "Game/IW3/SoundConstantsIW3.h"
#include "Sound/SpeakerMapCommon.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cmath>
#include <sstream>

using namespace IW3;

namespace
{
    class SpeakerMapLoader final : public SubAssetCreator<SubAssetSpeakerMap>
    {
    public:
        SpeakerMapLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateSubAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            auto* map = m_memory.Alloc<SpeakerMap>();
            map->name = m_memory.Dup(assetName.c_str());
            map->isDefault = assetName.empty();
            for (auto sourceMode = 0; sourceMode < 2; sourceMode++)
            {
                for (auto outputMode = 0; outputMode < 2; outputMode++)
                {
                    auto& channelMap = map->channelMaps[sourceMode][outputMode];
                    channelMap.speakerCount = outputMode == 0 ? 2 : 6;
                    for (auto speaker = 0; speaker < channelMap.speakerCount; speaker++)
                    {
                        auto& levels = channelMap.speakers[speaker];
                        levels.speaker = speaker;
                        levels.numLevels = sourceMode + 1;
                        // Com_InitDefaultSoundAliasSpeakerMap: mono is split equally; stereo routes left/right directly.
                        if (speaker < 2 && map->isDefault)
                        {
                            if (sourceMode == 0)
                                levels.levels[0] = 0.5f;
                            else
                                levels.levels[speaker] = 1.0f;
                        }
                    }
                }
            }
            if (!map->isDefault)
            {
                const auto fileName = speaker_map::GetFileNameForAssetName(assetName);
                const auto file = m_search_path.Open(fileName);
                if (!file.IsOpen())
                    return AssetCreationResult::NoAction();
                if (!Read(*file.m_stream, *map))
                {
                    con::error("Invalid speaker map '{}'", fileName);
                    return AssetCreationResult::Failure();
                }
            }
            return AssetCreationResult::Success(context.AddSubAsset<SubAssetSpeakerMap>(assetName, map));
        }

    private:
        static bool Read(std::istream& stream, SpeakerMap& map)
        {
            std::ostringstream text;
            std::string line;
            while (std::getline(stream, line))
                text << line.substr(0, line.find("//")) << '\n';
            std::istringstream tokens(text.str());
            std::string token;
            if (!(tokens >> token) || token != "SPKRMAP")
                return false;
            // Stock order: mono/stereo, stereo/stereo, mono/surround, stereo/surround.
            for (auto outputMode = 0; outputMode < 2; outputMode++)
            {
                for (auto sourceMode = 0; sourceMode < 2; sourceMode++)
                {
                    auto& channelMap = map.channelMaps[sourceMode][outputMode];
                    for (auto speaker = 0; speaker < channelMap.speakerCount; speaker++)
                    {
                        for (auto inputChannel = 0; inputChannel <= sourceMode; inputChannel++)
                        {
                            std::string inputName, outputName;
                            float level;
                            if (!(tokens >> inputName >> outputName >> level) || !std::isfinite(level) || level < 0.0f || level > 1.0f)
                                return false;
                            utils::MakeStringUpperCase(inputName);
                            utils::MakeStringUpperCase(outputName);
                            if (inputName != SOUND_SPEAKER_MAP_IDENTIFIERS[sourceMode ? SA_LEFTSOURCE + inputChannel : SA_MONOSOURCE]
                                || outputName != SOUND_SPEAKER_MAP_IDENTIFIERS[SA_LEFTSPEAKER + speaker])
                                return false;
                            channelMap.speakers[speaker].levels[inputChannel] = level;
                        }
                    }
                }
            }
            return !(tokens >> token);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace sound_alias
{
    std::unique_ptr<SubAssetCreator<SubAssetSpeakerMap>> CreateSpeakerMapLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<SpeakerMapLoader>(memory, searchPath);
    }
} // namespace sound_alias
