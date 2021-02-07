#include "Operation.h"

Operation::Operation(const OperationType* type)
    : m_operation_type(type)
{
}

EvaluationType Operation::GetType()
{
    return EvaluationType::OPERATION;
}

bool Operation::IsStatic()
{
    return m_operand1->IsStatic() && m_operand2->IsStatic();
}

int Operation::EvaluateNumeric()
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
