#include "SpeakerMapLoaderIW3.h"

#include "Game/IW3/SoundConstantsIW3.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cmath>
#include <format>
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
            for (auto source = 0; source < 2; source++)
            {
                for (auto output = 0; output < 2; output++)
                {
                    auto& channel = map->channelMaps[source][output];
                    channel.speakerCount = output ? 6 : 2;
                    for (auto speaker = 0; speaker < channel.speakerCount; speaker++)
                    {
                        auto& levels = channel.speakers[speaker];
                        levels.speaker = speaker;
                        levels.numLevels = source + 1;
                        // Com_InitDefaultSoundAliasSpeakerMap: mono is split equally; stereo routes left/right directly.
                        if (speaker < 2 && map->isDefault)
                        {
                            if (!source)
                                levels.levels[0] = 0.5f;
                            else
                                levels.levels[speaker] = 1.0f;
                        }
                    }
                }
            }
            if (!map->isDefault)
            {
                const auto fileName = std::format("soundaliases/{}.spkrmap", assetName);
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
            for (auto output = 0; output < 2; output++)
            {
                for (auto source = 0; source < 2; source++)
                {
                    auto& channel = map.channelMaps[source][output];
                    for (auto speaker = 0; speaker < channel.speakerCount; speaker++)
                    {
                        for (auto input = 0; input <= source; input++)
                        {
                            std::string inputName, outputName;
                            float level;
                            if (!(tokens >> inputName >> outputName >> level) || !std::isfinite(level) || level < 0.0f || level > 1.0f)
                                return false;
                            utils::MakeStringUpperCase(inputName);
                            utils::MakeStringUpperCase(outputName);
                            if (inputName != SOUND_SPEAKER_MAP_IDENTIFIERS[source ? SA_LEFTSOURCE + input : SA_MONOSOURCE]
                                || outputName != SOUND_SPEAKER_MAP_IDENTIFIERS[SA_LEFTSPEAKER + speaker])
                                return false;
                            channel.speakers[speaker].levels[input] = level;
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
