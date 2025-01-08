#include "Image/ImageIPakPostProcessor.h"

#include "Game/T6/T6.h"
#include "KeyValuePairs/KeyValuePairsCreator.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <filesystem>
#include <memory>

using namespace T6;
using namespace std::string_literals;

namespace
{
    class TestContext
    {
    public:
        TestContext()
            : m_zone("test", 0, IGame::GetGameById(GameId::T6)),
              m_zone_definition(),
              m_zone_definition_context(m_zone_definition),
              m_zone_states(m_zone),
              m_creators(m_zone),
              m_ignored_assets(),
              m_out_dir(),
              m_context(m_zone, &m_creators, &m_ignored_assets),
              m_ipak_creator(m_zone_states.GetZoneAssetCreationState<IPakCreator>())
        {
        }

        std::unique_ptr<IAssetPostProcessor> CreateSut()
        {
            return std::make_unique<ImageIPakPostProcessor<AssetImage>>(m_zone_definition_context, m_search_path, m_zone_states, m_out_dir);
        }

        Zone m_zone;
        ZoneDefinition m_zone_definition;
        ZoneDefinitionContext m_zone_definition_context;
        MockSearchPath m_search_path;
        ZoneAssetCreationStateContainer m_zone_states;
        AssetCreatorCollection m_creators;
        IgnoredAssetLookup m_ignored_assets;
        MockOutputPath m_out_dir;
        AssetCreationContext m_context;

        IPakCreator& m_ipak_creator;
    };
} // namespace

namespace test::image
{
    TEST_CASE("ImageIPakPostProcessor: Handles asset type of specified asset", "[image]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        REQUIRE(sut->GetHandlingAssetType() == ASSET_TYPE_IMAGE);
    }

    TEST_CASE("ImageIPakPostProcessor: Adds images to ipak when obj container is specified", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak", ZoneDefinitionObjContainerType::IPAK, 0, 2);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        auto result = testContext.m_ipak_creator.GetOrAddIPak("testIpak");
        REQUIRE(result);

        const auto& imageNames = result->GetImageNames();
        REQUIRE(imageNames.size() == 2u);
        REQUIRE(imageNames[0] == "testImage0");
        REQUIRE(imageNames[1] == "testImage1");
    }

    TEST_CASE("ImageIPakPostProcessor: Respects lower obj container boundary when adding images to ipak", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak", ZoneDefinitionObjContainerType::IPAK, 1, 3);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        auto result = testContext.m_ipak_creator.GetOrAddIPak("testIpak");
        REQUIRE(result);

        const auto& imageNames = result->GetImageNames();
        REQUIRE(imageNames.size() == 2u);
        REQUIRE(imageNames[0] == "testImage1");
        REQUIRE(imageNames[1] == "testImage2");
    }

    TEST_CASE("ImageIPakPostProcessor: Respects upper obj container boundary when adding images to ipak", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak", ZoneDefinitionObjContainerType::IPAK, 0, 2);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        auto result = testContext.m_ipak_creator.GetOrAddIPak("testIpak");
        REQUIRE(result);

        const auto& imageNames = result->GetImageNames();
        REQUIRE(imageNames.size() == 2u);
        REQUIRE(imageNames[0] == "testImage0");
        REQUIRE(imageNames[1] == "testImage1");
    }

    TEST_CASE("ImageIPakPostProcessor: Respects upper and lower obj container boundary when adding images to ipak", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak", ZoneDefinitionObjContainerType::IPAK, 1, 3);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset3(ASSET_TYPE_IMAGE, "testImage3", nullptr);
        sut->PostProcessAsset(imageAsset3, testContext.m_context);

        auto result = testContext.m_ipak_creator.GetOrAddIPak("testIpak");
        REQUIRE(result);

        const auto& imageNames = result->GetImageNames();
        REQUIRE(imageNames.size() == 2u);
        REQUIRE(imageNames[0] == "testImage1");
        REQUIRE(imageNames[1] == "testImage2");
    }

    TEST_CASE("ImageIPakPostProcessor: Can add images to multiple ipak", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak0", ZoneDefinitionObjContainerType::IPAK, 0, 2);
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak1", ZoneDefinitionObjContainerType::IPAK, 2, 4);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset3(ASSET_TYPE_IMAGE, "testImage3", nullptr);
        sut->PostProcessAsset(imageAsset3, testContext.m_context);

        auto result0 = testContext.m_ipak_creator.GetOrAddIPak("testIpak0");
        REQUIRE(result0);

        const auto& imageNames0 = result0->GetImageNames();
        REQUIRE(imageNames0.size() == 2u);
        REQUIRE(imageNames0[0] == "testImage0");
        REQUIRE(imageNames0[1] == "testImage1");

        auto result1 = testContext.m_ipak_creator.GetOrAddIPak("testIpak1");
        REQUIRE(result1);

        const auto& imageNames1 = result1->GetImageNames();
        REQUIRE(imageNames1.size() == 2u);
        REQUIRE(imageNames1[0] == "testImage2");
        REQUIRE(imageNames1[1] == "testImage3");
    }

    TEST_CASE("ImageIPakPostProcessor: Can add images to multiple ipak with gap between", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak0", ZoneDefinitionObjContainerType::IPAK, 0, 2);
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak1", ZoneDefinitionObjContainerType::IPAK, 3, 5);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset3(ASSET_TYPE_IMAGE, "testImage3", nullptr);
        sut->PostProcessAsset(imageAsset3, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset4(ASSET_TYPE_IMAGE, "testImage4", nullptr);
        sut->PostProcessAsset(imageAsset4, testContext.m_context);

        auto result0 = testContext.m_ipak_creator.GetOrAddIPak("testIpak0");
        REQUIRE(result0);

        const auto& imageNames0 = result0->GetImageNames();
        REQUIRE(imageNames0.size() == 2u);
        REQUIRE(imageNames0[0] == "testImage0");
        REQUIRE(imageNames0[1] == "testImage1");

        auto result1 = testContext.m_ipak_creator.GetOrAddIPak("testIpak1");
        REQUIRE(result1);

        const auto& imageNames1 = result1->GetImageNames();
        REQUIRE(imageNames1.size() == 2u);
        REQUIRE(imageNames1[0] == "testImage3");
        REQUIRE(imageNames1[1] == "testImage4");
    }

    TEST_CASE("ImageIPakPostProcessor: Writes IPak when finalizing", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIpak", ZoneDefinitionObjContainerType::IPAK, 0, 2);

        testContext.m_search_path.AddFileData("images/testImage0.iwi", "asdf0");
        testContext.m_search_path.AddFileData("images/testImage1.iwi", "asdf1");

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        sut->FinalizeZone(testContext.m_context);

        const auto* mockFile = testContext.m_out_dir.GetMockedFile("testIpak.ipak");
        REQUIRE(mockFile);
    }
} // namespace test::image
