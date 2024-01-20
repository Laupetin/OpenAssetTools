#include "Game/IW4/AssetLoaders/AssetLoaderStringTable.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/GameIW4.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace IW4;
using namespace std::literals;

namespace
{
    TEST_CASE("AssetLoaderStringTable(IW4): Can parse string table", "[iw4][stringtable][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("mp/cooltable.csv",
                               "test,data,lol\n"
                               "lorem,ipsum");

        Zone zone("MockZone", 0, &g_GameIW4);
        MockAssetLoadingManager assetLoadingManager(&zone, &searchPath);

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
        CHECK(stringTable->values[0].hash == 0x364492);
        CHECK(stringTable->values[1].string == "data"s);
        CHECK(stringTable->values[1].hash == 0x2eefaa);
        CHECK(stringTable->values[2].string == "lol"s);
        CHECK(stringTable->values[2].hash == 0x1a349);
        CHECK(stringTable->values[3].string == "lorem"s);
        CHECK(stringTable->values[3].hash == 0x6261837);
        CHECK(stringTable->values[4].string == "ipsum"s);
        CHECK(stringTable->values[4].hash == 0x5fc4bc4);
        CHECK(stringTable->values[5].string == ""s);
        CHECK(stringTable->values[5].hash == 0x0);
    }
} // namespace
