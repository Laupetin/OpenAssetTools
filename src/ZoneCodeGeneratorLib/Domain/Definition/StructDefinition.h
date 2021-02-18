#pragma once

#include "DefinitionWithMembers.h"

class StructDefinition final : public DefinitionWithMembers
{
//protected:
//    void CalculateSize() override;

public:
    StructDefinition(std::string _namespace, std::string name, int pack);

    _NODISCARD DataDefinitionType GetType() const override;
};
