#pragma once

#include "DefinitionWithMembers.h"

class UnionDefinition final : public DefinitionWithMembers
{
    // protected:
    //     void CalculateSize() override;

public:
    UnionDefinition(std::string _namespace, std::string name, int pack);

    _NODISCARD DataDefinitionType GetType() const override;
};
