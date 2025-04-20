#include "SequenceCondition.h"

#include "Domain/Evaluation/OperandStatic.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto TAG_ALWAYS = 1;
    static constexpr auto TAG_NEVER = 2;
    static constexpr auto TAG_EVALUATION = 3;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_EVALUATION = 2;
} // namespace

SequenceCondition::SequenceCondition()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("condition"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Or({
            create.Keyword("always").Tag(TAG_ALWAYS),
            create.Keyword("never").Tag(TAG_NEVER),
            create.Label(CommandsCommonMatchers::LABEL_EVALUATION).Tag(TAG_EVALUATION).Capture(CAPTURE_EVALUATION),
        }),
        create.Char(';'),
    });
}

void SequenceCondition::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> memberChain;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, memberChain))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (memberChain.empty())
        throw ParsingException(typeNameToken.GetPos(), "Conditions can only be set on members and not for types");

    std::unique_ptr<IEvaluation> conditionEvaluation;
    switch (result.NextTag())
    {
    case TAG_ALWAYS:
        conditionEvaluation = std::make_unique<OperandStatic>(1);
        break;

    case TAG_NEVER:
        conditionEvaluation = std::make_unique<OperandStatic>(0);
        break;

    case TAG_EVALUATION:
        conditionEvaluation = CommandsCommonMatchers::ProcessEvaluation(state, result, type);
        break;

    default:
        throw ParsingException(TokenPos(), "Unknown evaluation type @ Condition");
    }

    memberChain.back()->m_condition = std::move(conditionEvaluation);
}
