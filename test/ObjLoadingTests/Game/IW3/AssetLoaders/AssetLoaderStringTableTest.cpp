#include "Game/IW3/AssetLoaders/AssetLoaderStringTable.h"

#include "Game/IW3/GameIW3.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace IW3;
using namespace std::literals;

namespace
{
    TEST_CASE("AssetLoaderStringTable(IW3): Can parse string table", "[iw3][stringtable][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("mp/cooltable.csv",
                               "test,data,lol\n"
                               "lorem,ipsum");

        Zone zone("MockZone", 0, &g_GameIW3);
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

        REQUIRE(stringTable->values[0] == "test"s);
        REQUIRE(stringTable->values[1] == "data"s);
        REQUIRE(stringTable->values[2] == "lol"s);
        REQUIRE(stringTable->values[3] == "lorem"s);
        REQUIRE(stringTable->values[4] == "ipsum"s);
        REQUIRE(stringTable->values[5] == ""s);
    }
} // namespace
