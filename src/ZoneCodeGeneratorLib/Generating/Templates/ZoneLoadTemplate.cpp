#include "ZoneLoadTemplate.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Internal/BaseTemplate.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <sstream>

namespace
{
    constexpr int TAG_HEADER = 1;
    constexpr int TAG_SOURCE = 2;
    constexpr int TAG_ALL_LOADERS = 3;

    class PerTemplate final : BaseTemplate
    {
    public:
        PerTemplate(std::ostream& stream, const OncePerTemplateRenderingContext& context)
            : BaseTemplate(stream, context),
              m_env(context)
        {
        }

        void AllLoaders() const
        {
            AddGeneratedHint();

            LINE("#pragma once")
            LINE("")

            for (const auto* asset : m_env.m_assets)
            {
                LINEF("#include \"Game/{0}/XAssets/{1}/{1}_{2}_load_db.h\"", m_env.m_game, Lower(asset->m_definition->m_name), Lower(m_env.m_game))
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
            if (m_env.m_has_actions)
            {
                LINEF("#include \"Game/{0}/XAssets/{1}/{1}_actions.h\"", m_env.m_game, Lower(m_env.m_asset->m_definition->m_name))
            }
            LINE("#include \"Loading/AssetLoader.h\"")
            LINE("")
            LINE("#include <string>")
            LINE("")
            LINEF("namespace {0}", m_env.m_game)
            LINE("{")
            m_intendation++;
            LINEF("class {0} final : public AssetLoader", LoaderClassName(m_env.m_asset))
            LINE("{")
            m_intendation++;

            m_intendation--;
            LINE("public:")
            m_intendation++;
            PrintHeaderConstructor();
            PrintHeaderMainLoadMethodDeclaration(m_env.m_asset);

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
                for (const auto* type : m_env.m_used_types)
                {
                    if (type->m_info && type->m_type == type->m_info->m_definition && StructureComputations(type->m_info).GetDynamicMember())
                    {
                        PrintDynamicFillMethodDeclaration(type->m_info);
                    }
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists)
                {
                    PrintHeaderPtrArrayLoadMethodDeclaration(type->m_type);
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
                {
                    PrintHeaderArrayLoadMethodDeclaration(type->m_type);
                }
            }
            for (const auto* type : m_env.m_used_structures)
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

            LINEF("XAssetInfo<{0}>* m_asset_info;", m_env.m_asset->m_definition->GetFullName())
            if (m_env.m_has_actions)
            {
                LINEF("Actions_{0} m_actions;", m_env.m_asset->m_definition->m_name)
            }
            LINE(VariableDecl(m_env.m_asset->m_definition))
            LINE(PointerVariableDecl(m_env.m_asset->m_definition))
            LINE("")

            // Variable Declarations: type varType;
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && !type->m_info->m_definition->m_anonymous
                    && (!type->m_info->m_is_leaf || !type->m_info->m_has_matching_cross_platform_structure) && !StructureComputations(type->m_info).IsAsset())
                {
                    LINE(VariableDecl(type->m_type))
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists && !type->m_is_context_asset)
                {
                    LINE(PointerVariableDecl(type->m_type))
                }
            }

            m_intendation--;
            LINE("};")
            m_intendation--;
            LINE("}")
        }

        void Source()
        {
            AddGeneratedHint();

            LINEF("#include \"{0}_{1}_load_db.h\"", Lower(m_env.m_asset->m_definition->m_name), Lower(m_env.m_game))
            LINE("")
            LINEF("#include \"Game/{0}/AssetMarker{0}.h\"", m_env.m_game)
            LINE("")
            LINE("#include \"Loading/AssetInfoCollector.h\"")

            if (!m_env.m_referenced_assets.empty())
            {
                LINE("")
                LINE("// Referenced Assets:")
                for (const auto* type : m_env.m_referenced_assets)
                {
                    LINEF("#include \"../{0}/{0}_{1}_load_db.h\"", Lower(type->m_type->m_name), Lower(m_env.m_game))
                }
            }

            LINE("")
            LINE("#include <cassert>")
            LINE("#include <cstring>")
            LINE("#include <type_traits>")

            LINE("")
            LINEF("using namespace {0};", m_env.m_game)
            LINE("")
            PrintConstructorMethod();
            LINE("")
            PrintMainLoadMethod();

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
                for (const auto* type : m_env.m_used_types)
                {
                    if (type->m_info && type->m_type == type->m_info->m_definition && StructureComputations(type->m_info).GetDynamicMember())
                    {
                        LINE("")
                        PrintDynamicFillMethod(*type->m_info);
                    }
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_pointer_array_reference_exists)
                {
                    LINE("")
                    PrintLoadPtrArrayMethod(type->m_type, type->m_info, type->m_pointer_array_reference_is_reusable);
                }
            }
            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_array_reference_exists && type->m_info && !type->m_info->m_is_leaf && type->m_non_runtime_reference_exists)
                {
                    LINE("")
                    PrintLoadArrayMethod(type->m_type, type->m_info);
                }
            }
            for (const auto* type : m_env.m_used_structures)
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
            PrintLoadPtrMethod(m_env.m_asset);
            LINE("")
            PrintLoadAssetMethod(m_env.m_asset);
        }

    private:
        enum class MemberLoadType : std::uint8_t
        {
            ARRAY_POINTER,
            DYNAMIC_ARRAY,
            EMBEDDED,
            EMBEDDED_ARRAY,
            POINTER_ARRAY,
            SINGLE_POINTER
        };

        static std::string LoaderClassName(const StructureInformation* asset)
        {
            return std::format("Loader_{0}", asset->m_definition->m_name);
        }

        static std::string VariableDecl(const DataDefinition* def)
        {
            return std::format("{0}* var{1};", def->GetFullName(), MakeSafeTypeName(def));
        }

        static std::string PointerVariableDecl(const DataDefinition* def)
        {
            return std::format("{0}** var{1}Ptr;", def->GetFullName(), MakeSafeTypeName(def));
        }

        void PrintFillStructMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void FillStruct_{0}(const ZoneStreamFillReadAccessor& fillAccessor);", MakeSafeTypeName(info->m_definition))
        }

        void PrintDynamicFillMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("size_t LoadDynamicFill_{0}(const ZoneStreamFillReadAccessor& parentFill);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderPtrArrayLoadMethodDeclaration(const DataDefinition* def) const
        {
            LINEF("void LoadPtrArray_{0}(bool atStreamStart, size_t count);", MakeSafeTypeName(def))
        }

        void PrintHeaderArrayLoadMethodDeclaration(const DataDefinition* def) const
        {
            LINEF("void LoadArray_{0}(bool atStreamStart, size_t count);", MakeSafeTypeName(def))
        }

        void PrintHeaderLoadMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void Load_{0}(bool atStreamStart);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderTempPtrLoadMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void LoadPtr_{0}(bool atStreamStart);", MakeSafeTypeName(info->m_definition))
        }

        void PrintHeaderAssetLoadMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("void LoadAsset_{0}({1}** pAsset);", MakeSafeTypeName(info->m_definition), info->m_definition->GetFullName())
        }

        void PrintHeaderMainLoadMethodDeclaration(const StructureInformation* info) const
        {
            LINEF("XAssetInfo<{0}>* Load({0}** pAsset);", info->m_definition->GetFullName())
        }

        void PrintHeaderConstructor() const
        {
            LINEF("{0}(Zone& zone, ZoneInputStream& stream);", LoaderClassName(m_env.m_asset))
        }

        void PrintVariableInitialization(const DataDefinition* def) const
        {
            LINEF("var{0} = nullptr;", def->m_name)
        }

        void PrintPointerVariableInitialization(const DataDefinition* def) const
        {
            LINEF("var{0}Ptr = nullptr;", def->m_name)
        }

        void PrintConstructorMethod()
        {
            LINEF("{0}::{0}(Zone& zone, ZoneInputStream& stream)", LoaderClassName(m_env.m_asset))

            m_intendation++;
            LINE_STARTF(": AssetLoader({0}::EnumEntry, zone, stream)", m_env.m_asset->m_asset_name)
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

            for (const auto* type : m_env.m_used_types)
            {
                if (type->m_info && !type->m_info->m_definition->m_anonymous
                    && (!type->m_info->m_is_leaf || !type->m_info->m_has_matching_cross_platform_structure) && !StructureComputations(type->m_info).IsAsset())
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

        void PrintFillStruct_Member_DynamicArray(const StructureInformation& structInfo,
                                                 const MemberInformation& memberInfo,
                                                 const DeclarationModifierComputations& modifier,
                                                 const size_t nestedBaseOffset)
        {

            LINEF("const auto dynamicArraySize = static_cast<size_t>({0});", MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))
            LINE("if (dynamicArraySize > 0)")
            LINE("{")
            m_intendation++;

            const auto callFillForMember = memberInfo.m_type && !memberInfo.m_type->m_has_matching_cross_platform_structure;

            if (callFillForMember)
            {
                LINEF("{0} = &{1}[0];", MakeTypeVarName(memberInfo.m_member->m_type_declaration->m_type), MakeMemberAccess(&structInfo, &memberInfo, modifier))
                LINEF("FillStruct_{0}(fillAccessor.AtOffset({1}));",
                      MakeSafeTypeName(memberInfo.m_member->m_type_declaration->m_type),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
            }
            else
            {
                LINEF("fillAccessor.Fill({0}[0], {1});", MakeMemberAccess(&structInfo, &memberInfo, modifier), memberInfo.m_member->m_offset)
            }

            LINEF("for (auto i = 1uz; i < dynamicArraySize; i++)", structInfo.m_definition->m_name, memberInfo.m_member->m_name)
            LINE("{")
            m_intendation++;
            if (callFillForMember)
            {
                LINEF("{0} = &{1}[i];", MakeTypeVarName(memberInfo.m_member->m_type_declaration->m_type), MakeMemberAccess(&structInfo, &memberInfo, modifier))
                LINEF("FillStruct_{0}(fillAccessor.AtOffset({1} + i * {2}));",
                      MakeSafeTypeName(memberInfo.m_member->m_type_declaration->m_type),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset),
                      memberInfo.m_member->m_type_declaration->GetSize())
            }
            else
            {
                LINEF("fillAccessor.Fill({0}[i], {1} + i * {2});",
                      MakeMemberAccess(&structInfo, &memberInfo, modifier),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset),
                      memberInfo.m_member->m_type_declaration->GetSize())
            }
            m_intendation--;
            LINE("}")

            m_intendation--;
            LINE("}")
        }

        void PrintFillStruct_Member_PointerArray(const StructureInformation& structInfo,
                                                 const MemberInformation& memberInfo,
                                                 const DeclarationModifierComputations& modifier,
                                                 const size_t nestedBaseOffset)
        {
            if (modifier.IsArray())
            {
                LINEF("for (auto i = 0u; i < std::extent_v<decltype({0}::{1})>; i++)", structInfo.m_definition->m_name, memberInfo.m_member->m_name)
                LINE("{")
                m_intendation++;

                LINEF("fillAccessor.FillPtr({0}[i], {1} + {2} * i);",
                      MakeMemberAccess(&structInfo, &memberInfo, modifier),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset),
                      m_env.m_pointer_size)

                if (!StructureComputations(&structInfo).IsInTempBlock()
                    && (memberInfo.m_is_reusable || (memberInfo.m_type && StructureComputations(memberInfo.m_type).IsAsset())))
                {
                    LINEF("m_stream.AddPointerLookup(&{0}[i], fillAccessor.BlockBuffer({1} + {2} * i));",
                          MakeMemberAccess(&structInfo, &memberInfo, modifier),
                          OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset),
                          m_env.m_pointer_size)
                }

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINEF("fillAccessor.FillPtr({0}, {1});",
                      MakeMemberAccess(&structInfo, &memberInfo, modifier),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
            }
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

        // nestedBaseOffset: Base offset in case member is part of a nested anonymous sub-struct
        void PrintFillStruct_Member(const StructureInformation& structInfo,
                                    const MemberInformation& memberInfo,
                                    const DeclarationModifierComputations& modifier,
                                    const size_t nestedBaseOffset)
        {
            if (modifier.IsDynamicArray())
            {
                PrintFillStruct_Member_DynamicArray(structInfo, memberInfo, modifier, nestedBaseOffset);
            }
            else if (modifier.IsSinglePointer() || modifier.IsArrayPointer())
            {
                LINEF("fillAccessor.FillPtr({0}, {1});",
                      MakeMemberAccess(&structInfo, &memberInfo, modifier),
                      OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))

                if (!StructureComputations(&structInfo).IsInTempBlock()
                    && (memberInfo.m_is_reusable || (memberInfo.m_type && StructureComputations(memberInfo.m_type).IsAsset())))
                {
                    LINEF("m_stream.AddPointerLookup(&{0}, fillAccessor.BlockBuffer({1}));",
                          MakeMemberAccess(&structInfo, &memberInfo, modifier),
                          OffsetForMemberModifier(memberInfo, modifier, nestedBaseOffset))
                }
            }
            else if (modifier.IsPointerArray())
            {
                PrintFillStruct_Member_PointerArray(structInfo, memberInfo, modifier, nestedBaseOffset);
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

        void PrintFillStruct_Member_Condition_Struct(const StructureInformation& structInfo, const MemberInformation& member)
        {
            if (member.m_condition)
            {
                LINEF("if ({0})", MakeEvaluation(member.m_condition.get()))
                LINE("{")
                m_intendation++;

                PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);

                m_intendation--;
                LINE("}")
            }
            else
            {

                PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);
            }
        }

        void PrintFillStruct_Member_Condition_Union(const StructureInformation& structInfo, const MemberInformation& member)
        {
            const MemberComputations computations(&member);

            if (computations.IsFirstUsedMember())
            {
                if (member.m_condition)
                {
                    LINEF("if ({0})", MakeEvaluation(member.m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);
                }
            }
            else if (computations.IsLastUsedMember())
            {
                if (member.m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member.m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINE("else")
                    LINE("{")
                    m_intendation++;

                    PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);

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

                    PrintFillStruct_Member(structInfo, member, DeclarationModifierComputations(&member), 0u);

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
            if (info.m_reusable_reference_exists)
            {
                LINEF("m_stream.AddPointerLookup({0}, fillAccessor.BlockBuffer(0));", MakeTypeVarName(info.m_definition))
                LINE("")
            }

            const auto* dynamicMember = StructureComputations(&info).GetDynamicMember();

            if (dynamicMember)
            {
                if (info.m_definition->GetType() == DataDefinitionType::UNION)
                {
                    for (const auto& member : info.m_ordered_members)
                    {
                        const MemberComputations computations(member.get());
                        if (computations.ShouldIgnore())
                            continue;

                        PrintFillStruct_Member_Condition_Union(info, *member);
                    }
                }
                else
                {
                    for (const auto& member : info.m_ordered_members)
                    {
                        const MemberComputations computations(member.get());
                        if (computations.ShouldIgnore() || member.get() == dynamicMember)
                            continue;

                        PrintFillStruct_Member(info, *member, DeclarationModifierComputations(member.get()), 0u);
                    }

                    PrintFillStruct_Member_Condition_Struct(info, *dynamicMember);
                }
            }
            else
            {
                for (const auto& member : info.m_ordered_members)
                {
                    const MemberComputations computations(member.get());
                    if (computations.ShouldIgnore())
                        continue;

                    PrintFillStruct_Member(info, *member, DeclarationModifierComputations(member.get()), 0u);
                }
            }
        }

        void PrintFillStructMethod(const StructureInformation* info)
        {
            LINEF("void {0}::FillStruct_{1}(const ZoneStreamFillReadAccessor& fillAccessor)",
                  LoaderClassName(m_env.m_asset),
                  MakeSafeTypeName(info->m_definition))

            LINE("{")
            m_intendation++;

            PrintFillStruct_Struct(*info);

            m_intendation--;
            LINE("}")
        }

        void PrintDynamicOversize_DynamicMember(const StructureInformation& info, const MemberInformation& member) const
        {
            const MemberComputations memberComputations(&member);
            const DeclarationModifierComputations modifier(&member);

            if (memberComputations.HasDynamicArraySize())
            {
                LINEF("const auto dynamicArrayEntries = static_cast<size_t>({0});", MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))
                LINEF("m_stream.AppendToFill(dynamicArrayEntries * {0});", member.m_member->m_type_declaration->m_type->GetSize())
                LINEF("return dynamicArrayEntries * sizeof({0}{1}) + offsetof({2}, {3});",
                      MakeTypeDecl(member.m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetAllDeclarationModifiers()),
                      info.m_definition->GetFullName(),
                      member.m_member->m_name)
            }
            else if (member.m_type && StructureComputations(member.m_type).GetDynamicMember())
            {
                LINEF("return LoadDynamicFill_{0}(fillAccessor.AtOffset({1})) + offsetof({2}, {3});",
                      MakeSafeTypeName(member.m_type->m_definition),
                      member.m_member->m_offset,
                      info.m_definition->GetFullName(),
                      member.m_member->m_name)
            }
            else
            {
                LINEF("m_stream.AppendToFill({0});", member.m_member->m_type_declaration->GetSize())
                LINEF("return sizeof({0}{1}) + offsetof({2}, {3});",
                      MakeTypeDecl(member.m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetAllDeclarationModifiers()),
                      info.m_definition->GetFullName(),
                      member.m_member->m_name)
            }
        }

        void DynamicOverSize_Struct_Condition(const StructureInformation& info, const MemberInformation& member)
        {
            if (member.m_condition)
            {
                LINEF("if ({0})", MakeEvaluation(member.m_condition.get()))
                LINE("{")
                m_intendation++;

                PrintFillStruct_Member(info, member, DeclarationModifierComputations(&member), 0u);

                m_intendation--;
                LINE("}")
            }
            else
            {

                PrintFillStruct_Member(info, member, DeclarationModifierComputations(&member), 0u);
            }
        }

        void PrintDynamicOversize_Struct(const StructureInformation& info)
        {
            const StructureComputations structureComputations(&info);
            const auto dynamicMember = structureComputations.GetDynamicMember();

            LINEF("const auto fillAccessor = m_stream.AppendToFill({0}).AtOffset(parentFill.Offset());", dynamicMember->m_member->m_offset)
            LINE("")

            for (const auto& member : info.m_ordered_members)
            {
                const MemberComputations computations(member.get());
                if (computations.ShouldIgnore() || computations.IsDynamicMember())
                    continue;

                DynamicOverSize_Struct_Condition(info, *member);
            }

            LINE("")

            PrintDynamicOversize_DynamicMember(info, *dynamicMember);
        }

        void PrintDynamicOversize_Union(const StructureInformation& info)
        {
            LINE("const auto& fillAccessor = parentFill;")

            for (const auto& member : info.m_ordered_members)
            {
                const MemberComputations computations(member.get());
                if (computations.ShouldIgnore())
                    continue;

                if (computations.IsFirstUsedMember())
                {
                    LINE("")
                    if (member->m_condition)
                    {
                        LINEF("if ({0})", MakeEvaluation(member->m_condition.get()))
                        LINE("{")
                        m_intendation++;

                        PrintDynamicOversize_DynamicMember(info, *member);

                        m_intendation--;
                        LINE("}")
                    }
                    else
                    {
                        PrintDynamicOversize_DynamicMember(info, *member);
                    }
                }
                else if (computations.IsLastUsedMember())
                {
                    if (member->m_condition)
                    {
                        LINEF("else if ({0})", MakeEvaluation(member->m_condition.get()))
                        LINE("{")
                        m_intendation++;

                        PrintDynamicOversize_DynamicMember(info, *member);

                        m_intendation--;
                        LINE("}")
                    }
                    else
                    {
                        LINE("else")
                        LINE("{")
                        m_intendation++;

                        PrintDynamicOversize_DynamicMember(info, *member);

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

                        PrintDynamicOversize_DynamicMember(info, *member);

                        m_intendation--;
                        LINE("}")
                    }
                    else
                    {
                        LINEF("#error Middle member of union must have condition ({0})", member->m_member->m_name)
                    }
                }
            }
        }

        void PrintDynamicFillMethod(const StructureInformation& info)
        {
            LINEF("size_t {0}::LoadDynamicFill_{1}(const ZoneStreamFillReadAccessor& parentFill)",
                  LoaderClassName(m_env.m_asset),
                  MakeSafeTypeName(info.m_definition))

            LINE("{")
            m_intendation++;

            LINEF("{0} temp{1};", info.m_definition->GetFullName(), MakeSafeTypeName(info.m_definition))
            LINEF("{0} = &temp{1};", MakeTypeVarName(info.m_definition), MakeSafeTypeName(info.m_definition))

            if (info.m_definition->GetType() == DataDefinitionType::UNION)
            {
                PrintDynamicOversize_Union(info);
            }
            else
            {
                PrintDynamicOversize_Struct(info);
            }

            m_intendation--;
            LINE("}")
        }

        void PrintLoadPtrArrayMethod_Loading(const DataDefinition* def, const StructureInformation* info) const
        {
            if (info && !info->m_has_matching_cross_platform_structure && StructureComputations(info).GetDynamicMember())
            {
                LINE("// Alloc first for alignment, then proceed to read as game does")
                LINEF("m_stream.Alloc({0});", def->GetAlignment())
                LINEF("const auto allocSize = LoadDynamicFill_{0}(m_stream.LoadWithFill(0));", MakeSafeTypeName(def))
                LINEF("*{0} = static_cast<{1}*>(m_stream.AllocOutOfBlock(0, allocSize));", MakeTypePtrVarName(def), def->GetFullName())
            }
            else
            {
                LINEF("*{0} = m_stream.{1}<{2}>({3});",
                      MakeTypePtrVarName(def),
                      m_env.m_architecture_mismatch ? "AllocOutOfBlock" : "Alloc",
                      def->GetFullName(),
                      def->GetAlignment())
            }

            if (info && !info->m_is_leaf)
            {
                LINEF("{0} = *{1};", MakeTypeVarName(info->m_definition), MakeTypePtrVarName(def))
                LINEF("Load_{0}(true);", MakeSafeTypeName(def))
            }
            else
            {
                LINEF("m_stream.Load<{0}>(*{1});", def->GetFullName(), MakeTypePtrVarName(def))
            }
        }

        void PrintLoadPtrArrayMethod_PointerCheck(const DataDefinition* def, const StructureInformation* info, const bool reusable)
        {
            LINEF("if (*{0})", MakeTypePtrVarName(def))
            LINE("{")
            m_intendation++;

            if (info && StructureComputations(info).IsAsset())
            {
                LINEF("{0} loader(m_zone, m_stream);", LoaderClassName(info))
                LINEF("loader.Load({0});", MakeTypePtrVarName(def))
            }
            else
            {
                if (reusable)
                {
                    LINEF("const auto zonePtrType = GetZonePointerType(*{0});", MakeTypePtrVarName(def))
                    LINEF("if (zonePtrType == ZonePointerType::FOLLOWING)", MakeTypePtrVarName(def))
                    LINE("{")
                    m_intendation++;

                    PrintLoadPtrArrayMethod_Loading(def, info);

                    m_intendation--;
                    LINE("}")
                    LINE("else")
                    LINE("{")
                    m_intendation++;

                    if (info && !info->m_has_matching_cross_platform_structure)
                    {
                        LINEF("*{0} = m_stream.ConvertOffsetToPointerLookup(*{0}).Expect();", MakeTypePtrVarName(def))
                    }
                    else
                    {
                        LINEF("*{0} = m_stream.ConvertOffsetToPointerNative(*{0});", MakeTypePtrVarName(def))
                    }

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

        void PrintLoadPtrArrayMethod(const DataDefinition* def, const StructureInformation* info, const bool reusable)
        {
            LINEF("void {0}::LoadPtrArray_{1}(const bool atStreamStart, const size_t count)", LoaderClassName(m_env.m_asset), MakeSafeTypeName(def))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypePtrVarName(def))
            LINE("")

            LINE("if (atStreamStart)")

            if (m_env.m_architecture_mismatch)
            {
                LINE("{")
                m_intendation++;
                LINEF("const auto ptrArrayFill = m_stream.LoadWithFill({0} * count);", m_env.m_pointer_size)
                LINE("for (size_t index = 0; index < count; index++)")
                LINE("{")
                m_intendation++;
                LINEF("ptrArrayFill.FillPtr({0}[index], {1} * index);", MakeTypePtrVarName(def), m_env.m_pointer_size)

                if (reusable || (info && StructureComputations(info).IsAsset()))
                {
                    LINEF("m_stream.AddPointerLookup(&{0}[index], ptrArrayFill.BlockBuffer({1} * index));", MakeTypePtrVarName(def), m_env.m_pointer_size)
                }

                m_intendation--;
                LINE("}")
                m_intendation--;
                LINE("}")
            }
            else
            {
                m_intendation++;
                LINEF("m_stream.Load<{0}*>({1}, count);", def->GetFullName(), MakeTypePtrVarName(def))
                m_intendation--;
            }

            LINE("")
            LINEF("{0}** var = {1};", def->GetFullName(), MakeTypePtrVarName(def))
            LINE("for (size_t index = 0; index < count; index++)")
            LINE("{")
            m_intendation++;

            LINEF("{0} = var;", MakeTypePtrVarName(def))
            PrintLoadPtrArrayMethod_PointerCheck(def, info, reusable);
            LINE("")
            LINE("var++;")

            m_intendation--;
            LINE("}")
            m_intendation--;
            LINE("}")
        }

        void PrintLoadArrayMethod(const DataDefinition* def, const StructureInformation* info)
        {
            LINEF("void {0}::LoadArray_{1}(const bool atStreamStart, const size_t count)", LoaderClassName(m_env.m_asset), MakeSafeTypeName(def))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypeVarName(def))
            LINE("")
            LINE("if (atStreamStart)")

            if (info->m_has_matching_cross_platform_structure)
            {
                m_intendation++;
                LINEF("m_stream.Load<{0}>({1}, count);", info->m_definition->GetFullName(), MakeTypeVarName(def))
                m_intendation--;
            }
            else
            {
                LINE("{")
                m_intendation++;

                LINEF("const auto arrayFill = m_stream.LoadWithFill({0} * count);", def->GetSize())
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
            LINEF("auto* var = {0};", MakeTypeVarName(def))
            LINE("for (size_t index = 0; index < count; index++)")
            LINE("{")
            m_intendation++;

            LINEF("{0} = var;", MakeTypeVarName(info->m_definition))
            LINEF("Load_{0}(false);", info->m_definition->m_name)
            LINE("var++;")

            m_intendation--;
            LINE("}")

            m_intendation--;
            LINE("}")
        }

        void LoadMember_Asset(const StructureInformation* info,
                              const MemberInformation* member,
                              const DeclarationModifierComputations& modifier,
                              const MemberLoadType loadType) const
        {
            if (loadType == MemberLoadType::SINGLE_POINTER)
            {
                LINEF("{0} loader(m_zone, m_stream);", LoaderClassName(member->m_type))
                LINEF("loader.Load(&{0});", MakeMemberAccess(info, member, modifier))
            }
            else if (loadType == MemberLoadType::POINTER_ARRAY)
            {
                LoadMember_PointerArray(info, member, modifier);
            }
            else
            {
                assert(false);
                LINEF("#error unsupported loadType {0} for asset", static_cast<int>(loadType))
            }
        }

        void LoadMember_String(const StructureInformation* info,
                               const MemberInformation* member,
                               const DeclarationModifierComputations& modifier,
                               const MemberLoadType loadType) const
        {
            if (loadType == MemberLoadType::SINGLE_POINTER)
            {
                if (member->m_member->m_type_declaration->m_is_const)
                {
                    LINEF("varXString = &{0};", MakeMemberAccess(info, member, modifier))
                }
                else
                {
                    LINEF("varXString = const_cast<const char**>(&{0});", MakeMemberAccess(info, member, modifier))
                }
                LINE("LoadXString(false);")
            }
            else if (loadType == MemberLoadType::POINTER_ARRAY)
            {
                LINEF("varXString = {0};", MakeMemberAccess(info, member, modifier))
                if (modifier.IsArray())
                {
                    LINEF("LoadXStringArray(false, {0});", modifier.GetArraySize())
                }
                else
                {
                    LINEF("LoadXStringArray(true, {0});", MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))
                }
            }
            else
            {
                assert(false);
                LINEF("#error unsupported loadType {0} for string", static_cast<int>(loadType))
            }
        }

        void LoadMember_ArrayPointer(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
        {
            const MemberComputations computations(member);
            if (member->m_type && !member->m_type->m_is_leaf && !computations.IsInRuntimeBlock())
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("LoadArray_{0}(true, {1});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))

                if (member->m_type->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_type->m_post_load_action.get()))
                }

                if (member->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_post_load_action.get()))
                }
            }
            else if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
            {
                LINEF("const auto fillArraySize = static_cast<size_t>({0});", MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
                LINEF("const auto fill = m_stream.LoadWithFill({0} * fillArraySize);", member->m_member->m_type_declaration->m_type->GetSize())
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
                LINEF("m_stream.Load<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
            }
        }

        void LoadMember_PointerArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            LINEF("{0} = {1};", MakeTypePtrVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
            if (modifier.IsArray())
            {
                LINEF("LoadPtrArray_{0}(false, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), modifier.GetArraySize())
            }
            else
            {
                LINEF("LoadPtrArray_{0}(true, {1});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))
            }
        }

        void LoadMember_EmbeddedArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);
            std::string arraySizeStr;

            if (modifier.HasDynamicArrayCount())
                arraySizeStr = MakeEvaluation(modifier.GetDynamicArrayCountEvaluation());
            else
                arraySizeStr = std::to_string(modifier.GetArraySize());

            if (!member->m_is_leaf)
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))

                if (computations.IsAfterPartialLoad() && !m_env.m_architecture_mismatch)
                {
                    LINEF("LoadArray_{0}(true, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), arraySizeStr)
                }
                else
                {
                    LINEF("LoadArray_{0}(false, {1});", MakeSafeTypeName(member->m_member->m_type_declaration->m_type), arraySizeStr)
                }

                if (member->m_type->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_type->m_post_load_action.get()))
                }

                if (member->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_post_load_action.get()))
                }
            }
            else if (computations.IsAfterPartialLoad() && !m_env.m_architecture_mismatch)
            {
                LINEF("m_stream.Load<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      arraySizeStr)
            }
        }

        void LoadMember_DynamicArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
        {
            if (member->m_type && !member->m_type->m_is_leaf)
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("LoadArray_{0}({1}, {2});",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      m_env.m_architecture_mismatch ? "false" : "true",
                      MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))
            }
            else if (info->m_has_matching_cross_platform_structure)
            {
                if (m_env.m_architecture_mismatch)
                {
                    LINE("if (atStreamStart)")
                    m_intendation++;
                }

                LINEF("m_stream.Load<{0}{1}>({2}, {3});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier),
                      MakeEvaluation(modifier.GetDynamicArraySizeEvaluation()))

                if (m_env.m_architecture_mismatch)
                {
                    m_intendation--;
                }
            }
        }

        void LoadMember_Embedded(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);
            if (!member->m_is_leaf)
            {
                LINEF("{0} = &{1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))

                if (computations.IsAfterPartialLoad() && !m_env.m_architecture_mismatch)
                {
                    LINEF("Load_{0}(true);", MakeSafeTypeName(member->m_member->m_type_declaration->m_type))
                }
                else
                {
                    LINEF("Load_{0}(false);", MakeSafeTypeName(member->m_member->m_type_declaration->m_type))
                }

                if (member->m_type->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_type->m_post_load_action.get()))
                }

                if (member->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_post_load_action.get()))
                }
            }
            else if (computations.IsAfterPartialLoad() && !m_env.m_architecture_mismatch)
            {
                LINEF("m_stream.Load<{0}{1}>(&{2});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier))
            }
        }

        void LoadMember_SinglePointer(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier) const
        {
            const MemberComputations computations(member);
            if (member->m_type && !member->m_type->m_is_leaf && !computations.IsInRuntimeBlock())
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))

                LINEF("Load_{0}(true);", MakeSafeTypeName(member->m_type->m_definition))

                if (member->m_type->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_type->m_post_load_action.get()))
                }

                if (member->m_post_load_action)
                {
                    LINE("")
                    LINE(MakeCustomActionCall(member->m_post_load_action.get()))
                }
            }
            else if (member->m_type && !member->m_type->m_has_matching_cross_platform_structure)
            {
                LINEF("{0} = {1};", MakeTypeVarName(member->m_member->m_type_declaration->m_type), MakeMemberAccess(info, member, modifier))
                LINEF("FillStruct_{0}(m_stream.LoadWithFill({1}));",
                      MakeSafeTypeName(member->m_member->m_type_declaration->m_type),
                      member->m_member->m_type_declaration->m_type->GetSize())
            }
            else
            {
                LINEF("m_stream.Load<{0}{1}>({2});",
                      MakeTypeDecl(member->m_member->m_type_declaration.get()),
                      MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()),
                      MakeMemberAccess(info, member, modifier))
            }
        }

        void LoadMember_TypeCheck(const StructureInformation* info,
                                  const MemberInformation* member,
                                  const DeclarationModifierComputations& modifier,
                                  const MemberLoadType loadType)
        {
            if (member->m_is_string)
            {
                LoadMember_String(info, member, modifier, loadType);
            }
            else if (member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                LoadMember_Asset(info, member, modifier, loadType);
            }
            else
            {
                switch (loadType)
                {
                case MemberLoadType::ARRAY_POINTER:
                    LoadMember_ArrayPointer(info, member, modifier);
                    break;

                case MemberLoadType::SINGLE_POINTER:
                    LoadMember_SinglePointer(info, member, modifier);
                    break;

                case MemberLoadType::EMBEDDED:
                    LoadMember_Embedded(info, member, modifier);
                    break;

                case MemberLoadType::POINTER_ARRAY:
                    LoadMember_PointerArray(info, member, modifier);
                    break;

                case MemberLoadType::DYNAMIC_ARRAY:
                    LoadMember_DynamicArray(info, member, modifier);
                    break;

                case MemberLoadType::EMBEDDED_ARRAY:
                    LoadMember_EmbeddedArray(info, member, modifier);
                    break;

                default:
                    LINEF("// t={0}", static_cast<int>(loadType))
                    break;
                }
            }
        }

        static bool LoadMember_ShouldMakeAlloc(const MemberInformation* member, const DeclarationModifierComputations& modifier, const MemberLoadType loadType)
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

            if (member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] bool ShouldAllocOutOfBlock(const MemberInformation& member, const MemberLoadType loadType) const
        {
            return m_env.m_architecture_mismatch
                   && ((member.m_type && !member.m_type->m_has_matching_cross_platform_structure) || loadType == MemberLoadType::POINTER_ARRAY);
        }

        [[nodiscard]] bool
            ShouldPreAllocDynamic(const MemberInformation& member, const DeclarationModifierComputations& modifier, const MemberLoadType loadType) const
        {
            return ShouldAllocOutOfBlock(member, loadType) && (modifier.IsSinglePointer() || modifier.IsPointerArray()) && member.m_type
                   && StructureComputations(member.m_type).GetDynamicMember();
        }

        void LoadMember_Alloc(const StructureInformation* info,
                              const MemberInformation* member,
                              const DeclarationModifierComputations& modifier,
                              const MemberLoadType loadType)
        {
            if (!LoadMember_ShouldMakeAlloc(member, modifier, loadType))
            {
                LoadMember_TypeCheck(info, member, modifier, loadType);
                return;
            }

            const auto typeDecl = MakeTypeDecl(member->m_member->m_type_declaration.get());
            const auto followingReferences = MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers());

            const auto allocOutOfBlock = ShouldAllocOutOfBlock(*member, loadType);
            const auto preAllocDynamic = ShouldPreAllocDynamic(*member, modifier, loadType);

            if (preAllocDynamic)
            {
                LINE("// Alloc first for alignment, then proceed to read as game does")
                if (member->m_alloc_alignment)
                {
                    LINEF("m_stream.Alloc({0});", MakeEvaluation(member->m_alloc_alignment.get()))
                }
                else
                {
                    LINEF("m_stream.Alloc({0});", modifier.GetAlignment())
                }

                LINEF("const auto allocSize = LoadDynamicFill_{0}(m_stream.LoadWithFill(0));", MakeSafeTypeName(member->m_type->m_definition))

                // We do not align again, because we already did previously
                LINEF("{0} = static_cast<{1}{2}*>(m_stream.AllocOutOfBlock(0, allocSize));",
                      MakeMemberAccess(info, member, modifier),
                      typeDecl,
                      followingReferences)
            }
            else
            {
                LINE_STARTF("{0} = m_stream.", MakeMemberAccess(info, member, modifier))
                if (allocOutOfBlock)
                    LINE_MIDDLE("AllocOutOfBlock")
                else
                    LINE_MIDDLE("Alloc")

                LINE_MIDDLEF("<{0}{1}>(", typeDecl, followingReferences)

                // This used to use `alignof()` to calculate alignment but due to inconsistencies between compilers and bugs discovered in MSVC
                // (Alignment specified via `__declspec(align())` showing as correct via intellisense but is incorrect when compiled for types that have a
                // larger alignment than the specified value) this was changed to make ZoneCodeGenerator calculate what is supposed to be used as alignment when
                // allocating. This is more reliable when being used with different compilers and the value used can be seen in the source code directly
                if (member->m_alloc_alignment)
                {
                    LINE_MIDDLE(MakeEvaluation(member->m_alloc_alignment.get()))
                }
                else
                {
                    LINE_MIDDLEF("{0}", modifier.GetAlignment())
                }

                if (allocOutOfBlock && modifier.IsArrayPointer())
                    LINE_MIDDLEF(", {0}", MakeEvaluation(modifier.GetArrayPointerCountEvaluation()))
                else if (allocOutOfBlock && modifier.IsPointerArray())
                    LINE_MIDDLEF(", {0}", MakeEvaluation(modifier.GetPointerArrayCountEvaluation()))

                LINE_END(");")
            }

            const MemberComputations computations(member);
            if (computations.IsInTempBlock())
            {
                LINE("")

                if (m_env.m_architecture_mismatch)
                    LINE("uintptr_t toInsertLookupEntry = 0;")
                else
                    LINEF("{0}** toInsert = nullptr;", member->m_member->m_type_declaration->m_type->GetFullName())

                LINE("if (zonePtrType == ZonePointerType::INSERT)")
                m_intendation++;

                if (m_env.m_architecture_mismatch)
                    LINE("toInsertLookupEntry = m_stream.InsertPointerAliasLookup();")
                else
                    LINEF("toInsert = m_stream.InsertPointerNative<{0}>();", member->m_member->m_type_declaration->m_type->GetFullName())

                m_intendation--;
                LINE("")
            }

            LoadMember_TypeCheck(info, member, modifier, loadType);

            if (computations.IsInTempBlock())
            {
                LINE("")
                LINE("if (zonePtrType == ZonePointerType::INSERT)")
                m_intendation++;

                if (m_env.m_architecture_mismatch)
                    LINEF(
                        "m_stream.SetInsertedPointerAliasLookup(toInsertLookupEntry, {0}->{1});", MakeTypeVarName(info->m_definition), member->m_member->m_name)
                else
                    LINEF("*toInsert = {0}->{1};", MakeTypeVarName(info->m_definition), member->m_member->m_name)

                m_intendation--;
            }
        }

        static bool LoadMember_ShouldMakeReuse(const DeclarationModifierComputations& modifier, const MemberLoadType loadType)
        {
            if (loadType != MemberLoadType::ARRAY_POINTER && loadType != MemberLoadType::SINGLE_POINTER && loadType != MemberLoadType::POINTER_ARRAY)
            {
                return false;
            }

            if (loadType == MemberLoadType::POINTER_ARRAY && modifier.IsArray())
            {
                return false;
            }

            return true;
        }

        void LoadMember_Reuse(const StructureInformation* info,
                              const MemberInformation* member,
                              const DeclarationModifierComputations& modifier,
                              const MemberLoadType loadType)
        {
            if (!LoadMember_ShouldMakeReuse(modifier, loadType) || !member->m_is_reusable)
            {
                LoadMember_Alloc(info, member, modifier, loadType);
                return;
            }

            LINEF("const auto zonePtrType = GetZonePointerType({0});", MakeMemberAccess(info, member, modifier))

            const MemberComputations computations(member);
            if (computations.IsInTempBlock())
            {
                LINE("if (zonePtrType == ZonePointerType::FOLLOWING || zonePtrType == ZonePointerType::INSERT)")
                LINE("{")
                m_intendation++;

                LoadMember_Alloc(info, member, modifier, loadType);

                m_intendation--;
                LINE("}")
                LINE("else")
                LINE("{")
                m_intendation++;

                if (info->m_has_matching_cross_platform_structure)
                {
                    LINEF("{0} = m_stream.ConvertOffsetToAliasNative({0});", MakeMemberAccess(info, member, modifier))
                }
                else
                {
                    LINEF("{0} = m_stream.ConvertOffsetToAliasLookup({0});", MakeMemberAccess(info, member, modifier))
                }

                m_intendation--;
                LINE("}")
            }
            else
            {
                LINE("if (zonePtrType == ZonePointerType::FOLLOWING)")
                LINE("{")
                m_intendation++;

                LoadMember_Alloc(info, member, modifier, loadType);

                m_intendation--;
                LINE("}")
                LINE("else")
                LINE("{")
                m_intendation++;

                if (ShouldAllocOutOfBlock(*member, loadType))
                {
                    LINE_STARTF("{0} = m_stream.ConvertOffsetToPointerLookup({0})", MakeMemberAccess(info, member, modifier))
                    if (loadType == MemberLoadType::POINTER_ARRAY)
                    {
                        LINE_MIDDLEF(".OrNulled({0}uz * ({1}), sizeof({2}{3}) * ({1}), m_memory)",
                                     member->m_member->m_type_declaration->GetSize(),
                                     MakeEvaluation(modifier.GetPointerArrayCountEvaluation()),
                                     MakeTypeDecl(member->m_member->m_type_declaration.get()),
                                     MakeFollowingReferences(modifier.GetFollowingDeclarationModifiers()))
                    }
                    else
                    {
                        LINE_MIDDLE(".Expect()")
                    }

                    LINE_END(";")
                }
                else
                {
                    LINEF("{0} = m_stream.ConvertOffsetToPointerNative({0});", MakeMemberAccess(info, member, modifier))
                }

                m_intendation--;
                LINE("}")
            }
        }

        static bool
            LoadMember_ShouldMakePointerCheck(const MemberInformation* member, const DeclarationModifierComputations& modifier, const MemberLoadType loadType)
        {
            if (loadType != MemberLoadType::ARRAY_POINTER && loadType != MemberLoadType::POINTER_ARRAY && loadType != MemberLoadType::SINGLE_POINTER)
                return false;

            if (loadType == MemberLoadType::POINTER_ARRAY)
                return !modifier.IsArray();

            if (member->m_is_string)
                return false;

            return true;
        }

        void LoadMember_PointerCheck(const StructureInformation* info,
                                     const MemberInformation* member,
                                     const DeclarationModifierComputations& modifier,
                                     const MemberLoadType loadType)
        {
            if (LoadMember_ShouldMakePointerCheck(member, modifier, loadType))
            {
                LINEF("if ({0})", MakeMemberAccess(info, member, modifier))
                LINE("{")
                m_intendation++;

                LoadMember_Reuse(info, member, modifier, loadType);

                m_intendation--;
                LINE("}")
            }
            else
            {
                LoadMember_Reuse(info, member, modifier, loadType);
            }
        }

        void LoadMember_Block(const StructureInformation* info,
                              const MemberInformation* member,
                              const DeclarationModifierComputations& modifier,
                              const MemberLoadType loadType)
        {
            const MemberComputations computations(member);

            const auto notInDefaultNormalBlock = computations.IsNotInDefaultNormalBlock();
            if (notInDefaultNormalBlock)
            {
                LINEF("m_stream.PushBlock({0});", member->m_fast_file_block->m_name)
            }

            LoadMember_PointerCheck(info, member, modifier, loadType);

            if (notInDefaultNormalBlock)
            {
                LINE("m_stream.PopBlock();")
            }
        }

        void LoadMember_ReferenceArray(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
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

                LoadMember_Reference(info, member, entry);
            }
        }

        void LoadMember_Reference(const StructureInformation* info, const MemberInformation* member, const DeclarationModifierComputations& modifier)
        {
            if (modifier.IsDynamicArray())
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::DYNAMIC_ARRAY);
            }
            else if (modifier.IsSinglePointer())
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::SINGLE_POINTER);
            }
            else if (modifier.IsArrayPointer())
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::ARRAY_POINTER);
            }
            else if (modifier.IsPointerArray())
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::POINTER_ARRAY);
            }
            else if (modifier.IsArray() && modifier.GetNextDeclarationModifier() == nullptr)
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::EMBEDDED_ARRAY);
            }
            else if (modifier.GetDeclarationModifier() == nullptr)
            {
                LoadMember_Block(info, member, modifier, MemberLoadType::EMBEDDED);
            }
            else if (modifier.IsArray())
            {
                LoadMember_ReferenceArray(info, member, modifier);
            }
            else
            {
                assert(false);
                LINEF("#error LoadMemberReference failed @ {0}", member->m_member->m_name)
            }
        }

        void LoadMember_Condition_Struct(const StructureInformation* info, const MemberInformation* member)
        {
            LINE("")
            if (member->m_condition)
            {
                LINEF("if ({0})", MakeEvaluation(member->m_condition.get()))
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

        void LoadMember_Condition_Union(const StructureInformation* info, const MemberInformation* member)
        {
            const MemberComputations computations(member);

            if (computations.IsFirstUsedMember())
            {
                LINE("")
                if (member->m_condition)
                {
                    LINEF("if ({0})", MakeEvaluation(member->m_condition.get()))
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
            else if (computations.IsLastUsedMember())
            {
                if (member->m_condition)
                {
                    LINEF("else if ({0})", MakeEvaluation(member->m_condition.get()))
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
                    LINEF("else if ({0})", MakeEvaluation(member->m_condition.get()))
                    LINE("{")
                    m_intendation++;

                    LoadMember_Reference(info, member, DeclarationModifierComputations(member));

                    m_intendation--;
                    LINE("}")
                }
                else
                {
                    LINEF("#error Middle member of union must have condition ({0})", member->m_member->m_name)
                }
            }
        }

        void PrintLoadMemberIfNeedsTreatment(const StructureInformation* info, const MemberInformation* member)
        {
            const MemberComputations computations(member);
            if (computations.ShouldIgnore())
                return;

            if (member->m_is_string || computations.ContainsNonEmbeddedReference() || member->m_type && !member->m_type->m_is_leaf
                || computations.IsAfterPartialLoad())
            {
                if (info->m_definition->GetType() == DataDefinitionType::UNION)
                    LoadMember_Condition_Union(info, member);
                else
                    LoadMember_Condition_Struct(info, member);
            }
        }

        void PrintLoadMethod(const StructureInformation* info)
        {
            const StructureComputations computations(info);
            LINEF("void {0}::Load_{1}(const bool atStreamStart)", LoaderClassName(m_env.m_asset), info->m_definition->m_name)

            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypeVarName(info->m_definition))

            const auto* dynamicMember = computations.GetDynamicMember();
            if (!info->m_has_matching_cross_platform_structure && dynamicMember && !info->m_non_embedded_reference_exists)
            {
                LINE("assert(!atStreamStart);")
            }
            else if (!(info->m_definition->GetType() == DataDefinitionType::UNION && dynamicMember))
            {
                LINE("")
                LINE("if (atStreamStart)")

                m_intendation++;
                if (info->m_has_matching_cross_platform_structure)
                {
                    if (dynamicMember == nullptr)
                    {
                        LINEF("m_stream.Load<{0}>({1}); // Size: {2}",
                              info->m_definition->GetFullName(),
                              MakeTypeVarName(info->m_definition),
                              info->m_definition->GetSize())
                    }
                    else
                    {
                        LINEF("m_stream.LoadPartial<{0}>({1}, offsetof({0}, {2}));",
                              info->m_definition->GetFullName(),
                              MakeTypeVarName(info->m_definition),
                              dynamicMember->m_member->m_name)
                    }
                }
                else
                {
                    if (dynamicMember == nullptr)
                    {
                        LINEF("FillStruct_{0}(m_stream.LoadWithFill({1}));", MakeSafeTypeName(info->m_definition), info->m_definition->GetSize())
                    }
                    else if (info->m_non_embedded_reference_exists)
                    {
                        LINEF("FillStruct_{0}(m_stream.GetLastFill());", MakeSafeTypeName(info->m_definition))
                    }
                    else
                    {
                        LINE("assert(false);")
                    }
                }
                m_intendation--;
            }
            else if (!m_env.m_architecture_mismatch)
            {
                LINE("assert(atStreamStart);")
            }

            if (computations.IsAsset())
            {
                LINE("")
                LINEF("m_stream.PushBlock({0});", m_env.m_default_normal_block->m_name)
            }
            else if (info->m_block)
            {
                LINE("")
                LINEF("m_stream.PushBlock({0});", info->m_block->m_name)
            }

            for (const auto& member : info->m_ordered_members)
            {
                PrintLoadMemberIfNeedsTreatment(info, member.get());
            }

            if (info->m_block || computations.IsAsset())
            {
                LINE("")
                LINE("m_stream.PopBlock();")
            }

            m_intendation--;
            LINE("}")
        }

        void PrintLoadPtrMethod(const StructureInformation* info)
        {
            const bool inTemp = info->m_block && info->m_block->m_type == FastFileBlockType::TEMP;
            LINEF("void {0}::LoadPtr_{1}(const bool atStreamStart)", LoaderClassName(m_env.m_asset), MakeSafeTypeName(info->m_definition))
            LINE("{")
            m_intendation++;

            LINEF("assert({0} != nullptr);", MakeTypePtrVarName(info->m_definition))
            LINE("")

            if (!m_env.m_architecture_mismatch)
            {
                LINE("if (atStreamStart)")
                m_intendation++;
                LINEF("m_stream.Load<{0}*>({1});", info->m_definition->GetFullName(), MakeTypePtrVarName(info->m_definition))
                m_intendation--;
            }
            else
            {
                LINE("assert(!atStreamStart);")
            }

            LINE("")
            if (inTemp)
            {
                LINEF("m_stream.PushBlock({0});", m_env.m_default_temp_block->m_name)
                LINE("")
            }

            LINEF("if (*{0} != nullptr)", MakeTypePtrVarName(info->m_definition))
            LINE("{")
            m_intendation++;

            LINEF("const auto zonePtrType = GetZonePointerType(*{0});", MakeTypePtrVarName(info->m_definition))
            if (inTemp)
            {
                LINEF("if (zonePtrType == ZonePointerType::FOLLOWING || zonePtrType == ZonePointerType::INSERT)", MakeTypePtrVarName(info->m_definition))
            }
            else
            {
                LINEF("if (zonePtrType == ZonePointerType::FOLLOWING)", MakeTypePtrVarName(info->m_definition))
            }
            LINE("{")
            m_intendation++;

            LINEF("*{0} = m_stream.{1}<{2}>({3});",
                  MakeTypePtrVarName(info->m_definition),
                  m_env.m_architecture_mismatch ? "AllocOutOfBlock" : "Alloc",
                  info->m_definition->GetFullName(),
                  info->m_definition->GetAlignment())

            if (inTemp)
            {
                LINE("")

                if (m_env.m_architecture_mismatch)
                    LINE("uintptr_t toInsertLookupEntry = 0;")
                else
                    LINEF("{0}** toInsert = nullptr;", info->m_definition->GetFullName())

                LINE("if (zonePtrType == ZonePointerType::INSERT)")
                m_intendation++;

                if (m_env.m_architecture_mismatch)
                    LINE("toInsertLookupEntry = m_stream.InsertPointerAliasLookup();")
                else
                    LINEF("toInsert = m_stream.InsertPointerNative<{0}>();", info->m_definition->GetFullName())

                m_intendation--;
            }

            auto startLoadSection = true;

            if (!info->m_is_leaf)
            {
                if (startLoadSection)
                {
                    startLoadSection = false;
                    LINE("")
                }
                LINEF("{0} = *{1};", MakeTypeVarName(info->m_definition), MakeTypePtrVarName(info->m_definition))
                LINEF("Load_{0}(true);", MakeSafeTypeName(info->m_definition))
            }
            else
            {
                LINE("#error Ptr method cannot have leaf type")
            }

            if (info->m_post_load_action)
            {
                LINE("")
                LINE(MakeCustomActionCall(info->m_post_load_action.get()))
            }

            if (StructureComputations(info).IsAsset())
            {
                LINE("")
                LINEF("LoadAsset_{0}({1});", MakeSafeTypeName(info->m_definition), MakeTypePtrVarName(info->m_definition))
            }

            if (inTemp)
            {
                if (!startLoadSection)
                {
                    LINE("")
                }

                LINE("if (zonePtrType == ZonePointerType::INSERT)")
                m_intendation++;

                if (m_env.m_architecture_mismatch)
                    LINEF("m_stream.SetInsertedPointerAliasLookup(toInsertLookupEntry, *{0});", MakeTypePtrVarName(info->m_definition))
                else
                    LINEF("*toInsert = *{0};", MakeTypePtrVarName(info->m_definition))

                m_intendation--;
            }

            m_intendation--;
            LINE("}")
            LINE("else")
            LINE("{")
            m_intendation++;

            if (inTemp)
            {
                if (info->m_has_matching_cross_platform_structure)
                {
                    LINEF("*{0} = m_stream.ConvertOffsetToAliasNative(*{0});", MakeTypePtrVarName(info->m_definition))
                }
                else
                {
                    LINEF("*{0} = m_stream.ConvertOffsetToAliasLookup(*{0});", MakeTypePtrVarName(info->m_definition))
                }
            }
            else
            {
                LINEF("*{0} = m_stream.ConvertOffsetToPointerNative(*{0});", MakeTypePtrVarName(info->m_definition))
            }

            m_intendation--;
            LINE("}")

            m_intendation--;
            LINE("}")

            if (inTemp)
            {
                LINE("")
                LINE("m_stream.PopBlock();")
            }

            m_intendation--;
            LINE("}")
        }

        void PrintLoadAssetMethod(const StructureInformation* info)
        {
            LINEF("void {0}::LoadAsset_{1}({2}** pAsset)",
                  LoaderClassName(m_env.m_asset),
                  MakeSafeTypeName(info->m_definition),
                  info->m_definition->GetFullName())
            LINE("{")
            m_intendation++;

            LINE("assert(pAsset != nullptr);")
            LINE("")
            LINE("AssetInfoCollector assetInfo(m_zone);")
            LINEF("AssetMarker<{0}> marker(assetInfo);", m_env.m_asset->m_asset_name)
            LINE("marker.Mark(*pAsset);")
            LINE("")
            LINEF("auto* reallocatedAsset = m_zone.Memory().Alloc<{0}>();", info->m_definition->GetFullName())
            LINEF("std::memcpy(reallocatedAsset, *pAsset, sizeof({0}));", info->m_definition->GetFullName())
            LINE("")
            LINEF("m_asset_info = reinterpret_cast<XAssetInfo<{0}>*>(LinkAsset(AssetName<{1}>(**pAsset), reallocatedAsset, "
                  "assetInfo.GetDependencies(), "
                  "assetInfo.GetUsedScriptStrings(), assetInfo.GetIndirectAssetReferences()));",
                  info->m_definition->GetFullName(),
                  info->m_asset_name)
            LINE("*pAsset = m_asset_info->Asset();")

            m_intendation--;
            LINE("}")
        }

        void PrintMainLoadMethod()
        {
            LINEF("XAssetInfo<{0}>* {1}::Load({0}** pAsset)", m_env.m_asset->m_definition->GetFullName(), LoaderClassName(m_env.m_asset))
            LINE("{")
            m_intendation++;

            LINE("assert(pAsset != nullptr);")
            LINE("")
            LINE("m_asset_info = nullptr;")
            LINE("")
            LINEF("{0} = pAsset;", MakeTypePtrVarName(m_env.m_asset->m_definition))
            LINEF("LoadPtr_{0}(false);", MakeSafeTypeName(m_env.m_asset->m_definition))
            LINE("")
            LINE("if (m_asset_info == nullptr && *pAsset != nullptr)")
            m_intendation++;
            LINEF("m_asset_info = reinterpret_cast<XAssetInfo<{0}>*>(GetAssetInfo(AssetName<{1}>(**pAsset)));",
                  m_env.m_asset->m_definition->GetFullName(),
                  m_env.m_asset->m_asset_name)
            m_intendation--;
            LINE("")
            LINE("return m_asset_info;")

            m_intendation--;
            LINE("}")
        }

        const OncePerAssetRenderingContext& m_env;
    };
} // namespace

std::vector<CodeTemplateFile> ZoneLoadTemplate::GetFilesToRenderOncePerTemplate(const OncePerTemplateRenderingContext& context)
{
    std::vector<CodeTemplateFile> files;

    files.emplace_back(std::format("AssetLoader{0}.h", context.m_game), TAG_ALL_LOADERS);

    return files;
}

void ZoneLoadTemplate::RenderOncePerTemplateFile(std::ostream& stream, const CodeTemplateFileTag fileTag, const OncePerTemplateRenderingContext& context)
{
    assert(fileTag == TAG_ALL_LOADERS);

    const PerTemplate t(stream, context);
    t.AllLoaders();
}

std::vector<CodeTemplateFile> ZoneLoadTemplate::GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context.m_asset->m_definition->m_name;
    utils::MakeStringLowerCase(assetName);

    auto gameName = context.m_game;
    utils::MakeStringLowerCase(gameName);

    files.emplace_back(std::format("XAssets/{0}/{0}_{1}_load_db.h", assetName, gameName), TAG_HEADER);
    files.emplace_back(std::format("XAssets/{0}/{0}_{1}_load_db.cpp", assetName, gameName), TAG_SOURCE);

    return files;
}

void ZoneLoadTemplate::RenderOncePerAssetFile(std::ostream& stream, const CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context)
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
