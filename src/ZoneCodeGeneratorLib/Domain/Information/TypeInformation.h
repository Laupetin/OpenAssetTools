#pragma once

#include "Domain/Definition/DataDefinition.h"

class TypeInformation
{
public:
    explicit TypeInformation(DataDefinition* definition);

    DataDefinition* m_definition;
    bool m_reusable_reference_exists;
};
