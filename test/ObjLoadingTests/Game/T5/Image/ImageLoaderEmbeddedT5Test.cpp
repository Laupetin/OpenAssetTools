#include "Game/T5/Image/ImageLoaderEmbeddedT5.h"

#include "Game/T5/CommonT5.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/Image/ImageSemanticZoneStateT5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace T5;
using namespace std::literals;

namespace
{
    TEST_CASE("ImageLoaderEmbeddedT5: Can parse dds", "[t5][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/T5/Image/TestImage.dds";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/_testimage.dds", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::T5, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderEmbeddedT5(memory, searchPath);
        auto result = loader->CreateAsset("*testimage", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo());
        const auto* image = assetInfo->Asset();

        REQUIRE(image->name == "*testimage"s);
        REQUIRE(image->width == 64);
        REQUIRE(image->height == 64);
        REQUIRE(image->depth == 1);

        REQUIRE(image->texture.loadDef);
        REQUIRE(image->texture.loadDef->resourceSize > 0);

        REQUIRE(image->hash == Common::R_HashString("*testimage", 0));
        REQUIRE(image->category == IMG_CATEGORY_AUTO_GENERATED);
        REQUIRE(image->semantic == TS_FUNCTION);
        REQUIRE(image->delayLoadPixels);
        REQUIRE(!image->noPicmip);
        REQUIRE(image->cardMemory.platform[0] == image->texture.loadDef->resourceSize);
        REQUIRE(image->cardMemory.platform[1] == image->texture.loadDef->resourceSize);
        REQUIRE(image->texture.loadDef->levelCount == 1);
    }

    TEST_CASE("ImageLoaderEmbeddedT5: Can parse code image from iwi", "[t5][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/T5/Image/TestImage.iwi";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/$identitynormalmap.iwi", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::T5, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        context.GetZoneAssetCreationState<image::SemanticZoneStateT5>().SetDelayLoadPixels(false);

        auto loader = image::CreateLoaderEmbeddedT5(memory, searchPath);
        auto result = loader->CreateAsset("$identitynormalmap", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* image = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo())->Asset();

        REQUIRE(image->texture.loadDef);
        REQUIRE(image->texture.loadDef->resourceSize == 2048);
        REQUIRE(image->category == IMG_CATEGORY_AUTO_GENERATED);
        REQUIRE(image->semantic == TS_NORMAL_MAP);
        REQUIRE(!image->delayLoadPixels);
    }
} // namespace
