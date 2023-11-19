#include "SequenceArraySize.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceArraySize::SequenceArraySize()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({create.Keyword("set"),
                 create.Keyword("arraysize"),
                 create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
                 create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Capture(CAPTURE_EVALUATION),
                 create.Char(';')});
}

void SequenceArraySize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* structure;
    std::vector<MemberInformation*> memberChain;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), structure, memberChain))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (memberChain.empty())
        throw ParsingException(typeNameToken.GetPos(), "Must specify type with member");

    const auto& memberDeclarationModifiers = memberChain.back()->m_member->m_type_declaration->m_declaration_modifiers;
    ArrayDeclarationModifier* arrayModifier = nullptr;
    for (const auto& modifier : memberDeclarationModifiers)
    {
        if (modifier->GetType() == DeclarationModifierType::ARRAY)
        {
            arrayModifier = dynamic_cast<ArrayDeclarationModifier*>(modifier.get());
            break;
        }
    }

    if (arrayModifier == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Specified member is not an array");

    auto evaluation = CommandsCommonMatchers::ProcessEvaluation(state, result, structure);

    arrayModifier->m_dynamic_size_evaluation = std::move(evaluation);
}
