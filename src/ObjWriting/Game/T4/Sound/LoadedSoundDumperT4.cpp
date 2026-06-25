#include "LoadedSoundDumperT4.h"

#include "Utils/Endianness.h"
#include "Utils/Logging/Log.h"
#include "Utils/StreamUtils.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

using namespace T4;

namespace
{
    enum class WavFormat : uint16_t
    {
        Pcm = 1,
        MsAdpcm = 2,
    };

    constexpr auto FOURCC_SIZE = 4uz;
    constexpr auto RIFF_WAVE_HEADER_SIZE = 12uz;
    constexpr auto RIFF_CHUNK_HEADER_SIZE = 8uz;

    constexpr std::string_view RIFF_ID = "RIFF";
    constexpr std::string_view WAVE_FORM_TYPE = "WAVE";
    constexpr std::string_view WAVE_FMT_CHUNK_ID = "fmt ";
    constexpr std::string_view WAVE_DATA_CHUNK_ID = "data";

    // Standard Microsoft ADPCM step-size adaptation factors, indexed by encoded nibble.
    constexpr std::array<int, 16> MS_ADPCM_ADAPTATION_TABLE{
        230,
        230,
        230,
        230,
        307,
        409,
        512,
        614,
        768,
        614,
        512,
        409,
        307,
        230,
        230,
        230,
    };

    struct MsAdpcmCoef
    {
        int coeff1;
        int coeff2;
    };

    struct WavInfo
    {
        WavFormat format;
        uint16_t channels;
        uint32_t sampleRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint16_t samplesPerBlock;
        std::vector<MsAdpcmCoef> coefficients;
        const char* data;
        size_t dataSize;
    };

    struct DecodedPcm
    {
        uint16_t channels;
        uint32_t sampleRate;
        std::vector<int16_t> samples;
    };

    struct MsAdpcmChannelState
    {
        int coeff1;
        int coeff2;
        int delta;
        int sample1;
        int sample2;
    };

    [[nodiscard]] uint16_t ReadUint16LE(const char* data)
    {
        const auto* bytes = reinterpret_cast<const uint8_t*>(data);
        return static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
    }

    [[nodiscard]] int16_t ReadInt16LE(const char* data)
    {
        return static_cast<int16_t>(ReadUint16LE(data));
    }

    [[nodiscard]] uint32_t ReadUint32LE(const char* data)
    {
        const auto* bytes = reinterpret_cast<const uint8_t*>(data);
        return static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) | (static_cast<uint32_t>(bytes[2]) << 16)
               | (static_cast<uint32_t>(bytes[3]) << 24);
    }

    template<typename T> void WriteLittleEndian(std::ostream& stream, const T value)
    {
        stream::WriteValue(stream, endianness::ToLittleEndian(value));
    }

    [[nodiscard]] bool IsFourCc(const char* data, const std::string_view fourCc)
    {
        return fourCc.size() == FOURCC_SIZE && std::equal(fourCc.begin(), fourCc.end(), data);
    }

    [[nodiscard]] std::expected<WavInfo, std::string> ParseWav(const char* data, const size_t dataSize)
    {
        if (dataSize < RIFF_WAVE_HEADER_SIZE || !IsFourCc(data, RIFF_ID) || !IsFourCc(&data[RIFF_CHUNK_HEADER_SIZE], WAVE_FORM_TYPE))
            return std::unexpected<std::string>("not RIFF/WAVE");

        WavInfo wav{};
        auto hasFormat = false;
        auto hasData = false;

        for (size_t offset = RIFF_WAVE_HEADER_SIZE; offset + RIFF_CHUNK_HEADER_SIZE <= dataSize;)
        {
            const auto* chunk = &data[offset];
            const auto chunkSize = static_cast<size_t>(ReadUint32LE(&chunk[FOURCC_SIZE]));
            const auto payloadOffset = offset + RIFF_CHUNK_HEADER_SIZE;

            if (chunkSize > dataSize - payloadOffset)
                return std::unexpected<std::string>("WAV chunk extends past end of file");

            const auto* payload = &data[payloadOffset];
            if (IsFourCc(chunk, WAVE_FMT_CHUNK_ID))
            {
                if (chunkSize < 16)
                    return std::unexpected<std::string>("WAV fmt chunk is truncated");

                wav.format = static_cast<WavFormat>(ReadUint16LE(&payload[0]));
                wav.channels = ReadUint16LE(&payload[2]);
                wav.sampleRate = ReadUint32LE(&payload[4]);
                wav.blockAlign = ReadUint16LE(&payload[12]);
                wav.bitsPerSample = ReadUint16LE(&payload[14]);

                if (wav.format == WavFormat::MsAdpcm)
                {
                    if (chunkSize < 22)
                        return std::unexpected<std::string>("MS ADPCM fmt chunk is missing extension data");

                    wav.samplesPerBlock = ReadUint16LE(&payload[18]);
                    const auto coefficientCount = ReadUint16LE(&payload[20]);
                    if (chunkSize < 22uz + static_cast<size_t>(coefficientCount) * 4uz)
                        return std::unexpected<std::string>("MS ADPCM coefficient table is truncated");

                    wav.coefficients.reserve(coefficientCount);
                    for (auto i = 0u; i < coefficientCount; i++)
                    {
                        const auto coefficientOffset = 22uz + static_cast<size_t>(i) * 4uz;
                        wav.coefficients.push_back({
                            ReadInt16LE(&payload[coefficientOffset]),
                            ReadInt16LE(&payload[coefficientOffset + 2]),
                        });
                    }
                }

                hasFormat = true;
            }
            else if (IsFourCc(chunk, WAVE_DATA_CHUNK_ID))
            {
                wav.data = payload;
                wav.dataSize = chunkSize;
                hasData = true;
            }

            offset = payloadOffset + chunkSize;
            if ((chunkSize & 1uz) != 0 && offset < dataSize)
                offset++;
        }

        if (!hasFormat && !hasData)
            return std::unexpected<std::string>("WAV is missing fmt and data chunks");
        if (!hasFormat)
            return std::unexpected<std::string>("WAV is missing fmt chunk");
        if (!hasData)
            return std::unexpected<std::string>("WAV is missing data chunk");

        return wav;
    }

    [[nodiscard]] int16_t DecodeMsAdpcmNibble(MsAdpcmChannelState& state, const uint8_t nibble)
    {
        constexpr auto minSample = static_cast<int>(std::numeric_limits<int16_t>::min());
        constexpr auto maxSample = static_cast<int>(std::numeric_limits<int16_t>::max());

        const auto signedNibble = nibble >= 8 ? static_cast<int>(nibble) - 16 : static_cast<int>(nibble);
        const auto prediction = ((state.sample1 * state.coeff1) + (state.sample2 * state.coeff2)) / 256 + signedNibble * state.delta;
        const auto sample = std::clamp(prediction, minSample, maxSample);

        state.sample2 = state.sample1;
        state.sample1 = sample;
        state.delta = std::max(16, (MS_ADPCM_ADAPTATION_TABLE[nibble] * state.delta) / 256);

        return static_cast<int16_t>(sample);
    }

    [[nodiscard]] std::expected<DecodedPcm, std::string> DecodeMsAdpcm(const WavInfo& wav)
    {
        if (wav.format != WavFormat::MsAdpcm)
            return std::unexpected<std::string>("WAV format is not MS ADPCM");
        if (wav.channels == 0 || wav.channels > 2)
            return std::unexpected(std::format("MS ADPCM channel count {} is unsupported", wav.channels));
        if (wav.blockAlign == 0)
            return std::unexpected<std::string>("MS ADPCM blockAlign is zero");
        if (wav.bitsPerSample != 4)
            return std::unexpected(std::format("MS ADPCM bits per sample {} is unsupported", wav.bitsPerSample));
        if (wav.samplesPerBlock < 2)
            return std::unexpected(std::format("MS ADPCM samplesPerBlock {} is invalid", wav.samplesPerBlock));
        if (wav.coefficients.empty())
            return std::unexpected<std::string>("MS ADPCM coefficient table is empty");

        DecodedPcm result{.channels = wav.channels, .sampleRate = wav.sampleRate};
        const auto headerSize = static_cast<size_t>(7 * wav.channels);
        if (wav.blockAlign < headerSize)
            return std::unexpected(std::format("MS ADPCM blockAlign {} is smaller than the {} byte block header", wav.blockAlign, headerSize));

        for (size_t blockOffset = 0; blockOffset + headerSize <= wav.dataSize; blockOffset += wav.blockAlign)
        {
            const auto remainingSize = wav.dataSize - blockOffset;
            const auto blockSize = std::min(static_cast<size_t>(wav.blockAlign), remainingSize);
            if (blockSize < headerSize)
                break;

            const auto* block = &wav.data[blockOffset];
            std::array<MsAdpcmChannelState, 2> states{};
            auto cursor = 0uz;

            std::array<uint8_t, 2> predictor{};
            for (auto channel = 0u; channel < wav.channels; channel++)
                predictor[channel] = static_cast<uint8_t>(block[cursor++]);

            for (auto channel = 0u; channel < wav.channels; channel++)
                states[channel].delta = ReadInt16LE(&block[cursor + channel * 2]);
            cursor += 2uz * wav.channels;

            for (auto channel = 0u; channel < wav.channels; channel++)
                states[channel].sample1 = ReadInt16LE(&block[cursor + channel * 2]);
            cursor += 2uz * wav.channels;

            for (auto channel = 0u; channel < wav.channels; channel++)
                states[channel].sample2 = ReadInt16LE(&block[cursor + channel * 2]);
            cursor += 2uz * wav.channels;

            for (auto channel = 0u; channel < wav.channels; channel++)
            {
                if (predictor[channel] >= wav.coefficients.size())
                    return std::unexpected(std::format("MS ADPCM predictor index {} is out of range", predictor[channel]));

                states[channel].coeff1 = wav.coefficients[predictor[channel]].coeff1;
                states[channel].coeff2 = wav.coefficients[predictor[channel]].coeff2;
                states[channel].delta = std::max(16, states[channel].delta);
            }

            for (auto channel = 0u; channel < wav.channels; channel++)
                result.samples.push_back(static_cast<int16_t>(states[channel].sample2));
            for (auto channel = 0u; channel < wav.channels; channel++)
                result.samples.push_back(static_cast<int16_t>(states[channel].sample1));

            auto decodedFrames = 2u;
            const auto* encoded = &block[cursor];
            const auto encodedSize = blockSize - cursor;

            if (wav.channels == 1)
            {
                for (auto i = 0uz; i < encodedSize && decodedFrames < wav.samplesPerBlock; i++)
                {
                    const auto value = static_cast<uint8_t>(encoded[i]);
                    result.samples.push_back(DecodeMsAdpcmNibble(states[0], value >> 4));
                    decodedFrames++;

                    if (decodedFrames >= wav.samplesPerBlock)
                        break;

                    result.samples.push_back(DecodeMsAdpcmNibble(states[0], value & 0xF));
                    decodedFrames++;
                }
            }
            else
            {
                for (auto i = 0uz; i < encodedSize && decodedFrames < wav.samplesPerBlock; i++)
                {
                    const auto value = static_cast<uint8_t>(encoded[i]);
                    result.samples.push_back(DecodeMsAdpcmNibble(states[0], value >> 4));
                    result.samples.push_back(DecodeMsAdpcmNibble(states[1], value & 0xF));
                    decodedFrames++;
                }
            }
        }

        if (result.samples.empty())
            return std::unexpected<std::string>("MS ADPCM data contains no complete blocks");

        return result;
    }

    void WritePcmWav(std::ostream& stream, const DecodedPcm& pcm)
    {
        constexpr auto bytesPerSample = 2u;
        const auto dataSize = static_cast<uint32_t>(pcm.samples.size() * bytesPerSample);
        const auto byteRate = pcm.sampleRate * pcm.channels * bytesPerSample;
        const auto blockAlign = static_cast<uint16_t>(pcm.channels * bytesPerSample);

        stream::Write(stream, RIFF_ID.data(), FOURCC_SIZE);
        WriteLittleEndian(stream, 36u + dataSize);
        stream::Write(stream, WAVE_FORM_TYPE.data(), FOURCC_SIZE);
        stream::Write(stream, WAVE_FMT_CHUNK_ID.data(), FOURCC_SIZE);
        WriteLittleEndian(stream, 16u);
        WriteLittleEndian(stream, static_cast<uint16_t>(WavFormat::Pcm));
        WriteLittleEndian(stream, pcm.channels);
        WriteLittleEndian(stream, pcm.sampleRate);
        WriteLittleEndian(stream, byteRate);
        WriteLittleEndian(stream, blockAlign);
        WriteLittleEndian(stream, static_cast<uint16_t>(16));
        stream::Write(stream, WAVE_DATA_CHUNK_ID.data(), FOURCC_SIZE);
        WriteLittleEndian(stream, dataSize);
        stream::Write(stream, pcm.samples.data(), dataSize);
    }

    void DumpPcmWav(AssetDumpingContext& context, const XAssetInfo<LoadedSound>& asset, const char* soundData, const size_t soundDataSize)
    {
        const auto assetFile = context.OpenAssetFile(std::format("sound/{}", asset.m_name));
        if (!assetFile)
            return;

        assetFile->write(soundData, static_cast<std::streamsize>(soundDataSize));
    }

    void DumpDecodedMsAdpcm(AssetDumpingContext& context, const XAssetInfo<LoadedSound>& asset, const DecodedPcm& pcm)
    {
        const auto assetFile = context.OpenAssetFile(std::format("sound/{}", asset.m_name));
        if (!assetFile)
            return;

        WritePcmWav(*assetFile, pcm);
    }
} // namespace

namespace sound
{
    void LoadedSoundDumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<LoadedSound>& asset)
    {
        const auto* loadedSound = asset.Asset();
        if (!loadedSound->sound.data || loadedSound->sound.data_size <= 0)
            return;

        const auto* soundData = loadedSound->sound.data;
        const auto soundDataSize = static_cast<size_t>(loadedSound->sound.data_size);
        const auto wav = ParseWav(soundData, soundDataSize);
        if (!wav.has_value())
        {
            con::warn("Skipping unsupported loaded sound '{}': {}", loadedSound->name, wav.error());
            return;
        }

        const auto& wavInfo = wav.value();
        if (wavInfo.format == WavFormat::Pcm)
        {
            DumpPcmWav(context, asset, soundData, soundDataSize);
            return;
        }

        if (wavInfo.format == WavFormat::MsAdpcm)
        {
            const auto decoded = DecodeMsAdpcm(wavInfo);
            if (!decoded.has_value())
            {
                con::warn("Could not decode MS ADPCM loaded sound '{}': {}", loadedSound->name, decoded.error());
                return;
            }

            DumpDecodedMsAdpcm(context, asset, decoded.value());
            return;
        }

        con::warn("Skipping unsupported loaded sound '{}' with WAV format {}", loadedSound->name, static_cast<uint16_t>(wavInfo.format));
    }
} // namespace sound
