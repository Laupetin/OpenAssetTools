#include "Game/T5/AssetLoaders/AssetLoaderStringTable.h"

#include "Game/T5/GameT5.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T5;
using namespace std::literals;

namespace
{
    TEST_CASE("AssetLoaderStringTable(T5): Can parse string table", "[t5][stringtable][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("mp/cooltable.csv",
                               "test,data,lol\n"
                               "lorem,ipsum");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T5));
        MockAssetLoadingManager assetLoadingManager(zone, searchPath);

        AssetLoaderStringTable assetLoader;
        MemoryManager memory;

        assetLoader.LoadFromRaw("mp/cooltable.csv", &searchPath, &memory, &assetLoadingManager, &zone);

        auto* assetInfo = reinterpret_cast<XAssetInfo<StringTable>*>(assetLoadingManager.MockGetAddedAsset("mp/cooltable.csv"));
        REQUIRE(assetInfo != nullptr);

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
        CHECK(stringTable->cellIndex[0] == 2);
        CHECK(stringTable->cellIndex[1] == 4);
        CHECK(stringTable->cellIndex[2] == 3);
        CHECK(stringTable->cellIndex[3] == 1);
        CHECK(stringTable->cellIndex[4] == 0);
    }
} // namespace
