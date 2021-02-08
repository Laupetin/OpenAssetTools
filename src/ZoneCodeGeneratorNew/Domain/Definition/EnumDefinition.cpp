#include "EnumDefinition.h"

#include <cassert>

EnumDefinition::EnumDefinition(std::string _namespace, std::string name, const BaseTypeDefinition* parentType)
    : DataDefinition(std::move(_namespace), std::move(name)),
      m_parent_type(parentType)
{
    assert(parentType != nullptr);
}

DataDefinitionType EnumDefinition::GetType() const
{
    return DataDefinitionType::ENUM;
}

unsigned EnumDefinition::GetAlignment()
{
    return m_parent_type->GetAlignment();
}

bool EnumDefinition::GetForceAlignment()
{
    return m_parent_type->GetForceAlignment();
}

unsigned EnumDefinition::GetSize()
{
    return m_parent_type->GetSize();
}

void EnumDefinition::AddEnumMember(EnumMember enumMember)
{
    m_members.emplace_back(std::make_unique<EnumMember>(std::move(enumMember)));
}
