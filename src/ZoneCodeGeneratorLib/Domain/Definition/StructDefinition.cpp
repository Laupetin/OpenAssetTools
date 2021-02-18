#include "StructDefinition.h"

#include "Utils/AlignmentUtils.h"

//void StructDefinition::CalculateSize()
//{
//    m_size = 0;
//    auto currentBitOffset = 0u;
//
//    for(const auto& member : m_members)
//    {
//        if(member->m_type_declaration->m_has_custom_bit_size)
//        {
//            currentBitOffset += member->m_type_declaration->m_custom_bit_size;
//        }
//        else
//        {
//            if (currentBitOffset > 0)
//            {
//                currentBitOffset = AlignmentUtils::Align(currentBitOffset, 8u);
//                m_size += currentBitOffset / 8;
//                currentBitOffset = 0;
//            }
//
//            m_size = AlignmentUtils::Align(m_size, member->GetForceAlignment() ? member->GetAlignment() : std::min(member->GetAlignment(), m_pack));
//            m_size += member->m_type_declaration->GetSize();
//        }
//    }
//
//    if (currentBitOffset > 0)
//    {
//        currentBitOffset = AlignmentUtils::Align(currentBitOffset, 8u);
//        m_size += currentBitOffset / 8;
//    }
//
//    m_size = AlignmentUtils::Align(m_size, GetAlignment());
//
//    m_flags |= FLAG_SIZE_CALCULATED;
//}

StructDefinition::StructDefinition(std::string _namespace, std::string name, const int pack)
    : DefinitionWithMembers(std::move(_namespace), std::move(name), pack)
{
}

DataDefinitionType StructDefinition::GetType() const
{
    return DataDefinitionType::STRUCT;
}
