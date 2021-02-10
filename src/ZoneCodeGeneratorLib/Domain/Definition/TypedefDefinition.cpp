#include "TypedefDefinition.h"

TypedefDefinition::TypedefDefinition(std::string _namespace, std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration)
    : DataDefinition(std::move(_namespace), std::move(name)),
      m_has_alignment_override(false),
      m_alignment_override(0),
      m_type_declaration(std::move(typeDeclaration))
{
}

DataDefinitionType TypedefDefinition::GetType() const
{
    return DataDefinitionType::TYPEDEF;
}

unsigned TypedefDefinition::GetAlignment()
{
    if (m_has_alignment_override)
    {
        return m_alignment_override;
    }
    return m_type_declaration->GetAlignment();
}

bool TypedefDefinition::GetForceAlignment()
{
    return m_has_alignment_override || m_type_declaration->GetForceAlignment();
}

unsigned TypedefDefinition::GetSize()
{
    return m_type_declaration->GetSize();
}
