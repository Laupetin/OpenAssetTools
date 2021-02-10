#include "MemberComputations.h"

#include <algorithm>
#include <cassert>

#include "StructureComputations.h"
#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

MemberComputations::MemberComputations(const MemberInformation* member)
    : m_info(member)
{
    assert(m_info != nullptr);
}

bool MemberComputations::ShouldIgnore() const
{
    return m_info->m_condition && m_info->m_condition->IsStatic() && m_info->m_condition->EvaluateNumeric() == 0;
}

bool MemberComputations::ContainsNonEmbeddedReference() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    return std::any_of(declarationModifiers.begin(), declarationModifiers.end(), [](const std::unique_ptr<DeclarationModifier>& modifier)
    {
        return modifier->GetType() == DeclarationModifierType::POINTER;
    });
}

bool MemberComputations::ContainsSinglePointerReference() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return false;

    const auto* lastModifier = declarationModifiers[declarationModifiers.size() - 1].get();
    if (lastModifier->GetType() != DeclarationModifierType::POINTER)
        return false;

    return !dynamic_cast<const PointerDeclarationModifier*>(lastModifier)->AnyCountEvaluationIsArray();
}

bool MemberComputations::ContainsArrayPointerReference() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return false;

    const auto* lastModifier = declarationModifiers[declarationModifiers.size() - 1].get();
    if (lastModifier->GetType() != DeclarationModifierType::POINTER)
        return false;

    return dynamic_cast<const PointerDeclarationModifier*>(lastModifier)->AnyCountEvaluationIsArray();
}

bool MemberComputations::ContainsPointerArrayReference() const
{
    if (!ContainsSinglePointerReference())
        return false;

    const auto pointerDepth = GetPointerDepth();
    const auto isArray = IsArray();

    return isArray && pointerDepth == 1 || !isArray && pointerDepth == 2;
}

bool MemberComputations::ContainsArrayReference() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return false;

    const auto* lastModifier = declarationModifiers[declarationModifiers.size() - 1].get();
    return lastModifier->GetType() == DeclarationModifierType::ARRAY;
}

const IEvaluation* MemberComputations::GetArrayPointerCountEvaluation() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return nullptr;

    const auto* lastModifier = declarationModifiers[declarationModifiers.size() - 1].get();
    return lastModifier->GetType() == DeclarationModifierType::POINTER
               ? dynamic_cast<const PointerDeclarationModifier*>(lastModifier)->GetCountEvaluation()
               : nullptr;
}

bool MemberComputations::IsArray() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return false;

    return declarationModifiers[0]->GetType() == DeclarationModifierType::ARRAY;
}

std::vector<int> MemberComputations::GetArraySizes() const
{
    std::vector<int> sizes;
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    for (const auto& declarationModifier : declarationModifiers)
    {
        if (declarationModifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        sizes.push_back(dynamic_cast<const ArrayDeclarationModifier*>(declarationModifier.get())->m_size);
    }

    return sizes;
}

int MemberComputations::GetArrayDimension() const
{
    auto dimension = 0;
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    for (const auto& declarationModifier : declarationModifiers)
    {
        if (declarationModifier->GetType() != DeclarationModifierType::ARRAY)
            break;

        dimension++;
    }

    return dimension;
}

bool MemberComputations::IsPointerToArray() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    if (declarationModifiers.empty())
        return false;

    const auto* lastModifier = declarationModifiers[declarationModifiers.size() - 1].get();
    if (lastModifier->GetType() != DeclarationModifierType::ARRAY)
        return false;

    return std::any_of(declarationModifiers.begin(), declarationModifiers.end(), [](const std::unique_ptr<DeclarationModifier>& modifier)
    {
        return modifier->GetType() == DeclarationModifierType::POINTER;
    });
}

std::vector<int> MemberComputations::GetPointerToArraySizes() const
{
    std::vector<int> sizes;
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    for (const auto& declarationModifier : declarationModifiers)
    {
        if (declarationModifier->GetType() == DeclarationModifierType::ARRAY)
        {
            sizes.push_back(dynamic_cast<ArrayDeclarationModifier*>(declarationModifier.get())->m_size);
        }
        else
        {
            sizes.clear();
        }
    }

    return sizes;
}

int MemberComputations::GetPointerDepth() const
{
    auto depth = 0;
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;
    for(const auto& declarationModifier : declarationModifiers)
    {
        if (declarationModifier->GetType() == DeclarationModifierType::POINTER)
            depth++;
    }

    return depth;
}

bool MemberComputations::IsNotInDefaultNormalBlock() const
{
    return m_info->m_fast_file_block != nullptr && !(m_info->m_fast_file_block->m_type == FastFileBlockType::NORMAL && m_info->m_fast_file_block->m_is_default);
}

bool MemberComputations::IsInTempBlock() const
{
    return m_info->m_fast_file_block != nullptr && m_info->m_fast_file_block->m_type == FastFileBlockType::TEMP;
}

bool MemberComputations::IsInRuntimeBlock() const
{
    return m_info->m_fast_file_block != nullptr && m_info->m_fast_file_block->m_type == FastFileBlockType::RUNTIME;
}

bool MemberComputations::IsFirstMember() const
{
    const auto parentUsedMembers = StructureComputations(m_info->m_parent).GetUsedMembers();
    return !parentUsedMembers.empty() && parentUsedMembers[0] == m_info;
}

bool MemberComputations::IsLastMember() const
{
    const auto parentUsedMembers = StructureComputations(m_info->m_parent).GetUsedMembers();
    return !parentUsedMembers.empty() && parentUsedMembers[parentUsedMembers.size() - 1] == m_info;
}

bool MemberComputations::HasDynamicArraySize() const
{
    const auto& declarationModifiers = m_info->m_member->m_type_declaration->m_declaration_modifiers;

    return std::any_of(declarationModifiers.begin(), declarationModifiers.end(), [](const std::unique_ptr<DeclarationModifier>& declarationModifier)
    {
        return declarationModifier->GetType() == DeclarationModifierType::ARRAY
            && dynamic_cast<ArrayDeclarationModifier*>(declarationModifier.get())->m_dynamic_size_evaluation;
    });
}

bool MemberComputations::IsDynamicMember() const
{
    if (HasDynamicArraySize())
        return true;

    return !ContainsNonEmbeddedReference() && m_info->m_type && StructureComputations(m_info->m_type).GetDynamicMember() != nullptr;
}

bool MemberComputations::IsAfterPartialLoad() const
{
    if (IsDynamicMember())
        return true;

    return m_info->m_parent->m_definition->GetType() == DataDefinitionType::UNION && StructureComputations(m_info->m_parent).GetDynamicMember() != nullptr;
}
