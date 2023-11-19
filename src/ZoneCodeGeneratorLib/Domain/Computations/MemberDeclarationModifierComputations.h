#pragma once

#include "Domain/Evaluation/IEvaluation.h"
#include "Domain/Information/MemberInformation.h"
#include "Utils/ClassUtils.h"

class DeclarationModifierComputations
{
    const MemberInformation* const m_information;
    std::vector<int> m_modifier_indices;
    int m_combined_index;

    DeclarationModifierComputations(const MemberInformation* member, std::vector<int> modifierIndices);

public:
    explicit DeclarationModifierComputations(const MemberInformation* member);

    _NODISCARD DeclarationModifier* GetDeclarationModifier() const;
    _NODISCARD DeclarationModifier* GetNextDeclarationModifier() const;
    _NODISCARD std::vector<DeclarationModifier*> GetFollowingDeclarationModifiers() const;
    _NODISCARD std::vector<int> GetArrayIndices() const;
    _NODISCARD bool IsArray() const;
    _NODISCARD int GetArraySize() const;
    _NODISCARD bool HasDynamicArrayCount() const;
    _NODISCARD const IEvaluation* GetDynamicArrayCountEvaluation() const;
    _NODISCARD std::vector<DeclarationModifierComputations> GetArrayEntries() const;
    _NODISCARD bool IsSinglePointer() const;
    _NODISCARD bool IsArrayPointer() const;
    _NODISCARD const IEvaluation* GetArrayPointerCountEvaluation() const;
    _NODISCARD bool IsPointerArray() const;
    _NODISCARD const IEvaluation* GetPointerArrayCountEvaluation() const;
    _NODISCARD bool IsDynamicArray() const;
    _NODISCARD const IEvaluation* GetDynamicArraySizeEvaluation() const;
    _NODISCARD unsigned GetAlignment() const;
};
