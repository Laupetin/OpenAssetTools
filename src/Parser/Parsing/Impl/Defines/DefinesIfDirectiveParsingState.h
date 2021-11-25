#pragma once

#include <map>
#include <memory>

#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

class DefinesIfDirectiveParsingState
{
public:
    const std::map<std::string, DefinesStreamProxy::Define>& m_defines;
    std::unique_ptr<ISimpleExpression> m_expression;

    explicit DefinesIfDirectiveParsingState(const std::map<std::string, DefinesStreamProxy::Define>& defines);
};
