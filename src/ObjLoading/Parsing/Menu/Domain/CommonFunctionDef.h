#pragma once

#include <string>

#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonFunctionDef
    {
    public:
        std::string m_name;
        std::unique_ptr<ISimpleExpression> m_value;
    };
}
