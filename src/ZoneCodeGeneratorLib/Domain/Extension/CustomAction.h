#pragma once

#include <string>
#include <vector>

#include "Domain/Definition/DataDefinition.h"

class CustomAction
{
public:
    std::string m_action_name;
    std::vector<DataDefinition*> m_parameter_types;

    CustomAction(std::string actionName, std::vector<DataDefinition*> parameterTypes);
};
