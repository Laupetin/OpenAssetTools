#include "AssetDumperLoadedSound.h"

#include "Sound/WavTypes.h"

using namespace IW5;

bool AssetDumperLoadedSound::ShouldDump(XAssetInfo<LoadedSound>* asset)
{
    return true;
}

bool AssetDumperLoadedSound::CanDumpAsRaw()
{
    return true;
}

std::string AssetDumperLoadedSound::GetFileNameForAsset(Zone* zone, XAssetInfo<LoadedSound>* asset)
{
    return "sound/" + asset->m_name;
}

void AssetDumperLoadedSound::DumpWavPcm(AssetDumpingContext& context, const LoadedSound* asset, std::ostream& stream)
{
    const auto riffMasterChunkSize = sizeof(WAV_CHUNK_ID_RIFF)
        + sizeof(uint32_t)
        + sizeof(WAV_WAVE_ID)
        + sizeof(WavChunkHeader)
        + sizeof(WavFormatChunkPcm)
        + sizeof(WavChunkHeader)
        + sizeof(asset->sound.info.data_len);

    stream.write(reinterpret_cast<const char*>(&WAV_CHUNK_ID_RIFF), sizeof(WAV_CHUNK_ID_RIFF));
    stream.write(reinterpret_cast<const char*>(&riffMasterChunkSize), sizeof(riffMasterChunkSize));
    stream.write(reinterpret_cast<const char*>(&WAV_WAVE_ID), sizeof(WAV_WAVE_ID));

    const WavChunkHeader formatChunkHeader
    {
        WAV_CHUNK_ID_FMT,
        sizeof(WavFormatChunkPcm)
    };
    stream.write(reinterpret_cast<const char*>(&formatChunkHeader), sizeof(formatChunkHeader));

    WavFormatChunkPcm formatChunk
    {
        WavFormat::PCM,
        static_cast<uint16_t>(asset->sound.info.channels),
        asset->sound.info.rate,
        asset->sound.info.rate * asset->sound.info.channels * asset->sound.info.bits / 8,
        static_cast<uint16_t>(asset->sound.info.block_size),
        static_cast<uint16_t>(asset->sound.info.bits)
    };
    stream.write(reinterpret_cast<const char*>(&formatChunk), sizeof(formatChunk));

    const WavChunkHeader dataChunkHeader
    {
        WAV_CHUNK_ID_DATA,
        asset->sound.info.data_len
    };
    stream.write(reinterpret_cast<const char*>(&dataChunkHeader), sizeof(dataChunkHeader));
    stream.write(asset->sound.data, asset->sound.info.data_len);
}

void AssetDumperLoadedSound::DumpRaw(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset, std::ostream& stream)
{
    const auto* loadedSound = asset->Asset();
    switch (static_cast<WavFormat>(loadedSound->sound.info.format))
    {
    case WavFormat::PCM:
        DumpWavPcm(context, loadedSound, stream);
        break;

    default:
        printf("Unknown format %i for loaded sound: %s\n", loadedSound->sound.info.format, loadedSound->name);
        break;
    }
}
