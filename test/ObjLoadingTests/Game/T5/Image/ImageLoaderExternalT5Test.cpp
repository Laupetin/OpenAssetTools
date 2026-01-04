#include "Game/T5/Image/ImageLoaderExternalT5.h"

#include "Game/T5/GameT5.h"
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

        const auto* assetInfo = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo());
        const auto* image = assetInfo->Asset();

        REQUIRE(image->name == "testimage"s);
        REQUIRE(image->width == 64);
        REQUIRE(image->height == 64);
        REQUIRE(image->depth == 1);
    }
} // namespace
