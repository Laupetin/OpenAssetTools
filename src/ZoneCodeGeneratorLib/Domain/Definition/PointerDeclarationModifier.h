#pragma once

#include "DeclarationModifier.h"
#include "Domain/Evaluation/IEvaluation.h"

#include <memory>
#include <vector>

class PointerDeclarationModifier final : public DeclarationModifier
{
public:
    [[nodiscard]] DeclarationModifierType GetType() const override;
    [[nodiscard]] const IEvaluation* GetCountEvaluation() const;
    [[nodiscard]] const IEvaluation* GetCountEvaluationForArrayIndex(int index);

    [[nodiscard]] bool CountEvaluationIsArray() const;
    [[nodiscard]] bool CountEvaluationIsArray(int index) const;
    [[nodiscard]] bool AnyCountEvaluationIsArray() const;

    std::unique_ptr<IEvaluation> m_count_evaluation;
    std::vector<std::unique_ptr<IEvaluation>> m_count_evaluation_by_array_index;

private:
    static const IEvaluation* const DEFAULT_COUNT;
    static bool EvaluationIsArray(const IEvaluation* evaluation);
};
