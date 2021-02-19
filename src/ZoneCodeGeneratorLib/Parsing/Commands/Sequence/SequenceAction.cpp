#include "SequenceAction.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceAction::SequenceAction()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(
        {
            create.Char('('),
            create.Optional(create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_ARG_TYPE),
                create.OptionalLoop(create.And({
                    create.Char(','),
                    create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_ARG_TYPE)
                }))
            })),
            create.Char(')')
        }, LABEL_ACTION_ARGS);

    AddMatchers({
        create.Keyword("set"),
        create.Keyword("action"),
        create.Or({
            create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
                create.Identifier().Capture(CAPTURE_ACTION_NAME)
            }),
            create.Identifier().Capture(CAPTURE_ACTION_NAME)
        }),
        create.Label(LABEL_ACTION_ARGS),
        create.Char(';')
    });
}

void SequenceAction::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    StructureInformation* type;
    const auto& actionNameToken = result.NextCapture(CAPTURE_ACTION_NAME);

    // If a typename was specified then try to parse it
    // Otherwise take the name that was specified with the last use command
    if (result.HasNextCapture(CAPTURE_TYPE))
    {
        const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
        std::vector<MemberInformation*> memberChain;

        if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, memberChain))
            throw ParsingException(typeNameToken.GetPos(), "Unknown type");

        if (!memberChain.empty())
        {
            type = memberChain.back()->m_type;
            if (type == nullptr)
                throw ParsingException(typeNameToken.GetPos(), "Member is not a data type with members.");
        }
    }
    else if (state->GetInUse() != nullptr)
    {
        type = state->GetInUse();
    }
    else
        throw ParsingException(actionNameToken.GetPos(), "No type is used. Therefore one needs to be specified directly.");

    // Extract all parameter types that were specified
    std::vector<DataDefinition*> parameterTypes;
    while(result.HasNextCapture(CAPTURE_ARG_TYPE))
    {
        const auto& argTypeToken = result.NextCapture(CAPTURE_ARG_TYPE);
        auto* dataDefinition = state->GetRepository()->GetDataDefinitionByName(argTypeToken.TypeNameValue());

        if (dataDefinition == nullptr)
            throw ParsingException(argTypeToken.GetPos(), "Unknown type");

        parameterTypes.push_back(dataDefinition);
    }

    type->m_post_load_action = std::make_unique<CustomAction>(actionNameToken.IdentifierValue(), std::move(parameterTypes));
}
