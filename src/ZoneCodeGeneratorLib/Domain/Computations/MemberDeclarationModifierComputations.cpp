#include "MemberDeclarationModifierComputations.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Domain/Definition/PointerDeclarationModifier.h"
#include "MemberComputations.h"

#include <algorithm>
#include <cassert>

DeclarationModifierComputations::DeclarationModifierComputations(const MemberInformation* member, std::vector<int> modifierIndices)
    : m_information(member),
      m_modifier_indices(std::move(modifierIndices))
{
    auto combinedIndex = 0;
    auto arraySizes = MemberComputations(m_information).GetArraySizes();
    std::vector<int> sizePerDepth(arraySizes.size());

    auto currentDepthSize = 1;
    for (int i = arraySizes.size(); i > 0; i--)
    {
        sizePerDepth[i - 1] = currentDepthSize;
        currentDepthSize *= arraySizes[i - 1];
    }

    auto currentDepth = 0;
    for (auto modifierIndex : m_modifier_indices)
    {
        combinedIndex += sizePerDepth[currentDepth++] * modifierIndex;
    }

    m_combined_index = combinedIndex;
}

DeclarationModifierComputations::DeclarationModifierComputations(const MemberInformation* member)
    : m_information(member),
      m_combined_index(0)
{
}

DeclarationModifier* DeclarationModifierComputations::GetDeclarationModifier() const
{
    const auto& declarationModifiers = m_information->m_member->m_type_declaration->m_declaration_modifiers;
    if (m_modifier_indices.size() < declarationModifiers.size())
        return declarationModifiers[m_modifier_indices.size()].get();

    return nullptr;
}

DeclarationModifier* DeclarationModifierComputations::GetNextDeclarationModifier() const
{
    const auto& declarationModifiers = m_information->m_member->m_type_declaration->m_declaration_modifiers;
    if (m_modifier_indices.size() + 1 < declarationModifiers.size())
        return declarationModifiers[m_modifier_indices.size() + 1].get();

    return nullptr;
}

std::vector<DeclarationModifier*> DeclarationModifierComputations::GetFollowingDeclarationModifiers() const
{
    std::vector<DeclarationModifier*> following;
    const auto& declarationModifiers = m_information->m_member->m_type_declaration->m_declaration_modifiers;

    if (m_modifier_indices.size() + 1 < declarationModifiers.size())
    {
        for (auto i = declarationModifiers.begin() + m_modifier_indices.size() + 1; i != declarationModifiers.end(); ++i)
        {
            following.push_back(i->get());
        }
    }

    return following;
}

std::vector<int> DeclarationModifierComputations::GetArrayIndices() const
{
    return m_modifier_indices;
}

bool DeclarationModifierComputations::IsArray() const
{
    auto* declarationModifier = GetDeclarationModifier();
    return declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::ARRAY;
}

int DeclarationModifierComputations::GetArraySize() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::ARRAY)
    {
        return dynamic_cast<ArrayDeclarationModifier*>(declarationModifier)->m_size;
    }

    return 0;
}

bool DeclarationModifierComputations::HasDynamicArrayCount() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::ARRAY)
    {
        return dynamic_cast<ArrayDeclarationModifier*>(declarationModifier)->m_dynamic_count_evaluation != nullptr;
    }

    return false;
}

const IEvaluation* DeclarationModifierComputations::GetDynamicArrayCountEvaluation() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::ARRAY)
    {
        return dynamic_cast<ArrayDeclarationModifier*>(declarationModifier)->m_dynamic_count_evaluation.get();
    }

    return nullptr;
}

std::vector<DeclarationModifierComputations> DeclarationModifierComputations::GetArrayEntries() const
{
    std::vector<DeclarationModifierComputations> arrayEntries;

    const auto arraySize = GetArraySize();
    for (auto i = 0; i < arraySize; i++)
    {
        std::vector<int> childModifierIndices(m_modifier_indices.size() + 1);
        std::ranges::copy(m_modifier_indices, childModifierIndices.begin());
        childModifierIndices[childModifierIndices.size() - 1] = i;
        arrayEntries.push_back(DeclarationModifierComputations(m_information, std::move(childModifierIndices)));
    }

    return arrayEntries;
}

bool DeclarationModifierComputations::IsSinglePointer() const
{
    auto* declarationModifier = GetDeclarationModifier();

    if (declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::POINTER
        && !dynamic_cast<PointerDeclarationModifier*>(declarationModifier)->CountEvaluationIsArray(m_combined_index))
    {
        const auto following = GetFollowingDeclarationModifiers();

        return !std::ranges::any_of(following,
                                    [](const DeclarationModifier* modifier)
                                    {
                                        return modifier->GetType() == DeclarationModifierType::POINTER;
                                    });
    }

    return false;
}

bool DeclarationModifierComputations::IsArrayPointer() const
{
    auto* declarationModifier = GetDeclarationModifier();

    if (declarationModifier != nullptr && declarationModifier->GetType() == DeclarationModifierType::POINTER
        && dynamic_cast<PointerDeclarationModifier*>(declarationModifier)->CountEvaluationIsArray(m_combined_index))
    {
        const auto following = GetFollowingDeclarationModifiers();

        return !std::ranges::any_of(following,
                                    [](const DeclarationModifier* modifier)
                                    {
                                        return modifier->GetType() == DeclarationModifierType::POINTER;
                                    });
    }

    return false;
}

const IEvaluation* DeclarationModifierComputations::GetArrayPointerCountEvaluation() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier == nullptr || declarationModifier->GetType() != DeclarationModifierType::POINTER)
        return nullptr;

    return dynamic_cast<PointerDeclarationModifier*>(declarationModifier)->GetCountEvaluationForArrayIndex(m_combined_index);
}

bool DeclarationModifierComputations::IsPointerArray() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier == nullptr)
        return false;

    auto* nextDeclarationModifier = GetNextDeclarationModifier();
    if (nextDeclarationModifier == nullptr)
        return false;

    const auto thisDeclModIsArray = declarationModifier->GetType() == DeclarationModifierType::POINTER
                                        && dynamic_cast<PointerDeclarationModifier*>(declarationModifier)->CountEvaluationIsArray(m_combined_index)
                                    || declarationModifier->GetType() == DeclarationModifierType::ARRAY;

    if (!thisDeclModIsArray)
        return false;

    const auto nextDeclModIsSinglePointer = nextDeclarationModifier->GetType() == DeclarationModifierType::POINTER
                                            && !dynamic_cast<PointerDeclarationModifier*>(nextDeclarationModifier)->AnyCountEvaluationIsArray();

    return nextDeclModIsSinglePointer;
}

const IEvaluation* DeclarationModifierComputations::GetPointerArrayCountEvaluation() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier == nullptr)
        return nullptr;

    if (declarationModifier->GetType() == DeclarationModifierType::POINTER)
    {
        return dynamic_cast<PointerDeclarationModifier*>(declarationModifier)->GetCountEvaluationForArrayIndex(m_combined_index);
    }

    if (declarationModifier->GetType() == DeclarationModifierType::ARRAY)
    {
        auto* arrayDeclarationModifier = dynamic_cast<ArrayDeclarationModifier*>(declarationModifier);
        return arrayDeclarationModifier->m_dynamic_count_evaluation.get(); // This might be null in which case there is no evaluation but a static size
    }

    assert(false);
    return nullptr;
}

bool DeclarationModifierComputations::IsDynamicArray() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier == nullptr)
        return false;

    return declarationModifier->GetType() == DeclarationModifierType::ARRAY
           && dynamic_cast<ArrayDeclarationModifier*>(declarationModifier)->m_dynamic_size_evaluation != nullptr;
}

const IEvaluation* DeclarationModifierComputations::GetDynamicArraySizeEvaluation() const
{
    auto* declarationModifier = GetDeclarationModifier();
    if (declarationModifier == nullptr || declarationModifier->GetType() != DeclarationModifierType::ARRAY)
        return nullptr;

    return dynamic_cast<ArrayDeclarationModifier*>(declarationModifier)->m_dynamic_size_evaluation.get();
}

unsigned DeclarationModifierComputations::GetAlignment() const
{
    const auto following = GetFollowingDeclarationModifiers();

    return std::ranges::any_of(following,
                               [](const DeclarationModifier* modifier)
                               {
                                   return modifier->GetType() == DeclarationModifierType::POINTER;
                               })
               ? m_information->m_member->GetAlignment()
               : m_information->m_member->m_type_declaration->m_type->GetAlignment();
}
