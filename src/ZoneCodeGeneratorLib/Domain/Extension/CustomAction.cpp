#include "CustomAction.h"

CustomAction::CustomAction(std::string actionName, std::vector<DataDefinition*> parameterTypes)
    : m_action_name(std::move(actionName)),
      m_parameter_types(std::move(parameterTypes))
{
}
