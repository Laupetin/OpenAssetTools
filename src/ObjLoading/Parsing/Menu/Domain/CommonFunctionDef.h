#pragma once

#include "Parsing/Simple/Expression/ISimpleExpression.h"

#include <string>

namespace menu
{
    class CommonFunctionDef
    {
    public:
        std::string m_name;
        std::unique_ptr<ISimpleExpression> m_value;
    };
} // namespace menu
