#include "ZoneWriteTemplate.h"

#include "Domain/Computations/StructureComputations.h"
#include "Internal/BaseTemplate.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <cstdint>
#include <sstream>

namespace
{
    constexpr CodeTemplateFileTag TAG_HEADER = 1;
    constexpr CodeTemplateFileTag TAG_SOURCE = 2;
    constexpr CodeTemplateFileTag TAG_ALL_WRITERS = 3;

    class PerTemplate final : BaseTemplate
    {
    public:
        PerTemplate(std::ostream& stream, const OncePerTemplateRenderingContext& context)
            : BaseTemplate(stream, context),
              m_env(context)
        {
        }

        void AllWriters() const
        {
            AddGeneratedHint();

            LINE("#pragma once")
            LINE("")

            for (const auto* asset : m_env.m_assets)
            {
                LINEF("#include \"Game/{0}/XAssets/{1}/{1}_{2}_write_db.h\"", m_env.m_game, Lower(asset->m_definition->m_name), Lower(m_env.m_game))
            }
        }

    private:
        const OncePerTemplateRenderingContext& m_env;
    };

    class PerAsset final : BaseTemplate
    {
    public:
        PerAsset(std::ostream& stream, const OncePerAssetRenderingContext& context)
            : BaseTemplate(stream, context),
              m_env(context)
        {
        }

        void Header()
        {
            AddGeneratedHint();

            LINE("#pragma once")
            LINE("")
            LINEF("#include \"Game/{0}/{0}.h\"", m_env.m_game)
            LINE("#include \"Writing/AssetWriter.h\"")
            LINE("")
            LINE("#include <string>")
            LINE("")
            LINEF("namespace {0}", m_env.m_game)
            LINE("{")
            m_intendation++;
            LINEF("class {0} final : public AssetWriter", WriterClassName(m_env.m_asset))
            LINE("{")
            m_intendation++;

            m_intendation--;
            LINE("public:")
            m_intendation++;
            PrintHeaderConstructor();
            PrintHeaderMainWriteMethodDeclaration(m_env.m_asset);

            LINE("")
            m_intendation--;
            LINE("private:")
            m_intendation++;

            // Method Declarations
            if (m_env.m_architecture_mismatch)
            {
                for (const auto* type : m_env.m_used_types)
                {
                    if (ShouldGenerateFillMethod(*type))
                    {
                        PrintFillStructMethodDeclaration(type->m_info);
                    }
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists)
                {
                    PrintHeaderPtrArrayWriteMethodDeclaration(type->m_type);
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
                {
                    PrintHeaderArrayWriteMethodDeclaration(type->m_type);
                }
            }
            for (const auto* type : m_env.m_used_structures)
            {
                if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
                {
                    PrintHeaderWriteMethodDeclaration(type->m_info);
                }
            }
            PrintHeaderWriteMethodDeclaration(m_env.m_asset);
            PrintHeaderTempPtrWriteMethodDeclaration(m_env.m_asset);

            LINE("")

            LINE(VariableDecl(m_env.m_asset->m_definition))
            LINE(PointerVariableDecl(m_env.m_asset->m_definition))
            LINE(WrittenVariableDecl(m_env.m_asset->m_definition))
            LINE(WrittenPointerVariableDecl(m_env.m_asset->m_definition))
            LINE("")

            // Variable Declarations: type varType;
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && !type->m_info->m_definition->m_anonymous
                    && (!type->m_info->m_is_leaf || !type->m_info->m_has_matching_cross_platform_structure) && !StructureComputations(type->m_info).IsAsset())
                    LINE(VariableDecl(type->m_type))
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists && !type->m_is_context_asset)
                    LINE(PointerVariableDecl(type->m_type))
            }

            LINE("")

            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && !type->m_info->m_definition->m_anonymous && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
                    LINE(WrittenVariableDecl(type->m_type))
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists && !type->m_is_context_asset)
                    LINE(WrittenPointerVariableDecl(type->m_type))
            }

            m_intendation--;
            LINE("};")
            m_intendation--;
            LINE("}")
        }

        void Source()
        {
            AddGeneratedHint();

            LINEF("#include \"{0}_{1}_write_db.h\"", Lower(m_env.m_asset->m_definition->m_name), Lower(m_env.m_game))

            if (!m_env.m_referenced_assets.empty())
            {
                LINE("")
                LINE("// Referenced Assets:")
                for (const auto* type : m_env.m_referenced_assets)
                {
                    LINEF("#include \"../{0}/{0}_{1}_write_db.h\"", Lower(type->m_type->m_name), Lower(m_env.m_game))
                }
            }

            LINE("")
            LINE("#include <cassert>")
            LINE("")
            LINEF("using namespace {0};", m_env.m_game)
            LINE("")
            PrintConstructorMethod();
            LINE("")
            PrintMainWriteMethod();

            if (m_env.m_architecture_mismatch)
            {
                for (const auto* type : m_env.m_used_types)
                {
                    if (ShouldGenerateFillMethod(*type))
                    {
                        LINE("")
                        PrintFillStructMethod(type->m_info);
                    }
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists)
                {
                    LINE("")
                    PrintWritePtrArrayMethod(type->m_type, type->m_info, type->m_pointer_array_reference_is_reusable);
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
                {
                    LINE("")
                    PrintWriteArrayMethod(type->m_type, type->m_info);
                }
            }
            for (const auto* type : m_env.m_used_structures)
            {
                if (type->m_non_runtime_reference_exists && !type->m_info->m_is_leaf && !StructureComputations(type->m_info).IsAsset())
                {
                    LINE("")
                    PrintWriteMethod(type->m_info);
                }
            }
            LINE("")
            PrintWriteMethod(m_env.m_asset);
            LINE("")
            PrintWritePtrMethod(m_env.m_asset);
        }

    private:
        enum class MemberWriteType : std::uint8_t
        {
            ARRAY_POINTER,
            DYNAMIC_ARRAY,
            EMBEDDED,
            EMBEDDED_ARRAY,
            POINTER_ARRAY,
            SINGLE_POINTER
        };

        static std::string WriterClassName(const StructureInformation* asset)
        {
            return std::format("Writer_{0}", asset->m_definition->m_name);
        }

        static std::string VariableDecl(const DataDefinition* def)
        {
            return std::format("{0}* var{1};", def->GetFullName(), MakeSafeTypeName(def));
        }

        static std::string WrittenVariableDecl(const DataDefinition* def)
        {
            return std::format("ZoneOutputOffset var{0}Written;", MakeSafeTypeName(def));
        }

        static std::string PointerVariableDecl(const DataDefinition* def)
        {
            return std::format("{0}** var{1}Ptr;", def->GetFullName(), MakeSafeTypeName(def));
        }

        static std::string WrittenPointerVariableDecl(const DataDefinition* def)
        {
            return std::format("ZoneOutputOffset var{0}PtrWritten;", MakeSafeTypeName(def));
        }

        void PrintFillStructMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void FillStruct_{0}(const ZoneStreamFillWriteAccessor& fillAccessor);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderPtrArrayWriteMethodDeclaration(const DataDefinition* def) const
        {
            LINEF("void WritePtrArray_{0}(bool atStreamStart, size_t count);", MakeSafeTypeName(def))
        }

        void PrintHeaderArrayWriteMethodDeclaration(const DataDefinition* def) const
        {
            LINEF("void WriteArray_{0}(bool atStreamStart, size_t count);", MakeSafeTypeName(def))
        }

        void PrintHeaderWriteMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void Write_{0}(bool atStreamStart);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderTempPtrWriteMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void WritePtr_{0}(bool atStreamStart);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderMainWriteMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void Write({0}* pAsset, ZoneOutputOffset pointerWrittenOffset);", info->m_definition->GetFullName())
        }

        void PrintHeaderConstructor() const
        {
            LINEF("{0}({1}* asset, const Zone& zone, ZoneOutputStream& stream);", WriterClassName(m_env.m_asset), m_env.m_asset->m_definition->GetFullName())
        }

        void PrintVariableInitialization(const DataDefinition* def) const
        {
            LINEF("var{0} = nullptr;", def->m_name)
        }

        void PrintPointerVariableInitialization(const DataDefinition* def) const
        {
            LINEF("var{0}Ptr = nullptr;", def->m_name)
        }

        static void MakeTypeWrittenPtrVarNameInternal(const DataDefinition* def, std::ostringstream& str)
        {
            str << "var";
            MakeSafeTypeNameInternal(def, str);
            str << "PtrWritten";
        }

        static void MakeTypeWrittenVarNameInternal(const DataDefinition* def, std::ostringstream& str)
        {
            str << "var";
            MakeSafeTypeNameInternal(def, str);
            str << "Written";
        }

        static std::string MakeTypeWrittenVarName(const DataDefinition* def)
        {
            std::ostringstream str;
            MakeTypeWrittenVarNameInternal(def, str);
            return str.str();
        }

        static std::string MakeTypeWrittenPtrVarName(const DataDefinition* def)
        {
            std::ostringstream str;
            MakeTypeWrittenPtrVarNameInternal(def, str);
            return str.str();
        }

        std::string
            MakeWrittenMemberAccess(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            std::ostringstream str;
            MakeTypeWrittenVarNameInternal(info->m_definition, str);
            str << ".AtOffset(";

            if (m_env.m_architecture_mismatch)
            {
                str << OffsetForMemberModifier(*member, modifier, 0);
            }
            else
            {
                str << "offsetof(" << info->m_definition->GetFullName() << ", " << member->m_member->m_name;
                MakeArrayIndicesInternal(modifier, str);
                str << ')';
            }

            str << ')';

            return str.str();
        }

        void PrintConstructorMethod()
        {
            LINEF(
                "{0}::{0}({1}* asset, const Zone& zone, ZoneOutputStream& stream)", WriterClassName(m_env.m_asset), m_env.m_asset->m_definition->GetFullName())

            m_intendation++;
            LINEF(": AssetWriter(zone.m_pools->GetAssetOrAssetReference({0}::EnumEntry, NonReferenceAssetName(AssetName<{0}>(*asset))), zone, stream)",
                  m_env.m_asset->m_asset_name)
            m_intendation--;

            LINE("{")
            m_intendation++;

            PrintVariableInitialization(m_env.m_asset->m_definition);
            PrintPointerVariableInitialization(m_env.m_asset->m_definition);
            LINE("")

            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && !type->m_info->m_definition->m_anonymous
                    && (!type->m_info->m_is_leaf || !type->m_info->m_has_matching_cross_platform_structure) && !StructureComputations(type->m_info).IsAsset())
                    PrintVariableInitialization(type->m_type);
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && type->m_pointer_array_reference_exists && !type->m_is_context_asset)
                    PrintPointerVariableInitialization(type->m_type);
            }

            m_intendation--;
            LINE("}")
        }

        void WriteMember_ScriptString(const StructureInformation* info,
                                      const MemberInformation* member,
                                      const DeclarationModifierComputations& modifier,
                                      const MemberWriteType writeType) const
        {
            if (writeType == MemberWriteType::ARRAY_POINTER)
            {
                LINEF("varScriptString = {0};", MakeMemberAccess(info, member, modifier))
                LINEF("m_stream->MarkFollowing({0});", MakeWrittenMemberAccess(info, member, modifier))
                LINEF("WriteScriptStringArray(true, {0});", MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
            }
            else if (writeType == MemberWriteType::EMBEDDED_ARRAY)
            {
                LINEF("varScriptString = {0};", MakeMemberAccess(info, member, modifier))
                LINEF("varScriptStringWritten = {0};", MakeWrittenMemberAccess(info, member, modifier))
                LINEF("WriteScriptStringArray(false, {0});", MakeArrayCount(dynamic_cast<ArrayDeclarationModifier*>(modifier.GetDeclarationModifier())))
            }
            else if (writeType == MemberWriteType::EMBEDDED)
            {
                LINEF("UseScriptString({0}, {1});", MakeMemberAccess(info, member, modifier), MakeWrittenMemberAccess(info, member, modifier))
            }
            else
            {
                assert(false);
                LINEF("#error unsupported writeType {0} for script string", static_cast<int>(writeType))
            }
        }

        void WriteMember_Asset(const StructureInformation* info,
                               const MemberInformation* member,
                               const DeclarationModifierComputations& modifier,
                               const MemberWriteType writeType) const
        {
            if (writeType == MemberWriteType::SINGLE_POINTER)
            {
                LINEF("{0} writer({1}, m_zone, *m_stream);", WriterClassName(member->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("writer.Write({0}, {1});", MakeMemberAccess(info, member, modifier), MakeWrittenMemberAccess(info, member, modifier))
            }
            else if (writeType == MemberWriteType::POINTER_ARRAY)
            {
                WriteMember_PointerArray(info, member, modifier);
            }
            else
            {
                assert(false);
                LINEF("#error unsupported writeType {0} for asset", static_cast<int>(writeType))
            }
        }

        void WriteMember_String(const StructureInformation* info,
                                const MemberInformation* member,
                                const DeclarationModifierComputations& modifier,
                                const MemberWriteType writeType) const
        {
            if (writeType == MemberWriteType::SINGLE_POINTER)
            {
                if (member->m_member->m_type_declaration->m_is_const)
                {
                    LINEF("varXString = &{0};", MakeMemberAccess(info, member, modifier))
                }
                else
                {
                    LINEF("varXString = const_cast<const char**>(&{0});", MakeMemberAccess(info, member, modifier))
                }
                LINEF("varXStringWritten = {0};", MakeWrittenMemberAccess(info, member, modifier))
                LINE("WriteXString(false);")
            }
            else if (writeType == MemberWriteType::POINTER_ARRAY)
            {
                if (modifier.IsArray())
                {
                    LINEF("varXString = {0};", MakeMemberAccess(info, member, modifier))
                    LINEF("varXStringWritten = {0};", MakeWrittenMemberAccess(info, member, modifier))
                    LINEF("WriteXStringArray(false, {0});", modifier.GetArraySize())
                }
                else
                {
                    LINEF("varXString = {0};", MakeMemberAccess(info, member, modifier))
                    LINEF("m_stream->MarkFollowing({0});", MakeWrittenMemberAccess(info, member, modifier))
                    LINEF("WriteXStringArray(true, {0});", MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))
                }
            }
            else
            {
                assert(false);
                LINEF("#error unsupported writeType {0} for string", static_cast<int>(writeType))
            }
        }

        void WriteMember_ArrayPointer(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
        {
            const MemberComputations computations(member);
            LINEF("m_stream->MarkFollowing({0});", MakeWrittenMemberAccess(info, member, modifier))
            if (member->m_type && !member->m_type->m_is_leaf && !computations.IsInRuntimeBlock())
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("WriteArray_{0}(true, {1});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
            }
            else if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
            {
                LINEF("const auto fillArraySize = static_cast<size_t>({0});", MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
                if (!computations.IsInRuntimeBlock())
                {
                    LINEF("const auto fill = m_stream->WriteWithFill({0} * fillArraySize);", member->m_member->m_type_declaration->m_type->GetSize())
                    LINE("for (auto i = 0uz; i < fillArraySize; i++)")
                    LINE("{")
                    m_intendation++;
                    LINEF("{0} = &{1}[i];", MakeTypeVarName(member->m_type->m_definition), MakeMemberAccess(info, member, modifier))
                    LINEF("FillStruct_{0}(fill.AtOffset(i * {1}));",
                          MakeSafeTypeName(member->m_type->m_definition),
                          member->m_member->m_type_declaration->m_type->GetSize())
                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINEF("m_stream->IncBlockPos({0} * fillArraySize);", member->m_member->m_type_declaration->m_type->GetSize())
                }
            }
            else
            {
                LINEF("m_stream->Write<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
            }
        }

        void WriteMember_PointerArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            LINEF("{0} = {1};", MakeTypePtrVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
            if (modifier.IsArray())
            {
                LINEF("{0} = {1};", MakeTypeWrittenPtrVarName(member->m_member->m_type_declaration->m_type), MakeWrittenMemberAccess(info, member, modifier))
                LINEF("WritePtrArray_{0}(false, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), modifier.GetArraySize())
            }
            else
            {
                LINEF("m_stream->MarkFollowing({0});", MakeWrittenMemberAccess(info, member, modifier))
                LINEF("WritePtrArray_{0}(true, {1});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))
            }
        }

        void WriteMember_EmbeddedArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);
            std::string arraySizeStr;

            if (modifier.HasDynamicArrayCount())
                arraySizeStr = MakeEvaluation(modifier.GetDynamicArrayCountEvaluation());
            else
                arraySizeStr = std::to_string(modifier.GetArraySize());

            if (!member->m_is_leaf)
            {
                if (computations.IsAfterPartialLoad())
                {
                    LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                    LINEF("WriteArray_{0}(true, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), arraySizeStr)
                }
                else
                {
                    LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                    LINEF("{0} = {1};", MakeTypeWrittenVarName(member->m_member->m_type_declaration->m_type), MakeWrittenMemberAccess(info, member, modifier))
                    LINEF("WriteArray_{0}(false, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), arraySizeStr)
                }
            }
            else if (computations.IsAfterPartialLoad())
            {
                LINEF("m_stream->Write<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      arraySizeStr)
            }
        }

        void WriteMember_DynamicArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            if (member->m_type && !member->m_type->m_is_leaf)
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("WriteArray_{0}(true, {1});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))
            }
            else
            {
                LINEF("m_stream->Write<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))
            }
        }

        void WriteMember_Embedded(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);

            if (!member->m_is_leaf)
            {
                if (computations.IsAfterPartialLoad())
                {
                    LINEF("{0} = &{1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                    LINEF("Write_{0}(true);", MakeSafeTypeName(member->m_member->m_type_declaration->m_type))
                }
                else
                {
                    LINEF("{0} = &{1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                    LINEF("{0} = {1};", MakeTypeWrittenVarName(member->m_member->m_type_declaration->m_type), MakeWrittenMemberAccess(info, member, modifier))
                    LINEF("Write_{0}(false);", MakeSafeTypeName(member->m_member->m_type_declaration->m_type))
                }
            }
            else if (computations.IsAfterPartialLoad())
            {
                LINEF("m_stream->Write<{0}{1}>(&{2});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier))
            }
        }

        void WriteMember_SinglePointer(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);
            LINEF("m_stream->MarkFollowing({0});", MakeWrittenMemberAccess(info, member, modifier))
            if (member->m_type && !member->m_type->m_is_leaf && !computations.IsInRuntimeBlock())
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("Write_{0}(true);", MakeSafeTypeName(member->m_type->m_definition))
            }
            else if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("FillStruct_{0}(m_stream->WriteWithFill({1}));",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      member->m_member->m_type_declaration->m_type->GetSize())
            }
            else
            {
                LINEF("m_stream->Write<{0}{1}>({2});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier))
            }
        }

        void WriteMember_TypeCheck(const StructureInformation* info,
                                   const MemberInformation* member,
                                   const DeclarationModifierComputations& modifier,
                                   const MemberWriteType writeType)
        {
            if (member->m_is_string)
            {
                WriteMember_String(info, member, modifier, writeType);
            }
            else if (member->m_is_script_string)
            {
                WriteMember_ScriptString(info, member, modifier, writeType);
            }
            else if (member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                WriteMember_Asset(info, member, modifier, writeType);
            }
            else
            {
                switch (writeType)
                {
                case MemberWriteType::ARRAY_POINTER:
                    WriteMember_ArrayPointer(info, member, modifier);
                    break;

                case MemberWriteType::SINGLE_POINTER:
                    WriteMember_SinglePointer(info, member, modifier);
                    break;

                case MemberWriteType::EMBEDDED:
                    WriteMember_Embedded(info, member, modifier);
                    break;

                case MemberWriteType::POINTER_ARRAY:
                    WriteMember_PointerArray(info, member, modifier);
                    break;

                case MemberWriteType::DYNAMIC_ARRAY:
                    WriteMember_DynamicArray(info, member, modifier);
                    break;

                case MemberWriteType::EMBEDDED_ARRAY:
                    WriteMember_EmbeddedArray(info, member, modifier);
                    break;

                default:
                    LINEF("// t={0}", static_cast<int>(writeType))
                    break;
                }
            }
        }

        static bool
            WriteMember_ShouldMakeInsertReuse(const MemberInformation* member, const DeclarationModifierComputations& modifier, const MemberWriteType writeType)
        {
            if (writeType != MemberWriteType::ARRAY_POINTER && writeType != MemberWriteType::SINGLE_POINTER && writeType != MemberWriteType::POINTER_ARRAY)
            {
                return false;
            }

            if (writeType == MemberWriteType::POINTER_ARRAY && modifier.IsArray())
            {
                return false;
            }

            if (member->m_is_string)
            {
                return false;
            }

            if (member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                return false;
            }

            if (member->m_is_reusable)
                return true;

            if (member->m_type == nullptr || !member->m_type->m_reusable_reference_exists)
                return false;

            return true;
        }

        void WriteMember_InsertReuse(const StructureInformation* info,
                                     const MemberInformation* member,
                                     const DeclarationModifierComputations& modifier,
                                     const MemberWriteType writeType)
        {
            if (!WriteMember_ShouldMakeInsertReuse(member, modifier, writeType))
            {
                WriteMember_TypeCheck(info, member, modifier, writeType);
                return;
            }

            LINE_STARTF("m_stream->ReusableAddOffset({0}", MakeMemberAccess(info, member, modifier))

            if (writeType == MemberWriteType::ARRAY_POINTER)
            {
                if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
                {
                    LINE_MIDDLEF(", {0}", member->m_type->m_definition->GetSize())
                }

                LINE_MIDDLEF(", {0}", MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
            }
            else if (writeType == MemberWriteType::POINTER_ARRAY)
            {
                if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
                {
                    LINE_MIDDLEF(", {0}", m_env.m_pointer_size)
                }

                const auto* evaluation = modifier.GetPointerArrayCountEvaluation();

                if (evaluation)
                {
                    LINE_MIDDLEF(", {0}", MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))
                }
                else
                {
                    LINE_MIDDLEF(", {0}", modifier.GetArraySize())
                }
            }

            LINE_END(");")

            WriteMember_TypeCheck(info, member, modifier, writeType);
        }

        static bool
            WriteMember_ShouldMakeAlign(const MemberInformation* member, const DeclarationModifierComputations& modifier, const MemberWriteType writeType)
        {
            if (writeType != MemberWriteType::ARRAY_POINTER && writeType != MemberWriteType::POINTER_ARRAY && writeType != MemberWriteType::SINGLE_POINTER)
            {
                return false;
            }

            if (writeType == MemberWriteType::POINTER_ARRAY)
            {
                return !modifier.IsArray();
            }

            if (member->m_is_string)
            {
                return false;
            }

            if (member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                return false;
            }

            return true;
        }

        void WriteMember_Align(const StructureInformation* info,
                               const MemberInformation* member,
                               const DeclarationModifierComputations& modifier,
                               const MemberWriteType writeType)
        {
            if (!WriteMember_ShouldMakeAlign(member, modifier, writeType))
            {
                WriteMember_InsertReuse(info, member, modifier, writeType);
                return;
            }

            const auto typeDecl = MakeTypeDecl(member->m_member->m_type_declaration.get());
            const auto followingReferences = MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers());

            if (member->m_alloc_alignment)
            {
                LINEF("m_stream->Align({0});", MakeEvaluation(member->m_alloc_alignment.get()))
            }
            else
            {
                LINEF("m_stream->Align({0});", modifier.GetAlignment())
            }

            WriteMember_InsertReuse(info, member, modifier, writeType);
        }

        static bool
            WriteMember_ShouldMakeReuse(const MemberInformation* member, const DeclarationModifierComputations& modifier, const MemberWriteType writeType)
        {
            if (writeType != MemberWriteType::ARRAY_POINTER && writeType != MemberWriteType::SINGLE_POINTER && writeType != MemberWriteType::POINTER_ARRAY)
            {
                return false;
            }

            if (writeType == MemberWriteType::POINTER_ARRAY && modifier.IsArray())
            {
                return false;
            }

            return member->m_is_reusable;
        }

        std::string MakeReusableInnerOffset(const DataDefinition* dataDefinition, const Variable* member) const
        {
            if (m_env.m_architecture_mismatch)
                return std::to_string(member->m_offset);

            return std::format("offsetof({0}, {1})", dataDefinition->GetFullName(), member->m_name);
        }

        void WriteMember_Reuse(const StructureInformation* info,
                               const MemberInformation* member,
                               const DeclarationModifierComputations& modifier,
                               const MemberWriteType writeType)
        {
            if (!WriteMember_ShouldMakeReuse(member, modifier, writeType))
            {
                WriteMember_Align(info, member, modifier, writeType);
                return;
            }

            LINEF("if (m_stream->ReusableShouldWrite({0}, {1}.AtOffset({2})))",
                  MakeMemberAccess(info, member, modifier),
                  MakeTypeWrittenVarName(info->m_definition),
                  MakeReusableInnerOffset(info->m_definition, member->m_member))
            LINE("{")
            m_intendation++;

            WriteMember_Align(info, member, modifier, writeType);

            m_intendation--;
            LINE("}")
        }

        static bool WriteMember_ShouldMakePointerCheck(const MemberInformation* member,
                                                       const DeclarationModifierComputations& modifier,
                                                       const MemberWriteType writeType)
        {
            if (writeType != MemberWriteType::ARRAY_POINTER && writeType != MemberWriteType::POINTER_ARRAY && writeType != MemberWriteType::SINGLE_POINTER)
            {
                return false;
            }

            if (writeType == MemberWriteType::POINTER_ARRAY)
            {
                return !modifier.IsArray();
            }

            if (member->m_is_string)
            {
                return false;
            }

            return true;
        }

        void WriteMember_PointerCheck(const StructureInformation* info,
                                      const MemberInformation* member,
                                      const DeclarationModifierComputations& modifier,
                                      const MemberWriteType writeType)
        {
            if (WriteMember_ShouldMakePointerCheck(member, modifier, writeType))
            {
                LINEF("if ({0})", MakeMemberAccess(info, member, modifier))
                LINE("{")
                m_intendation++;

                WriteMember_Reuse(info, member, modifier, writeType);

                m_intendation--;
                LINE("}")
            }
            else
            {
                WriteMember_Reuse(info, member, modifier, writeType);
            }
        }

        void WriteMember_Block(const StructureInformation* info,
                               const MemberInformation* member,
                               const DeclarationModifierComputations& modifier,
                               const MemberWriteType writeType)
        {
            const MemberComputations computations(member);

            const auto notInDefaultNormalBlock = computations.IsNotInDefaultNormalBlock();
            if (notInDefaultNormalBlock)
            {
                LINEF("m_stream->PushBlock({0});", member->m_fast_file_block->m_name)
            }

            WriteMember_PointerCheck(info, member, modifier, writeType);

            if (notInDefaultNormalBlock)
            {
                LINE("m_stream->PopBlock();")
            }
        }

        void WriteMember_ReferenceArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
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

                WriteMember_Reference(info, member, entry);
            }
        }

        void WriteMember_Reference(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
        {
            if (modifier.IsDynamicArray())
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::DYNAMIC_ARRAY);
            }
            else if (modifier.IsSinglePointer())
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::SINGLE_POINTER);
            }
            else if (modifier.IsArrayPointer())
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::ARRAY_POINTER);
            }
            else if (modifier.IsPointerArray())
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::POINTER_ARRAY);
            }
            else if (modifier.IsArray() && modifier.GetNextDeclarationModifier() == nullptr)
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::EMBEDDED_ARRAY);
            }
            else if (modifier.GetDeclarationModifier() == nullptr)
            {
                WriteMember_Block(info, member, modifier, MemberWriteType::EMBEDDED);
            }
            else if (modifier.IsArray())
            {
                WriteMember_ReferenceArray(info, member, modifier);
            }
            else
            {
                assert(false);
                LINEF("#error WriteMemberReference failed @ {0}", member->m_member->m_name)
            }
        }

        void WriteMember_Condition_Struct(const StructureInformation* info, const MemberInformation* member)
        {
            LINE("")
            if (member->m_condition)
            {
                LINEF("if ({0})", MakeEvaluation(member->m_condition.get()))
                LINE("{")
                m_intendation++;

                WriteMember_Reference(info, member, DeclarationModifierComputations(member));

                m_intendation--;
                LINE("}")
            }
            else
            {
                WriteMember_Reference(info, member, DeclarationModifierComputations(member));
            }
        }

        void WriteMember_Condition_Union(const StructureInformation* info, const MemberInformation* member)
        {
            const MemberComputations computations(member);

            if (computations.IsFirstUsedMember(false))
            {
                LINE("")
                if (member->m_condition)
                {
                    LINEF("if ({0})", MakeEvaluation(member->m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    WriteMember_Reference(info, member, DeclarationModifierComputations(member));

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    WriteMember_Reference(info, member, DeclarationModifierComputations(member));
                }
            }
            else if (computations.IsLastUsedMember(false))
            {
                if (member->m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member->m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    WriteMember_Reference(info, member, DeclarationModifierComputations(member));

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINE("else")
                    LINE("{")
                    m_intendation++;

                    WriteMember_Reference(info, member, DeclarationModifierComputations(member));

                    m_intendation--;
                    LINE("}")
                }
            }
            else
            {
                if (member->m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member->m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    WriteMember_Reference(info, member, DeclarationModifierComputations(member));

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINEF("#error Middle member of union must have condition ({0})", member->m_member->m_name)
                }
            }
        }

        void PrintWriteMemberIfNeedsTreatment(const StructureInformation* info, const MemberInformation* member)
        {
            const MemberComputations computations(member);
            if (computations.ShouldIgnore())
                return;

            if (member->m_is_string || member->m_is_script_string || computations.ContainsNonEmbeddedReference() || member->m_type && !member->m_type->m_is_leaf
                || computations.IsAfterPartialLoad())
            {
                if (info->m_definition->GetType() == DataDefinitionType::UNION)
                    WriteMember_Condition_Union(info, member);
                else
                    WriteMember_Condition_Struct(info, member);
            }
        }

        void PrintWriteMethod(const StructureInformation* info)
        {
            const StructureComputations computations(info);
            LINEF("void {0}::Write_{1}(const bool atStreamStart)", WriterClassName(m_env.m_asset), info->m_definition->m_name)
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypeVarName(info->m_definition))
            LINE("")

            const auto* dynamicMember = computations.GetDynamicMember();
            if (!(info->m_definition->GetType() == DataDefinitionType::UNION && dynamicMember))
            {
                LINE("if (atStreamStart)")

                if (info->m_has_matching_cross_platform_structure)
                {
                    m_intendation++;
                    if (dynamicMember == nullptr)
                    {
                        LINEF("{0} = m_stream->Write<{1}>({2}); // Size: {3}",
                              MakeTypeWrittenVarName(info->m_definition),
                              info->m_definition->GetFullName(),
                              MakeTypeVarName(info->m_definition),
                              info->m_definition->GetSize())
                    }
                    else
                    {
                        LINEF("{0} = m_stream->WritePartial({1}, offsetof({2}, {3}));",
                              MakeTypeWrittenVarName(info->m_definition),
                              MakeTypeVarName(info->m_definition),
                              info->m_definition->GetFullName(),
                              dynamicMember->m_member->m_name)
                    }
                    m_intendation--;
                }
                else
                {
                    LINE("{")
                    m_intendation++;
                    LINEF("const auto fillAccessor = m_stream->WriteWithFill({0});",
                          dynamicMember == nullptr ? info->m_definition->GetSize() : dynamicMember->m_member->m_offset)
                    LINEF("{0} = fillAccessor.Offset();", MakeTypeWrittenVarName(info->m_definition))
                    LINEF("FillStruct_{0}(fillAccessor);", MakeSafeTypeName(info->m_definition))
                    m_intendation--;
                    LINE("}")
                }

                LINE("")
                LINEF("assert({0}.Offset() != nullptr);", MakeTypeWrittenVarName(info->m_definition))
            }
            else
            {
                LINE("assert(atStreamStart);")
            }

            if (computations.IsAsset())
            {
                LINE("")
                LINEF("m_stream->PushBlock({0});", m_env.m_default_normal_block->m_name)
            }
            else if (info->m_block)
            {
                LINE("")
                LINEF("m_stream->PushBlock({0});", info->m_block->m_name)
            }

            for (const auto& member : info->m_ordered_members)
            {
                PrintWriteMemberIfNeedsTreatment(info, member.get());
            }

            if (info->m_block || computations.IsAsset())
            {
                LINE("")
                LINE("m_stream->PopBlock();")
            }

            m_intendation--;
            LINE("}")
        }

        void PrintWritePtrMethod(const StructureInformation* info)
        {
            const bool inTemp = info->m_block && info->m_block->m_type == FastFileBlockType::TEMP;
            LINEF("void {0}::WritePtr_{1}(const bool atStreamStart)", WriterClassName(m_env.m_asset), MakeSafeTypeName(info->m_definition))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypePtrVarName(info->m_definition))
            LINE("")

            LINE("if (atStreamStart)")
            m_intendation++;
            LINEF("{0} = m_stream->WriteWithFill(m_stream->GetPointerByteCount()).Offset();", MakeTypeWrittenPtrVarName(info->m_definition))
            m_intendation--;

            LINE("")
            LINEF("assert({0}.Offset() != nullptr);", MakeTypeWrittenPtrVarName(info->m_definition))

            LINE("")
            if (inTemp)
            {
                LINEF("m_stream->PushBlock({0});", m_env.m_default_temp_block->m_name)
                LINE("")
            }
            LINEF("if (m_stream->ReusableShouldWrite(*{0}, {1}))", MakeTypePtrVarName(info->m_definition), MakeTypeWrittenPtrVarName(info->m_definition))
            LINE("{")
            m_intendation++;

            LINEF("m_stream->Align({0});", info->m_definition->GetAlignment())
            LINEF("m_stream->ReusableAddOffset(*{0});", MakeTypePtrVarName(info->m_definition))
            LINE("")
            if (!info->m_is_leaf)
            {
                LINEF("{0} = *{1};", MakeTypeVarName(info->m_definition), MakeTypePtrVarName(info->m_definition))
                LINEF("Write_{0}(true);", MakeSafeTypeName(info->m_definition))
            }
            else
            {
                LINE("#error Ptr method cannot have leaf type")
            }

            LINE("")
            LINEF("m_stream->MarkFollowing({0});", MakeTypeWrittenPtrVarName(info->m_definition))

            m_intendation--;
            LINE("}")

            if (inTemp)
            {
                LINE("")
                LINE("m_stream->PopBlock();")
            }

            m_intendation--;
            LINE("}")
        }

        void PrintMainWriteMethod()
        {
            LINEF("void {0}::Write({1}* pAsset, const ZoneOutputOffset pointerWrittenOffset)",
                  WriterClassName(m_env.m_asset),
                  m_env.m_asset->m_definition->GetFullName())
            LINE("{")
            m_intendation++;

            LINE("assert(pAsset != nullptr);")
            LINE("assert(m_asset != nullptr);")
            LINE("assert(m_asset->m_ptr != nullptr);")
            LINE("")
            LINEF("auto* zoneAsset = static_cast<{0}*>(m_asset->m_ptr);", m_env.m_asset->m_definition->GetFullName())
            LINEF("{0} = &zoneAsset;", MakeTypePtrVarName(m_env.m_asset->m_definition))
            LINEF("{0} = pointerWrittenOffset;", MakeTypeWrittenPtrVarName(m_env.m_asset->m_definition))
            LINEF("WritePtr_{0}(false);", MakeSafeTypeName(m_env.m_asset->m_definition))

            m_intendation--;
            LINE("}")
        }

        void PrintFillStruct_Member_EmbeddedArray(const StructureInformation& structInfo,
                                                  const MemberInformation& memberInfo,
                                                  const DeclarationModifierComputations& modifier,
                                                  const size_t nestedBaseOffset)
        {
            if (memberInfo.m_type && !memberInfo.m_type->m_has_matching_cross_platform_structure)
            {
                LINEF("for (auto i = 0u; i < std::extent_v<decltype({0}::{1})>; i++)", structInfo.m_definition->m_name, memberInfo.m_member->m_name)
                LINE("{")
                m_intendation++;
                LINEF("{0} = &{1}[i];", MakeTypeVarName(memberInfo.m_member->m_type_declaration->m_type), MakeMemberAccess(&structInfo, &memberInfo, modifier))
                LINEF("FillStruct_{0}(fillAccessor.AtOffset({1} + i * {2}));",
                      MakeSafeTypeName(memberInfo.m_member->m_type_declaration->m_type),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset),
                      memberInfo.m_member->m_type_declaration->m_type->GetSize())
                m_intendation--;
                LINE("}")
            }
            else
            {
                LINEF("fillAccessor.FillArray({0}, {1});",
                      MakeMemberAccess(&structInfo, &memberInfo, modifier),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
            }
        }

        void PrintFillStruct_Member_Embedded(const StructureInformation& structInfo,
                                             const MemberInformation& memberInfo,
                                             const DeclarationModifierComputations& modifier,
                                             const size_t nestedBaseOffset)
        {
            const auto hasAnonymousType = memberInfo.m_type && memberInfo.m_type->m_definition->m_anonymous;

            if (!hasAnonymousType)
            {
                const auto hasMismatchingStructure = memberInfo.m_type && !memberInfo.m_type->m_has_matching_cross_platform_structure;
                const auto hasDynamicMember = memberInfo.m_type && StructureComputations(memberInfo.m_type).GetDynamicMember();
                if (hasMismatchingStructure || hasDynamicMember)
                {
                    LINEF("{0} = &{1};", MakeTypeVarName(memberInfo.m_member->m_type_declaration->m_type), MakeMemberAccess(&structInfo, &memberInfo, modifier))
                    LINEF("FillStruct_{0}(fillAccessor.AtOffset({1}));",
                          MakeSafeTypeName(memberInfo.m_member->m_type_declaration->m_type),
                          OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
                }
                else
                {
                    LINEF("fillAccessor.Fill({0}, {1});",
                          MakeMemberAccess(&structInfo, &memberInfo, modifier),
                          OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
                }
            }
            else if (memberInfo.m_member->m_name.empty())
            {
                const auto anonymousMemberOffset = memberInfo.m_member->m_offset + nestedBaseOffset;
                for (const auto& anonymousMember : memberInfo.m_type->m_ordered_members)
                {
                    PrintFillStruct_Member(structInfo, *anonymousMember, DeclarationModifierComputations(anonymousMember.get()), anonymousMemberOffset);
                }
            }
            else
            {
                LINEF("#error Unsupported anonymous struct with name: {0}", memberInfo.m_member->m_name)
            }
        }

        void PrintFillStruct_Member_ReferenceArray(const StructureInformation& info,
                                                   const MemberInformation& member,
                                                   const DeclarationModifierComputations& modifier,
                                                   const size_t nestedBaseOffset)
        {
            for (const auto& entry : modifier.GetArrayEntries())
            {
                PrintFillStruct_Member(info, member, entry, nestedBaseOffset);
            }
        }

        static bool ShouldFillMember(const DeclarationModifierComputations& modifier)
        {
            return !modifier.HasPointerModifier();
        }

        // nestedBaseOffset: Base offset in case member is part of a nested anonymous sub-struct
        void PrintFillStruct_Member(const StructureInformation& structInfo,
                                    const MemberInformation& memberInfo,
                                    const DeclarationModifierComputations& modifier,
                                    const size_t nestedBaseOffset)
        {
            if (!ShouldFillMember(modifier))
                return;

            if (modifier.IsDynamicArray())
            {
            }
            else if (modifier.IsArray() && modifier.GetNextDeclarationModifier() == nullptr)
            {
                PrintFillStruct_Member_EmbeddedArray(structInfo, memberInfo, modifier, nestedBaseOffset);
            }
            else if (modifier.GetDeclarationModifier() == nullptr)
            {
                PrintFillStruct_Member_Embedded(structInfo, memberInfo, modifier, nestedBaseOffset);
            }
            else if (modifier.IsArray())
            {
                PrintFillStruct_Member_ReferenceArray(structInfo, memberInfo, modifier, nestedBaseOffset);
            }
            else
            {
                assert(false);
                LINEF("#error PrintFillStruct_Member failed @ {0}", memberInfo.m_member->m_name)
            }
        }

        void PrintFillStruct_Member_Condition_Union(const StructureInformation& structInfo,
                                                    const MemberInformation& member,
                                                    const DeclarationModifierComputations& modifier)
        {
            const MemberComputations computations(&member);

            if (computations.IsFirstUsedMember(true))
            {
                if (member.m_condition)
                {
                    LINEF("if ({0})", MakeEvaluation(member.m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, modifier, 0u);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    PrintFillStruct_Member(structInfo, member, modifier, 0u);
                }
            }
            else if (computations.IsLastUsedMember(true))
            {
                if (member.m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member.m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, modifier, 0u);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINE("else")
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, modifier, 0u);

                    m_intendation--;
                    LINE("}")
                }
            }
            else
            {
                if (member.m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member.m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, modifier, 0u);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINEF("#error Middle member of union2 must have condition ({0})", member.m_member->m_name)
                }
            }
        }

        void PrintFillStruct_Struct(const StructureInformation& info)
        {
            const auto* dynamicMember = StructureComputations(&info).GetDynamicMember();
            if (info.m_definition->GetType() == DataDefinitionType::UNION)
            {
                for (const auto& member : info.m_ordered_members)
                {
                    const MemberComputations computations(member.get());
                    if (computations.ShouldIgnore())
                        continue;

                    DeclarationModifierComputations modifier(member.get());
                    PrintFillStruct_Member_Condition_Union(info, *member, modifier);
                }
            }
            else
            {
                for (const auto& member : info.m_ordered_members)
                {
                    const MemberComputations computations(member.get());
                    if (computations.ShouldIgnore() || member.get() == dynamicMember)
                        continue;

                    DeclarationModifierComputations modifier(member.get());
                    PrintFillStruct_Member(info, *member, modifier, 0u);
                }
            }
        }

        void PrintFillStructMethod(const StructureInformation* info)
        {
            LINEF("void {0}::FillStruct_{1}(const ZoneStreamFillWriteAccessor& fillAccessor)",
                  WriterClassName(m_env.m_asset),
                  MakeSafeTypeName(info->m_definition))

            LINE("{")
            m_intendation++;

            PrintFillStruct_Struct(*info);

            m_intendation--;
            LINE("}")
        }

        void PrintWritePtrArrayMethod_Loading(const DataDefinition* def, const StructureInformation* info, const bool reusable) const
        {
            LINEF("m_stream->Align({0});", def->GetAlignment())

            if (reusable)
            {
                LINEF("m_stream->ReusableAddOffset(*{0});", MakeTypePtrVarName(def))
            }

            if (info && !info->m_is_leaf)
            {
                LINEF("{0} = *{1};", MakeTypeVarName(info->m_definition), MakeTypePtrVarName(def))
                LINEF("Write_{0}(true);", MakeSafeTypeName(def))
            }
            else
            {
                LINEF("m_stream->Write<{0}>(*{1});", def->GetFullName(), MakeTypePtrVarName(def))
            }
            LINEF("m_stream->MarkFollowing({0});", MakeTypeWrittenPtrVarName(def))
        }

        void PrintWritePtrArrayMethod_PointerCheck(const DataDefinition* def, const StructureInformation* info, const bool reusable)
        {
            LINEF("if (*{0})", MakeTypePtrVarName(def))
            LINE("{")
            m_intendation++;

            if (info && StructureComputations(info).IsAsset())
            {
                LINEF("{0} writer(*{1}, m_zone, *m_stream);", WriterClassName(info), MakeTypePtrVarName(def))
                LINEF("writer.Write(*{0}, {1});", MakeTypePtrVarName(def), MakeTypeWrittenPtrVarName(def))
            }
            else
            {
                if (reusable)
                {
                    LINEF("if (m_stream->ReusableShouldWrite(*{0}, {1}))", MakeTypePtrVarName(def), MakeTypeWrittenPtrVarName(def))
                    LINE("{")
                    m_intendation++;

                    PrintWritePtrArrayMethod_Loading(def, info, reusable);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    PrintWritePtrArrayMethod_Loading(def, info, reusable);
                }
            }

            m_intendation--;
            LINE("}")
        }

        void PrintWritePtrArrayMethod(const DataDefinition* def, const StructureInformation* info, const bool reusable)
        {
            LINEF("void {0}::WritePtrArray_{1}(const bool atStreamStart, const size_t count)", WriterClassName(m_env.m_asset), MakeSafeTypeName(def))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypePtrVarName(def))
            LINE("")

            LINE("if (atStreamStart)")
            m_intendation++;
            LINEF("{0} = m_stream->WriteWithFill(m_stream->GetPointerByteCount() * count).Offset();", MakeTypeWrittenPtrVarName(def))
            m_intendation--;

            LINE("")
            LINEF("assert({0}.Offset() != nullptr);", MakeTypeWrittenPtrVarName(def))

            LINE("")
            LINEF("{0}** var = {1};", def->GetFullName(), MakeTypePtrVarName(def))
            LINEF("ZoneOutputOffset varWritten = {1};", def->GetFullName(), MakeTypeWrittenPtrVarName(def))
            LINE("for (size_t index = 0; index < count; index++)")
            LINE("{")
            m_intendation++;

            LINEF("{0} = var;", MakeTypePtrVarName(def))
            LINEF("{0} = varWritten;", MakeTypeWrittenPtrVarName(def))
            PrintWritePtrArrayMethod_PointerCheck(def, info, reusable);
            LINE("")
            LINE("var++;")
            LINE("varWritten.Inc(m_stream->GetPointerByteCount());")

            m_intendation--;
            LINE("}")
            m_intendation--;
            LINE("}")
        }

        void PrintWriteArrayMethod(const DataDefinition* def, const StructureInformation* info)
        {
            LINEF("void {0}::WriteArray_{1}(const bool atStreamStart, const size_t count)", WriterClassName(m_env.m_asset), MakeSafeTypeName(def))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypeVarName(def))
            LINE("")
            LINE("if (atStreamStart)")

            if (info->m_has_matching_cross_platform_structure)
            {
                m_intendation++;
                LINEF("{0} = m_stream->Write<{1}>({2}, count);", MakeTypeWrittenVarName(def), def->GetFullName(), MakeTypeVarName(def))
                m_intendation--;
            }
            else
            {
                LINE("{")
                m_intendation++;

                LINEF("const auto arrayFill = m_stream->WriteWithFill({0} * count);", def->GetSize())
                LINEF("{0} = arrayFill.Offset();", MakeTypeWrittenVarName(def))
                LINEF("auto* arrayStart = {0};", MakeTypeVarName(def))
                LINEF("auto* var = {0};", MakeTypeVarName(def))
                LINE("for (size_t index = 0; index < count; index++)")
                LINE("{")
                m_intendation++;

                LINEF("{0} = var;", MakeTypeVarName(info->m_definition))
                LINEF("FillStruct_{0}(arrayFill.AtOffset(0 + {1} * index));", info->m_definition->m_name, def->GetSize())
                LINE("var++;")

                m_intendation--;
                LINE("}")

                LINEF("{0} = arrayStart;", MakeTypeVarName(def))

                m_intendation--;
                LINE("}")
            }

            LINE("")
            LINEF("assert({0}.Offset() != nullptr);", MakeTypeWrittenVarName(def))

            LINE("")
            LINEF("{0}* var = {1};", def->GetFullName(), MakeTypeVarName(def))
            LINEF("ZoneOutputOffset varWritten = {1};", def->GetFullName(), MakeTypeWrittenVarName(def))
            LINE("for (size_t index = 0; index < count; index++)")
            LINE("{")
            m_intendation++;

            LINEF("{0} = var;", MakeTypeVarName(info->m_definition))
            LINEF("{0} = varWritten;", MakeTypeWrittenVarName(info->m_definition))
            LINEF("Write_{0}(false);", info->m_definition->m_name)
            LINE("var++;")
            LINEF("varWritten.Inc({0});", def->GetSize())

            m_intendation--;
            LINE("}")

            m_intendation--;
            LINE("}")
        }

        const OncePerAssetRenderingContext& m_env;
    };
} // namespace

std::vector<CodeTemplateFile> ZoneWriteTemplate::GetFilesToRenderOncePerTemplate(const OncePerTemplateRenderingContext& context)
{
    std::vector<CodeTemplateFile> files;

    files.emplace_back(std::format("AssetWriter{0}.h", context.m_game), TAG_ALL_WRITERS);

    return files;
}

void ZoneWriteTemplate::RenderOncePerTemplateFile(std::ostream& stream, const CodeTemplateFileTag fileTag, const OncePerTemplateRenderingContext& context)
{
    assert(fileTag == TAG_ALL_WRITERS);

    const PerTemplate t(stream, context);
    t.AllWriters();
}

std::vector<CodeTemplateFile> ZoneWriteTemplate::GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context.m_asset->m_definition->m_name;
    utils::MakeStringLowerCase(assetName);

    auto gameName = context.m_game;
    utils::MakeStringLowerCase(gameName);

    files.emplace_back(std::format("XAssets/{0}/{0}_{1}_write_db.h", assetName, gameName), TAG_HEADER);
    files.emplace_back(std::format("XAssets/{0}/{0}_{1}_write_db.cpp", assetName, gameName), TAG_SOURCE);

    return files;
}

void ZoneWriteTemplate::RenderOncePerAssetFile(std::ostream& stream, const CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context)
{
    PerAsset t(stream, context);

    if (fileTag == TAG_HEADER)
    {
        t.Header();
    }
    else
    {
        assert(fileTag == TAG_SOURCE);
        t.Source();
    }
}
