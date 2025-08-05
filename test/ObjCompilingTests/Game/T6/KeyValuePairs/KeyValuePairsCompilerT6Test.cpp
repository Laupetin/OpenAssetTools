#include "Game/T6/KeyValuePairs/KeyValuePairsCompilerT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "KeyValuePairs/KeyValuePairsCreator.h"
#include "Utils/TestMemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>

using namespace T6;
using namespace std::string_literals;

namespace
{
    class TestContext
    {
    public:
        TestContext()
            : m_memory(),
              m_zone("test", 0, IGame::GetGameById(GameId::T6)),
              m_zone_definition(),
              m_zone_states(m_zone),
              m_creators(m_zone),
              m_ignored_assets(),
              m_context(m_zone, &m_creators, &m_ignored_assets),
              m_kvp_creator(m_zone_states.GetZoneAssetCreationState<::key_value_pairs::Creator>())
        {
        }

        std::unique_ptr<IAssetCreator> CreateSut()
        {
            return T6::key_value_pairs::CreateCompiler(m_memory, m_zone, m_zone_definition, m_zone_states);
        }

        TestMemoryManager m_memory;
        Zone m_zone;
        ZoneDefinition m_zone_definition;
        ZoneAssetCreationStateContainer m_zone_states;
        AssetCreatorCollection m_creators;
        IgnoredAssetLookup m_ignored_assets;
        AssetCreationContext m_context;

        ::key_value_pairs::Creator& m_kvp_creator;
    };
} // namespace

namespace test::game::t6::keyvaluepairs
{
    TEST_CASE("KeyValuePairsCompilerT6: Does not handle any asset type", "[keyvaluepairs][t6]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        REQUIRE(!sut->GetHandlingAssetType().has_value());
    }

    TEST_CASE("KeyValuePairsCompilerT6: Does not take any action", "[keyvaluepairs][t6]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        REQUIRE(sut->CreateAsset("anyAsset", testContext.m_context).HasTakenAction() == false);
    }

    TEST_CASE("KeyValuePairsCompilerT6: Does nothing without any KeyValuePairs", "[keyvaluepairs][t6]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        sut->FinalizeZone(testContext.m_context);

        REQUIRE(testContext.m_memory.GetAllocationCount() == 0u);
        REQUIRE(testContext.m_zone.m_pools->GetTotalAssetCount() == 0u);
    }

    TEST_CASE("KeyValuePairsCompilerT6: Creates KeyValuePairs asset with identical name to the zone", "[keyvaluepairs][t6]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        testContext.m_kvp_creator.AddKeyValuePair(::key_value_pairs::CommonKeyValuePair("ipak_read", "test_ipak"));

        sut->FinalizeZone(testContext.m_context);

        REQUIRE(testContext.m_memory.GetAllocationCount() > 0u);
        REQUIRE(testContext.m_zone.m_pools->GetTotalAssetCount() == 1u);

        XAssetInfo<KeyValuePairs>* assetInfo = *dynamic_cast<GameAssetPoolT6*>(testContext.m_zone.m_pools.get())->m_key_value_pairs->begin();
        REQUIRE(assetInfo);
        REQUIRE(assetInfo->m_name == "test");

        auto* asset = assetInfo->Asset();
        REQUIRE(asset->name == "test"s);
        REQUIRE(asset->numVariables == 1u);
        REQUIRE(asset->keyValuePairs != nullptr);

        REQUIRE(asset->keyValuePairs[0].keyHash == 0x0001bdc1);
        REQUIRE(asset->keyValuePairs[0].namespaceHash == 0x0000d2d3);
        REQUIRE(asset->keyValuePairs[0].value == "test_ipak"s);
    }

    TEST_CASE("KeyValuePairsCompilerT6: Creates KeyValuePairs asset with predefined hash", "[keyvaluepairs][t6]")
    {
        TestContext testContext;
        const auto sut = testContext.CreateSut();

        testContext.m_kvp_creator.AddKeyValuePair(::key_value_pairs::CommonKeyValuePair(0xDDEEFFAA, "hello_there"));

        sut->FinalizeZone(testContext.m_context);

        REQUIRE(testContext.m_memory.GetAllocationCount() > 0u);
        REQUIRE(testContext.m_zone.m_pools->GetTotalAssetCount() == 1u);

        XAssetInfo<KeyValuePairs>* assetInfo = *dynamic_cast<GameAssetPoolT6*>(testContext.m_zone.m_pools.get())->m_key_value_pairs->begin();
        REQUIRE(assetInfo);
        REQUIRE(assetInfo->m_name == "test");

        auto* asset = assetInfo->Asset();
        REQUIRE(asset->name == "test"s);
        REQUIRE(asset->numVariables == 1u);
        REQUIRE(asset->keyValuePairs != nullptr);

        REQUIRE(asset->keyValuePairs[0].keyHash == 0xDDEEFFAA);
        REQUIRE(asset->keyValuePairs[0].namespaceHash == 0x0000d2d3);
        REQUIRE(asset->keyValuePairs[0].value == "hello_there"s);
    }
} // namespace test::game::t6::keyvaluepairs
