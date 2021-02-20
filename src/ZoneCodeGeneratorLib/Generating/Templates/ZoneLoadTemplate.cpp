#include "ZoneLoadTemplate.h"

#include <iostream>
#include <sstream>


#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Internal/BaseTemplate.h"

class ZoneLoadTemplate::Internal final : BaseTemplate
{
    static std::string LoaderClassName(StructureInformation* asset)
    {
        std::ostringstream str;
        str << "Loader_" << asset->m_definition->m_name;
        return str.str();
    }

    static std::string VariableDecl(const DataDefinition* def)
    {
        std::ostringstream str;
        str << def->GetFullName() << "* var" << SafeTypeName(def) << ";";
        return str.str();
    }

    static std::string PointerVariableDecl(const DataDefinition* def)
    {
        std::ostringstream str;
        str << def->GetFullName() << "** var" << SafeTypeName(def) << "Ptr;";
        return str.str();
    }

    void PrintHeaderPtrArrayLoadMethodDeclaration(const DataDefinition* def) const
    {
        LINE("void LoadPtrArray_"<<SafeTypeName(def)<<"(bool atStreamStart, size_t count);")
    }

    void PrintHeaderArrayLoadMethodDeclaration(const DataDefinition* def) const
    {
        LINE("void LoadArray_"<<SafeTypeName(def)<<"(bool atStreamStart, size_t count);")
    }

    void PrintHeaderLoadMethodDeclaration(const StructureInformation* info) const
    {
        LINE("void Load_"<<SafeTypeName(info->m_definition)<<"(bool atStreamStart);")
    }

    void PrintHeaderTempPtrLoadMethodDeclaration(const StructureInformation* info) const
    {
        LINE("void LoadPtr_"<<SafeTypeName(info->m_definition)<<"(bool atStreamStart);")
    }

    void PrintHeaderAssetLoadMethodDeclaration(const StructureInformation* info) const
    {
        LINE("void LoadAsset_"<<SafeTypeName(info->m_definition)<<"("<<info->m_definition->GetFullName()<<"** pAsset);")
    }

    void PrintHeaderGetNameMethodDeclaration(const StructureInformation* info) const
    {
        LINE("static std::string GetAssetName("<<info->m_definition->GetFullName()<<"* pAsset);")
    }

    void PrintHeaderMainLoadMethodDeclaration(const StructureInformation* info) const
    {
        LINE("XAssetInfo<"<<info->m_definition->GetFullName()<<">* Load("<<info->m_definition->GetFullName()<<"** pAsset);")
    }

    void PrintHeaderConstructor() const
    {
        LINE(LoaderClassName(m_env.m_asset)<<"(Zone* zone, IZoneInputStream* stream);")
    }

    void PrintVariableInitialization(const DataDefinition* def) const
    {
        LINE("var"<<def->m_name<<" = nullptr;")
    }

    void PrintPointerVariableInitialization(const DataDefinition* def) const
    {
        LINE("var"<<def->m_name<<"Ptr = nullptr;")
    }

    void PrintConstructorMethod()
    {
        LINE(LoaderClassName(m_env.m_asset) << "::" << LoaderClassName(m_env.m_asset) << "(Zone* zone, IZoneInputStream* stream)")

        m_intendation++;
        LINE_START(": AssetLoader("<<m_env.m_asset->m_asset_enum_entry->m_name<<", zone, stream)")
        if (m_env.m_has_actions)
        {
            LINE_MIDDLE(", m_actions(zone)")
        }
        LINE_END("")
        m_intendation--;

        LINE("{")
        m_intendation++;

        LINE("m_asset_info = nullptr;")
        PrintVariableInitialization(m_env.m_asset->m_definition);
        PrintPointerVariableInitialization(m_env.m_asset->m_definition);
        LINE("")

        for (auto* type : m_env.m_used_types)
        {
            if (type->m_info && !type->m_info->m_definition->m_anonymous && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                PrintVariableInitialization(type->m_type);
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_info && type->m_pointer_array_reference_exists && !type->m_is_context_asset)
            {
                PrintPointerVariableInitialization(type->m_type);
            }
        }

        m_intendation--;
        LINE("}")
    }

    void PrintLoadPtrArrayMethod_Loading(const DataDefinition* def, StructureInformation* info) const
    {
        LINE("*"<<TypePtrVarName(def)<<" = m_stream->Alloc<"<<def->GetFullName()<<">(alignof("<<def->GetFullName()<<")); // "<<def->GetAlignment())

        if (info && info->m_is_leaf)
        {
            LINE(TypeVarName(info->m_definition)<<" = *"<<TypePtrVarName(def)<<";")
            LINE("Load_"<<SafeTypeName(def)<<"(true);")
        }
        else
        {
            LINE("m_stream->Load<"<<def->GetFullName()<<">(*"<<TypePtrVarName(def)<<");")
        }
    }

    void PrintLoadPtrArrayMethod_PointerCheck(const DataDefinition* def, StructureInformation* info, const bool reusable)
    {
        LINE("if (*" << TypePtrVarName(def) << ")")
        LINE("{")
        m_intendation++;

        if (info && StructureComputations(info).IsAsset())
        {
            LINE(LoaderClassName(info)<<" loader(m_zone, m_stream);")
            LINE("AddDependency(loader.Load("<<TypePtrVarName(def)<<"));")
        }
        else
        {
            if (reusable)
            {
                LINE("if(*" << TypePtrVarName(def) << " == PTR_FOLLOWING)")
                LINE("{")
                m_intendation++;

                PrintLoadPtrArrayMethod_Loading(def, info);

                m_intendation--;
                LINE("}")
                LINE("else")
                LINE("{")
                m_intendation++;

                LINE("*"<<TypePtrVarName(def)<<" = m_stream->ConvertOffsetToPointer(*"<<TypePtrVarName(def)<<");")

                m_intendation--;
                LINE("}")
            }
            else
            {
                PrintLoadPtrArrayMethod_Loading(def, info);
            }
        }

        m_intendation--;
        LINE("}")
    }

    void PrintLoadPtrArrayMethod(const DataDefinition* def, StructureInformation* info, const bool reusable)
    {
        LINE("void "<<LoaderClassName(m_env.m_asset)<<"::LoadPtrArray_"<<SafeTypeName(def)<<"(const bool atStreamStart, const size_t count)")
        LINE("{")
        m_intendation++;

        LINE("assert(" << TypePtrVarName(def) << " != nullptr);")
        LINE("")

        LINE("if(atStreamStart)")
        m_intendation++;
        LINE("m_stream->Load<"<<def->GetFullName()<<"*>("<<TypePtrVarName(def)<<", count);")
        m_intendation--;

        LINE("")
        LINE(def->GetFullName() << "** var = " << TypePtrVarName(def) << ";")
        LINE("for(size_t index = 0; index < count; index++)")
        LINE("{")
        m_intendation++;

        LINE(TypePtrVarName(def) << " = var;")
        PrintLoadPtrArrayMethod_PointerCheck(def, info, reusable);
        LINE("")
        LINE("var++;")

        m_intendation--;
        LINE("}")
        m_intendation--;
        LINE("}")
    }

    void PrintLoadArrayMethod(const DataDefinition* def, StructureInformation* info)
    {
        LINE("void " << LoaderClassName(m_env.m_asset) << "::LoadArray_" << SafeTypeName(def) << "(const bool atStreamStart, const size_t count)")
        LINE("{")
        m_intendation++;

        LINE("assert(" << TypeVarName(def) << " != nullptr);")
        LINE("")
        LINE("if(atStreamStart)")
        m_intendation++;
        LINE("m_stream->Load<"<<def->GetFullName()<<">("<<TypeVarName(def)<<", count);")
        m_intendation--;

        LINE("")
        LINE(def->GetFullName() << "* var = " << TypeVarName(def) << ";")
        LINE("for(size_t index = 0; index < count; index++)")
        LINE("{")
        m_intendation++;

        /*if (info == nullptr)
        {
            LINE(TypeVarName(def)<<" = var;")
        }
        else
        {
            LINE(TypeVarName(info->m_definition) << " = var;")
        }*/
        LINE(TypeVarName(info->m_definition) << " = var;")
        LINE("Load_"<<info->m_definition->m_name<<"(false);")
        LINE("var++;")

        m_intendation--;
        LINE("}")

        m_intendation--;
        LINE("}")
    }

    void LoadDynamicArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadDynamicArray: "<<member->m_member->m_name)
    }

    void LoadSinglePointer(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadSinglePointer: " << member->m_member->m_name)
    }

    void LoadArrayPointer(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadArrayPointer: " << member->m_member->m_name)
    }

    void LoadPointerArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadPointerArray: " << member->m_member->m_name)
    }

    void LoadEmbeddedArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadEmbeddedArray: " << member->m_member->m_name)
    }

    void LoadEmbedded(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadEmbedded: " << member->m_member->m_name)
    }

    void LoadMember_ReferenceArray(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        LINE("// LoadMember_ReferenceArray: " << member->m_member->m_name)
    }

    void LoadMember_Reference(StructureInformation* info, MemberInformation* member, const DeclarationModifierComputations& modifier)
    {
        if (modifier.IsDynamicArray())
        {
            LoadDynamicArray(info, member, modifier);
        }
        else if (modifier.IsSinglePointer())
        {
            LoadSinglePointer(info, member, modifier);
        }
        else if (modifier.IsArrayPointer())
        {
            LoadArrayPointer(info, member, modifier);
        }
        else if (modifier.IsPointerArray())
        {
            LoadPointerArray(info, member, modifier);
        }
        else if (modifier.IsArray() && modifier.GetNextDeclarationModifier() == nullptr)
        {
            LoadEmbeddedArray(info, member, modifier);
        }
        else if (modifier.GetDeclarationModifier() == nullptr)
        {
            LoadEmbedded(info, member, modifier);
        }
        else if (modifier.IsArray())
        {
            LoadMember_ReferenceArray(info, member, modifier);
        }
        else
        {
            LINE("#error LoadMemberReference failed @ "<<member->m_member->m_name)
        }
    }

    void LoadMember_Condition_Struct(StructureInformation* info, MemberInformation* member)
    {
        LINE("")
        if (member->m_condition)
        {
            LINE_START("if(")
            PrintEvaluation(member->m_condition.get());
            LINE_END(")")
            LINE("{")
            m_intendation++;

            LoadMember_Reference(info, member, DeclarationModifierComputations(member));

            m_intendation--;
            LINE("}")
        }
        else
        {
            LoadMember_Reference(info, member, DeclarationModifierComputations(member));
        }
    }

    void LoadMember_Condition_Union(StructureInformation* info, MemberInformation* member)
    {
        const MemberComputations computations(member);

        if (computations.IsFirstMember())
        {
            LINE("")
            if (member->m_condition)
            {
                LINE_START("if(")
                PrintEvaluation(member->m_condition.get());
                LINE_END(")")
                LINE("{")
                m_intendation++;

                LoadMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                LoadMember_Reference(info, member, DeclarationModifierComputations(member));
            }
        }
        else if (computations.IsLastMember())
        {
            if (member->m_condition)
            {
                LINE_START("else if(")
                PrintEvaluation(member->m_condition.get());
                LINE_END(")")
                LINE("{")
                m_intendation++;

                LoadMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINE("else")
                LINE("{")
                m_intendation++;

                LoadMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
        }
        else
        {
            if (member->m_condition)
            {
                LINE_START("else if(")
                PrintEvaluation(member->m_condition.get());
                LINE_END(")")
                LINE("{")
                m_intendation++;

                LoadMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINE("#error Middle member of union must have condition ("<<member->m_member->m_name<<")")
            }
        }
    }

    void PrintLoadMemberIfNeedsTreatment(StructureInformation* info, MemberInformation* member)
    {
        const MemberComputations computations(member);
        if (computations.ShouldIgnore())
            return;

        if (member->m_is_string
            || member->m_is_script_string
            || computations.ContainsNonEmbeddedReference()
            || member->m_type && !member->m_type->m_is_leaf
            || computations.IsAfterPartialLoad())
        {
            if (info->m_definition->GetType() == DataDefinitionType::UNION)
                LoadMember_Condition_Union(info, member);
            else
                LoadMember_Condition_Struct(info, member);
        }
    }

    void PrintLoadMethod(StructureInformation* info)
    {
        const StructureComputations computations(info);
        LINE("void " << LoaderClassName(m_env.m_asset) << "::Load_" << info->m_definition->m_name << "(const bool atStreamStart)")
        LINE("{")
        m_intendation++;

        LINE("assert(" << TypeVarName(info->m_definition) << " != nullptr);")

        auto* dynamicMember = computations.GetDynamicMember();
        if (!(info->m_definition->GetType() == DataDefinitionType::UNION && dynamicMember))
        {
            LINE("")
            LINE("if(atStreamStart)")
            m_intendation++;

            if (dynamicMember == nullptr)
            {
                LINE("m_stream->Load<"<<info->m_definition->GetFullName()<<">("<<TypeVarName(info->m_definition)<<"); // Size: "<<info->m_definition->GetSize())
            }
            else
            {
                LINE("m_stream->LoadPartial<"<<info->m_definition->GetFullName()<<">("<<TypeVarName(info->m_definition)<<", offsetof("<<info->m_definition->GetFullName()
                    <<", "<<dynamicMember->m_member->m_name<<"));")
            }

            m_intendation--;
        }
        else
        {
            LINE("assert(atStreamStart);")
        }

        if (info->m_block)
        {
            LINE("")
            LINE("m_stream->PushBlock("<<info->m_block->m_name<<");")
        }
        else if (computations.IsAsset())
        {
            LINE("")
            LINE("m_stream->PushBlock("<<m_env.m_default_normal_block->m_name<<");")
        }

        for (const auto& member : info->m_ordered_members)
        {
            PrintLoadMemberIfNeedsTreatment(info, member.get());
        }

        if (info->m_block || computations.IsAsset())
        {
            LINE("")
            LINE("m_stream->PopBlock();")
        }

        m_intendation--;
        LINE("}")
    }

    void PrintLoadTempPtrMethod(StructureInformation* info)
    {
        LINE("void "<<LoaderClassName(m_env.m_asset)<<"::LoadPtr_"<<SafeTypeName(info->m_definition)<<"(const bool atStreamStart)")
        LINE("{")
        m_intendation++;

        LINE("assert("<<TypePtrVarName(info->m_definition)<<" != nullptr);")
        LINE("")

        LINE("if(atStreamStart)")
        m_intendation++;
        LINE("m_stream->Load<"<<info->m_definition->GetFullName()<<"*>("<<TypePtrVarName(info->m_definition)<<");")
        m_intendation--;

        LINE("")
        LINE("m_stream->PushBlock("<<m_env.m_default_temp_block->m_name<<");")
        LINE("")
        LINE("if(*"<<TypePtrVarName(info->m_definition)<<" != nullptr)")
        LINE("{")
        m_intendation++;

        LINE("if(*" << TypePtrVarName(info->m_definition) << " == PTR_FOLLOWING || *" << TypePtrVarName(info->m_definition) << " == PTR_INSERT)")
        LINE("{")
        m_intendation++;

        LINE(info->m_definition->GetFullName() << "* ptr = *" << TypePtrVarName(info->m_definition) << ";")
        LINE("*" << TypePtrVarName(info->m_definition) << " = m_stream->Alloc<" << info->m_definition->GetFullName() << ">(alignof(" << info->m_definition->GetFullName() << "));")
        LINE("")
        LINE(info->m_definition->GetFullName() << "** toInsert = nullptr;")
        LINE("if(ptr == PTR_INSERT)")
        m_intendation++;
        LINE("toInsert = m_stream->InsertPointer<"<<info->m_definition->GetFullName()<<">();")
        m_intendation--;

        auto startLoadSection = true;

        if (!info->m_is_leaf)
        {
            if (startLoadSection)
            {
                startLoadSection = false;
                LINE("")
            }
            LINE(TypeVarName(info->m_definition)<<" = *"<<TypePtrVarName(info->m_definition)<<";")
            LINE("Load_"<<SafeTypeName(info->m_definition)<<"(true);")
        }

        if (info->m_post_load_action)
        {
            if (startLoadSection)
            {
                startLoadSection = false;
                LINE("")
            }
            PrintCustomAction(info->m_post_load_action.get());
        }

        if (StructureComputations(info).IsAsset())
        {
            if (startLoadSection)
            {
                startLoadSection = false;
                LINE("")
            }
            LINE("LoadAsset_"<<SafeTypeName(info->m_definition)<<"("<<TypePtrVarName(info->m_definition)<<");")
        }

        if (!startLoadSection)
        {
            LINE("")
        }

        LINE("if(toInsert != nullptr)")
        m_intendation++;
        LINE("*toInsert = *"<<TypePtrVarName(info->m_definition)<<";")
        m_intendation--;

        m_intendation--;
        LINE("}")
        LINE("else")
        LINE("{")
        m_intendation++;

        LINE("*"<<TypePtrVarName(info->m_definition)<<" = m_stream->ConvertOffsetToAlias(*"<<TypePtrVarName(info->m_definition)<<");")

        m_intendation--;
        LINE("}")

        m_intendation--;
        LINE("}")

        LINE("")
        LINE("m_stream->PopBlock();")

        m_intendation--;
        LINE("}")
    }

    void PrintLoadAssetMethod(StructureInformation* info)
    {
        LINE("void " << LoaderClassName(m_env.m_asset) << "::LoadAsset_" << SafeTypeName(info->m_definition) << "(" << info->m_definition->GetFullName() << "** pAsset)")
        LINE("{")
        m_intendation++;

        LINE("assert(pAsset != nullptr);")
        LINE("m_asset_info = reinterpret_cast<XAssetInfo<"<<info->m_definition->GetFullName()<<">*>(LinkAsset(GetAssetName(*pAsset), *pAsset));")
        LINE("*pAsset = m_asset_info->Asset();")

        m_intendation--;
        LINE("}")
    }

    void PrintMainLoadMethod()
    {
        LINE("XAssetInfo<" << m_env.m_asset->m_definition->GetFullName() << ">* " << LoaderClassName(m_env.m_asset) << "::Load(" << m_env.m_asset->m_definition->GetFullName() << "** pAsset)")
        LINE("{")
        m_intendation++;

        LINE("assert(pAsset != nullptr);")
        LINE("")
        LINE("m_asset_info = nullptr;")
        LINE("")
        LINE(TypePtrVarName(m_env.m_asset->m_definition) << " = pAsset;")
        LINE("LoadPtr_" << SafeTypeName(m_env.m_asset->m_definition) << "(false);")
        LINE("")
        LINE("if(m_asset_info == nullptr && *pAsset != nullptr)")
        m_intendation++;
        LINE("m_asset_info = reinterpret_cast<XAssetInfo<"<<m_env.m_asset->m_definition->GetFullName()<<">*>(GetAssetInfo(GetAssetName(*pAsset)));")
        m_intendation--;
        LINE("")
        LINE("return m_asset_info;")

        m_intendation--;
        LINE("}")
    }

    void PrintGetNameMethod()
    {
        LINE("std::string " << LoaderClassName(m_env.m_asset) << "::GetAssetName(" << m_env.m_asset->m_definition->GetFullName() << "* pAsset)")
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
                    LINE_MIDDLE("->"<<member->m_member->m_name)
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
            LINE("return \""<<m_env.m_asset->m_definition->m_name<<"\";")
        }

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
        LINE("#include \"Loading/AssetLoader.h\"")
        LINE("#include \"Game/" << m_env.m_game << "/" << m_env.m_game << ".h\"")
        if (m_env.m_has_actions)
        {
            LINE("#include \"Game/" << m_env.m_game << "/XAssets/" << Lower(m_env.m_asset->m_definition->m_name) << "/" << Lower(m_env.m_asset->m_definition->m_name) << "_actions.h\"")
        }
        LINE("#include <string>")
        LINE("")
        LINE("namespace " << m_env.m_game)
        LINE("{")
        m_intendation++;
        LINE("class " << LoaderClassName(m_env.m_asset) << " final : public AssetLoader")
        LINE("{")
        m_intendation++;

        LINE("XAssetInfo<"<<m_env.m_asset->m_definition->GetFullName()<<">* m_asset_info;")
        if (m_env.m_has_actions)
        {
            LINE("Actions_"<<m_env.m_asset->m_definition->m_name<<" m_actions;")
        }
        LINE(VariableDecl(m_env.m_asset->m_definition))
        LINE(PointerVariableDecl(m_env.m_asset->m_definition))
        LINE("")

        // Variable Declarations: type varType;
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_info && !type->m_info->m_definition->m_anonymous && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                LINE(VariableDecl(type->m_type));
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists && !type->m_is_context_asset)
            {
                LINE(PointerVariableDecl(type->m_type));
            }
        }

        LINE("")

        // Method Declarations
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists)
            {
                PrintHeaderPtrArrayLoadMethodDeclaration(type->m_type);
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
            {
                PrintHeaderArrayLoadMethodDeclaration(type->m_type);
            }
        }
        for (auto* type : m_env.m_used_structures)
        {
            if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                PrintHeaderLoadMethodDeclaration(type->m_info);
            }
        }
        PrintHeaderLoadMethodDeclaration(m_env.m_asset);
        PrintHeaderTempPtrLoadMethodDeclaration(m_env.m_asset);
        PrintHeaderAssetLoadMethodDeclaration(m_env.m_asset);
        LINE("")
        m_intendation--;
        LINE("public:")
        m_intendation++;
        PrintHeaderConstructor();
        PrintHeaderMainLoadMethodDeclaration(m_env.m_asset);
        PrintHeaderGetNameMethodDeclaration(m_env.m_asset);

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
        LINE("#include \""<<Lower(m_env.m_asset->m_definition->m_name)<<"_load_db.h\"")
        LINE("#include <cassert>")
        LINE("")

        if (!m_env.m_referenced_assets.empty())
        {
            LINE("// Referenced Assets:")
            for (auto* type : m_env.m_referenced_assets)
            {
                LINE("#include \"../"<<Lower(type->m_type->m_name)<<"/"<<Lower(type->m_type->m_name)<<"_load_db.h\"")
            }
            LINE("")
        }
        LINE("using namespace " << m_env.m_game << ";")
        LINE("")
        PrintConstructorMethod();

        for (auto* type : m_env.m_used_types)
        {
            if (type->m_pointer_array_reference_exists)
            {
                LINE("")
                PrintLoadPtrArrayMethod(type->m_type, type->m_info, type->m_pointer_array_reference_is_reusable);
            }
        }
        for (auto* type : m_env.m_used_types)
        {
            if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
            {
                LINE("")
                PrintLoadArrayMethod(type->m_type, type->m_info);
            }
        }
        for (auto* type : m_env.m_used_structures)
        {
            if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
            {
                LINE("")
                PrintLoadMethod(type->m_info);
            }
        }
        LINE("")
        PrintLoadMethod(m_env.m_asset);
        LINE("")
        PrintLoadTempPtrMethod(m_env.m_asset);
        LINE("")
        PrintLoadAssetMethod(m_env.m_asset);
        LINE("")
        PrintMainLoadMethod();
        LINE("")
        PrintGetNameMethod();
    }
};

std::vector<CodeTemplateFile> ZoneLoadTemplate::GetFilesToRender(RenderingContext* context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context->m_asset->m_definition->m_name;
    for (auto& c : assetName)
        c = static_cast<char>(tolower(c));

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_load_db.h";
        files.emplace_back(str.str(), TAG_HEADER);
    }

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_load_db.cpp";
        files.emplace_back(str.str(), TAG_SOURCE);
    }

    return files;
}

void ZoneLoadTemplate::RenderFile(std::ostream& stream, const int fileTag, RenderingContext* context)
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
        std::cout << "Unknown tag for ZoneLoadTemplate: " << fileTag << "\n";
    }
}
