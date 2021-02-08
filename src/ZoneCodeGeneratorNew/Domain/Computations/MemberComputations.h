#pragma once

#include "Domain/Information/MemberInformation.h"

class MemberComputations
{
    const MemberInformation* const m_info;

public:
    explicit MemberComputations(const MemberInformation* member);

    _NODISCARD bool ShouldIgnore() const;
    _NODISCARD bool ContainsNonEmbeddedReference() const;
    _NODISCARD bool ContainsSinglePointerReference() const;
    _NODISCARD bool ContainsArrayPointerReference() const;
    _NODISCARD bool ContainsPointerArrayReference() const;
    _NODISCARD bool ContainsArrayReference() const;
    _NODISCARD const IEvaluation* GetArrayPointerCountEvaluation() const;
    _NODISCARD bool IsArray() const;
    _NODISCARD std::vector<int> GetArraySizes() const;
    _NODISCARD int GetArrayDimension() const;
    _NODISCARD bool IsPointerToArray() const;
    _NODISCARD std::vector<int> GetPointerToArraySizes() const;
    _NODISCARD int GetPointerDepth() const;
    _NODISCARD bool IsNotInDefaultNormalBlock() const;
    _NODISCARD bool IsInTempBlock() const;
    _NODISCARD bool IsInRuntimeBlock() const;
    _NODISCARD bool IsFirstMember() const;
    _NODISCARD bool IsLastMember() const;
    _NODISCARD bool HasDynamicArraySize() const;
    _NODISCARD bool IsDynamicMember() const;
    _NODISCARD bool IsAfterPartialLoad() const;
};
