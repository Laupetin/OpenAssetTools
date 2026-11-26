#pragma once

#include <optional>
#include <string>
#include <vector>

class ISearchPath;

namespace sound_alias
{
    inline constexpr auto IW3_CHANNELS_FILE = "soundaliases/channels.def";

    struct SoundAliasChannelsIW3
    {
        std::vector<std::string> m_names;
        std::string m_definition;
    };

    std::optional<SoundAliasChannelsIW3> ReadSoundAliasChannelsIW3(ISearchPath& searchPath);
} // namespace sound_alias
