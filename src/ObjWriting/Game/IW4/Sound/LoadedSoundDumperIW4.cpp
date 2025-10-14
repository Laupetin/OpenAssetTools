#include "LoadedSoundDumperIW4.h"

#include "Sound/WavTypes.h"
#include "Sound/WavWriter.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace IW4;

namespace
{
    void DumpWavPcm(const LoadedSound* asset, std::ostream& stream)
    {
        const WavWriter writer(stream);

        const WavMetaData metaData{.channelCount = static_cast<unsigned>(asset->sound.info.channels),
                                   .samplesPerSec = static_cast<unsigned>(asset->sound.info.rate),
                                   .bitsPerSample = static_cast<unsigned>(asset->sound.info.bits)};

        writer.WritePcmHeader(metaData, asset->sound.info.data_len);
        writer.WritePcmData(asset->sound.data, asset->sound.info.data_len);
    }
} // namespace

namespace sound
{
    void LoadedSoundDumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<LoadedSound>& asset)
    {
        const auto* loadedSound = asset.Asset();
        const auto assetFile = context.OpenAssetFile(std::format("sound/{}", asset.m_name));

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        switch (static_cast<WavFormat>(loadedSound->sound.info.format))
        {
        case WavFormat::PCM:
            DumpWavPcm(loadedSound, stream);
            break;

        default:
            con::error("Unknown format {} for loaded sound: {}", loadedSound->sound.info.format, loadedSound->name);
            break;
        }
    }
} // namespace sound
