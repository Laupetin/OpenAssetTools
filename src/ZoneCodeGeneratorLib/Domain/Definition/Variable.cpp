#include "Variable.h"

Variable::Variable(std::string name, std::unique_ptr<TypeDeclaration> typeDeclaration)
    : m_name(std::move(name)),
      m_has_alignment_override(false),
      m_alignment_override(0),
      m_offset(0),
      m_type_declaration(std::move(typeDeclaration))
{
}

unsigned Variable::GetAlignment(const WordSize wordSize) const
{
    if (m_has_alignment_override)
        return m_alignment_override;

    return m_type_declaration->GetAlignment(wordSize);
}

bool Variable::GetForceAlignment() const
{
    return m_has_alignment_override || m_type_declaration->GetForceAlignment();
}
