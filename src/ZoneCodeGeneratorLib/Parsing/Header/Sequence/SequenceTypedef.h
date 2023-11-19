#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceTypedef final : public HeaderParser::sequence_t
{
    static constexpr auto TAG_ARRAY_OF_POINTERS = 1;
    static constexpr auto TAG_POINTER_TO_ARRAY = 2;
    static constexpr auto TAG_CONST = 3;
    static constexpr auto TAG_POINTER = 4;

    static constexpr auto CAPTURE_NAME = 1;
    static constexpr auto CAPTURE_ALIGN = 2;
    static constexpr auto CAPTURE_TYPE = 3;
    static constexpr auto CAPTURE_ARRAY = 4;

    static constexpr auto LABEL_ARRAY_OF_POINTERS = 1;
    static constexpr auto LABEL_POINTER_TO_ARRAY = 2;

    void AddPointerDeclarationModifiers(SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const;
    void AddArrayDeclarationModifiers(HeaderParserState* state, SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceTypedef();
};
