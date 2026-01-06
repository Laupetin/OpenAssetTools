#include "AssetStructTestsTemplate.h"

#include "Domain/Computations/StructureComputations.h"
#include "Internal/BaseTemplate.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <format>

namespace
{
    constexpr int TAG_SOURCE = 1;

    class PerAsset final : BaseTemplate
    {
    public:
        PerAsset(std::ostream& stream, const OncePerAssetRenderingContext& context)
            : BaseTemplate(stream),
              m_env(context)
        {
        }

        void Source()
        {
            AddGeneratedHint();

            LINEF("#include \"Game/{0}/{0}.h\"", m_env.m_game)
            LINE("")
            LINE("#include <catch2/catch_test_macros.hpp>")
            LINE("#include <catch2/generators/catch_generators.hpp>")
            LINE("#include <cstddef>")
            LINE("")
            LINEF("using namespace {0};", m_env.m_game)
            LINE("")
            LINEF("namespace game::{0}::xassets::asset_{1}", m_env.m_game, Lower(m_env.m_asset->m_definition->m_name))
            LINE("{")
            m_intendation++;

            if (m_env.m_asset->m_has_matching_cross_platform_structure)
                TestMethod(m_env.m_asset);

            for (const auto* structure : m_env.m_used_structures)
            {
                StructureComputations computations(structure->m_info);
                if (!structure->m_info->m_definition->m_anonymous && !computations.IsAsset() && structure->m_info->m_has_matching_cross_platform_structure)
                    TestMethod(structure->m_info);
            }

            m_intendation--;
            LINE("}")
        }

    private:
        void TestMethod(const StructureInformation* structure)
        {
            LINEF("TEST_CASE(\"{0}::{1}: Tests for {2}\", \"[assetstruct]\")",
                  m_env.m_game,
                  m_env.m_asset->m_definition->GetFullName(),
                  structure->m_definition->GetFullName())
            LINE("{")
            m_intendation++;

            for (const auto& member : structure->m_ordered_members)
            {
                if (!member->m_member->m_name.empty() && !member->m_member->m_type_declaration->m_has_custom_bit_size)
                {
                    LINEF("REQUIRE(offsetof({0}, {1}) == {2});", structure->m_definition->GetFullName(), member->m_member->m_name, member->m_member->m_offset)
                }
            }

            LINE("")

            LINEF("REQUIRE({0}u == sizeof({1}));", structure->m_definition->GetSize(), structure->m_definition->GetFullName())
            LINEF("REQUIRE({0}u == alignof({1}));", structure->m_definition->GetAlignment(), structure->m_definition->GetFullName())
            m_intendation--;
            LINE("}")
        }

        const OncePerAssetRenderingContext& m_env;
    };
} // namespace

std::vector<CodeTemplateFile> AssetStructTestsTemplate::GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context.m_asset->m_definition->m_name;
    utils::MakeStringLowerCase(assetName);

    auto gameName = context.m_game;
    utils::MakeStringLowerCase(gameName);

    files.emplace_back(std::format("XAssets/{0}/{0}_{1}_struct_test.cpp", assetName, gameName), TAG_SOURCE);

    return files;
}

void AssetStructTestsTemplate::RenderOncePerAssetFile(std::ostream& stream, const CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context)
{
    PerAsset t(stream, context);

    assert(fileTag == TAG_SOURCE);
    if (fileTag == TAG_SOURCE)
        t.Source();
}
