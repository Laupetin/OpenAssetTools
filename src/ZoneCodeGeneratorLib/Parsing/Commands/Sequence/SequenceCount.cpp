#include "SequenceCount.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Domain/Definition/PointerDeclarationModifier.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceCount::SequenceCount()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddLabeledMatchers(CommandsCommonMatchers::ArrayDef(this), CommandsCommonMatchers::LABEL_ARRAY_DEF);
    AddMatchers({
        create.Keyword("set").Capture(CAPTURE_START),
        create.Keyword("count"),
        create.OptionalLoop(create.Char('*').Tag(TAG_POINTER_RESOLVE)),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.OptionalLoop(create.Label(CommandsCommonMatchers::LABEL_ARRAY_DEF).Capture(CAPTURE_ARRAY_INDEX)),
        create.Label(CommandsCommonMatchers::LABEL_EVALUATION),
        create.Char(';'),
    });
}

void SequenceCount::SetCountByArrayIndex(CommandsParserState* state,
                                         SequenceResult<CommandsParserValue>& result,
                                         MemberInformation* member,
                                         PointerDeclarationModifier* pointer,
                                         std::unique_ptr<IEvaluation> evaluation)
{
    std::vector<int> arraySizes;
    std::vector<int> depthSize;
    for (const auto& modifier : member->m_member->m_type_declaration->m_declaration_modifiers)
    {
        if (modifier->GetType() == DeclarationModifierType::ARRAY)
            arraySizes.push_back(dynamic_cast<ArrayDeclarationModifier*>(modifier.get())->m_size);
    }

    depthSize.resize(arraySizes.size());
    auto currentDepthSize = 1u;
    for (auto i = arraySizes.size(); i > 0; i--)
    {
        if (i < arraySizes.size())
            currentDepthSize *= arraySizes[i];
        depthSize[i - 1] = currentDepthSize;
    }

    if (pointer->m_count_evaluation_by_array_index.empty())
    {
        auto neededCapacity = 0u;
        for (auto arraySize : arraySizes)
        {
            if (neededCapacity == 0)
                neededCapacity = arraySize;
            else
                neededCapacity *= arraySize;
        }
        pointer->m_count_evaluation_by_array_index.resize(neededCapacity);
    }

    auto currentIndex = 0u;
    auto currentIndexOffset = 0u;
    while (result.HasNextCapture(CAPTURE_ARRAY_INDEX) && currentIndexOffset < depthSize.size())
    {
        const auto& arrayIndexToken = result.NextCapture(CAPTURE_ARRAY_INDEX);
        if (arrayIndexToken.m_type == CommandsParserValueType::INTEGER)
        {
            currentIndex += depthSize[currentIndexOffset++] * arrayIndexToken.IntegerValue();
        }
        else
        {
            auto* enumEntry = state->GetRepository()->GetEnumMemberByName(arrayIndexToken.IdentifierValue());
            if (enumEntry == nullptr)
                throw ParsingException(arrayIndexToken.GetPos(), "Unknown enum entry");

            currentIndex += depthSize[currentIndexOffset++] * enumEntry->m_value;
        }
    }

    pointer->m_count_evaluation_by_array_index[currentIndex] = std::move(evaluation);
}

void SequenceCount::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> memberChain;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, memberChain))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (memberChain.empty())
        throw ParsingException(typeNameToken.GetPos(), "Conditions can only be set on members and not for types");

    auto resolve = 0u;
    while (result.PeekAndRemoveIfTag(TAG_POINTER_RESOLVE) == TAG_POINTER_RESOLVE)
        resolve++;

    auto* lastMember = memberChain.back();
    PointerDeclarationModifier* pointer = nullptr;
    for (const auto& modifier : lastMember->m_member->m_type_declaration->m_declaration_modifiers)
    {
        if (modifier->GetType() == DeclarationModifierType::POINTER)
        {
            if (resolve > 0)
            {
                resolve--;
            }
            else
            {
                pointer = dynamic_cast<PointerDeclarationModifier*>(modifier.get());
                break;
            }
        }
    }

    if (pointer == nullptr)
        throw ParsingException(result.NextCapture(CAPTURE_START).GetPos(), "No matching pointer modifier found");

    auto countEvaluation = CommandsCommonMatchers::ProcessEvaluation(state, result, type);

    if (!result.HasNextCapture(CAPTURE_ARRAY_INDEX))
    {
        pointer->m_count_evaluation = std::move(countEvaluation);
    }
    else
    {
        SetCountByArrayIndex(state, result, lastMember, pointer, std::move(countEvaluation));
    }
}
