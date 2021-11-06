#include "CommonEventHandlerSetLocalVar.h"

using namespace menu;

CommonEventHandlerSetLocalVar::CommonEventHandlerSetLocalVar()
    : m_type(SetLocalVarType::UNKNOWN)
{
}

CommonEventHandlerSetLocalVar::CommonEventHandlerSetLocalVar(SetLocalVarType type, std::string varName, std::unique_ptr<ICommonExpression> value)
    : m_type(type),
      m_var_name(std::move(varName)),
      m_value(std::move(value))
{
}

CommonEventHandlerElementType CommonEventHandlerSetLocalVar::GetType()
{
    return CommonEventHandlerElementType::SET_LOCAL_VAR;
}
