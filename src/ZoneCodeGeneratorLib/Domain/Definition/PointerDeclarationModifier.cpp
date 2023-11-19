#include "PointerDeclarationModifier.h"

#include "Domain/Evaluation/OperandStatic.h"

const IEvaluation* const PointerDeclarationModifier::DEFAULT_COUNT = new OperandStatic(1);

DeclarationModifierType PointerDeclarationModifier::GetType() const
{
    return DeclarationModifierType::POINTER;
}

const IEvaluation* PointerDeclarationModifier::GetCountEvaluation() const
{
    if (m_count_evaluation)
        return m_count_evaluation.get();

    return DEFAULT_COUNT;
}

const IEvaluation* PointerDeclarationModifier::GetCountEvaluationForArrayIndex(const int index)
{
    if (index >= 0 && m_count_evaluation_by_array_index.size() > static_cast<unsigned>(index))
    {
        return m_count_evaluation_by_array_index[index].get();
    }

    if (m_count_evaluation)
    {
        return m_count_evaluation.get();
    }

    return DEFAULT_COUNT;
}

bool PointerDeclarationModifier::EvaluationIsArray(const IEvaluation* evaluation)
{
    return !evaluation->IsStatic() || evaluation->EvaluateNumeric() > 1;
}

bool PointerDeclarationModifier::CountEvaluationIsArray() const
{
    if (m_count_evaluation)
    {
        return EvaluationIsArray(m_count_evaluation.get());
    }

    return EvaluationIsArray(DEFAULT_COUNT);
}

bool PointerDeclarationModifier::CountEvaluationIsArray(const int index) const
{
    if (index >= 0 && m_count_evaluation_by_array_index.size() > static_cast<unsigned>(index))
    {
        return EvaluationIsArray(m_count_evaluation_by_array_index[index].get());
    }

    return CountEvaluationIsArray();
}

bool PointerDeclarationModifier::AnyCountEvaluationIsArray() const
{
    if (m_count_evaluation && EvaluationIsArray(m_count_evaluation.get()))
        return true;

    for (const auto& arrayCountEvaluation : m_count_evaluation_by_array_index)
    {
        if (EvaluationIsArray(arrayCountEvaluation.get()))
            return true;
    }

    return EvaluationIsArray(DEFAULT_COUNT);
}
