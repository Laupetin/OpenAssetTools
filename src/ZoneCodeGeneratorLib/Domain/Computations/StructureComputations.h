#pragma once

#include "Domain/Information/StructureInformation.h"

class StructureComputations
{
    const StructureInformation* const m_info;

public:
    explicit StructureComputations(const StructureInformation* structure);

    _NODISCARD bool IsAsset() const;
    _NODISCARD MemberInformation* GetDynamicMember() const;
    _NODISCARD std::vector<MemberInformation*> GetUsedMembers() const;
};
