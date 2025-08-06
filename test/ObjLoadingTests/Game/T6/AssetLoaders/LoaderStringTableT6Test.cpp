#include "Game/T6/StringTable/LoaderStringTableT6.h"

#include "Game/T6/GameT6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T6;
using namespace std::literals;

namespace
{
    TEST_CASE("AssetLoaderStringTable(T6): Can parse string table", "[t6][stringtable][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("mp/cooltable.csv",
                               "test,data,lol\n"
                               "lorem,ipsum");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T6));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = string_table::CreateLoaderT6(memory, searchPath);
        auto result = loader->CreateAsset("mp/cooltable.csv", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<StringTable>*>(result.GetAssetInfo());
        const auto* stringTable = assetInfo->Asset();

        REQUIRE(stringTable->name == "mp/cooltable.csv"s);
        REQUIRE(stringTable->columnCount == 3);
        REQUIRE(stringTable->rowCount == 2);

        CHECK(stringTable->values[0].string == "test"s);
        CHECK(stringTable->values[0].hash == 0x7c9e6865);
        CHECK(stringTable->values[1].string == "data"s);
        CHECK(stringTable->values[1].hash == 0x7c95915f);
        CHECK(stringTable->values[2].string == "lol"s);
        CHECK(stringTable->values[2].hash == 0xb888d0c);
        CHECK(stringTable->values[3].string == "lorem"s);
        CHECK(stringTable->values[3].hash == 0xfe02704);
        CHECK(stringTable->values[4].string == "ipsum"s);
        CHECK(stringTable->values[4].hash == 0xfaa7033);
        CHECK(stringTable->values[5].string == ""s);
        CHECK(stringTable->values[5].hash == 0x1505);

        REQUIRE(stringTable->cellIndex != nullptr);
        CHECK(stringTable->cellIndex[0] == 5);
        CHECK(stringTable->cellIndex[1] == 2);
        CHECK(stringTable->cellIndex[2] == 4);
        CHECK(stringTable->cellIndex[3] == 3);
        CHECK(stringTable->cellIndex[4] == 1);
        CHECK(stringTable->cellIndex[5] == 0);
    }
} // namespace
