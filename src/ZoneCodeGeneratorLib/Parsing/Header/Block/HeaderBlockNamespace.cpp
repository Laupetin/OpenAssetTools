#include "HeaderBlockNamespace.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceForwardDecl.h"
#include "Parsing/Header/Sequence/SequenceNamespace.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceTypedef.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"

HeaderBlockNamespace::HeaderBlockNamespace(std::string namespaceName)
    : m_namespace_name(std::move(namespaceName))
{
}

HeaderBlockType HeaderBlockNamespace::GetType()
{
    return HeaderBlockType::NAMESPACE;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockNamespace::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({new SequenceCloseBlock(false),
                                           new SequenceEnum(),
                                           new SequenceForwardDecl(),
                                           new SequenceNamespace(),
                                           new SequenceStruct(),
                                           new SequenceTypedef(),
                                           new SequenceUnion()});

    return tests;
}

void HeaderBlockNamespace::OnOpen(HeaderParserState* state)
{
    state->m_namespace.Push(m_namespace_name);
}

void HeaderBlockNamespace::OnClose(HeaderParserState* state)
{
    state->m_namespace.Pop();
}

void HeaderBlockNamespace::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) {}
