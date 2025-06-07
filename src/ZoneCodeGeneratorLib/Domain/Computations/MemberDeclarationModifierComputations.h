#pragma once

#include "Domain/Evaluation/IEvaluation.h"
#include "Domain/Information/MemberInformation.h"

#include <vector>

class DeclarationModifierComputations
{
public:
    explicit DeclarationModifierComputations(const MemberInformation* member);

    [[nodiscard]] DeclarationModifier* GetDeclarationModifier() const;
    [[nodiscard]] DeclarationModifier* GetNextDeclarationModifier() const;
    [[nodiscard]] std::vector<DeclarationModifier*> GetAllDeclarationModifiers() const;
    [[nodiscard]] std::vector<DeclarationModifier*> GetFollowingDeclarationModifiers() const;
    [[nodiscard]] const std::vector<int>& GetArrayIndices() const;
    [[nodiscard]] bool IsArray() const;
    [[nodiscard]] int GetArraySize() const;
    [[nodiscard]] bool HasDynamicArrayCount() const;
    [[nodiscard]] const IEvaluation* GetDynamicArrayCountEvaluation() const;
    [[nodiscard]] std::vector<DeclarationModifierComputations> GetArrayEntries() const;
    [[nodiscard]] bool IsSinglePointer() const;
    [[nodiscard]] bool IsArrayPointer() const;
    [[nodiscard]] const IEvaluation* GetArrayPointerCountEvaluation() const;
    [[nodiscard]] bool IsPointerArray() const;
    [[nodiscard]] const IEvaluation* GetPointerArrayCountEvaluation() const;
    [[nodiscard]] bool IsDynamicArray() const;
    [[nodiscard]] const IEvaluation* GetDynamicArraySizeEvaluation() const;
    [[nodiscard]] bool HasPointerModifier() const;
    [[nodiscard]] unsigned GetAlignment() const;

private:
    DeclarationModifierComputations(const MemberInformation* member, std::vector<int> modifierIndices);

    const MemberInformation* const m_information;
    std::vector<int> m_modifier_indices;
    int m_combined_index;
};
