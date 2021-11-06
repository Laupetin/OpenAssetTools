#pragma once
#include <memory>
#include <string>

#include "ICommonEventHandlerElement.h"
#include "Parsing/Menu/Domain/Expression/ICommonExpression.h"

namespace menu
{
    enum class SetLocalVarType
    {
        UNKNOWN,
        BOOL,
        INT,
        FLOAT,
        STRING
    };

    class CommonEventHandlerSetLocalVar final : public ICommonEventHandlerElement
    {
    public:
        SetLocalVarType m_type;
        std::string m_var_name;
        std::unique_ptr<ICommonExpression> m_value;

        CommonEventHandlerSetLocalVar();
        CommonEventHandlerSetLocalVar(SetLocalVarType type, std::string varName, std::unique_ptr<ICommonExpression> value);

        CommonEventHandlerElementType GetType() override;
    };
}
