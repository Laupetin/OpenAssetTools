#include "AssetDumperLoadedSound.h"

#include "Sound/WavTypes.h"
#include "Sound/WavWriter.h"

using namespace IW4;

bool AssetDumperLoadedSound::ShouldDump(XAssetInfo<LoadedSound>* asset)
{
    return true;
}

void AssetDumperLoadedSound::DumpWavPcm(AssetDumpingContext& context, const LoadedSound* asset, std::ostream& stream)
{
    const WavWriter writer(stream);

    const WavMetaData metaData{
        static_cast<unsigned>(asset->sound.info.channels), static_cast<unsigned>(asset->sound.info.rate), static_cast<unsigned>(asset->sound.info.bits)};

    writer.WritePcmHeader(metaData, asset->sound.info.data_len);
    writer.WritePcmData(asset->sound.data, asset->sound.info.data_len);
}

void AssetDumperLoadedSound::DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset)
{
    const auto* loadedSound = asset->Asset();
    const auto assetFile = context.OpenAssetFile("sound/" + asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
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
