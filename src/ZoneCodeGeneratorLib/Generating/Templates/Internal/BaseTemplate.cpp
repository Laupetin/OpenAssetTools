#include "BaseTemplate.h"

#include <sstream>

#include "Domain/Definition/ArrayDeclarationModifier.h"

BaseTemplate::BaseTemplate(std::ostream& stream, RenderingContext* context)
    : m_out(stream),
      m_env(*context),
      m_intendation(0u)
{
}

void BaseTemplate::DoIntendation() const
{
    for (auto i = 0u; i < m_intendation; i++)
        m_out << INTENDATION;
}

std::string BaseTemplate::Upper(std::string str)
{
    for (auto& c : str)
        c = static_cast<char>(toupper(c));

    return str;
}

std::string BaseTemplate::Lower(std::string str)
{
    for (auto& c : str)
        c = static_cast<char>(tolower(c));

    return str;
}

std::string BaseTemplate::TypeVarName(const DataDefinition* def)
{
    std::ostringstream str;
    str << "var" << SafeTypeName(def);
    return str.str();
}

std::string BaseTemplate::TypePtrVarName(const DataDefinition* def)
{
    std::ostringstream str;
    str << "var" << SafeTypeName(def) << "Ptr";
    return str.str();
}

std::string BaseTemplate::SafeTypeName(const DataDefinition* def)
{
    auto safeName(def->m_name);

    for (auto& c : safeName)
    {
        if (isspace(c))
            c = '_';
    }

    return safeName;
}

void BaseTemplate::TypeDecl(const TypeDeclaration* decl) const
{
    if (decl->m_is_const)
    {
        LINE_MIDDLE("const ")
    }
    LINE_MIDDLE(decl->m_type->GetFullName())
}

void BaseTemplate::PrintFollowingReferences(const std::vector<std::unique_ptr<DeclarationModifier>>& modifiers) const
{
    for (const auto& modifier : modifiers)
    {
        if (modifier->GetType() == DeclarationModifierType::ARRAY)
        {
            const auto* array = dynamic_cast<const ArrayDeclarationModifier*>(modifier.get());
            LINE_MIDDLE("["<< array->m_size <<"]")
        }
        else
        {
            LINE_MIDDLE("*")
        }
    }
}

void BaseTemplate::PrintArrayIndices(const DeclarationModifierComputations& modifierComputations) const
{
    for (auto index : modifierComputations.GetArrayIndices())
    {
        LINE_MIDDLE("["<<index<<"]")
    }
}

void BaseTemplate::PrintCustomAction(CustomAction* action) const
{
    LINE_START("m_actions." << action->m_action_name << "(")

    auto first = true;
    for (auto* def : action->m_parameter_types)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            LINE_MIDDLE(", ")
        }

        LINE_MIDDLE(TypeVarName(def))
    }

    LINE_END(");")
}

void BaseTemplate::PrintOperandStatic(const OperandStatic* op) const
{
    if (op->m_enum_member != nullptr)
    {
        LINE_MIDDLE(op->m_enum_member->m_name)
    }
    else
    {
        LINE_MIDDLE(op->m_value)
    }
}

void BaseTemplate::PrintOperandDynamic(const OperandDynamic* op) const
{
    LINE_MIDDLE(TypeVarName(op->m_structure->m_definition))

    auto first = true;
    for (const auto* chainMember : op->m_referenced_member_chain)
    {
        if (first)
        {
            first = false;
            LINE_MIDDLE("->"<< chainMember->m_member->m_name)
        }
        else
        {
            LINE_MIDDLE("." << chainMember->m_member->m_name)
        }
    }

    for (const auto& arrayIndex : op->m_array_indices)
    {
        LINE_MIDDLE("[")
        PrintEvaluation(arrayIndex.get());
        LINE_MIDDLE("]")
    }
}

void BaseTemplate::PrintOperation(const Operation* operation) const
{
    if (operation->Operand1NeedsParenthesis())
    {
        LINE_MIDDLE("(")
        PrintEvaluation(operation->m_operand1.get());
        LINE_MIDDLE(")")
    }
    else
        PrintEvaluation(operation->m_operand1.get());

    LINE_MIDDLE(" "<<operation->m_operation_type->m_syntax<<" ")

    if (operation->Operand2NeedsParenthesis())
    {
        LINE_MIDDLE("(")
        PrintEvaluation(operation->m_operand2.get());
        LINE_MIDDLE(")")
    }
    else
        PrintEvaluation(operation->m_operand2.get());
}

void BaseTemplate::PrintEvaluation(const IEvaluation* evaluation) const
{
    if (evaluation->GetType() == EvaluationType::OPERATION)
        PrintOperation(dynamic_cast<const Operation*>(evaluation));
    else if (evaluation->GetType() == EvaluationType::OPERAND_STATIC)
        PrintOperandStatic(dynamic_cast<const OperandStatic*>(evaluation));
    else if (evaluation->GetType() == EvaluationType::OPERAND_DYNAMIC)
        PrintOperandDynamic(dynamic_cast<const OperandDynamic*>(evaluation));
}
