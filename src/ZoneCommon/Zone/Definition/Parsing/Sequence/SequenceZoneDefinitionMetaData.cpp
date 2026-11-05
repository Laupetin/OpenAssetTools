#include "SequenceZoneDefinitionMetaData.h"

#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Zone/Definition/Parsing/Matcher/ZoneDefinitionMatcherFactory.h"

#include <cstdint>
#include <format>
#include <optional>
#include <utility>

namespace
{
    constexpr auto METADATA_GAME = "game";
    constexpr auto METADATA_VARIANT = "variant";
    constexpr auto METADATA_GDT = "gdt";
    constexpr auto METADATA_NAME = "name";
    constexpr auto METADATA_TYPE = "type";

    constexpr auto METADATA_IPAK = "ipak";
    constexpr auto METADATA_IWD = "iwd";

    std::optional<GameId> GetGameByName(const std::string& gameName)
    {
        auto upperGameName = gameName;
        utils::MakeStringUpperCase(upperGameName);

        for (auto curGameIdNum = 0u; curGameIdNum < std::to_underlying(GameId::COUNT); curGameIdNum++)
        {
            const auto curGameId = static_cast<GameId>(curGameIdNum);
            const auto& curGameName = IGame::GetGameById(curGameId)->GetShortName();
            if (upperGameName == curGameName)
                return curGameId;
        }

        return std::nullopt;
    }

    std::optional<GameVariantId> GetVariantByName(const GameId gameId, const std::string& variantName)
    {
        auto lowerVariantName = variantName;
        utils::MakeStringLowerCase(lowerVariantName);

        for (auto curVariantIdNum = 0u; curVariantIdNum < std::to_underlying(GameVariantId::COUNT); curVariantIdNum++)
        {
            const auto curVariantId = static_cast<GameVariantId>(curVariantIdNum);
            const auto* variant = IGameVariant::GetVariantById(curVariantId);
            if (variant->GetGameId() != gameId)
                continue;

            const auto& curVariantName = variant->GetName();
            if (lowerVariantName == curVariantName)
                return curVariantId;
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

        if (!state->m_game)
            state->SetGame(*game);
        else if ((*state->m_game)->GetId() != *game)
            throw ParsingException(valueToken.GetPos(), std::format("Game was previously defined as: {}", (*state->m_game)->GetShortName()));
    }

    void ProcessMetaDataVariant(ZoneDefinitionParserState* state, const ZoneDefinitionParserValue& valueToken, const std::string& value)
    {
        if (!state->m_game)
            throw ParsingException(valueToken.GetPos(), "The game must be specified before the variant");

        const auto variantId = GetVariantByName((*state->m_game)->GetId(), value);
        if (!variantId)
            throw ParsingException(valueToken.GetPos(), "Unknown variant name");

        if (!state->m_variant)
            state->SetVariant(*variantId);
        else if ((*state->m_variant)->GetId() != *variantId)
            throw ParsingException(valueToken.GetPos(), std::format("Variant was previously defined as: {}", (*state->m_variant)->GetName()));
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
        con::warn("Deprecated: {} L{}: Zone definition \">type,{}\" is deprecated and should be removed. {}",
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
    else if (key == METADATA_VARIANT)
    {
        ProcessMetaDataVariant(state, valueToken, value);
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
