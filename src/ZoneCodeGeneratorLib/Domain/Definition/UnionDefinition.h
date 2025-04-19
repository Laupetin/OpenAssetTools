#pragma once

#include "DefinitionWithMembers.h"

class UnionDefinition final : public DefinitionWithMembers
{
public:
    UnionDefinition(std::string _namespace, std::string name, int pack);

    [[nodiscard]] DataDefinitionType GetType() const override;
};
