#include "Game/IW3/Sound/LoadedSoundLoaderIW3.h"

#include "SearchPath/MockSearchPath.h"
#include "Sound/WavTypes.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <string>

using namespace IW3;
using namespace std::literals;

namespace
{
    std::string PcmWav(const uint16_t channels = 1, const uint16_t bits = 16)
    {
        const auto blockSize = static_cast<uint16_t>(channels * bits / 8);
        const WavHeader header{
            WAV_CHUNK_ID_RIFF,
            40,
            WAV_WAVE_ID,
            {WAV_CHUNK_ID_FMT, 16},
            {WavFormat::PCM, channels, 22050, 22050u * blockSize, blockSize, bits},
            {WAV_CHUNK_ID_DATA, 4}
        };
        std::string data(reinterpret_cast<const char*>(&header), sizeof(header));
        data.append("\x00\x00\xFF\x7F", 4);
        return data;
    }

    TEST_CASE("LoadedSoundLoaderIW3: Validates RIFF chunks", "[iw3][loaded-sound][assetloader]")
    {
        MockSearchPath search;
        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignored;
        AssetCreationContext context(zone, &creators, &ignored);
        creators.AddAssetCreator(sound::CreateLoadedSoundLoaderIW3(memory, search));
        auto wav = PcmWav();
        uint16_t channels = 1;
        uint16_t bits = 16;
        bool valid = false;
        SECTION("Plain PCM")
        {
            valid = true;
        }
        SECTION("Mono 8-bit PCM")
        {
            bits = 8;
            wav = PcmWav(channels, bits);
            valid = true;
        }
        SECTION("Stereo 8-bit PCM")
        {
            channels = 2;
            bits = 8;
            wav = PcmWav(channels, bits);
            valid = true;
        }
        SECTION("Stereo 16-bit PCM")
        {
            channels = 2;
            wav = PcmWav(channels, bits);
            valid = true;
        }
        SECTION("Odd unknown chunk")
        {
            wav.insert(12, "JUNK\x01\x00\x00\x00x\x00"s);
            uint32_t size = static_cast<uint32_t>(wav.size() - 8);
            std::memcpy(wav.data() + 4, &size, sizeof(size));
            valid = true;
        }
        SECTION("Truncated data")
        {
            wav.pop_back();
        }
        SECTION("Unsupported codec")
        {
            wav[20] = 3;
        }
        SECTION("Invalid block alignment")
        {
            wav[32] = 0;
        }
        SECTION("Oversized chunk")
        {
            wav[40] = '\xFF';
        }
        search.AddFileData("sound/test.wav", wav);
        auto* info = context.LoadDependency<AssetLoadedSound>("test.wav");
        if (!valid)
        {
            REQUIRE(info == nullptr);
            return;
        }
        REQUIRE(info != nullptr);
        REQUIRE(info->Asset()->name == "test.wav"s);
        const auto& sound = info->Asset()->sound;
        REQUIRE(sound.info.format == 1);
        REQUIRE(sound.info.channels == channels);
        REQUIRE(sound.info.bits == bits);
        REQUIRE(sound.info.block_size == channels * bits / 8);
        REQUIRE(sound.info.rate == 22050);
        REQUIRE(sound.info.samples == 4 / (channels * bits / 8));
        REQUIRE(sound.info.data_len == 4);
        REQUIRE(std::string(sound.data, 4) == "\x00\x00\xFF\x7F"s);
    }
} // namespace
