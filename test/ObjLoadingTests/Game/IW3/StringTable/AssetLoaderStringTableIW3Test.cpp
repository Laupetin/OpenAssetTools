#include "Game/IW3/StringTable/AssetLoaderStringTableIW3.h"

#include "Game/IW3/GameIW3.h"
#include "Mock/MockAssetLoadingManager.h"
#include "Mock/MockSearchPath.h"
#include "Pool/ZoneAssetPools.h"
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

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::IW3));
        zone.m_pools = ZoneAssetPools::CreateForGame(GameId::IW3, &zone, 1);

        const auto assetTypeCount = zone.m_pools->GetAssetTypeCount();
        for (auto i = 0; i < assetTypeCount; i++)
            zone.m_pools->InitPoolDynamic(i);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        AssetLoaderStringTable assetLoader(memory, searchPath);
        IgnoredAssetLookup ignoredAssetLookup;

        AssetCreationContext context(&zone, &creatorCollection, &ignoredAssetLookup);

        const auto result = assetLoader.CreateAsset("mp/cooltable.csv", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<StringTable>*>(result.GetAssetInfo());
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