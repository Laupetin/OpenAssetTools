#include "HeaderBlockNone.h"

#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceForwardDecl.h"
#include "Parsing/Header/Sequence/SequenceNamespace.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceTypedef.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"

HeaderBlockType HeaderBlockNone::GetType()
{
    return HeaderBlockType::NONE;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockNone::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests(
        {new SequenceEnum(), new SequenceForwardDecl(), new SequenceNamespace(), new SequenceStruct(), new SequenceTypedef(), new SequenceUnion()});

    return tests;
}

void HeaderBlockNone::OnOpen(HeaderParserState* state) {}

void HeaderBlockNone::OnClose(HeaderParserState* state) {}

void HeaderBlockNone::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) {}
