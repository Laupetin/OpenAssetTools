#include "SequenceZoneDefinitionMetaData.h"

#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Zone/Definition/Parsing/Matcher/ZoneDefinitionMatcherFactory.h"

#include <cstdint>
#include <format>
#include <optional>

namespace
{
    constexpr auto METADATA_MAP_TYPE = "map";

    constexpr auto METADATA_GAME = "game";
    constexpr auto METADATA_GDT = "gdt";
    constexpr auto METADATA_NAME = "name";
    constexpr auto METADATA_TYPE = "type";

    constexpr auto METADATA_IPAK = "ipak";
    constexpr auto METADATA_IWD = "iwd";

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

    enum class ProjectType : std::uint8_t
    {
        NONE,
        FASTFILE,
        IPAK,

        COUNT
    };
    constexpr const char* ProjectType_Names[]{
        "none",
        "fastfile",
        "ipak",
    };
    static_assert(std::extent_v<decltype(ProjectType_Names)> == static_cast<unsigned>(ProjectType::COUNT));

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

namespace
{
    void ProcessMetaDataGame(ZoneDefinitionParserState* state, const ZoneDefinitionParserValue& valueToken, const std::string& value)
    {
        const auto game = GetGameByName(value);
        if (!game)
            throw ParsingException(valueToken.GetPos(), "Unknown game name");

        const auto previousGame = state->m_definition->m_game;
        if (previousGame != GameId::COUNT && previousGame != *game)
            throw ParsingException(valueToken.GetPos(), std::format("Game was previously defined as: {}", GameId_Names[static_cast<unsigned>(previousGame)]));

        state->SetGame(*game);
    }

    void ProcessMetaDataType(ZoneDefinitionParserState* state, const ZoneDefinitionParserValue& keyToken, const ZoneDefinitionParserValue& valueToken)
    {
        const auto projectType = GetProjectTypeByName(valueToken.FieldValue());
        if (!projectType)
            throw ParsingException(valueToken.GetPos(), "Unknown project type name");

        // TODO: Remove deprecated type

        std::string deprecationSuggestedAction;
        if (*projectType == ProjectType::IPAK)
        {
            deprecationSuggestedAction = "Use \">ipak,name_of_ipak\" instead.";
            state->StartIPak(state->m_definition->m_name);
        }
        else if (*projectType == ProjectType::FASTFILE)
        {
            deprecationSuggestedAction = "A fastfile will always be built when appropriate assets have been added.";
        }
        else
        {
            deprecationSuggestedAction = "It now has no effect.";
        }

        const auto keyPos = keyToken.GetPos();
        con::error("Warning: {} L{}: Zone definition \">type,{}\" is deprecated and should be removed. {}",
                   keyPos.m_filename.get(),
                   keyPos.m_line,
                   keyToken.FieldValue(),
                   deprecationSuggestedAction);
    }
} // namespace

void SequenceZoneDefinitionMetaData::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    const auto& keyToken = result.NextCapture(CAPTURE_KEY);
    auto key = keyToken.FieldValue();
    const auto& valueToken = result.NextCapture(CAPTURE_VALUE);
    const auto& value = valueToken.FieldValue();

    utils::MakeStringLowerCase(key);

    if (key == METADATA_GAME)
    {
        ProcessMetaDataGame(state, valueToken, value);
    }
    else if (key == METADATA_MAP_TYPE)
    {
        std::string valueLowerCase = value;
        std::transform(valueLowerCase.begin(),
                       valueLowerCase.end(),
                       valueLowerCase.begin(),
                       [](unsigned char c)
                       {
                           return std::tolower(c);
                       });
        if (valueLowerCase.compare("sp") == 0)
            state->SetMapType(ZoneDefinitionMapType::SP);
        else if (valueLowerCase.compare("mp") == 0)
            state->SetMapType(ZoneDefinitionMapType::MP);
        else if (valueLowerCase.compare("zm") == 0)
            state->SetMapType(ZoneDefinitionMapType::ZM);
        else
            throw ParsingException(valueToken.GetPos(), "map must be SP, MP or ZM");
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
        ProcessMetaDataType(state, keyToken, valueToken);
    }
    else if (key == METADATA_IPAK)
    {
        if (!value.empty())
            state->StartIPak(value);
        else
            throw ParsingException(valueToken.GetPos(), "IPak must have a name");
    }
    else if (key == METADATA_IWD)
    {
        if (!value.empty())
            state->StartIwd(value);
        else
            throw ParsingException(valueToken.GetPos(), "IWD must have a name");
    }
    else
    {
        state->m_definition->m_properties.AddProperty(key, value);
    }
}
