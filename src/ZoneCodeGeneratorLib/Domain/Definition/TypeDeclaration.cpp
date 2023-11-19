#include "TypeDeclaration.h"

#include "ArrayDeclarationModifier.h"

#include <cassert>

TypeDeclaration::TypeDeclaration(const DataDefinition* type)
    : m_flags(0),
      m_size(0),
      m_alignment(0),
      m_is_const(false),
      m_has_custom_bit_size(false),
      m_type(type),
      m_custom_bit_size(0)
{
    assert(m_type != nullptr);
}

unsigned TypeDeclaration::GetSize() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_size;
}

unsigned TypeDeclaration::GetAlignment() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_alignment;
}

bool TypeDeclaration::GetForceAlignment() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_flags & FLAG_ALIGNMENT_FORCED;
}
