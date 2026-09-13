#include "SequencePointerArrayWordSize.h"

#include "Domain/Environment/WordSize.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_WORD_SIZE = 2;
} // namespace

SequencePointerArrayWordSize::SequencePointerArrayWordSize()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("pointerarraywordsize"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Integer().Capture(CAPTURE_WORD_SIZE),
        create.Char(';'),
    });
}

void SequencePointerArrayWordSize::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    const auto& wordSizeToken = result.NextCapture(CAPTURE_WORD_SIZE);

    StructureInformation* type;
    std::vector<MemberInformation*> members;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, members))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");
    if (members.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a pointer-array member.");

    switch (wordSizeToken.IntegerValue())
    {
    case 32:
        members.back()->m_pointer_array_word_size = WordSize::BITS_32;
        break;
    case 64:
        members.back()->m_pointer_array_word_size = WordSize::BITS_64;
        break;
    default:
        throw ParsingException(wordSizeToken.GetPos(), "Unknown pointer-array word size");
    }
}
