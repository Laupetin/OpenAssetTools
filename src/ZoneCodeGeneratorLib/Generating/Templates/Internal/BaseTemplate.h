#pragma once

#include "Domain/Computations/MemberDeclarationModifierComputations.h"
#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Domain/Evaluation/OperandDynamic.h"
#include "Domain/Evaluation/OperandStatic.h"
#include "Domain/Evaluation/Operation.h"
#include "Generating/RenderingContext.h"

#include <format>
#include <ostream>

class BaseTemplate
{
protected:
    static constexpr auto INTENDATION = "    ";

    BaseTemplate(std::ostream& stream, const RenderingContext& context);

    void DoIntendation() const;

    static std::string Upper(std::string str);
    static std::string Lower(std::string str);
    static std::string MakeTypeVarName(const DataDefinition* def);
    static std::string MakeTypeWrittenVarName(const DataDefinition* def);
    static std::string MakeTypePtrVarName(const DataDefinition* def);
    static std::string MakeTypeWrittenPtrVarName(const DataDefinition* def);
    static std::string MakeSafeTypeName(const DataDefinition* def);
    static std::string MakeMemberAccess(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier);
    static std::string
        MakeWrittenMemberAccess(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier);
    static std::string MakeTypeDecl(const TypeDeclaration* decl);
    static std::string MakeFollowingReferences(const std::vector<DeclarationModifier*>& modifiers);
    static std::string MakeArrayIndices(const DeclarationModifierComputations& modifierComputations);
    static std::string MakeCustomActionCall(const CustomAction* action);
    static std::string MakeArrayCount(const ArrayDeclarationModifier* arrayModifier);
    static std::string MakeEvaluation(const IEvaluation* evaluation);

    std::ostream& m_out;
    const RenderingContext& m_env;
    unsigned m_intendation;

private:
    static void MakeSafeTypeNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypeVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypeWrittenVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypePtrVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeTypeWrittenPtrVarNameInternal(const DataDefinition* def, std::ostringstream& str);
    static void MakeArrayIndicesInternal(const DeclarationModifierComputations& modifierComputations, std::ostringstream& str);
    static void MakeOperandStatic(const OperandStatic* op, std::ostringstream& str);
    static void MakeOperandDynamic(const OperandDynamic* op, std::ostringstream& str);
    static void MakeOperation(const Operation* operation, std::ostringstream& str);
    static void MakeEvaluationInternal(const IEvaluation* evaluation, std::ostringstream& str);
};

#define LINE(x)                                                                                                                                                \
    {                                                                                                                                                          \
        DoIntendation();                                                                                                                                       \
        m_out << x << "\n";                                                                                                                                    \
    }
#define LINEF(...)                                                                                                                                             \
    {                                                                                                                                                          \
        DoIntendation();                                                                                                                                       \
        m_out << std::format(__VA_ARGS__) << "\n";                                                                                                             \
    }
#define LINE_START(x)                                                                                                                                          \
    {                                                                                                                                                          \
        DoIntendation();                                                                                                                                       \
        m_out << x;                                                                                                                                            \
    }
#define LINE_STARTF(...)                                                                                                                                       \
    {                                                                                                                                                          \
        DoIntendation();                                                                                                                                       \
        m_out << std::format(__VA_ARGS__);                                                                                                                     \
    }
#define LINE_MIDDLE(x)                                                                                                                                         \
    {                                                                                                                                                          \
        m_out << x;                                                                                                                                            \
    }
#define LINE_MIDDLEF(...)                                                                                                                                      \
    {                                                                                                                                                          \
        m_out << std::format(__VA_ARGS__);                                                                                                                     \
    }
#define LINE_END(x)                                                                                                                                            \
    {                                                                                                                                                          \
        m_out << x << "\n";                                                                                                                                    \
    }
#define LINE_ENDF(...)                                                                                                                                         \
    {                                                                                                                                                          \
        m_out << std::format(__VA_ARGS__) << "\n";                                                                                                             \
    }
