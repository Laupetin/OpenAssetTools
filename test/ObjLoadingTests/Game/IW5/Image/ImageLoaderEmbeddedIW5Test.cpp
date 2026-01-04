#include "Game/IW5/Image/ImageLoaderEmbeddedIW5.h"

#include "Game/IW5/GameIW5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using namespace IW5;
using namespace std::literals;

namespace
{
    TEST_CASE("ImageLoaderEmbeddedIW5: Can parse dds", "[iw5][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/IW5/Image/TestImage.dds";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/_testimage.dds", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderEmbeddedIW5(memory, searchPath);
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
    }
} // namespace
