#pragma once

#include "Domain/Information/MemberInformation.h"

#include <vector>

class MemberComputations
{
public:
    explicit MemberComputations(const MemberInformation* member);

    [[nodiscard]] bool ShouldIgnore() const;
    [[nodiscard]] bool ContainsNonEmbeddedReference() const;
    [[nodiscard]] bool ContainsSinglePointerReference() const;
    [[nodiscard]] bool ContainsArrayPointerReference() const;
    [[nodiscard]] bool ContainsPointerArrayReference() const;
    [[nodiscard]] bool ContainsArrayReference() const;
    [[nodiscard]] const IEvaluation* GetArrayPointerCountEvaluation() const;
    [[nodiscard]] bool IsArray() const;
    [[nodiscard]] std::vector<int> GetArraySizes() const;
    [[nodiscard]] int GetArrayDimension() const;
    [[nodiscard]] bool IsPointerToArray() const;
    [[nodiscard]] std::vector<int> GetPointerToArraySizes() const;
    [[nodiscard]] int GetPointerDepth() const;
    [[nodiscard]] bool IsNotInDefaultNormalBlock() const;
    [[nodiscard]] bool IsInTempBlock() const;
    [[nodiscard]] bool IsInRuntimeBlock() const;
    [[nodiscard]] bool IsFirstUsedMember() const;
    [[nodiscard]] bool IsLastUsedMember() const;
    [[nodiscard]] bool HasDynamicArraySize() const;
    [[nodiscard]] bool IsDynamicMember() const;
    [[nodiscard]] bool IsAfterPartialLoad() const;

private:
    const MemberInformation* const m_info;
};
