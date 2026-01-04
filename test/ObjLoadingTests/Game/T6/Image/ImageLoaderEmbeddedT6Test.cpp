#include "Game/T6/Image/ImageLoaderEmbeddedT6.h"

#include "Game/T6/GameT6.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace T6;
using namespace std::literals;

namespace
{
    TEST_CASE("ImageLoaderEmbeddedT6: Can parse dds", "[t6][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/T6/Image/TestImage.dds";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/_testimage.dds", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderEmbeddedT6(memory, searchPath);
        auto result = loader->CreateAsset("*testimage", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo());
        const auto* image = assetInfo->Asset();

        REQUIRE(image->name == "*testimage"s);
        REQUIRE(image->width == 64);
        REQUIRE(image->height == 64);
        REQUIRE(image->depth == 1);
        REQUIRE(image->streamedPartCount == 0);

        REQUIRE(image->texture.loadDef);
        REQUIRE(image->texture.loadDef->resourceSize > 0);
    }
} // namespace
