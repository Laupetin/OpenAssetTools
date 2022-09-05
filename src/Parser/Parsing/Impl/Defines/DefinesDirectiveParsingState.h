#pragma once

#include <map>
#include <memory>

#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

class DefinesDirectiveParsingState
{
public:
    const std::map<std::string, DefinesStreamProxy::Define>& m_defines;
    std::unique_ptr<ISimpleExpression> m_expression;

    explicit DefinesDirectiveParsingState(const std::map<std::string, DefinesStreamProxy::Define>& defines);
};
