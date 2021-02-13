#include "HeaderBlockUnion.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"
#include "Parsing/Header/Sequence/SequenceVariable.h"

HeaderBlockType HeaderBlockUnion::GetType()
{
    return HeaderBlockType::UNION;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockUnion::GetTestsForBlock()
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

void HeaderBlockUnion::OnOpen()
{
}

void HeaderBlockUnion::OnClose()
{
}

void HeaderBlockUnion::OnChildBlockClose(IHeaderBlock* block)
{
}
