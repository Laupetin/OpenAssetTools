#include "Game/IW5/Sound/LoadedSoundDumperIW5.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <string>

using namespace IW5;

namespace
{
    void AppendU16(std::string& data, const uint16_t value)
    {
        data.push_back(static_cast<char>(value & 0xFF));
        data.push_back(static_cast<char>((value >> 8) & 0xFF));
    }

    void AppendU32(std::string& data, const uint32_t value)
    {
        data.push_back(static_cast<char>(value & 0xFF));
        data.push_back(static_cast<char>((value >> 8) & 0xFF));
        data.push_back(static_cast<char>((value >> 16) & 0xFF));
        data.push_back(static_cast<char>((value >> 24) & 0xFF));
    }

    std::string CreateExpectedPcmWav(const std::string& pcmData)
    {
        std::string result;
        result.append("RIFF", 4);
        AppendU32(result, 36u + static_cast<uint32_t>(pcmData.size()));
        result.append("WAVE", 4);
        result.append("fmt ", 4);
        AppendU32(result, 16u);
        AppendU16(result, 1u);
        AppendU16(result, 1u);
        AppendU32(result, 8000u);
        AppendU32(result, 16000u);
        AppendU16(result, 2u);
        AppendU16(result, 16u);
        result.append("data", 4);
        AppendU32(result, static_cast<uint32_t>(pcmData.size()));
        result.append(pcmData);
        return result;
    }

    TEST_CASE("LoadedSoundDumperIW5: Can dump PCM loaded sound", "[iw5][loaded-sound][assetdumper]")
    {
        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        std::string pcmData{"\0\0\xE8\x03\x18\xFC\xFF\x7F", 8};
        auto loadedSound = std::make_unique<LoadedSound>();
        loadedSound->name = "test.wav";
        loadedSound->sound.info.format = 1;
        loadedSound->sound.info.rate = 8000;
        loadedSound->sound.info.channels = 1;
        loadedSound->sound.info.bits = 16;
        loadedSound->sound.info.data_len = static_cast<unsigned>(pcmData.size());
        loadedSound->sound.data = pcmData.data();

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<LoadedSound>>(ASSET_TYPE_LOADED_SOUND, loadedSound->name, loadedSound.get()));

        sound::LoadedSoundDumperIW5 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/test.wav");
        REQUIRE(file != nullptr);
        REQUIRE(file->AsString() == CreateExpectedPcmWav(pcmData));
    }

    TEST_CASE("LoadedSoundDumperIW5: Does not create a file for unsupported formats", "[iw5][loaded-sound][assetdumper]")
    {
        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        auto loadedSound = std::make_unique<LoadedSound>();
        loadedSound->name = "empty.wav";

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<LoadedSound>>(ASSET_TYPE_LOADED_SOUND, loadedSound->name, loadedSound.get()));

        sound::LoadedSoundDumperIW5 dumper;
        dumper.Dump(context);

        REQUIRE(mockOutput.GetMockedFile("sound/empty.wav") == nullptr);
    }
} // namespace
