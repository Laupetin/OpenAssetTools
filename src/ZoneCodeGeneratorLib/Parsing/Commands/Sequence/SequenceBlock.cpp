#include "SequenceBlock.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceBlock::SequenceBlock()
{
    const CommandsMatcherFactory create(this);

#define DEFINE_FAST_FILE_BLOCK_TYPE(type) AddFastFileBlockToLookup(#type, FastFileBlockType::type)
    DEFINE_FAST_FILE_BLOCK_TYPE(TEMP);
    DEFINE_FAST_FILE_BLOCK_TYPE(RUNTIME);
    DEFINE_FAST_FILE_BLOCK_TYPE(DELAY);
    DEFINE_FAST_FILE_BLOCK_TYPE(NORMAL);
#undef DEFINE_FAST_FILE_BLOCK_TYPE

    AddMatchers({
        create.Keyword("block"),
        create.Identifier().Capture(CAPTURE_BLOCK_TYPE),
        create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY),
        create.Optional(create.Keyword("default").Tag(TAG_DEFAULT)),
        create.Char(';'),
    });
}

void SequenceBlock::AddFastFileBlockToLookup(std::string name, const FastFileBlockType type)
{
    for (auto& c : name)
        c = static_cast<char>(tolower(c));

    m_type_lookup[name] = type;
}

bool SequenceBlock::GetFastFileBlockNameByType(const std::string& name, FastFileBlockType& type) const
{
    const auto foundEntry = m_type_lookup.find(name);
    if (foundEntry == m_type_lookup.end())
        return false;

    type = foundEntry->second;
    return true;
}

void SequenceBlock::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& enumEntryToken = result.NextCapture(CAPTURE_BLOCK_ENUM_ENTRY);
    auto* enumMember = state->GetRepository()->GetEnumMemberByName(enumEntryToken.IdentifierValue());
    if (enumMember == nullptr)
        throw ParsingException(enumEntryToken.GetPos(), "Unknown block enum entry");

    const auto& typeToken = result.NextCapture(CAPTURE_BLOCK_TYPE);
    FastFileBlockType type;
    if (!GetFastFileBlockNameByType(typeToken.IdentifierValue(), type))
        throw ParsingException(typeToken.GetPos(), "Unknown fastfile block type");

    auto isDefault = result.PeekAndRemoveIfTag(TAG_DEFAULT) == TAG_DEFAULT;

    state->AddBlock(std::make_unique<FastFileBlock>(enumMember->m_name, static_cast<int>(enumMember->m_value), type, isDefault));
}
