#pragma once
#include "Domain/Information/MemberInformation.h"
#include "Domain/Information/StructureInformation.h"
#include "IEvaluation.h"

#include <memory>
#include <vector>

class OperandDynamic final : public IEvaluation
{
public:
    explicit OperandDynamic(StructureInformation* structure);
    OperandDynamic(StructureInformation* structure,
                   std::vector<MemberInformation*> referencedMemberChain,
                   std::vector<std::unique_ptr<IEvaluation>> arrayIndices);

    [[nodiscard]] EvaluationType GetType() const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] int EvaluateNumeric() const override;

    StructureInformation* const m_structure;
    std::vector<MemberInformation*> m_referenced_member_chain;
    std::vector<std::unique_ptr<IEvaluation>> m_array_indices;
};
