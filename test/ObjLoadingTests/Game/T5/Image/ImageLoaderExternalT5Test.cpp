#include "Game/T5/Image/ImageLoaderExternalT5.h"

#include "Game/T5/CommonT5.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/Image/ImageSemanticZoneStateT5.h"
#include "Image/D3DFormat.h"
#include "Image/IwiTypes.h"
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
    TEST_CASE("ImageLoaderExternalT5: Can parse iwi", "[t5][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/T5/Image/TestImage.iwi";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/testimage.iwi", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::T5, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderExternalT5(memory, searchPath);
        auto result = loader->CreateAsset("testimage", context);
        REQUIRE(result.HasBeenSuccessful());

        auto* assetInfo = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo());
        auto* image = assetInfo->Asset();

        REQUIRE(image->name == "testimage"s);
        REQUIRE(image->width == 64);
        REQUIRE(image->height == 64);
        REQUIRE(image->depth == 1);

        REQUIRE(image->hash == Common::R_HashString("testimage", 0));
        REQUIRE(image->category == IMG_CATEGORY_LOAD_FROM_FILE);
        REQUIRE(image->mapType == MAPTYPE_2D);
        REQUIRE(image->semantic == TS_2D);
        REQUIRE(!image->delayLoadPixels);
        REQUIRE(image->noPicmip);
        REQUIRE(!image->streaming);
        REQUIRE(image->cardMemory.platform[0] == 2048);
        REQUIRE(image->cardMemory.platform[1] == 2048);

        REQUIRE(image->texture.loadDef);
        REQUIRE(image->texture.loadDef->resourceSize == 0);
        REQUIRE(image->texture.loadDef->format == oat::D3DFMT_DXT1);
        REQUIRE(image->texture.loadDef->levelCount == 1);
        REQUIRE(image->texture.loadDef->flags == image::iwi13::IMG_FLAG_NOMIPMAPS);

        auto& semanticState = context.GetZoneAssetCreationState<image::SemanticZoneStateT5>();
        semanticState.AssignSemantic(*image, TS_COLOR_MAP);
        semanticState.AssignSemantic(*image, TS_NORMAL_MAP);

        REQUIRE(image->semantic == TS_COLOR_MAP);
        REQUIRE(image->delayLoadPixels);
    }
} // namespace
