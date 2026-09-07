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
    std::string PcmWav()
    {
        const WavHeader header{
            WAV_CHUNK_ID_RIFF, 40, WAV_WAVE_ID, {WAV_CHUNK_ID_FMT, 16},
               {WavFormat::PCM, 1, 22050, 44100, 2, 16},
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
        bool valid = false;
        SECTION("Plain PCM")
        {
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
        const auto& sound = info->Asset()->sound;
        REQUIRE(sound.info.format == 1);
        REQUIRE(sound.info.channels == 1);
        REQUIRE(sound.info.bits == 16);
        REQUIRE(sound.info.block_size == 2);
        REQUIRE(sound.info.data_len == 4);
        REQUIRE(std::string(sound.data, 4) == "\x00\x00\xFF\x7F"s);
    }
} // namespace
