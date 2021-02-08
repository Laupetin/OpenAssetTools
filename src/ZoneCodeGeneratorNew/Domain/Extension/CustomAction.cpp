#include "CustomAction.h"

CustomAction::CustomAction(std::string actionName)
    : m_action_name(std::move(actionName))
{
}
