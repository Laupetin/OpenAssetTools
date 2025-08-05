#include "KeyValuePairs/KeyValuePairsCreator.h"

#include "Utils/TestMemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>

using namespace std::string_literals;

namespace test::keyvaluepairs
{
    TEST_CASE("KeyValuePairsCreator: ZoneDefinition with no properties produces no KeyValuePairs", "[keyvaluepairs]")
    {
        key_value_pairs::Creator sut;

        ZoneDefinition zoneDefinition;
        sut.Finalize(zoneDefinition);

        const auto kvps = sut.GetFinalKeyValuePairs();

        REQUIRE(kvps.empty());
    }

    TEST_CASE("KeyValuePairsCreator: ZoneDefinition with unrelated properties produce no KeyValuePairs", "[keyvaluepairs]")
    {
        key_value_pairs::Creator sut;

        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("linker.test", "yes");
        sut.Finalize(zoneDefinition);

        const auto kvps = sut.GetFinalKeyValuePairs();

        REQUIRE(kvps.empty());
    }

    TEST_CASE("KeyValuePairsCreator: ZoneDefinition with level properties produce KeyValuePairs", "[keyvaluepairs]")
    {
        key_value_pairs::Creator sut;

        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("linker.test", "yes");
        zoneDefinition.m_properties.AddProperty("level.random", "lemao");
        sut.Finalize(zoneDefinition);

        const auto kvps = sut.GetFinalKeyValuePairs();

        REQUIRE(kvps.size() == 1);
        REQUIRE(kvps[0].m_key_str);
        REQUIRE(!kvps[0].m_key_hash);
        REQUIRE(*kvps[0].m_key_str == "random");
        REQUIRE(kvps[0].m_value == "lemao");
    }

    TEST_CASE("KeyValuePairsCreator: ZoneDefinition can have level properties with hash", "[keyvaluepairs]")
    {
        key_value_pairs::Creator sut;

        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("level.@D34DB33F", "yes");
        zoneDefinition.m_properties.AddProperty("level.@112233", "no");
        zoneDefinition.m_properties.AddProperty("level.@0", "maybe");
        sut.Finalize(zoneDefinition);

        const auto kvps = sut.GetFinalKeyValuePairs();

        REQUIRE(kvps.size() == 3);
        REQUIRE(*kvps[0].m_key_hash == 0);
        REQUIRE(kvps[0].m_value == "maybe");
        REQUIRE(*kvps[1].m_key_hash == 0x112233);
        REQUIRE(kvps[1].m_value == "no");
        REQUIRE(*kvps[2].m_key_hash == 0xD34DB33F);
        REQUIRE(kvps[2].m_value == "yes");
    }

    TEST_CASE("KeyValuePairsCreator: ZoneDefinition can have level properties with name and/or hash", "[keyvaluepairs]")
    {
        key_value_pairs::Creator sut;

        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("level.ipak_read", "asdf");
        zoneDefinition.m_properties.AddProperty("level.hello_world", "foo");
        zoneDefinition.m_properties.AddProperty("level.@D34DB33F", "yes");
        zoneDefinition.m_properties.AddProperty("level.@112233", "no");
        sut.Finalize(zoneDefinition);

        const auto kvps = sut.GetFinalKeyValuePairs();

        REQUIRE(kvps.size() == 4);
        REQUIRE(*kvps[0].m_key_str == "hello_world");
        REQUIRE(kvps[0].m_value == "foo");
        REQUIRE(*kvps[1].m_key_str == "ipak_read");
        REQUIRE(kvps[1].m_value == "asdf");
        REQUIRE(*kvps[2].m_key_hash == 0x112233);
        REQUIRE(kvps[2].m_value == "no");
        REQUIRE(*kvps[3].m_key_hash == 0xD34DB33F);
        REQUIRE(kvps[3].m_value == "yes");
    }
} // namespace test::keyvaluepairs
