#pragma once

#include "Domain/Definition/DataDefinition.h"

#include <string>
#include <vector>

class CustomAction
{
public:
    CustomAction(std::string actionName, std::vector<DataDefinition*> parameterTypes);

    std::string m_action_name;
    std::vector<DataDefinition*> m_parameter_types;
};
