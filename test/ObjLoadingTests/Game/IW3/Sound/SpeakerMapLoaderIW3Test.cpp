#include "Game/IW3/Sound/SpeakerMapLoaderIW3.h"

#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iterator>
#include <string>

using namespace IW3;

namespace
{
    TEST_CASE("SpeakerMapLoaderIW3: Loads SDK speaker order and shares maps", "[iw3][speaker-map][assetloader]")
    {
        MockSearchPath search;
        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignored;
        AssetCreationContext context(zone, &creators, &ignored);
        creators.AddSubAssetCreator(speaker_map::CreateSpeakerMapLoaderIW3(memory, search));
        const auto filePath = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/IW3/Sound/TestSpeakerMap.spkrmap";
        std::ifstream file(filePath);
        REQUIRE(file.is_open());
        std::string contents(std::istreambuf_iterator<char>{file}, {});
        bool valid = true;
        SECTION("Valid map") {}
        SECTION("Truncated map")
        {
            contents.resize(contents.size() / 2);
            valid = false;
        }
        SECTION("Invalid level")
        {
            const auto levelPosition = contents.find("LEFTSPEAKER 0.0");
            REQUIRE(levelPosition != std::string::npos);
            contents.replace(levelPosition, std::string("LEFTSPEAKER 0.0").size(), "LEFTSPEAKER 2.0");
            valid = false;
        }
        search.AddFileData("soundaliases/custom.spkrmap", contents);
        auto* info = context.LoadSubAsset<SubAssetSpeakerMap>("custom");
        if (!valid)
        {
            REQUIRE(info == nullptr);
            return;
        }
        REQUIRE(info != nullptr);
        REQUIRE_FALSE(info->Asset()->isDefault);
        REQUIRE(info->Asset()->channelMaps[1][1].speakers[5].levels[1] == Catch::Approx(0.6f));
        REQUIRE(context.LoadSubAsset<SubAssetSpeakerMap>("custom") == info);
    }

} // namespace
