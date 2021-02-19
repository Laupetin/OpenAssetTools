#include "Operation.h"

Operation::Operation(const OperationType* type)
    : m_operation_type(type)
{
}

Operation::Operation(const OperationType* type, std::unique_ptr<IEvaluation> operand1, std::unique_ptr<IEvaluation> operand2)
    : m_operation_type(type),
      m_operand1(std::move(operand1)),
      m_operand2(std::move(operand2))
{
}

EvaluationType Operation::GetType() const
{
    return EvaluationType::OPERATION;
}

bool Operation::IsStatic() const
{
    return m_operand1->IsStatic() && m_operand2->IsStatic();
}

int Operation::EvaluateNumeric() const
{
    return m_operation_type->m_evaluation_function(m_operand1->EvaluateNumeric(), m_operand2->EvaluateNumeric());
}

bool Operation::Operand1NeedsParenthesis() const
{
    return m_operand1->GetType() == EvaluationType::OPERATION
        && dynamic_cast<Operation*>(m_operand1.get())->m_operation_type->m_precedence > m_operation_type->m_precedence;
}

bool Operation::Operand2NeedsParenthesis() const
{
    return m_operand2->GetType() == EvaluationType::OPERATION
        && dynamic_cast<Operation*>(m_operand2.get())->m_operation_type->m_precedence > m_operation_type->m_precedence;
}
