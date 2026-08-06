#include "Game/QOS/Sound/SoundDumperQOS.h"

#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

using namespace QOS;

namespace
{
    uint16_t ReadU16(const std::string& data, const size_t offset)
    {
        return static_cast<uint16_t>(static_cast<unsigned char>(data[offset])) | static_cast<uint16_t>(static_cast<unsigned char>(data[offset + 1])) << 8u;
    }

    uint32_t ReadU32(const std::string& data, const size_t offset)
    {
        return static_cast<uint32_t>(static_cast<unsigned char>(data[offset])) | static_cast<uint32_t>(static_cast<unsigned char>(data[offset + 1])) << 8u
               | static_cast<uint32_t>(static_cast<unsigned char>(data[offset + 2])) << 16u
               | static_cast<uint32_t>(static_cast<unsigned char>(data[offset + 3])) << 24u;
    }

    TEST_CASE("SoundDumperQOS: Can dump inline MSS PCM data as WAV", "[qos][sound][assetdumper]")
    {
        constexpr std::array<unsigned char, 8> pcmData{0x00, 0x00, 0xE8, 0x03, 0x18, 0xFC, 0xFF, 0x7F};

        Zone zone("MockZone", 0, GameId::QOS, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        alignas(MssSound) std::array<unsigned char, offsetof(MssSound, data) + pcmData.size()> soundStorage{};
        auto* mssSound = std::construct_at(reinterpret_cast<MssSound*>(soundStorage.data()));
        mssSound->info.format = QOS_SOUND_FORMAT_PCM;
        mssSound->info.data_len = static_cast<unsigned int>(pcmData.size());
        mssSound->info.rate = 8000;
        mssSound->info.bits = 16;
        mssSound->info.channels = 1;
        mssSound->info.samples = static_cast<unsigned int>(pcmData.size() / 2);
        std::ranges::copy(pcmData, mssSound->data);

        SoundFile soundFile{};
        soundFile.dir = "weapons";
        soundFile.name = "test.raw";
        soundFile.u.mssSound = mssSound;
        soundFile.type = SAT_LOADED;

        snd_alias_t alias{};
        alias.aliasName = "test_alias";
        alias.soundFile = &soundFile;

        snd_alias_list_t aliasList{};
        aliasList.name = alias.aliasName;
        aliasList.head = &alias;
        aliasList.count = 1;

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, aliasList.name, &aliasList));

        sound::DumperQOS dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/weapons/test.wav");
        REQUIRE(file != nullptr);

        const auto output = file->AsString();
        REQUIRE(output.size() == 44 + pcmData.size());
        REQUIRE(output.substr(0, 4) == "RIFF");
        REQUIRE(output.substr(8, 4) == "WAVE");
        REQUIRE(ReadU16(output, 20) == 1);
        REQUIRE(ReadU16(output, 22) == 1);
        REQUIRE(ReadU32(output, 24) == 8000);
        REQUIRE(ReadU16(output, 34) == 16);
        REQUIRE(output.substr(36, 4) == "data");
        REQUIRE(ReadU32(output, 40) == pcmData.size());
        REQUIRE(std::memcmp(pcmData.data(), output.data() + 44, pcmData.size()) == 0);
    }

    TEST_CASE("SoundDumperQOS: Can dump inline MSS IMA ADPCM data as WAV", "[qos][sound][assetdumper]")
    {
        constexpr std::array<unsigned char, 8> adpcmData{0xFF, 0xFF, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};

        Zone zone("MockZone", 0, GameId::QOS, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        alignas(MssSound) std::array<unsigned char, offsetof(MssSound, data) + adpcmData.size()> soundStorage{};
        auto* mssSound = std::construct_at(reinterpret_cast<MssSound*>(soundStorage.data()));
        mssSound->info.format = QOS_SOUND_FORMAT_IMA_ADPCM;
        mssSound->info.data_len = static_cast<unsigned int>(adpcmData.size());
        mssSound->info.rate = 8000;
        mssSound->info.bits = 16;
        mssSound->info.channels = 1;
        mssSound->info.samples = 9;
        mssSound->info.block_size = static_cast<unsigned int>(adpcmData.size());
        std::ranges::copy(adpcmData, mssSound->data);

        SoundFile soundFile{};
        soundFile.dir = "exerts";
        soundFile.name = "pain.mp3";
        soundFile.u.mssSound = mssSound;
        soundFile.type = SAT_LOADED;

        snd_alias_t alias{};
        alias.aliasName = "pain_alias";
        alias.soundFile = &soundFile;

        snd_alias_list_t aliasList{};
        aliasList.name = alias.aliasName;
        aliasList.head = &alias;
        aliasList.count = 1;

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, aliasList.name, &aliasList));

        sound::DumperQOS dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/exerts/pain.wav");
        REQUIRE(file != nullptr);

        const auto output = file->AsString();
        REQUIRE(output.size() == 60 + adpcmData.size());
        REQUIRE(output.substr(0, 4) == "RIFF");
        REQUIRE(ReadU32(output, 4) == static_cast<uint32_t>(output.size() - 8));
        REQUIRE(output.substr(8, 4) == "WAVE");
        REQUIRE(ReadU16(output, 20) == 0x11);
        REQUIRE(ReadU16(output, 22) == 1);
        REQUIRE(ReadU32(output, 24) == 8000);
        REQUIRE(ReadU16(output, 32) == static_cast<uint16_t>(adpcmData.size()));
        REQUIRE(ReadU16(output, 34) == 4);
        REQUIRE(ReadU16(output, 38) == 9);
        REQUIRE(output.substr(40, 4) == "fact");
        REQUIRE(ReadU32(output, 48) == 9);
        REQUIRE(output.substr(52, 4) == "data");
        REQUIRE(ReadU32(output, 56) == static_cast<uint32_t>(adpcmData.size()));
        REQUIRE(std::memcmp(adpcmData.data(), output.data() + 60, adpcmData.size()) == 0);
    }

    TEST_CASE("SoundDumperQOS: Can dump a stream-backed sound from a pack", "[qos][sound][assetdumper]")
    {
        constexpr std::array<unsigned char, 8> primingData{0xFF, 0xFF, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
        const std::string mp3Data{"\xFF\xFB\x90\x64\x01\x02\x03\x04", 8};
        const std::string packPrefix{"pack-prefix"};

        Zone zone("MockZone", 0, GameId::QOS, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        mockObjPath.AddFileData("bxpack000.bin", packPrefix + mp3Data + "pack-suffix");
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        QosPrimedSound primedSound{};
        SoundFile soundFile{};
        soundFile.dir = "music";
        soundFile.name = "test.mp3";
        soundFile.exists = 1;
        soundFile.packageIndex[4] = 0;
        soundFile.packageOffset[4] = static_cast<unsigned int>(packPrefix.size());
        soundFile.packageSize[4] = static_cast<unsigned int>(mp3Data.size());

        SECTION("streamed")
        {
            soundFile.type = SAT_STREAMED;
        }

        SECTION("primed")
        {
            primedSound.buffer = reinterpret_cast<snd_align_char*>(const_cast<unsigned char*>(primingData.data()));
            primedSound.size = static_cast<unsigned int>(primingData.size());
            soundFile.u.primeSnd = &primedSound;
            soundFile.type = SAT_PRIMED;
        }

        snd_alias_t alias{};
        alias.aliasName = "music_alias";
        alias.soundFile = &soundFile;

        snd_alias_list_t aliasList{};
        aliasList.name = alias.aliasName;
        aliasList.head = &alias;
        aliasList.count = 1;

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, aliasList.name, &aliasList));

        sound::DumperQOS dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/music/test.mp3");
        REQUIRE(file != nullptr);
        REQUIRE(file->AsString() == mp3Data);
        REQUIRE(mockOutput.GetMockedFile("sound/music/test.bin") == nullptr);
    }

    TEST_CASE("SoundDumperQOS: Can dump a loose streamed sound", "[qos][sound][assetdumper]")
    {
        const std::string wavData{"RIFF\x04\0\0\0WAVE", 12};

        Zone zone("MockZone", 0, GameId::QOS, GamePlatform::PC);
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        mockObjPath.AddFileData("sound/music/loose.wav", wavData);
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

        SoundFile soundFile{};
        soundFile.dir = "music";
        soundFile.name = "loose";
        soundFile.exists = 1;
        soundFile.packageIndex[0] = SOUND_PACKAGE_INDEX_LOOSE;
        soundFile.type = SAT_STREAMED;

        snd_alias_t alias{};
        alias.aliasName = "loose_alias";
        alias.soundFile = &soundFile;

        snd_alias_list_t aliasList{};
        aliasList.name = alias.aliasName;
        aliasList.head = &alias;
        aliasList.count = 1;

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, aliasList.name, &aliasList));

        sound::DumperQOS dumper;
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("sound/music/loose.wav");
        REQUIRE(file != nullptr);
        REQUIRE(file->AsString() == wavData);
    }
} // namespace
