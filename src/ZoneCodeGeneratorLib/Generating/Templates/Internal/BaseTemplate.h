#pragma once

#include <ostream>

#include "Domain/Computations/MemberDeclarationModifierComputations.h"
#include "Domain/Evaluation/OperandDynamic.h"
#include "Domain/Evaluation/OperandStatic.h"
#include "Domain/Evaluation/Operation.h"
#include "Generating/RenderingContext.h"

class BaseTemplate
{
protected:
    static constexpr const char* INTENDATION = "    ";

    std::ostream& m_out;
    RenderingContext& m_env;
    unsigned m_intendation;

    BaseTemplate(std::ostream& stream, RenderingContext* context);

    void DoIntendation() const;

    static std::string Upper(std::string str);
    static std::string Lower(std::string str);
    static std::string TypeVarName(const DataDefinition* def);
    static std::string TypePtrVarName(const DataDefinition* def);
    static std::string SafeTypeName(const DataDefinition* def);
    void TypeDecl(const TypeDeclaration* decl) const;
    void PrintFollowingReferences(const std::vector<std::unique_ptr<DeclarationModifier>>& modifiers) const;
    void PrintArrayIndices(const DeclarationModifierComputations& modifierComputations) const;
    
    void PrintOperandStatic(const OperandStatic* op) const;
    void PrintOperandDynamic(const OperandDynamic* op) const;
    void PrintOperation(const Operation* operation) const;
    void PrintEvaluation(const IEvaluation* evaluation) const;
};

#define LINE(x) {DoIntendation(); m_out << x << "\n";}
#define LINE_START(x) {DoIntendation(); m_out << x;}
#define LINE_MIDDLE(x) {m_out << x;}
#define LINE_END(x) {m_out << x << "\n";}