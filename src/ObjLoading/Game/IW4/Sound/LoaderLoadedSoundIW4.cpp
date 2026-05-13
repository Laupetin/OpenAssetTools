#include "LoaderLoadedSoundIW4.h"

#include "Sound/WavReader.h"
#include "Sound/WavTypes.h"
#include "Utils/Logging/Log.h"

#include <Sound/LoadedSoundCommon.h>
#include <format>

using namespace IW4;

namespace
{
    class LoadedSoundLoader final : public AssetCreator<AssetLoadedSound>
    {
    public:
        LoadedSoundLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = loaded_sound::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* loadedSound = m_memory.Alloc<LoadedSound>();
            loadedSound->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetLoadedSound> registration(assetName, loadedSound);

            MssSound& mss = loadedSound->sound;
            WavReader reader(*file.m_stream);
            WavMetaData meta;
            size_t dataLen = 0;
            if (!reader.ReadPcmHeader(meta, dataLen))
            {
                con::error("Invalid or unsupported WAV file!");
                return AssetCreationResult::NoAction();
            }

            std::vector<uint8_t> pcm(dataLen);
            if (!reader.ReadPcmData(pcm.data(), dataLen))
            {
                con::error("Failed to read PCM data!");
                return AssetCreationResult::NoAction();
            }

            char* buf = nullptr;
            if (dataLen > 0)
            {
                buf = static_cast<char*>(std::malloc(dataLen));
                if (!buf)
                {
                    con::error("Out of memory allocating PCM buffer");
                    return AssetCreationResult::NoAction();
                }
                std::memcpy(buf, pcm.data(), dataLen);
            }

            mss.data = buf;
            mss.info.format = (int)WavFormat::PCM;
            mss.info.data_ptr = static_cast<const void*>(mss.data);
            mss.info.data_len = static_cast<unsigned int>(dataLen);
            mss.info.rate = static_cast<unsigned int>(meta.samplesPerSec);
            mss.info.bits = static_cast<int>(meta.bitsPerSample);
            mss.info.channels = static_cast<int>(meta.channelCount);

            // compute block size and samples
            const unsigned int bytesPerSample = (mss.info.bits / 8u);
            const unsigned int blockSize = (mss.info.channels > 0 && bytesPerSample > 0) ? (static_cast<unsigned int>(mss.info.channels) * bytesPerSample) : 0u;
            mss.info.block_size = blockSize;
            mss.info.samples = (blockSize > 0) ? (mss.info.data_len / blockSize) : 0u;
            mss.info.initial_ptr = mss.info.data_ptr;

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace loaded_sound
{
    std::unique_ptr<AssetCreator<AssetLoadedSound>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoadedSoundLoader>(memory, searchPath);
    }
}; // namespace loaded_sound
