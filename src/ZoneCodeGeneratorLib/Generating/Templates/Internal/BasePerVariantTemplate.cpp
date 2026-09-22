#include "BasePerVariantTemplate.h"

#include "Domain/Computations/StructureComputations.h"
#include "Domain/Definition/ArrayDeclarationModifier.h"

#include <sstream>

BasePerVariantTemplate::BasePerVariantTemplate(std::ostream& stream, const PerVariantRenderingContext& context)
    : BaseTemplate(stream, context),
      m_env(context)
{
}

std::string BasePerVariantTemplate::MakeAllocAlignment(const StructureInformation& info) const
{
    if (info.m_alloc_alignment)
        return MakeEvaluation(info.m_alloc_alignment.get());

    return std::to_string(info.m_definition->GetAlignment(m_env.m_word_size));
}

size_t BasePerVariantTemplate::SizeForDeclModifierLevel(const MemberInformation& memberInfo, const size_t level) const
{
    const auto& declModifiers = memberInfo.m_member->m_type_declaration->m_declaration_modifiers;
    if (declModifiers.empty())
        return memberInfo.m_member->m_type_declaration->GetSize(m_env.m_word_size);

    if (level == 0)
        return memberInfo.m_member->m_type_declaration->GetSize(m_env.m_word_size);

    size_t currentSize = memberInfo.m_member->m_type_declaration->m_type->GetSize(m_env.m_word_size);
    const auto end = declModifiers.rbegin() + (declModifiers.size() - level);
    for (auto i = declModifiers.rbegin(); i != end; ++i)
    {
        if ((*i)->GetType() == DeclarationModifierType::POINTER)
            currentSize = m_env.m_pointer_size;
        else
            currentSize *= dynamic_cast<ArrayDeclarationModifier*>(i->get())->m_size;
    }

    return currentSize;
}

size_t BasePerVariantTemplate::OffsetForMemberModifier(const MemberInformation& memberInfo,
                                                       const DeclarationModifierComputations& modifier,
                                                       const size_t nestedBaseOffset) const
{
    size_t curOffset = memberInfo.m_member->m_offset;

    auto curLevel = 0u;
    for (const auto index : modifier.GetArrayIndices())
    {
        if (index > 0)
            curOffset += index * SizeForDeclModifierLevel(memberInfo, curLevel + 1);

        curLevel++;
    }

    return curOffset + nestedBaseOffset;
}

bool BasePerVariantTemplate::MemoryLayoutMatches(const StructureInformation& structureInfo) const
{
    return !m_env.m_word_size_mismatch || structureInfo.m_has_matching_cross_platform_memory_layout;
}
