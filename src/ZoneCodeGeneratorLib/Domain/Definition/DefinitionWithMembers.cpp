#include "DefinitionWithMembers.h"

#include <cassert>

DefinitionWithMembers::DefinitionWithMembers(std::string _namespace, std::string name, const unsigned pack)
    : DataDefinition(std::move(_namespace), std::move(name)),
      m_flags(0),
      m_size(0),
      m_alignment(0),
      m_has_alignment_override(false),
      m_anonymous(false),
      m_pack(pack),
      m_alignment_override(0)
{
}

unsigned DefinitionWithMembers::GetAlignment() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);

    return m_alignment;
}

bool DefinitionWithMembers::GetForceAlignment() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);

    return m_flags & FLAG_ALIGNMENT_FORCED;
}

unsigned DefinitionWithMembers::GetSize() const
{
    assert(m_flags & FLAG_FIELDS_CALCULATED);

    return m_size;
}
