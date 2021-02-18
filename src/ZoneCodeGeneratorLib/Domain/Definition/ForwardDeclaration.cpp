#include "ForwardDeclaration.h"

ForwardDeclaration::ForwardDeclaration(std::string _namespace, std::string name, DataDefinitionType type)
    : DataDefinition(std::move(_namespace), std::move(name)),
      m_forwarded_type(type),
      m_definition(nullptr)
{
}

DataDefinitionType ForwardDeclaration::GetType() const
{
    return DataDefinitionType::FORWARD_DECLARATION;
}

unsigned ForwardDeclaration::GetAlignment() const
{
    return 0;
}

bool ForwardDeclaration::GetForceAlignment() const
{
    return false;
}

unsigned ForwardDeclaration::GetSize() const
{
    return 0;
}
