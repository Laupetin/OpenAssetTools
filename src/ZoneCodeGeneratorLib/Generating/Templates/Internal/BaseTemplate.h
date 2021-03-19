#pragma once

#include <ostream>
#include <sstream>

#include "Domain/Computations/MemberDeclarationModifierComputations.h"
#include "Domain/Definition/ArrayDeclarationModifier.h"
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

private:
    static void MakeSafeTypeNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypeVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypePtrVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeArrayIndicesInternal(const DeclarationModifierComputations& modifierComputations, std::ostringstream& str);
    static void MakeOperandStatic(const OperandStatic* op, std::ostringstream& str);
    static void MakeOperandDynamic(const OperandDynamic* op, std::ostringstream& str);
    static void MakeOperation(const Operation* operation, std::ostringstream& str);
    static void MakeEvaluationInternal(const IEvaluation* evaluation, std::ostringstream& str);

protected:
    static std::string Upper(std::string str);
    static std::string Lower(std::string str);
    static std::string MakeTypeVarName(const DataDefinition* def);
    static std::string MakeTypePtrVarName(const DataDefinition* def);
    static std::string MakeSafeTypeName(const DataDefinition* def);
    static std::string MakeMemberAccess(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier);
    static std::string MakeMemberAccess(const std::string& variableName, StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier);
    static std::string MakeTypeDecl(const TypeDeclaration* decl);
    static std::string MakeFollowingReferences(const std::vector<DeclarationModifier*>& modifiers);
    static std::string MakeArrayIndices(const DeclarationModifierComputations& modifierComputations);
    static std::string MakeCustomActionCall(CustomAction* action);
    static std::string MakeArrayCount(const ArrayDeclarationModifier* arrayModifier);
    static std::string MakeEvaluation(const IEvaluation* evaluation);
};

#define LINE(x) {DoIntendation(); m_out << x << "\n";}
#define LINE_START(x) {DoIntendation(); m_out << x;}
#define LINE_MIDDLE(x) {m_out << x;}
#define LINE_END(x) {m_out << x << "\n";}