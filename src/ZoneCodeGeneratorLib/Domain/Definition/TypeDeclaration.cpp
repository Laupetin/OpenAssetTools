#include "TypeDeclaration.h"

#include "ArrayDeclarationModifier.h"

#include <cassert>

TypeDeclaration::TypeDeclaration(const DataDefinition* type)
    : m_is_const(false),
      m_has_custom_bit_size(false),
      m_type(type),
      m_custom_bit_size(0),
      m_flags(0),
      m_size{},
      m_alignment{}
{
    assert(m_type != nullptr);
}

unsigned TypeDeclaration::GetSize(const WordSize wordSize) const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_size[std::to_underlying(wordSize)];
}

unsigned TypeDeclaration::GetAlignment(const WordSize wordSize) const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_alignment[std::to_underlying(wordSize)];
}

bool TypeDeclaration::GetForceAlignment() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);
    return m_flags & FLAG_ALIGNMENT_FORCED;
}
