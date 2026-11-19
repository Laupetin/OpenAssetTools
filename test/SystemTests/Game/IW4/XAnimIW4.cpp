#include "Game/IW4/XAnim/XAnimDumperIW4.h"
#include "Game/IW4/XAnim/XAnimLoaderIW4.h"
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
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace std::literals;
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

    TEST_CASE("XAnim GLTF Loading/Dumping (IW4)", "[iw4][system]")
    {
        const auto [animName] = GENERATE(Catch::Generators::table<std::string>({
            {"test_anim"},
            {"test_anim2"},
        }));
        const auto [outputFormat, extension] = GENERATE(Catch::Generators::table<XAnimOutputFormat_e, std::string>({
            {XAnimOutputFormat_e::GLTF, ".gltf"},
            {XAnimOutputFormat_e::GLB,  ".glb" },
        }));

        CAPTURE(animName);
        CAPTURE(extension);
        const auto filePath = oat::paths::GetTestDirectory() / std::format("SystemTests/Game/IW4/XAnim/{}", animName);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());
        const auto data = std::make_unique<char[]>(fileSize);
        file.read(data.get(), fileSize);

        MockSearchPath binarySearchPath;
        binarySearchPath.AddFileData(std::format("xanim/{}", animName), std::string(data.get(), fileSize));

        Zone sourceZone("MockZone", 0, GameId::IW4, GamePlatform::PC);
        AssetCreatorCollection sourceCreators(sourceZone);
        IgnoredAssetLookup ignoredAssetLookup;
        MemoryManager sourceMemory;
        const auto binaryLoader = xanim::CreateLoaderIW4(sourceMemory, binarySearchPath, sourceZone);
        AssetCreationContext sourceContext(sourceZone, &sourceCreators, &ignoredAssetLookup);
        const auto sourceResult = binaryLoader->CreateAsset(animName, sourceContext);
        REQUIRE(sourceResult.HasBeenSuccessful());
        const auto* sourceParts = reinterpret_cast<XAssetInfo<IW4::AssetXAnim::Type>*>(sourceResult.GetAssetInfo())->Asset();

        MockSearchPath mockObjPath;
        MockOutputPath output;
        const XAnimOutputFormatScope outputFormatScope(outputFormat);
        xanim::DumperIW4 dumper;
        AssetDumpingContext dumpingContext(sourceZone, "", output, mockObjPath, std::nullopt);
        dumper.Dump(dumpingContext);

        const auto xanimFileName = std::format("xanim/{}{}", animName, extension);
        const auto* xanimFile = output.GetMockedFile(xanimFileName);
        REQUIRE(xanimFile != nullptr);
        const auto xanimData = xanimFile->AsString();

        std::optional<nlohmann::json> gltfJson;
        if (outputFormat == XAnimOutputFormat_e::GLTF)
        {
            gltfJson = nlohmann::json::parse(xanimData);
            REQUIRE(gltfJson->at("animations").size() == 1u);
            REQUIRE(gltfJson->at("extras").contains("OAT_xanim"));
        }

        MockSearchPath gltfSearchPath;
        gltfSearchPath.AddFileData(xanimFileName, xanimData);
        Zone loadedZone("LoadedZone", 0, GameId::IW4, GamePlatform::PC);
        AssetCreatorCollection loadedCreators(loadedZone);
        MemoryManager loadedMemory;
        const auto gltfLoader = xanim::CreateLoaderIW4(loadedMemory, gltfSearchPath, loadedZone);
        AssetCreationContext loadedContext(loadedZone, &loadedCreators, &ignoredAssetLookup);
        const auto loadedResult = gltfLoader->CreateAsset(animName, loadedContext);

        REQUIRE(loadedResult.HasBeenSuccessful());
        const auto* loadedParts = reinterpret_cast<XAssetInfo<IW4::AssetXAnim::Type>*>(loadedResult.GetAssetInfo())->Asset();
        REQUIRE(loadedParts->numframes == sourceParts->numframes);
        REQUIRE(loadedParts->boneCount[IW4::PART_TYPE_ALL] == sourceParts->boneCount[IW4::PART_TYPE_ALL]);
        REQUIRE(loadedParts->notifyCount == sourceParts->notifyCount);
        REQUIRE(std::abs(loadedParts->framerate - sourceParts->framerate) < 0.001f);
        REQUIRE(static_cast<bool>(loadedParts->deltaPart) == static_cast<bool>(sourceParts->deltaPart));
        constexpr auto preservedFlags = IW4::ANIM_LOOP | IW4::ANIM_DELTA | IW4::ANIM_DELTA_3D;
        REQUIRE((loadedParts->flags & preservedFlags) == (sourceParts->flags & preservedFlags));

        if (gltfJson)
        {
            gltfJson->erase("extras");
            const auto authoredGltfData = gltfJson->dump();
            MockSearchPath authoredGltfSearchPath;
            authoredGltfSearchPath.AddFileData(std::format("xanim/{}.gltf", animName), authoredGltfData);
            Zone authoredZone("AuthoredZone", 0, GameId::IW4, GamePlatform::PC);
            AssetCreatorCollection authoredCreators(authoredZone);
            MemoryManager authoredMemory;
            const auto authoredGltfLoader = xanim::CreateLoaderIW4(authoredMemory, authoredGltfSearchPath, authoredZone);
            AssetCreationContext authoredContext(authoredZone, &authoredCreators, &ignoredAssetLookup);
            const auto authoredResult = authoredGltfLoader->CreateAsset(animName, authoredContext);

            REQUIRE(authoredResult.HasBeenSuccessful());
            const auto* authoredParts = reinterpret_cast<XAssetInfo<IW4::AssetXAnim::Type>*>(authoredResult.GetAssetInfo())->Asset();
            REQUIRE(authoredParts->numframes > 0u);
            REQUIRE(authoredParts->boneCount[IW4::PART_TYPE_ALL] > 0u);
        }
    }
} // namespace
