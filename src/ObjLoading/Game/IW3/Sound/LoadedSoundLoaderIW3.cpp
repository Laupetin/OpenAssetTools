#include "LoadedSoundLoaderIW3.h"

#include "Sound/WavTypes.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <limits>

using namespace IW3;

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
            const auto fileName = std::format("sound/{}", assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* sound = m_memory.Alloc<LoadedSound>();
            sound->name = m_memory.Dup(assetName.c_str());
            if (!ReadWav(*file.m_stream, file.m_length, *sound))
            {
                con::error("Invalid or unsupported WAV '{}': expected mono/stereo 8/16-bit PCM", fileName);
                return AssetCreationResult::Failure();
            }
            return AssetCreationResult::Success(context.AddAsset<AssetLoadedSound>(assetName, sound));
        }

    private:
        bool ReadWav(std::istream& stream, const int64_t length, LoadedSound& sound)
        {
            WavChunkHeader riff{};
            uint32_t wave{};
            if (!stream.read(reinterpret_cast<char*>(&riff), sizeof(riff)) || !stream.read(reinterpret_cast<char*>(&wave), sizeof(wave))
                || riff.chunkID != WAV_CHUNK_ID_RIFF || wave != WAV_WAVE_ID || riff.chunkSize < 4 || uint64_t(riff.chunkSize) + 8 > uint64_t(length))
                return false;

            WavFormatChunkPcm format{};
            bool hasFormat = false;
            bool hasData = false;
            uint64_t remaining = riff.chunkSize - 4;
            while (remaining >= sizeof(WavChunkHeader))
            {
                WavChunkHeader chunk{};
                if (!stream.read(reinterpret_cast<char*>(&chunk), sizeof(chunk)))
                    return false;
                remaining -= sizeof(chunk);
                const auto paddedSize = uint64_t(chunk.chunkSize) + (chunk.chunkSize & 1u);
                if (paddedSize > remaining)
                    return false;
                if (chunk.chunkID == WAV_CHUNK_ID_FMT)
                {
                    if (hasFormat || chunk.chunkSize < sizeof(format) || !stream.read(reinterpret_cast<char*>(&format), sizeof(format)))
                        return false;
                    hasFormat = true;
                    stream.ignore(chunk.chunkSize - sizeof(format));
                }
                else if (chunk.chunkID == WAV_CHUNK_ID_DATA)
                {
                    if (hasData || !chunk.chunkSize || chunk.chunkSize > static_cast<uint32_t>(std::numeric_limits<int>::max()))
                        return false;
                    hasData = true;
                    sound.sound.data = m_memory.Alloc<char>(chunk.chunkSize);
                    sound.sound.info.data_len = chunk.chunkSize;
                    stream.read(sound.sound.data, chunk.chunkSize);
                }
                else
                    stream.ignore(chunk.chunkSize);
                if (chunk.chunkSize & 1u)
                    stream.ignore(1);
                if (!stream)
                    return false;
                remaining -= paddedSize;
            }
            if (remaining || !hasFormat || !hasData || format.wFormatTag != WavFormat::PCM || (format.nChannels != 1 && format.nChannels != 2)
                || (format.wBitsPerSample != 8 && format.wBitsPerSample != 16) || !format.nSamplesPerSec
                || format.nBlockAlign != format.nChannels * format.wBitsPerSample / 8
                || uint64_t(format.nAvgBytesPerSec) != uint64_t(format.nSamplesPerSec) * format.nBlockAlign || sound.sound.info.data_len % format.nBlockAlign)
                return false;

            auto& info = sound.sound.info;
            info.format = static_cast<int>(WavFormat::PCM);
            info.rate = format.nSamplesPerSec;
            info.bits = format.wBitsPerSample;
            info.channels = format.nChannels;
            info.samples = info.data_len / format.nBlockAlign;
            info.block_size = format.nBlockAlign;
            info.data_ptr = sound.sound.data;
            return true;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace sound
{
    std::unique_ptr<AssetCreator<AssetLoadedSound>> CreateLoadedSoundLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoadedSoundLoader>(memory, searchPath);
    }
} // namespace sound
