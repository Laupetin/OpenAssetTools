#include "SequenceReorder.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

#include <list>

namespace
{
    static constexpr auto TAG_FIND_FIRST = 1;

    static constexpr auto CAPTURE_START = 1;
    static constexpr auto CAPTURE_TYPE = 2;
    static constexpr auto CAPTURE_ENTRY = 3;

    StructureInformation* GetType(CommandsParserState* state, SequenceResult<CommandsParserValue>& result)
    {
        if (result.HasNextCapture(CAPTURE_TYPE))
        {
            const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
            StructureInformation* information;
            std::vector<MemberInformation*> memberChain;

            if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), information, memberChain))
                throw ParsingException(typeNameToken.GetPos(), "Unknown type");

            if (memberChain.empty())
                return information;

            auto* lastMember = memberChain.back();

            if (lastMember->m_type == nullptr)
                throw ParsingException(typeNameToken.GetPos(), "Member type must either be struct or union");

            return lastMember->m_type;
        }

        if (state->GetInUse() != nullptr)
            return state->GetInUse();

        throw ParsingException(result.NextCapture(CAPTURE_START).GetPos(), "No type is used. Therefore one needs to be specified directly.");
    }
} // namespace

SequenceReorder::SequenceReorder()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("reorder").Capture(CAPTURE_START),
        create.Optional(create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE)),
        create.Char(':'),
        create.Optional(create
                            .And({
                                create.Char('.'),
                                create.Char('.'),
                                create.Char('.'),
                            })
                            .Tag(TAG_FIND_FIRST)),
        create.Loop(create.Identifier().Capture(CAPTURE_ENTRY)),
        create.Char(';'),
    });
}

void SequenceReorder::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    auto* information = GetType(state, result);
    auto findFirst = result.PeekAndRemoveIfTag(TAG_FIND_FIRST) == TAG_FIND_FIRST;

    std::string firstVariableName;
    if (findFirst)
        firstVariableName = result.NextCapture(CAPTURE_ENTRY).IdentifierValue();

    std::vector<std::unique_ptr<MemberInformation>> newMembers;
    std::list<std::unique_ptr<MemberInformation>> oldMembers;

    for (auto& oldMember : information->m_ordered_members)
        oldMembers.emplace_back(std::move(oldMember));

    while (result.HasNextCapture(CAPTURE_ENTRY))
    {
        const auto& entryToken = result.NextCapture(CAPTURE_ENTRY);
        const auto& nextVariableName = entryToken.IdentifierValue();
        auto foundEntry = false;

        for (auto i = oldMembers.begin(); i != oldMembers.end(); ++i)
        {
            if (i->get()->m_member->m_name == nextVariableName)
            {
                newMembers.emplace_back(std::move(*i));
                oldMembers.erase(i);
                foundEntry = true;
                break;
            }
        }

        if (!foundEntry)
            throw ParsingException(entryToken.GetPos(), "Could not find member with specified name");
    }

    information->m_ordered_members.clear();

    while (findFirst && !oldMembers.empty())
    {
        if (oldMembers.front()->m_member->m_name == firstVariableName)
            findFirst = false;

        information->m_ordered_members.push_back(std::move(oldMembers.front()));
        oldMembers.pop_front();
    }

    for (auto& newMember : newMembers)
        information->m_ordered_members.push_back(std::move(newMember));

    for (auto& oldMember : oldMembers)
        information->m_ordered_members.emplace_back(std::move(oldMember));
}
