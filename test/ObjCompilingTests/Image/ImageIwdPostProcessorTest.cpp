#include "Image/ImageIwdPostProcessor.h"

#include "Game/T6/T6.h"
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
              m_iwd_creator(m_zone_states.GetZoneAssetCreationState<IwdCreator>())
        {
        }

        std::unique_ptr<IAssetPostProcessor> CreateSut()
        {
            return std::make_unique<ImageIwdPostProcessor<AssetImage>>(m_zone_definition_context, m_search_path, m_zone_states, m_out_dir);
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

        IwdCreator& m_iwd_creator;
    };
} // namespace

namespace test::image
{
    TEST_CASE("ImageIwdPostProcessor: Handles asset type of specified asset", "[image]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        REQUIRE(sut->GetHandlingAssetType() == ASSET_TYPE_IMAGE);
    }

    TEST_CASE("ImageIwdPostProcessor: Adds images to iwd when obj container is specified", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd", ZoneDefinitionObjContainerType::IWD, 0, 2);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        auto result = testContext.m_iwd_creator.GetOrAddIwd("testIwd");
        REQUIRE(result);

        const auto& filePaths = result->GetFilePaths();
        REQUIRE(filePaths.size() == 2u);
        REQUIRE(filePaths[0] == "images/testImage0.iwi");
        REQUIRE(filePaths[1] == "images/testImage1.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Respects lower obj container boundary when adding images to iwd", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd", ZoneDefinitionObjContainerType::IWD, 1, 3);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        auto result = testContext.m_iwd_creator.GetOrAddIwd("testIwd");
        REQUIRE(result);

        const auto& filePaths = result->GetFilePaths();
        REQUIRE(filePaths.size() == 2u);
        REQUIRE(filePaths[0] == "images/testImage1.iwi");
        REQUIRE(filePaths[1] == "images/testImage2.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Respects upper obj container boundary when adding images to iwd", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd", ZoneDefinitionObjContainerType::IWD, 0, 2);

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset2(ASSET_TYPE_IMAGE, "testImage2", nullptr);
        sut->PostProcessAsset(imageAsset2, testContext.m_context);

        auto result = testContext.m_iwd_creator.GetOrAddIwd("testIwd");
        REQUIRE(result);

        const auto& filePaths = result->GetFilePaths();
        REQUIRE(filePaths.size() == 2u);
        REQUIRE(filePaths[0] == "images/testImage0.iwi");
        REQUIRE(filePaths[1] == "images/testImage1.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Respects upper and lower obj container boundary when adding images to iwd", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd", ZoneDefinitionObjContainerType::IWD, 1, 3);

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

        auto result = testContext.m_iwd_creator.GetOrAddIwd("testIwd");
        REQUIRE(result);

        const auto& filePaths = result->GetFilePaths();
        REQUIRE(filePaths.size() == 2u);
        REQUIRE(filePaths[0] == "images/testImage1.iwi");
        REQUIRE(filePaths[1] == "images/testImage2.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Can add images to multiple iwd", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd0", ZoneDefinitionObjContainerType::IWD, 0, 2);
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd1", ZoneDefinitionObjContainerType::IWD, 2, 4);

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

        auto result0 = testContext.m_iwd_creator.GetOrAddIwd("testIwd0");
        REQUIRE(result0);

        const auto& filePaths0 = result0->GetFilePaths();
        REQUIRE(filePaths0.size() == 2u);
        REQUIRE(filePaths0[0] == "images/testImage0.iwi");
        REQUIRE(filePaths0[1] == "images/testImage1.iwi");

        auto result1 = testContext.m_iwd_creator.GetOrAddIwd("testIwd1");
        REQUIRE(result1);

        const auto& filePaths1 = result1->GetFilePaths();
        REQUIRE(filePaths1.size() == 2u);
        REQUIRE(filePaths1[0] == "images/testImage2.iwi");
        REQUIRE(filePaths1[1] == "images/testImage3.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Can add images to multiple iwd with gap between", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd0", ZoneDefinitionObjContainerType::IWD, 0, 2);
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd1", ZoneDefinitionObjContainerType::IWD, 3, 5);

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

        auto result0 = testContext.m_iwd_creator.GetOrAddIwd("testIwd0");
        REQUIRE(result0);

        const auto& filePaths0 = result0->GetFilePaths();
        REQUIRE(filePaths0.size() == 2u);
        REQUIRE(filePaths0[0] == "images/testImage0.iwi");
        REQUIRE(filePaths0[1] == "images/testImage1.iwi");

        auto result1 = testContext.m_iwd_creator.GetOrAddIwd("testIwd1");
        REQUIRE(result1);

        const auto& filePaths1 = result1->GetFilePaths();
        REQUIRE(filePaths1.size() == 2u);
        REQUIRE(filePaths1[0] == "images/testImage3.iwi");
        REQUIRE(filePaths1[1] == "images/testImage4.iwi");
    }

    TEST_CASE("ImageIwdPostProcessor: Writes Iwd when finalizing", "[image]")
    {
        TestContext testContext;
        testContext.m_zone_definition.m_obj_containers.emplace_back("testIwd", ZoneDefinitionObjContainerType::IWD, 0, 2);

        testContext.m_search_path.AddFileData("images/testImage0.iwi", "asdf0");
        testContext.m_search_path.AddFileData("images/testImage1.iwi", "asdf1");

        const auto sut = testContext.CreateSut();

        XAssetInfo<GfxImage> imageAsset0(ASSET_TYPE_IMAGE, "testImage0", nullptr);
        sut->PostProcessAsset(imageAsset0, testContext.m_context);

        ++testContext.m_zone_definition_context.m_asset_index_in_definition;
        XAssetInfo<GfxImage> imageAsset1(ASSET_TYPE_IMAGE, "testImage1", nullptr);
        sut->PostProcessAsset(imageAsset1, testContext.m_context);

        sut->FinalizeZone(testContext.m_context);

        const auto* mockFile = testContext.m_out_dir.GetMockedFile("testIwd.iwd");
        REQUIRE(mockFile);
    }
} // namespace test::image
