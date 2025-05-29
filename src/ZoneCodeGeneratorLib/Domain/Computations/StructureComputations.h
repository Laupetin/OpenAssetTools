#pragma once

#include "Domain/Information/StructureInformation.h"

class StructureComputations
{
public:
    explicit StructureComputations(const StructureInformation* structure);

    [[nodiscard]] bool IsAsset() const;
    [[nodiscard]] MemberInformation* GetDynamicMember() const;
    [[nodiscard]] bool HasNonDynamicMember() const;
    [[nodiscard]] std::vector<MemberInformation*> GetUsedMembers() const;

private:
    const StructureInformation* m_info;
};
