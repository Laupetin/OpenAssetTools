#include "SequenceSetVariant.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    constexpr auto CAPTURE_START = 1;
    constexpr auto CAPTURE_TYPE_OR_VARIANT = 2;
    constexpr auto CAPTURE_VARIANT = 3;

    std::optional<unsigned> GetVariantIndexForName(const IDataRepository& repository, const std::string& name)
    {
        for (const auto* variant : repository.GetGameVariants())
        {
            if (utils::StringEqualsIgnoreCase(variant->m_name, name))
                return variant->m_index;
        }

        return std::nullopt;
    }
} // namespace

SequenceSetVariant::SequenceSetVariant()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set").Capture(CAPTURE_START),
        create.Keyword("variant"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE_OR_VARIANT),
        create.OptionalLoop(create.Identifier().Capture(CAPTURE_VARIANT)),
        create.Char(';'),
    });
}

void SequenceSetVariant::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeOrVariantToken = result.NextCapture(CAPTURE_TYPE_OR_VARIANT);
    const auto& typeOrVariantValue = typeOrVariantToken.TypeNameValue();
    const auto maybeTypeOrVariantAsVariantIndex = GetVariantIndexForName(*state->GetRepository(), typeOrVariantValue);

    StructureInformation* information;
    std::vector<MemberInformation*> memberChain;
    if (state->GetTypenameAndMembersFromTypename(typeOrVariantValue, information, memberChain))
    {
        if (maybeTypeOrVariantAsVariantIndex)
            throw ParsingException(typeOrVariantToken.GetPos(), "Expression is ambiguous, could both be a variant name or a type.");

        if (!memberChain.empty())
        {
            information = memberChain.back()->m_type;
            if (information == nullptr)
                throw ParsingException(typeOrVariantToken.GetPos(), "Specified type must be a structure.");
        }
    }
    else
    {
        information = state->GetInUse();
        if (information == nullptr)
            throw ParsingException(result.NextCapture(CAPTURE_START).GetPos(), "No type is used. Therefore one needs to be specified directly.");
    }

    if (maybeTypeOrVariantAsVariantIndex.has_value())
        information->m_variants.emplace(*maybeTypeOrVariantAsVariantIndex);

    while (result.HasNextCapture(CAPTURE_VARIANT))
    {
        const auto& variantToken = result.NextCapture(CAPTURE_VARIANT);
        const auto maybeVariant = GetVariantIndexForName(*state->GetRepository(), variantToken.TypeNameValue());
        if (!maybeVariant)
            throw ParsingException(variantToken.GetPos(), "No variant with this name.");

        information->m_variants.emplace(*maybeVariant);
    }
}
