#include "CommonEventHandlerScript.h"

using namespace menu;

CommonEventHandlerScript::CommonEventHandlerScript()
= default;

CommonEventHandlerScript::CommonEventHandlerScript(std::string script)
    : m_script(std::move(script))
{
}

CommonEventHandlerElementType CommonEventHandlerScript::GetType() const
{
    return CommonEventHandlerElementType::SCRIPT;
}
