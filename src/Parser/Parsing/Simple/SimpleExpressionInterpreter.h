#pragma once
#include "Expression/ISimpleExpression.h"
#include "Parsing/IParserLineStream.h"

class SimpleExpressionInterpreter
{
public:
    explicit SimpleExpressionInterpreter(IParserLineStream* input);

    std::unique_ptr<ISimpleExpression> Evaluate() const;

private:
    IParserLineStream* m_input;
};
