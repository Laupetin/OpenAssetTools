#pragma once

#include "DefinitionWithMembers.h"

class StructDefinition final : public DefinitionWithMembers
{
public:
    StructDefinition(std::string _namespace, std::string name, int pack);

    _NODISCARD DataDefinitionType GetType() const override;
};
