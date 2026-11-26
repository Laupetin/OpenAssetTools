#include "SoundAliasChannelsIW3.h"

#include "Csv/CsvStream.h"
#include "SearchPath/ISearchPath.h"
#include "SoundConstantsIW3.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>

namespace sound_alias
{
    std::optional<SoundAliasChannelsIW3> ReadSoundAliasChannelsIW3(ISearchPath& searchPath)
    {
        constexpr size_t maxChannels = (IW3::SND_ALIAS_FLAG_CHANNEL_MASK >> IW3::SND_ALIAS_FLAG_CHANNEL_SHIFT) + 1;
        constexpr size_t maxNameLength = 64;

        const auto file = searchPath.Open(IW3_CHANNELS_FILE);
        if (!file.IsOpen())
            return SoundAliasChannelsIW3{
                {std::begin(IW3::SOUND_CHANNEL_NAMES), std::end(IW3::SOUND_CHANNEL_NAMES)},
                {}
            };

        std::ostringstream definition;
        if (!(definition << file.m_stream->rdbuf()))
        {
            con::error("Could not read IW3 channel definitions from '{}'", IW3_CHANNELS_FILE);
            return std::nullopt;
        }

        SoundAliasChannelsIW3 channels{{}, std::move(definition).str()};
        std::istringstream input(channels.m_definition);
        if (channels.m_definition.starts_with("\xEF\xBB\xBF"))
            input.ignore(3);

        std::string line;
        std::vector<std::string> columns;
        size_t lineNumber = 0;
        while (std::getline(input, line))
        {
            ++lineNumber;
            std::istringstream row(std::move(line));
            if (!CsvInputStream(row).NextRow(columns) || columns.empty())
                continue;

            auto& name = columns.front();
            utils::StringTrim(name);
            if (name.empty() || name.starts_with('#'))
                continue;

            if (name.size() > maxNameLength || channels.m_names.size() == maxChannels
                || std::ranges::any_of(channels.m_names,
                                       [&name](const auto& existing)
                                       {
                                           return utils::StringEqualsIgnoreCase(existing, name);
                                       }))
            {
                con::error("Invalid or duplicate IW3 sound channel '{}' at {}:{}", name, IW3_CHANNELS_FILE, lineNumber);
                return std::nullopt;
            }

            channels.m_names.push_back(std::move(name));
        }

        if (channels.m_names.empty())
        {
            con::error("No IW3 sound channels defined in '{}'", IW3_CHANNELS_FILE);
            return std::nullopt;
        }

        return channels;
    }
} // namespace sound_alias
