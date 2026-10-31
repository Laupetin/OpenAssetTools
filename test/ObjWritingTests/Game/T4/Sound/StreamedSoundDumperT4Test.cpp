#include "Game/T4/Sound/StreamedSoundDumperT4.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <memory>
#include <string>

using namespace T4;

namespace
{
    TEST_CASE("StreamedSoundDumperT4: Copies every variant using its original path and bytes", "[t4][streamed-sound][assetdumper]")
    {
        const auto* directory = GENERATE("sfx/levels/test", "");
        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MockSearchPath searchPath;
        MockOutputPath outputPath;
        AssetDumpingContext context(zone, "", outputPath, searchPath, std::nullopt);

        const auto prefix = std::string("sound/") + directory + (*directory ? "/" : "");
        const std::string wav("RIFF\0\0\0\0WAVE", 12);
        const std::string xwma("RIFF\0\0\0\0XWMA", 12);
        searchPath.AddFileData(prefix + "first.wav", wav);
        searchPath.AddFileData(prefix + "second.xwma", xwma);
        SoundFile files[2]{};
        files[0].type = SAT_STREAMED;
        files[1].type = SAT_PRIMED;
        files[0].exists = files[1].exists = true;
        files[0].u.streamSnd.filename = {0, directory, "first.wav"};
        files[1].u.streamSnd.filename = {0, directory, "second.xwma"};
        snd_alias_t aliases[2]{};
        aliases[0].soundFile = &files[0];
        aliases[1].soundFile = &files[1];
        snd_alias_list_t asset{"different_alias_name", aliases, 2};
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, asset.aliasName, &asset));

        sound::StreamedSoundDumperT4 dumper;
        dumper.Dump(context);

        const auto* first = outputPath.GetMockedFile(prefix + "first.wav");
        const auto* second = outputPath.GetMockedFile(prefix + "second.xwma");
        REQUIRE(first);
        REQUIRE(second);
        CHECK(first->AsString() == wav);
        CHECK(second->AsString() == xwma);
        CHECK(outputPath.GetMockedFileList().size() == 2);
    }

    TEST_CASE("StreamedSoundDumperT4: Skips loaded and absent sounds and continues past missing files", "[t4][streamed-sound][assetdumper]")
    {
        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MockSearchPath searchPath;
        MockOutputPath outputPath;
        AssetDumpingContext context(zone, "", outputPath, searchPath, std::nullopt);
        searchPath.AddFileData("sound/last.wav", "last stream");
        SoundFile files[5]{};
        files[0].type = SAT_LOADED;
        files[0].exists = true;
        files[1].type = SAT_STREAMED;
        files[1].u.streamSnd.filename = {0, "", "absent.wav"};
        files[2].type = SAT_STREAMED;
        files[2].exists = true;
        files[2].u.streamSnd.filename = {0, "", "missing.wav"};
        files[3].type = SAT_STREAMED;
        files[3].exists = true;
        files[3].u.streamSnd.filename = {0, "", "last.wav"};
        files[4].type = SAT_UNKNOWN;
        files[4].exists = true;
        snd_alias_t aliases[6]{};
        for (auto i = 0; i < 5; i++)
            aliases[i].soundFile = &files[i];
        snd_alias_list_t asset{"test", aliases, 6};
        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, asset.aliasName, &asset));

        sound::StreamedSoundDumperT4 dumper;
        dumper.Dump(context);

        REQUIRE(outputPath.GetMockedFileList().size() == 1);
        const auto* file = outputPath.GetMockedFile("sound/last.wav");
        REQUIRE(file);
        CHECK(file->AsString() == "last stream");
    }
} // namespace
