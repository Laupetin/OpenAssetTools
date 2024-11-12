#include "Game/T6/T6.h"
#include "Zone/Definition/ZoneDefinitionStream.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>

namespace test::zone::definition::zone_definition_stream
{
    TEST_CASE("ZoneDefinitionInputStream: Ensure can read simple ZoneDefinition", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,common_mp

techniqueset,,trivial_9z33feqw
material,gradient_top
menu,demo_ingame
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_game == GameId::T6);
        REQUIRE(result->m_name == "common_mp");
        REQUIRE(result->m_assets.size() == 3u);

        REQUIRE(result->m_assets[0].m_asset_name == "trivial_9z33feqw");
        REQUIRE(result->m_assets[0].m_asset_type == T6::ASSET_TYPE_TECHNIQUE_SET);
        REQUIRE(result->m_assets[0].m_is_reference == true);

        REQUIRE(result->m_assets[1].m_asset_name == "gradient_top");
        REQUIRE(result->m_assets[1].m_asset_type == T6::ASSET_TYPE_MATERIAL);
        REQUIRE(result->m_assets[1].m_is_reference == false);

        REQUIRE(result->m_assets[2].m_asset_name == "demo_ingame");
        REQUIRE(result->m_assets[2].m_asset_type == T6::ASSET_TYPE_MENU);
        REQUIRE(result->m_assets[2].m_is_reference == false);
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure ZoneDefinition name is target name by default", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6

material,demo_material
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_game == GameId::T6);
        REQUIRE(result->m_name == "test");
        REQUIRE(result->m_assets.size() == 1u);
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can include other ZoneDefinitions", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod

include,demo_gun
include,demo_scripts
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.empty());
        REQUIRE(result->m_includes.size() == 2);

        REQUIRE(result->m_includes[0] == "demo_gun");
        REQUIRE(result->m_includes[1] == "demo_scripts");
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can include assetlists", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod

assetlist,code_post_gfx_mp
assetlist,common_mp

material,test_material
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.size() == 1);
        REQUIRE(result->m_asset_lists.size() == 2);

        REQUIRE(result->m_asset_lists[0] == "code_post_gfx_mp");
        REQUIRE(result->m_asset_lists[1] == "common_mp");
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can define other build targets", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod

build,other_mod

material,test_material

build,more_mods
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.size() == 1);
        REQUIRE(result->m_targets_to_build.size() == 2);

        REQUIRE(result->m_targets_to_build[0] == "other_mod");
        REQUIRE(result->m_targets_to_build[1] == "more_mods");
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can ignore other zones", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod

ignore,code_post_gfx_mp
ignore,common_mp

material,test_material
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.size() == 1);
        REQUIRE(result->m_ignores.size() == 2);

        REQUIRE(result->m_ignores[0] == "code_post_gfx_mp");
        REQUIRE(result->m_ignores[1] == "common_mp");
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can read gdts", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod
>gdt,crazy_gdt
>gdt,even_crazier_gdt

material,test_material
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.size() == 1);
        REQUIRE(result->m_gdts.size() == 2);

        REQUIRE(result->m_gdts[0] == "crazy_gdt");
        REQUIRE(result->m_gdts[1] == "even_crazier_gdt");
    }

    TEST_CASE("ZoneDefinitionInputStream: Ensure can define meta data", "[zonedefinition]")
    {
        std::istringstream inputData(R"sampledata(
// Call Of Duty: Black Ops II
>game,T6
>name,test_mod
>foo,bar

material,test_material

>level.ipak_read,base
>level.ipak_read,code_post_gfx
)sampledata");

        ZoneDefinitionInputStream inputStream(inputData, "test", "test.zone", true);

        const auto result = inputStream.ReadDefinition();
        REQUIRE(result);

        REQUIRE(result->m_assets.size() == 1);
        REQUIRE(result->m_properties.m_properties.size() == 3);

        auto fooResults = result->m_properties.m_properties.equal_range("foo");
        REQUIRE(fooResults.first != fooResults.second);
        REQUIRE(fooResults.first->second == "bar");
        REQUIRE(++fooResults.first == fooResults.second);

        auto ipakReadResults = result->m_properties.m_properties.equal_range("level.ipak_read");
        auto iterator = ipakReadResults.first;
        REQUIRE(iterator != ipakReadResults.second);
        REQUIRE(iterator->second == "base");
        ++iterator;
        REQUIRE(iterator->second == "code_post_gfx");
        ++iterator;
        REQUIRE(iterator == ipakReadResults.second);
    }
} // namespace test::zone::definition::zone_definition_stream
