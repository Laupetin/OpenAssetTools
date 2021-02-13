#include "HeaderBlockStruct.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"
#include "Parsing/Header/Sequence/SequenceVariable.h"

HeaderBlockType HeaderBlockStruct::GetType()
{
    return HeaderBlockType::STRUCT;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockStruct::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({
        new SequenceCloseBlock(),
        new SequenceEnum(),
        new SequenceStruct(),
        new SequenceUnion(),
        new SequenceVariable()
    });

    return tests;
}

void HeaderBlockStruct::OnOpen()
{
}

void HeaderBlockStruct::OnClose()
{
}

void HeaderBlockStruct::OnChildBlockClose(IHeaderBlock* block)
{
}
