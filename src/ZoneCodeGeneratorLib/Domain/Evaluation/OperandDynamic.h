#pragma once
#include "Domain/Information/MemberInformation.h"
#include "Domain/Information/StructureInformation.h"
#include "IEvaluation.h"

#include <memory>
#include <vector>

class OperandDynamic final : public IEvaluation
{
public:
    StructureInformation* const m_structure;
    std::vector<MemberInformation*> m_referenced_member_chain;
    std::vector<std::unique_ptr<IEvaluation>> m_array_indices;

    explicit OperandDynamic(StructureInformation* structure);
    OperandDynamic(StructureInformation* structure,
                   std::vector<MemberInformation*> referencedMemberChain,
                   std::vector<std::unique_ptr<IEvaluation>> arrayIndices);

    _NODISCARD EvaluationType GetType() const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD int EvaluateNumeric() const override;
};
