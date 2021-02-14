#pragma once

#include <vector>

#include "Parsing/Sequence/AbstractSequence.h"
#include "Parsing/Header/Impl/HeaderParserState.h"

enum class HeaderBlockType
{
    NONE,
    NAMESPACE,
    ENUM,
    STRUCT,
    UNION
};

class HeaderParserValue;
class HeaderParserState;
class IHeaderBlock
{
public:
    typedef AbstractSequence<HeaderParserValue, HeaderParserState> sequence_t;

    IHeaderBlock() = default;
    virtual ~IHeaderBlock() = default;
    IHeaderBlock(const IHeaderBlock& other) = default;
    IHeaderBlock(IHeaderBlock&& other) noexcept = default;
    IHeaderBlock& operator=(const IHeaderBlock& other) = default;
    IHeaderBlock& operator=(IHeaderBlock&& other) noexcept = default;

    virtual HeaderBlockType GetType() = 0;

    virtual const std::vector<sequence_t*>& GetTestsForBlock() = 0;

    virtual void OnOpen(HeaderParserState* state) = 0;
    virtual void OnClose(HeaderParserState* state) = 0;
    virtual void OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block) = 0;
};