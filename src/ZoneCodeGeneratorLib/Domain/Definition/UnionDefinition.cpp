#include "UnionDefinition.h"

UnionDefinition::UnionDefinition(std::string _namespace, std::string name, const int pack)
    : DefinitionWithMembers(std::move(_namespace), std::move(name), pack)
{
}

DataDefinitionType UnionDefinition::GetType() const
{
    return DataDefinitionType::UNION;
}
