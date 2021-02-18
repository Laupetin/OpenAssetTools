#include "HeaderBlockEnum.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnumMember.h"

HeaderBlockType HeaderBlockEnum::GetType()
{
    return HeaderBlockType::UNION;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockEnum::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({
        new SequenceCloseBlock(true),
        new SequenceEnumMember()
    });

    return tests;
}

void HeaderBlockEnum::OnOpen(HeaderParserState* state)
{
}

void HeaderBlockEnum::OnClose(HeaderParserState* state)
{
}

void HeaderBlockEnum::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block)
{
}

void HeaderBlockEnum::SetVariableName(std::string name)
{
    m_variable_name = std::move(name);
}
