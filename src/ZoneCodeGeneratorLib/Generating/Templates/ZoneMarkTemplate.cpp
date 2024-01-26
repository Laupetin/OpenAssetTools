#include "ZoneMarkTemplate.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Internal/BaseTemplate.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <iostream>
#include <sstream>

class ZoneMarkTemplate::Internal final : BaseTemplate
{
    enum class MemberLoadType
    {
        ARRAY_POINTER,
        DYNAMIC_ARRAY,
        EMBEDDED,
        EMBEDDED_ARRAY,
        POINTER_ARRAY,
        SINGLE_POINTER
    };

    static std::string MarkerClassName(StructureInformation* asset)
    {
        std::ostringstream str;
        str << "Marker_" << asset->m_definition->m_name;
        return str.str();
    }

    static std::string VariableDecl(const DataDefinition* def)
    {
        std::ostringstream str;
        str << def->GetFullName() << "* var" << MakeSafeTypeName(def) << ";";
        return str.str();
    }

    static std::string PointerVariableDecl(const DataDefinition* def)
    {
        std::ostringstream str;
        str << def->GetFullName() << "** var" << MakeSafeTypeName(def) << "Ptr;";
        return str.str();
    }

    void PrintHeaderPtrArrayMarkMethodDeclaration(const DataDefinition* def) const
    {
        LINE("void MarkPtrArray_" << MakeSafeTypeName(def) << "(size_t count);")
    }

    void PrintHeaderArrayMarkMethodDeclaration(const DataDefinition* def) const
    {
        LINE("void MarkArray_" << MakeSafeTypeName(def) << "(size_t count);")
    }

    void PrintHeaderMarkMethodDeclaration(const StructureInformation* info) const
    {
        LINE("void Mark_" << MakeSafeTypeName(info->m_definition) << "();")
    }

    void PrintHeaderGetAssetInfoMethodDeclaration(const StructureInformation* info) const
    {
        LINE("XAssetInfo<" << info->m_definition->GetFullName() << ">* GetAssetInfo(" << info->m_definition->GetFullName() << "* pAsset) const;")
    }

    void PrintHeaderGetNameMethodDeclaration(const StructureInformation* info) const
    {
        LINE("static std::string GetAssetName(" << info->m_definition->GetFullName() << "* pAsset);")
    }

    void PrintHeaderConstructor() const
    {
        LINE(MarkerClassName(m_env.m_asset) << "(Zone* zone);")
    }

    void PrintHeaderMainMarkMethodDeclaration(const StructureInformation* info) const
    {
        LINE("void Mark(" << info->m_definition->GetFullName() << "* pAsset);")
    }

    void PrintVariableInitialization(const DataDefinition* def) const
    {
        LINE("var" << def->m_name << " = nullptr;")
    }

    void PrintPointerVariableInitialization(const DataDefinition* def) const
    {
        LINE("var" << def->m_name << "Ptr = nullptr;")
    }

    void PrintConstructorMethod()
    {
        LINE(MarkerClassName(m_env.m_asset) << "::" << MarkerClassName(m_env.m_asset) << "(Zone* zone)")

        m_intendation++;
        LINE(": AssetMarker(" << m_env.m_asset->m_asset_enum_entry->m_name << ", zone)")
        m_intendation--;

        LINE("{")
        m_intendation++;

        PrintVariableInitialization(m_env.m_asset->m_definition);
        PrintPointerVariableInitialization(m_env.m_asset->m_definition);
        LINE("")

        for (const auto* type : m_env.m_used_types)
        {
            if (type->m_info && !type->m_info->m_definition->m_anonymous && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                PrintVariableInitialization(type->m_type);
            }
        }
        for (const auto* type : m_env.m_used_types)
        {
            if (type->m_info && type->m_pointer_array_reference_exists && !type->m_is_context_asset)
            {
                PrintPointerVariableInitialization(type->m_type);
            }
        }

        m_intendation--;
        LINE("}")
    }

    void PrintMarkPtrArrayMethod_Loading(const DataDefinition* def, const StructureInformation* info) const
    {
        if (info && !info->m_is_leaf)
        {
            LINE(MakeTypeVarName(info->m_definition) << " = *" << MakeTypePtrVarName(def) << ";")
            LINE("Mark_" << MakeSafeTypeName(def) << "();")
        }
    }

    void PrintMarkPtrArrayMethod_PointerCheck(const DataDefinition* def, StructureInformation* info, const bool reusable)
    {
        LINE("if (*" << MakeTypePtrVarName(def) << ")")
        LINE("{")
        m_intendation++;

        if (info && StructureComputations(info).IsAsset())
        {
            LINE("AddDependency(" << MarkerClassName(info) << "(m_zone).GetAssetInfo(*" << MakeTypePtrVarName(def) << "));")
        }
        else
        {
            PrintMarkPtrArrayMethod_Loading(def, info);
        }

        m_intendation--;
        LINE("}")
    }

    void PrintMarkPtrArrayMethod(const DataDefinition* def, StructureInformation* info, const bool reusable)
    {
        LINE("void " << MarkerClassName(m_env.m_asset) << "::MarkPtrArray_" << MakeSafeTypeName(def) << "(const size_t count)")
        LINE("{")
        m_intendation++;

        LINE("assert(" << MakeTypePtrVarName(def) << " != nullptr);")
        LINE("")

        LINE(def->GetFullName() << "** var = " << MakeTypePtrVarName(def) << ";")
        LINE("for(size_t index = 0; index < count; index++)")
        LINE("{")
        m_intendation++;

        LINE(MakeTypePtrVarName(def) << " = var;")
        PrintMarkPtrArrayMethod_PointerCheck(def, info, reusable);
        LINE("")
        LINE("var++;")

        m_intendation--;
        LINE("}")
        m_intendation--;
        LINE("}")
    }

    void PrintMarkArrayMethod(const DataDefinition* def, const StructureInformation* info)
    {
        LINE("void " << MarkerClassName(m_env.m_asset) << "::MarkArray_" << MakeSafeTypeName(def) << "(const size_t count)")
        LINE("{")
        m_intendation++;

        LINE("assert(" << MakeTypeVarName(def) << " != nullptr);")
        LINE("")

        LINE(def->GetFullName() << "* var = " << MakeTypeVarName(def) << ";")
        LINE("for(size_t index = 0; index < count; index++)")
        LINE("{")
        m_intendation++;

        LINE(MakeTypeVarName(info->m_definition) << " = var;")
        LINE("Mark_" << info->m_definition->m_name << "();")
        LINE("var++;")

        m_intendation--;
        LINE("}")

        m_intendation--;
        LINE("}")
    }

    void MarkMember_ScriptString(StructureInformation* info,
                                 MemberInformation* member,
                                 const DeclarationModifierComputations& modifier,
                                 const MemberLoadType loadType) const
    {
        if (loadType == MemberLoadType::ARRAY_POINTER)
        {
            LINE("MarkArray_ScriptString(" << MakeMemberAccess(info, member, modifier) << ", " << MakeEvaluation(modifier.GetArrayPointerCountEvaluation())
                                           << ");")
        }
        else if (loadType == MemberLoadType::EMBEDDED_ARRAY)
        {
            LINE("MarkArray_ScriptString(" << MakeMemberAccess(info, member, modifier) << ", "
                                           << MakeArrayCount(dynamic_cast<ArrayDeclarationModifier*>(modifier.GetDeclarationModifier())) << ");")
        }
        else if (loadType == MemberLoadType::EMBEDDED)
        {
            LINE("Mark_ScriptString(" << MakeMemberAccess(info, member, modifier) << ");")
        }
        else
        {
            assert(false);
            LINE("#error unsupported loadType " << static_cast<int>(loadType) << " for scriptstring")
        }
    }

    void MarkMember_Asset(StructureInformation* info,
                          MemberInformation* member,
                          const DeclarationModifierComputations& modifier,
                          const MemberLoadType loadType) const
    {
        if (loadType == MemberLoadType::SINGLE_POINTER)
        {
            LINE("AddDependency(" << MarkerClassName(member->m_type) << "(m_zone).GetAssetInfo(" << MakeMemberAccess(info, member, modifier) << "));")
        }
        else if (loadType == MemberLoadType::POINTER_ARRAY)
        {
            MarkMember_PointerArray(info, member, modifier);
        }
        else
        {
            assert(false);
            LINE("#error unsupported loadType " << static_cast<int>(loadType) << " for asset")
        }
    }

    void MarkMember_ArrayPointer(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        LINE(MakeTypeVarName(member->m_member->m_type_declaration->m_type) << " = " << MakeMemberAccess(info, member, modifier) << ";")
        LINE("MarkArray_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "(" << MakeEvaluation(modifier.GetArrayPointerCountEvaluation())
                          << ");")
    }

    void MarkMember_PointerArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        LINE(MakeTypePtrVarName(member->m_member->m_type_declaration->m_type) << " = " << MakeMemberAccess(info, member, modifier) << ";")
        if (modifier.IsArray())
        {
            LINE("MarkPtrArray_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "(" << modifier.GetArraySize() << ");")
        }
        else
        {
            LINE("MarkPtrArray_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "("
                                 << MakeEvaluation(modifier.GetPointerArrayCountEvaluation()) << ");")
        }
    }

    void MarkMember_EmbeddedArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        const MemberComputations computations(member);
        std::string arraySizeStr;

        if (modifier.HasDynamicArrayCount())
            arraySizeStr = MakeEvaluation(modifier.GetDynamicArrayCountEvaluation());
        else
            arraySizeStr = std::to_string(modifier.GetArraySize());

        LINE(MakeTypeVarName(member->m_member->m_type_declaration->m_type) << " = " << MakeMemberAccess(info, member, modifier) << ";")
        LINE("MarkArray_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "(" << arraySizeStr << ");")
    }

    void MarkMember_DynamicArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        LINE(MakeTypeVarName(member->m_member->m_type_declaration->m_type) << " = " << MakeMemberAccess(info, member, modifier) << ";")
        LINE("MarkArray_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "(" << MakeEvaluation(modifier.GetDynamicArraySizeEvaluation())
                          << ");")
    }

    void MarkMember_Embedded(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        LINE(MakeTypeVarName(member->m_member->m_type_declaration->m_type) << " = &" << MakeMemberAccess(info, member, modifier) << ";")
        LINE("Mark_" << MakeSafeTypeName(member->m_member->m_type_declaration->m_type) << "();")
    }

    void MarkMember_SinglePointer(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier) const
    {
        LINE(MakeTypeVarName(member->m_member->m_type_declaration->m_type) << " = " << MakeMemberAccess(info, member, modifier) << ";")
        LINE("Mark_" << MakeSafeTypeName(member->m_type->m_definition) << "();")
    }

    void MarkMember_TypeCheck(StructureInformation* info,
                              MemberInformation* member,
                              const DeclarationModifierComputations& modifier,
                              const MemberLoadType loadType) const
    {
        if (member->m_is_script_string)
        {
            MarkMember_ScriptString(info, member, modifier, loadType);
        }
        else if (member->m_type && StructureComputations(member->m_type).IsAsset())
        {
            MarkMember_Asset(info, member, modifier, loadType);
        }
        else
        {
            switch (loadType)
            {
            case MemberLoadType::ARRAY_POINTER:
                MarkMember_ArrayPointer(info, member, modifier);
                break;

            case MemberLoadType::SINGLE_POINTER:
                MarkMember_SinglePointer(info, member, modifier);
                break;

            case MemberLoadType::EMBEDDED:
                MarkMember_Embedded(info, member, modifier);
                break;

            case MemberLoadType::POINTER_ARRAY:
                MarkMember_PointerArray(info, member, modifier);
                break;

            case MemberLoadType::DYNAMIC_ARRAY:
                MarkMember_DynamicArray(info, member, modifier);
                break;

            case MemberLoadType::EMBEDDED_ARRAY:
                MarkMember_EmbeddedArray(info, member, modifier);
                break;

            default:
                LINE("// t=" << static_cast<int>(loadType))
                break;
            }
        }
    }

    static bool MarkMember_ShouldMakePointerCheck(StructureInformation* info,
                                                  MemberInformation* member,
                                                  const DeclarationModifierComputations& modifier,
                                                  MemberLoadType loadType)
    {
        if (loadType != MemberLoadType::ARRAY_POINTER && loadType != MemberLoadType::POINTER_ARRAY && loadType != MemberLoadType::SINGLE_POINTER)
        {
            return false;
        }

        if (loadType == MemberLoadType::POINTER_ARRAY)
        {
            return !modifier.IsArray();
        }

        if (member->m_is_string)
        {
            return false;
        }

        return true;
    }

    void MarkMember_PointerCheck(StructureInformation* info,
                                 MemberInformation* member,
                                 const DeclarationModifierComputations& modifier,
                                 const MemberLoadType loadType)
    {
        if (MarkMember_ShouldMakePointerCheck(info, member, modifier, loadType))
        {
            LINE("if (" << MakeMemberAccess(info, member, modifier) << ")")
            LINE("{")
            m_intendation++;

            MarkMember_TypeCheck(info, member, modifier, loadType);

            m_intendation--;
            LINE("}")
        }
        else
        {
            MarkMember_TypeCheck(info, member, modifier, loadType);
        }
    }

    void MarkMember_ReferenceArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        auto first = true;
        for (const auto& entry : modifier.GetArrayEntries())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                LINE("")
            }

            MarkMember_Reference(info, member, entry);
        }
    }

    void MarkMember_Reference(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        if (modifier.IsDynamicArray())
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::DYNAMIC_ARRAY);
        }
        else if (modifier.IsSinglePointer())
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::SINGLE_POINTER);
        }
        else if (modifier.IsArrayPointer())
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::ARRAY_POINTER);
        }
        else if (modifier.IsPointerArray())
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::POINTER_ARRAY);
        }
        else if (modifier.IsArray() && modifier.GetNextDeclarationModifier() == nullptr)
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::EMBEDDED_ARRAY);
        }
        else if (modifier.GetDeclarationModifier() == nullptr)
        {
            MarkMember_PointerCheck(info, member, modifier, MemberLoadType::EMBEDDED);
        }
        else if (modifier.IsArray())
        {
            MarkMember_ReferenceArray(info, member, modifier);
        }
        else
        {
            assert(false);
            LINE("#error MarkMemberReference failed @ " << member->m_member->m_name)
        }
    }

    void MarkMember_Condition_Struct(StructureInformation* info, MemberInformation* member)
    {
        LINE("")
        if (member->m_condition)
        {
            LINE("if (" << MakeEvaluation(member->m_condition.get()) << ")")
            LINE("{")
            m_intendation++;

            MarkMember_Reference(info, member, DeclarationModifierComputations(member));

            m_intendation--;
            LINE("}")
        }
        else
        {
            MarkMember_Reference(info, member, DeclarationModifierComputations(member));
        }
    }

    void MarkMember_Condition_Union(StructureInformation* info, MemberInformation* member)
    {
        const MemberComputations computations(member);

        if (computations.IsFirstMember())
        {
            LINE("")
            if (member->m_condition)
            {
                LINE("if (" << MakeEvaluation(member->m_condition.get()) << ")")
                LINE("{")
                m_intendation++;

                MarkMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                MarkMember_Reference(info, member, DeclarationModifierComputations(member));
            }
        }
        else if (computations.IsLastMember())
        {
            if (member->m_condition)
            {
                LINE("else if (" << MakeEvaluation(member->m_condition.get()) << ")")
                LINE("{")
                m_intendation++;

                MarkMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINE("else")
                LINE("{")
                m_intendation++;

                MarkMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
        }
        else
        {
            if (member->m_condition)
            {
                LINE("else if (" << MakeEvaluation(member->m_condition.get()) << ")")
                LINE("{")
                m_intendation++;

                MarkMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINE("#error Middle member of union must have condition (" << member->m_member->m_name << ")")
            }
        }
    }

    void PrintMarkMemberIfNeedsTreatment(StructureInformation* info, MemberInformation* member)
    {
        const MemberComputations computations(member);
        if (computations.ShouldIgnore() || computations.IsInRuntimeBlock())
            return;

        if (member->m_is_script_string || member->m_type && (member->m_type->m_requires_marking || StructureComputations(member->m_type).IsAsset()))
        {
            if (info->m_definition->GetType() == DataDefinitionType::UNION)
                MarkMember_Condition_Union(info, member);
            else
                MarkMember_Condition_Struct(info, member);
        }
    }

    void PrintMarkMethod(StructureInformation* info)
    {
        const StructureComputations computations(info);
        LINE("void " << MarkerClassName(m_env.m_asset) << "::Mark_" << info->m_definition->m_name << "()")
        LINE("{")
        m_intendation++;

        LINE("assert(" << MakeTypeVarName(info->m_definition) << " != nullptr);")

        for (const auto& member : info->m_ordered_members)
        {
            PrintMarkMemberIfNeedsTreatment(info, member.get());
        }

        m_intendation--;
        LINE("}")
    }

    void PrintGetNameMethod()
    {
        LINE("std::string " << MarkerClassName(m_env.m_asset) << "::GetAssetName(" << m_env.m_asset->m_definition->GetFullName() << "* pAsset)")
        LINE("{")
        m_intendation++;

        if (!m_env.m_asset->m_name_chain.empty())
        {
            LINE_START("return pAsset")

            auto first = true;
            for (auto* member : m_env.m_asset->m_name_chain)
            {
                if (first)
                {
                    first = false;
                    LINE_MIDDLE("->" << member->m_member->m_name)
                }
                else
                {
                    LINE_MIDDLE("." << member->m_member->m_name)
                }
            }
            LINE_END(";")
        }
        else
        {
            LINE("return \"" << m_env.m_asset->m_definition->m_name << "\";")
        }

        m_intendation--;
        LINE("}")
    }

    void PrintGetAssetInfoMethod()
    {
        LINE("XAssetInfo<" << m_env.m_asset->m_definition->GetFullName() << ">* " << MarkerClassName(m_env.m_asset) << "::GetAssetInfo("
                           << m_env.m_asset->m_definition->GetFullName() << "* pAsset) const")
        LINE("{")
        m_intendation++;

        LINE("return reinterpret_cast<XAssetInfo<" << m_env.m_asset->m_definition->GetFullName() << ">*>(GetAssetInfoByName(GetAssetName(pAsset)));")

        m_intendation--;
        LINE("}")
    }

    void PrintMainMarkMethod()
    {
        LINE("void " << MarkerClassName(m_env.m_asset) << "::Mark(" << m_env.m_asset->m_definition->GetFullName() << "* pAsset)")
        LINE("{")
        m_intendation++;

        LINE("assert(pAsset != nullptr);")
        LINE("")
        LINE(MakeTypeVarName(m_env.m_asset->m_definition) << " = pAsset;")
        LINE("Mark_" << MakeSafeTypeName(m_env.m_asset->m_definition) << "();")

        m_intendation--;
        LINE("}")
    }

public:
    Internal(std::ostream& stream, RenderingContext* context)
        : BaseTemplate(stream, context)
    {
    }

    void Header()
    {
        LINE("// ====================================================================")
        LINE("// This file has been generated by ZoneCodeGenerator.")
        LINE("// Do not modify. ")
        LINE("// Any changes will be discarded when regenerating.")
        LINE("// ====================================================================")
        LINE("")
        LINE("#pragma once")
        LINE("")
        LINE("#include \"Loading/AssetMarker.h\"")
        LINE("#include \"Game/" << m_env.m_game << "/" << m_env.m_game << ".h\"")
        LINE("#include <string>")
        LINE("")
        LINE("namespace " << m_env.m_game)
        LINE("{")
        m_intendation++;
        LINE("class " << MarkerClassName(m_env.m_asset) << " final : public AssetMarker")
        LINE("{")
        m_intendation++;

        LINE(VariableDecl(m_env.m_asset->m_definition))
        LINE(PointerVariableDecl(m_env.m_asset->m_definition))
        LINE("")

        m_intendation--;
        LINE("public:")
        m_intendation++;

        // Variable Declarations: type varType;
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_info && !type->m_info->m_definition->m_anonymous && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                LINE(VariableDecl(type->m_type))
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists && !type->m_is_context_asset)
            {
                LINE(PointerVariableDecl(type->m_type))
            }
        }

        LINE("")

        // Method Declarations
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists && type->m_info->m_requires_marking)
            {
                PrintHeaderPtrArrayMarkMethodDeclaration(type->m_type);
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_info->m_requires_marking
                && type->m_non_runtime_reference_exists)
            {
                PrintHeaderArrayMarkMethodDeclaration(type->m_type);
            }
        }
        for (const auto* type : m_env.m_used_structures)
        {
            if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && type->m_info->m_requires_marking
                && !StructureComputations(type->m_info).IsAsset())
            {
                PrintHeaderMarkMethodDeclaration(type->m_info);
            }
        }
        PrintHeaderMarkMethodDeclaration(m_env.m_asset);
        LINE("")
        PrintHeaderGetNameMethodDeclaration(m_env.m_asset);
        PrintHeaderGetAssetInfoMethodDeclaration(m_env.m_asset);
        LINE("")
        PrintHeaderConstructor();
        PrintHeaderMainMarkMethodDeclaration(m_env.m_asset);

        m_intendation--;
        LINE("};")
        m_intendation--;
        LINE("}")
    }

    void Source()
    {
        LINE("// ====================================================================")
        LINE("// This file has been generated by ZoneCodeGenerator.")
        LINE("// Do not modify. ")
        LINE("// Any changes will be discarded when regenerating.")
        LINE("// ====================================================================")
        LINE("")
        LINE("#include \"" << Lower(m_env.m_asset->m_definition->m_name) << "_mark_db.h\"")
        LINE("#include <cassert>")
        LINE("")

        if (!m_env.m_referenced_assets.empty())
        {
            LINE("// Referenced Assets:")
            for (const auto* type : m_env.m_referenced_assets)
            {
                LINE("#include \"../" << Lower(type->m_type->m_name) << "/" << Lower(type->m_type->m_name) << "_mark_db.h\"")
            }
            LINE("")
        }
        LINE("using namespace " << m_env.m_game << ";")
        LINE("")
        PrintConstructorMethod();

        for (const auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists && type->m_info->m_requires_marking)
            {
                LINE("")
                PrintMarkPtrArrayMethod(type->m_type, type->m_info, type->m_pointer_array_reference_is_reusable);
            }
        }
        for (const auto* type : m_env.m_used_types)
        {
            if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_info->m_requires_marking
                && type->m_non_runtime_reference_exists)
            {
                LINE("")
                PrintMarkArrayMethod(type->m_type, type->m_info);
            }
        }
        for (const auto* type : m_env.m_used_structures)
        {
            if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && type->m_info->m_requires_marking
                && !StructureComputations(type->m_info).IsAsset())
            {
                LINE("")
                PrintMarkMethod(type->m_info);
            }
        }
        LINE("")
        PrintMarkMethod(m_env.m_asset);
        LINE("")
        PrintMainMarkMethod();
        LINE("")
        PrintGetNameMethod();
        PrintGetAssetInfoMethod();
    }
};

std::vector<CodeTemplateFile> ZoneMarkTemplate::GetFilesToRender(RenderingContext* context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context->m_asset->m_definition->m_name;
    utils::MakeStringLowerCase(assetName);

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_mark_db.h";
        files.emplace_back(str.str(), TAG_HEADER);
    }

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_mark_db.cpp";
        files.emplace_back(str.str(), TAG_SOURCE);
    }

    return files;
}

void ZoneMarkTemplate::RenderFile(std::ostream& stream, const int fileTag, RenderingContext* context)
{
    Internal internal(stream, context);

    if (fileTag == TAG_HEADER)
    {
        internal.Header();
    }
    else if (fileTag == TAG_SOURCE)
    {
        internal.Source();
    }
    else
    {
        std::cout << "Unknown tag for ZoneMarkTemplate: " << fileTag << "\n";
    }
}
