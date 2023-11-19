#pragma once

#include "DeclarationModifier.h"
#include "Domain/Evaluation/IEvaluation.h"

#include <memory>
#include <vector>

class PointerDeclarationModifier final : public DeclarationModifier
{
    static const IEvaluation* const DEFAULT_COUNT;

    static bool EvaluationIsArray(const IEvaluation* evaluation);

public:
    std::unique_ptr<IEvaluation> m_count_evaluation;
    std::vector<std::unique_ptr<IEvaluation>> m_count_evaluation_by_array_index;

    _NODISCARD DeclarationModifierType GetType() const override;
    _NODISCARD const IEvaluation* GetCountEvaluation() const;
    _NODISCARD const IEvaluation* GetCountEvaluationForArrayIndex(int index);

    _NODISCARD bool CountEvaluationIsArray() const;
    _NODISCARD bool CountEvaluationIsArray(int index) const;
    _NODISCARD bool AnyCountEvaluationIsArray() const;
};
