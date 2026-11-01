#include "Game/IW3/Image/ImageLoaderEmbeddedIW3.h"

#include "Game/IW3/GameIW3.h"
#include "Image/DdsWriter.h"
#include "Image/Texture.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace IW3;
using namespace std::literals;

namespace
{
    void FillCube(image::TextureCube& texture)
    {
        for (auto face = 0; face < texture.GetFaceCount(); face++)
        {
            for (auto mip = 0; mip < texture.GetMipMapCount(); mip++)
            {
                const auto value = static_cast<unsigned char>(face * 16 + mip + 1);
                std::memset(texture.GetBufferForMipLevel(mip, face), value, texture.GetSizeOfMipLevel(mip));
            }
        }
    }

    TEST_CASE("ImageLoaderEmbeddedIW3: Can parse dds", "[iw3][image]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/IW3/Image/TestImage.dds";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("images/_testimage.dds", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderEmbeddedIW3(memory, searchPath);
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

    TEST_CASE("ImageLoaderEmbeddedIW3: Stores cubemap mip chains face-major", "[iw3][image]")
    {
        image::TextureCube source(&image::format::A8, 8, 8, true);
        source.Allocate();
        FillCube(source);

        std::stringstream ddsStream(std::ios::in | std::ios::out | std::ios::binary);
        image::DdsWriter writer;
        writer.DumpImage(ddsStream, &source);

        MockSearchPath searchPath;
        searchPath.AddFileData("images/_testcube.dds", ddsStream.str());

        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        auto loader = image::CreateLoaderEmbeddedIW3(memory, searchPath);
        const auto result = loader->CreateAsset("*testcube", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<GfxImage>*>(result.GetAssetInfo());
        const auto* loadDef = assetInfo->Asset()->texture.loadDef;
        REQUIRE(loadDef);

        auto offset = 0uz;
        for (auto face = 0; face < source.GetFaceCount(); face++)
        {
            for (auto mip = 0; mip < source.GetMipMapCount(); mip++)
            {
                const auto mipSize = source.GetSizeOfMipLevel(mip);
                REQUIRE(std::memcmp(loadDef->data + offset, source.GetBufferForMipLevel(mip, face), mipSize) == 0);
                offset += mipSize;
            }
        }

        REQUIRE(offset == loadDef->resourceSize);
    }
} // namespace
