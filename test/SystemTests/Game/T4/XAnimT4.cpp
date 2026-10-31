#include "Game/T4/XAnim/XAnimDumperT4.h"
#include "Game/T4/XAnim/XAnimLoaderT4.h"
#include "OatTestPaths.h"
#include "ObjWriting.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

namespace fs = std::filesystem;

namespace
{
    class XAnimOutputFormatScope
    {
    public:
        explicit XAnimOutputFormatScope(const XAnimOutputFormat_e format)
            : m_previous(ObjWriting::Configuration.XAnimOutputFormat)
        {
            ObjWriting::Configuration.XAnimOutputFormat = format;
        }

        ~XAnimOutputFormatScope()
        {
            ObjWriting::Configuration.XAnimOutputFormat = m_previous;
        }

    private:
        XAnimOutputFormat_e m_previous;
    };

    TEST_CASE("XAnim GLTF Loading/Dumping (T4)", "[t4][system]")
    {
        constexpr auto animName = "test_anim";
        const auto [outputFormat, extension] = GENERATE(Catch::Generators::table<XAnimOutputFormat_e, std::string>({
            {XAnimOutputFormat_e::GLTF, ".gltf"},
            {XAnimOutputFormat_e::GLB,  ".glb" },
        }));

        CAPTURE(extension);

        // T4 uses the same compiled XAnim layout as IW3, so it can share the version 17 fixture.
        const auto filePath = oat::paths::GetTestDirectory() / "SystemTests/Game/IW3/XAnim/test_anim";
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));
        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());
        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        MockSearchPath binarySearchPath;
        binarySearchPath.AddFileData(std::format("xanim/{}", animName), std::string(data.get(), fileSize));

        Zone sourceZone("MockZone", 0, GameId::T4, GamePlatform::PC);
        AssetCreatorCollection sourceCreators(sourceZone);
        IgnoredAssetLookup ignoredAssetLookup;
        MemoryManager sourceMemory;
        const auto binaryLoader = xanim::CreateLoaderT4(sourceMemory, binarySearchPath, sourceZone);
        AssetCreationContext sourceContext(sourceZone, &sourceCreators, &ignoredAssetLookup);
        const auto sourceResult = binaryLoader->CreateAsset(animName, sourceContext);
        REQUIRE(sourceResult.HasBeenSuccessful());
        const auto* sourceParts = reinterpret_cast<XAssetInfo<T4::AssetXAnim::Type>*>(sourceResult.GetAssetInfo())->Asset();

        MockSearchPath mockObjPath;
        MockOutputPath output;
        const XAnimOutputFormatScope outputFormatScope(outputFormat);
        xanim::DumperT4 dumper;
        AssetDumpingContext dumpingContext(sourceZone, "", output, mockObjPath, std::nullopt);
        dumper.Dump(dumpingContext);

        const auto xanimFileName = std::format("xanim/{}{}", animName, extension);
        const auto* xanimFile = output.GetMockedFile(xanimFileName);
        REQUIRE(xanimFile != nullptr);

        MockSearchPath gltfSearchPath;
        gltfSearchPath.AddFileData(xanimFileName, xanimFile->AsString());
        Zone loadedZone("LoadedZone", 0, GameId::T4, GamePlatform::PC);
        AssetCreatorCollection loadedCreators(loadedZone);
        MemoryManager loadedMemory;
        const auto gltfLoader = xanim::CreateLoaderT4(loadedMemory, gltfSearchPath, loadedZone);
        AssetCreationContext loadedContext(loadedZone, &loadedCreators, &ignoredAssetLookup);
        const auto loadedResult = gltfLoader->CreateAsset(animName, loadedContext);

        REQUIRE(loadedResult.HasBeenSuccessful());
        const auto* loadedParts = reinterpret_cast<XAssetInfo<T4::AssetXAnim::Type>*>(loadedResult.GetAssetInfo())->Asset();
        REQUIRE(loadedParts->numframes == sourceParts->numframes);
        REQUIRE(loadedParts->boneCount[T4::PART_TYPE_ALL] == sourceParts->boneCount[T4::PART_TYPE_ALL]);
        REQUIRE(loadedParts->notifyCount == sourceParts->notifyCount);
        REQUIRE(std::abs(loadedParts->framerate - sourceParts->framerate) < 0.001f);
        REQUIRE(loadedParts->bLoop == sourceParts->bLoop);
        REQUIRE(loadedParts->bDelta == sourceParts->bDelta);
        REQUIRE(static_cast<bool>(loadedParts->deltaPart) == static_cast<bool>(sourceParts->deltaPart));
    }
} // namespace
