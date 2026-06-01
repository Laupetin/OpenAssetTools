#include "Game/IW3/XAnim/XAnimDumperIW3.h"
#include "Game/IW3/XAnim/XAnimLoaderIW3.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    TEST_CASE("XAnim Loading/Dumping (IW3)", "[iw3][system]")
    {
        MockSearchPath searchPath;

        const auto filePath = oat::paths::GetTestDirectory() / "SystemTests/Game/IW3/XAnim/test_anim";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData("xanim/test_anim", std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        MemoryManager memoryManager;
        const auto loader = xanim::CreateLoaderIW3(memoryManager, searchPath, zone);
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        const auto result = loader->CreateAsset("test_anim", context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* assetInfo = reinterpret_cast<XAssetInfo<IW3::AssetXAnim::Type>*>(result.GetAssetInfo());
        const auto* parts = assetInfo->Asset();

        REQUIRE(parts->name == "test_anim"s);
        REQUIRE(parts->numframes > 0);

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        xanim::DumperIW3 dumper;
        AssetDumpingContext dumpingContext(zone, "", mockOutput, mockObjPath, std::nullopt);
        dumper.Dump(dumpingContext);

        const auto* outAnimFile = mockOutput.GetMockedFile("xanim/test_anim");
        REQUIRE(outAnimFile != nullptr);

        REQUIRE(outAnimFile->m_data.size() == fileSize);
        REQUIRE(memcmp(outAnimFile->m_data.data(), data.get(), fileSize) == 0);
    }
} // namespace
