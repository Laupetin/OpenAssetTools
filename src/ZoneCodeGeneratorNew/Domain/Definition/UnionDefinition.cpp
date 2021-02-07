#include "UnionDefinition.h"

#include "Utils/AlignmentUtils.h"

void UnionDefinition::CalculateSize()
{
    m_size = 0;

    for(const auto& member : m_members)
    {
        const auto memberSize = member->m_type_declaration->GetSize();
        if (memberSize > m_size)
            m_size = memberSize;
    }

    m_size = AlignmentUtils::Align(m_size, GetAlignment());

    m_flags |= FLAG_SIZE_CALCULATED;
}

UnionDefinition::UnionDefinition(std::string _namespace, std::string name, const int pack)
    : DefinitionWithMembers(std::move(_namespace), std::move(name), pack)
{
}

DataDefinitionType UnionDefinition::GetType() const
{
    return DataDefinitionType::UNION;
}
