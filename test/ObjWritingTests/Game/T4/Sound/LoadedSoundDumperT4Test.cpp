#include "Game/T4/Sound/LoadedSoundDumperT4.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <string>

using namespace T4;

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

    std::string CreatePcmWav()
    {
        constexpr auto dataSize = 8u;

        std::string result;
        result.append("RIFF", 4);
        AppendU32(result, 36u + dataSize);
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
        AppendU32(result, dataSize);
        AppendU16(result, 0u);
        AppendU16(result, 1000u);
        AppendU16(result, static_cast<uint16_t>(-1000));
        AppendU16(result, 32767u);
        return result;
    }

    TEST_CASE("LoadedSoundDumperT4: Can dump PCM WAV loaded sound", "[t4][loaded-sound][assetdumper]")
    {
        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        auto wavData = CreatePcmWav();
        auto loadedSound = std::make_unique<LoadedSound>();
        loadedSound->name = "test.wav";
        loadedSound->sound.data = wavData.data();
        loadedSound->sound.data_size = static_cast<int>(wavData.size());

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<LoadedSound>>(ASSET_TYPE_LOADED_SOUND, loadedSound->name, loadedSound.get()));

        sound::LoadedSoundDumperT4 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/test.wav");
        REQUIRE(file != nullptr);
        REQUIRE(file->AsString() == wavData);
    }
} // namespace
