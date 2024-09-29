#include "SequenceZoneDefinitionMetaData.h"

#include "Game/IW3/AssetNameResolverIW3.h"
#include "Game/IW4/AssetNameResolverIW4.h"
#include "Game/IW5/AssetNameResolverIW5.h"
#include "Game/T5/AssetNameResolverT5.h"
#include "Game/T6/AssetNameResolverT6.h"
#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"
#include "Utils/StringUtils.h"

#include <format>
#include <optional>

namespace
{
    constexpr auto METADATA_GAME = "game";
    constexpr auto METADATA_GDT = "gdt";
    constexpr auto METADATA_NAME = "name";
    constexpr auto METADATA_TYPE = "type";

    std::optional<GameId> GetGameByName(const std::string& gameName)
    {
        auto upperGameName = gameName;
        utils::MakeStringUpperCase(upperGameName);

        for (auto i = 0u; i < static_cast<unsigned>(GameId::COUNT); i++)
        {
            if (upperGameName == GameId_Names[i])
                return static_cast<GameId>(i);
        }

        return std::nullopt;
    }

    std::optional<ProjectType> GetProjectTypeByName(const std::string& projectTypeName)
    {
        auto lowerProjectTypeName = projectTypeName;
        utils::MakeStringLowerCase(lowerProjectTypeName);

        for (auto i = 0u; i < static_cast<unsigned>(ProjectType::COUNT); i++)
        {
            if (lowerProjectTypeName == ProjectType_Names[i])
                return static_cast<ProjectType>(i);
        }

        return std::nullopt;
    }
} // namespace

SequenceZoneDefinitionMetaData::SequenceZoneDefinitionMetaData()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Char('>'),
        create.Field().Capture(CAPTURE_KEY),
        create.Char(','),
        create.Field().Capture(CAPTURE_VALUE),
    });
}

void SequenceZoneDefinitionMetaData::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    const auto& keyToken = result.NextCapture(CAPTURE_KEY);
    auto key = keyToken.FieldValue();
    const auto& valueToken = result.NextCapture(CAPTURE_VALUE);
    const auto& value = result.NextCapture(CAPTURE_VALUE).FieldValue();

    utils::MakeStringLowerCase(key);

    if (key == METADATA_GAME)
    {
        const auto game = GetGameByName(value);
        if (!game)
            throw ParsingException(valueToken.GetPos(), "Unknown game name");

        const auto previousGame = state->m_definition->m_game;
        if (previousGame != GameId::COUNT && previousGame != *game)
            throw ParsingException(valueToken.GetPos(), std::format("Game was previously defined as: {}", GameId_Names[static_cast<unsigned>(previousGame)]));

        state->m_definition->m_game = *game;
        state->m_asset_name_resolver = IAssetNameResolver::GetResolverForGame(*game);
    }
    else if (key == METADATA_GDT)
    {
        state->m_definition->m_gdts.emplace_back(value);
    }
    else if (key == METADATA_NAME)
    {
        state->m_definition->m_name = value;
    }
    else if (key == METADATA_TYPE)
    {
        const auto projectType = GetProjectTypeByName(value);
        if (!projectType)
            throw ParsingException(valueToken.GetPos(), "Unknown project type name");
    }
    else
    {
        state->m_definition->m_properties.AddProperty(key, value);
    }
}
