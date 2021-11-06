#include "CommonEventHandlerSetLocalVar.h"

using namespace menu;

CommonEventHandlerSetLocalVar::CommonEventHandlerSetLocalVar()
= default;

CommonEventHandlerSetLocalVar::CommonEventHandlerSetLocalVar(std::string varName, std::unique_ptr<ICommonExpression> value)
    : m_var_name(std::move(varName)),
      m_value(std::move(value))
{
}

CommonEventHandlerElementType CommonEventHandlerSetLocalVar::GetType()
{
    return CommonEventHandlerElementType::SET_LOCAL_VAR;
}
