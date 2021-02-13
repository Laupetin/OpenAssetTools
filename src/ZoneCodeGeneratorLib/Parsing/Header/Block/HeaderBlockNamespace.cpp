#include "HeaderBlockNamespace.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceForwardDecl.h"
#include "Parsing/Header/Sequence/SequenceNamespace.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceTypedef.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"

HeaderBlockType HeaderBlockNamespace::GetType()
{
    return HeaderBlockType::NAMESPACE;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockNamespace::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({
        new SequenceCloseBlock(),
        new SequenceEnum(),
        new SequenceForwardDecl(),
        new SequenceNamespace(),
        new SequenceStruct(),
        new SequenceTypedef(),
        new SequenceUnion()
    });

    return tests;
}

void HeaderBlockNamespace::OnOpen()
{
}

void HeaderBlockNamespace::OnClose()
{
}

void HeaderBlockNamespace::OnChildBlockClose(IHeaderBlock* block)
{
}
