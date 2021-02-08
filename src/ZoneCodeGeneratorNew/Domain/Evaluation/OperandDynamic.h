#pragma once
#include <memory>
#include <vector>

#include "IEvaluation.h"
#include "Domain/Information/MemberInformation.h"
#include "Domain/Information/StructureInformation.h"

class OperandDynamic final : public IEvaluation
{
public:
    StructureInformation* const m_structure;
    std::vector<MemberInformation*> m_referenced_member_chain;
    std::vector<std::unique_ptr<IEvaluation>> m_array_indices;

    explicit OperandDynamic(StructureInformation* structure);

    _NODISCARD EvaluationType GetType() const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD int EvaluateNumeric() const override;
};
