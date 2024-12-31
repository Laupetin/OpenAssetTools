#include "Game/IW5/StringTable/LoaderStringTableIW5.h"

#include "Game/IW5/GameIW5.h"
#include "Mock/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace IW5;
using namespace std::literals;

namespace
{
    TEST_CASE("AssetLoaderStringTable(IW5): Can parse string table", "[iw5][stringtable][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("mp/cooltable.csv",
                               "test,data,lol\n"
                               "lorem,ipsum");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::IW5));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(&zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = CreateStringTableLoader(memory, searchPath);
        auto result = loader->CreateAsset("mp/cooltable.csv", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<StringTable>*>(result.GetAssetInfo());
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
