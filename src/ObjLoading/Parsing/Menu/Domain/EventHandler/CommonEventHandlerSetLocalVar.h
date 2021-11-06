#pragma once
#include <memory>
#include <string>

#include "ICommonEventHandlerElement.h"
#include "Parsing/Menu/Domain/Expression/ICommonExpression.h"

namespace menu
{
    class CommonEventHandlerSetLocalVar final : public ICommonEventHandlerElement
    {
    public:
        std::string m_var_name;
        std::unique_ptr<ICommonExpression> m_value;

        CommonEventHandlerSetLocalVar();
        CommonEventHandlerSetLocalVar(std::string varName, std::unique_ptr<ICommonExpression> value);

        CommonEventHandlerElementType GetType() override;
    };
}
