#include "Game/IW4/XAnim/XAnimDumperIW4.h"
#include "Game/IW4/XAnim/XAnimLoaderIW4.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>

using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    TEST_CASE("XAnim Loading/Dumping (IW4)", "[iw4][system]")
    {
        MockSearchPath searchPath;

        const auto [animName] = GENERATE(Catch::Generators::table<std::string>({
            {"test_anim"},
            {"test_anim2"},
        }));

        CAPTURE(animName);

        const auto filePath = oat::paths::GetTestDirectory() / std::format("SystemTests/Game/IW4/XAnim/{}", animName);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        searchPath.AddFileData(std::format("xanim/{}", animName), std::string(data.get(), fileSize));

        Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        MemoryManager memoryManager;
        const auto loader = xanim::CreateLoaderIW4(memoryManager, searchPath, zone);
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        const auto result = loader->CreateAsset(animName, context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* assetInfo = reinterpret_cast<XAssetInfo<IW4::AssetXAnim::Type>*>(result.GetAssetInfo());
        const auto* parts = assetInfo->Asset();

        REQUIRE(parts->name == animName);
        REQUIRE(parts->numframes > 0);

        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        xanim::DumperIW4 dumper;
        AssetDumpingContext dumpingContext(zone, "", mockOutput, mockObjPath, std::nullopt);
        dumper.Dump(dumpingContext);

        const auto* outAnimFile = mockOutput.GetMockedFile(std::format("xanim/{}", animName));
        REQUIRE(outAnimFile != nullptr);

        REQUIRE(outAnimFile->m_data.size() == fileSize);
        REQUIRE(memcmp(outAnimFile->m_data.data(), data.get(), fileSize) == 0);
    }
} // namespace
