#include "RawFile/BrandingAssetCreator.h"

#include "Game/IW3/IW3.h"
#include "Game/IW4/IW4.h"
#include "GitVersion.h"
#include "Utils/TestMemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <zlib.h>

using namespace std::string_literals;

namespace test::rawfile
{
    TEST_CASE("BrandingAssetCreator: Branding text uses target name as mod name fallback", "[rawfile][branding]")
    {
        ZoneDefinition zoneDefinition;

        REQUIRE(raw_file::CreateBrandingText(zoneDefinition, "target", "v1.2.3") == "generator.name=OpenAssetTools\ngenerator.version=v1.2.3\nmod.name=target");
    }

    TEST_CASE("BrandingAssetCreator: Branding text uses configured mod metadata", "[rawfile][branding]")
    {
        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("mod.name", "My Mod");
        zoneDefinition.m_properties.AddProperty("mod.author", "The Author");

        REQUIRE(raw_file::CreateBrandingText(zoneDefinition, "target", "v1.2.3")
                == "generator.name=OpenAssetTools\ngenerator.version=v1.2.3\nmod.name=My Mod\nmod.author=The Author");
    }

    TEST_CASE("BrandingAssetCreator: Branding text does not require mod author", "[rawfile][branding]")
    {
        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("mod.name", "My Mod");

        REQUIRE(raw_file::CreateBrandingText(zoneDefinition, "target", "v1.2.3") == "generator.name=OpenAssetTools\ngenerator.version=v1.2.3\nmod.name=My Mod");
    }

    TEST_CASE("BrandingAssetCreator: Branding text can use author with fallback mod name", "[rawfile][branding]")
    {
        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("mod.author", "The Author");

        REQUIRE(raw_file::CreateBrandingText(zoneDefinition, "target", "v1.2.3")
                == "generator.name=OpenAssetTools\ngenerator.version=v1.2.3\nmod.name=target\nmod.author=The Author");
    }

    TEST_CASE("BrandingAssetCreator: Creates uncompressed branding rawfile at end of zone", "[rawfile][branding][iw3]")
    {
        using namespace IW3;

        TestMemoryManager memory;
        Zone zone("target", 0, GameId::IW3, GamePlatform::PC);
        ZoneDefinition zoneDefinition;
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignoredAssets;
        AssetCreationContext context(zone, &creators, &ignoredAssets);

        auto* originalRawFile = memory.Alloc<RawFile>();
        originalRawFile->name = memory.Dup("target");
        originalRawFile->len = 3;
        originalRawFile->buffer = memory.Dup("old");
        context.AddAsset<AssetRawFile>("target", originalRawFile);

        const auto sut = raw_file::CreateBrandingAssetCreator<AssetRawFile>(memory, zone, zoneDefinition);
        REQUIRE_FALSE(sut->GetHandlingAssetType().has_value());
        REQUIRE_FALSE(sut->CreateAsset("target", context).HasTakenAction());

        sut->FinalizeZone(context);

        REQUIRE(zone.m_pools.GetTotalAssetCount() == 2u);

        const auto* assetInfo = zone.m_pools.GetAsset<AssetRawFile>("target");
        REQUIRE(assetInfo);
        REQUIRE(assetInfo->m_name == "target");

        auto lastAsset = zone.m_pools.end();
        --lastAsset;
        REQUIRE(*lastAsset == assetInfo);

        const auto expectedBranding = "generator.name=OpenAssetTools\ngenerator.version=" GIT_VERSION "\nmod.name=target"s;
        const auto* rawFile = assetInfo->Asset();
        REQUIRE(rawFile->name == "target"s);
        REQUIRE(rawFile->len == static_cast<int>(expectedBranding.size()));
        REQUIRE(std::string_view(rawFile->buffer, static_cast<size_t>(rawFile->len)) == expectedBranding);
    }

    TEST_CASE("BrandingAssetCreator: Creates compressed branding rawfile", "[rawfile][branding][iw4]")
    {
        using namespace IW4;

        TestMemoryManager memory;
        Zone zone("target", 0, GameId::IW4, GamePlatform::PC);
        ZoneDefinition zoneDefinition;
        zoneDefinition.m_properties.AddProperty("mod.name", "My Mod");
        zoneDefinition.m_properties.AddProperty("mod.author", "The Author");
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignoredAssets;
        AssetCreationContext context(zone, &creators, &ignoredAssets);

        const auto sut = raw_file::CreateBrandingAssetCreator<AssetRawFile>(memory, zone, zoneDefinition);
        sut->FinalizeZone(context);

        const auto* assetInfo = zone.m_pools.GetAsset<AssetRawFile>("target");
        REQUIRE(assetInfo);

        const auto expectedBranding = "generator.name=OpenAssetTools\ngenerator.version=" GIT_VERSION "\nmod.name=My Mod\nmod.author=The Author"s;
        const auto* rawFile = assetInfo->Asset();
        REQUIRE(rawFile->name == "target"s);
        REQUIRE(rawFile->len == static_cast<int>(expectedBranding.size()));
        REQUIRE(rawFile->compressedLen > 0);
        REQUIRE(rawFile->data.compressedBuffer);

        std::vector<char> uncompressedBuffer(expectedBranding.size());
        auto uncompressedSize = static_cast<uLongf>(uncompressedBuffer.size());
        REQUIRE(uncompress(reinterpret_cast<Bytef*>(uncompressedBuffer.data()),
                           &uncompressedSize,
                           reinterpret_cast<const Bytef*>(rawFile->data.compressedBuffer),
                           static_cast<uLong>(rawFile->compressedLen))
                == Z_OK);
        REQUIRE(uncompressedSize == expectedBranding.size());
        REQUIRE(std::string_view(uncompressedBuffer.data(), uncompressedBuffer.size()) == expectedBranding);
    }
} // namespace test::rawfile
