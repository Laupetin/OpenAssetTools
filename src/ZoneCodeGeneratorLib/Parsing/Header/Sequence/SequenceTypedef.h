#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceTypedef final : public HeaderParser::sequence_t
{
public:
    SequenceTypedef();

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

private:
    void AddPointerDeclarationModifiers(SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const;
    void AddArrayDeclarationModifiers(HeaderParserState* state, SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const;
};
