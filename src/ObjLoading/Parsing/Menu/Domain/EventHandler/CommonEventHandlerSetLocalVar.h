#pragma once
#include <memory>
#include <string>

#include "ICommonEventHandlerElement.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

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
        std::unique_ptr<ISimpleExpression> m_value;

        CommonEventHandlerSetLocalVar();
        CommonEventHandlerSetLocalVar(SetLocalVarType type, std::string varName, std::unique_ptr<ISimpleExpression> value);

        _NODISCARD CommonEventHandlerElementType GetType() const override;
    };
}
