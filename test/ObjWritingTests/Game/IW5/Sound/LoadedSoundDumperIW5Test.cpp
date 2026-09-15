#include "Game/IW5/Sound/LoadedSoundDumperIW5.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

using namespace IW5;

namespace
{
    uint16_t ReadU16(const std::string& data, const size_t offset)
    {
        return static_cast<uint16_t>(static_cast<uint8_t>(data[offset])) | static_cast<uint16_t>(static_cast<uint8_t>(data[offset + 1])) << 8u;
    }

    uint32_t ReadU32(const std::string& data, const size_t offset)
    {
        return static_cast<uint32_t>(static_cast<uint8_t>(data[offset])) | static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 1])) << 8u
               | static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 2])) << 16u | static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 3])) << 24u;
    }

#if defined(ARCH_x64)
    TEST_CASE("IW5 x64 loaded sound metadata matches its serialized PCM header", "[iw5][loaded-sound][asset-layout]")
    {
        STATIC_REQUIRE(sizeof(AILSOUNDINFO) == 48u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, format) == 0u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, channels) == 2u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, rate) == 4u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, bytesPerSecond) == 8u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, blockAlign) == 12u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, bits) == 14u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, data_len) == 24u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, data_ptr) == 32u);
        STATIC_REQUIRE(offsetof(AILSOUNDINFO, initial_ptr) == 40u);
    }
#endif

    TEST_CASE("LoadedSoundDumperIW5 writes mono 44100 Hz PCM metadata", "[iw5][loaded-sound][assetdumper]")
    {
        Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        std::array<char, 4> pcmData{};
        auto loadedSound = std::make_unique<LoadedSound>();
        loadedSound->name = "test.wav";
        loadedSound->sound.info.format = 1;
        loadedSound->sound.info.channels = 1;
        loadedSound->sound.info.rate = 44100;
        loadedSound->sound.info.bits = 16;
        loadedSound->sound.info.data_len = static_cast<unsigned int>(pcmData.size());
        loadedSound->sound.data = pcmData.data();

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<LoadedSound>>(ASSET_TYPE_LOADED_SOUND, loadedSound->name, loadedSound.get()));

        sound::LoadedSoundDumperIW5 dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/test.wav");
        REQUIRE(file != nullptr);
        const auto wav = file->AsString();
        REQUIRE(wav.size() == 44u + pcmData.size());
        REQUIRE(wav.substr(0, 4) == "RIFF");
        REQUIRE(wav.substr(8, 4) == "WAVE");
        REQUIRE(ReadU16(wav, 20) == 1u);
        REQUIRE(ReadU16(wav, 22) == 1u);
        REQUIRE(ReadU32(wav, 24) == 44100u);
        REQUIRE(ReadU32(wav, 28) == 88200u);
        REQUIRE(ReadU16(wav, 32) == 2u);
        REQUIRE(ReadU16(wav, 34) == 16u);
        REQUIRE(ReadU32(wav, 40) == pcmData.size());
    }
} // namespace
