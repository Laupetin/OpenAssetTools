#include "LoadedSoundDumperIW6.h"

#include "Sound/WavTypes.h"
#include "Sound/WavWriter.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace IW6;

namespace
{
    void DumpWavPcm(const LoadedSound* asset, std::ostream& stream)
    {
        const WavWriter writer(stream);

        const WavMetaData metaData{.channelCount = static_cast<unsigned>(asset->sound.format.channels),
                                   .samplesPerSec = static_cast<unsigned>(asset->sound.format.sampleRate),
                                   .bitsPerSample = static_cast<unsigned>(asset->sound.format.numBits)};

        writer.WritePcmHeader(metaData, asset->sound.format.dataByteCount);
        writer.WritePcmData(asset->sound.data, asset->sound.format.dataByteCount);
    }
} // namespace

namespace sound
{
    void LoadedSoundDumperIW6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetLoadedSound::Type>& asset)
    {
        const auto* loadedSound = asset.Asset();
        const auto assetFile = context.OpenAssetFile(std::format("sound/{}", asset.m_name));

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        switch (static_cast<WavFormat>(loadedSound->sound.format.format))
        {
        case WavFormat::PCM:
            DumpWavPcm(loadedSound, stream);
            break;
            // TODO: Figure out
            // case WavFormat::XMA2:
            //    DumpWavXma2(loadedSound, stream);
            //    break;
            // case WavFormat::ADPCM:
            //    DumpWavADPcm(loadedSound, stream);
            //    break;
            // case WavFormat::XWMA:
            //    DumpWavXWMA(loadedSound, stream);
            //    break;
            // case WavFormat::MP3:
            //    DumpWavMp3(loadedSound, stream);
            //    break;
            // case WavFormat::WIIUDSP:
            //    DumpWavWiiUDSP(loadedSound, stream);
            //    break;
            // case WavFormat::FLAC:
            //    DumpWavFlac(loadedSound, stream);
            //    break;

        default:
            // con::error("Unknown format {} for loaded sound: {}", loadedSound->sound.format.format, loadedSound->name);
            break;
        }
    }
} // namespace sound
