#include "StructDefinition.h"

StructDefinition::StructDefinition(std::string _namespace, std::string name, const int pack)
    : DefinitionWithMembers(std::move(_namespace), std::move(name), pack)
{
}

DataDefinitionType StructDefinition::GetType() const
{
    return DataDefinitionType::STRUCT;
}
