#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

using namespace image;

namespace
{
    class BitWriter
    {
    public:
        void WriteBits(const unsigned value, const unsigned bitCount)
        {
            for (auto bitIndex = 0u; bitIndex < bitCount; bitIndex++)
            {
                if (m_bit_count % 8u == 0)
                    m_data.emplace_back(0);

                m_data.back() |= ((value >> bitIndex) & 1u) << (m_bit_count % 8u);
                m_bit_count++;
            }
        }

        [[nodiscard]] const std::vector<std::uint8_t>& Data() const
        {
            return m_data;
        }

    private:
        std::vector<std::uint8_t> m_data;
        unsigned m_bit_count = 0;
    };

    void AppendUint16(std::string& data, const std::uint16_t value)
    {
        data.push_back(static_cast<char>(value));
        data.push_back(static_cast<char>(value >> 8u));
    }

    void AppendUint32(std::string& data, const std::uint32_t value)
    {
        data.push_back(static_cast<char>(value));
        data.push_back(static_cast<char>(value >> 8u));
        data.push_back(static_cast<char>(value >> 16u));
        data.push_back(static_cast<char>(value >> 24u));
    }

    std::string MakeIwi6(const iwi6::IwiFormat format, const std::vector<std::uint8_t>& payload)
    {
        std::string data{'I', 'W', 'i', static_cast<char>(IwiVersion::IWI_6)};
        data.push_back(static_cast<char>(format));
        data.push_back(0);
        AppendUint16(data, 2);
        AppendUint16(data, 2);
        AppendUint16(data, 1);

        const auto fileSize = static_cast<std::uint32_t>(sizeof(IwiVersionHeader) + sizeof(iwi6::IwiHeader) + payload.size());
        for (auto picmip = 0u; picmip < 4u; picmip++)
            AppendUint32(data, fileSize);

        data.append(reinterpret_cast<const char*>(payload.data()), payload.size());
        return data;
    }

    std::string MakeIwi8(const iwi8::IwiFormat format, const std::vector<std::uint8_t>& payload)
    {
        std::string data{'I', 'W', 'i', static_cast<char>(IwiVersion::IWI_8)};
        AppendUint32(data, iwi8::IwiFlags::IMG_FLAG_MAPTYPE_2D);
        data.push_back(static_cast<char>(format));
        data.push_back(0);
        AppendUint16(data, 2);
        AppendUint16(data, 2);
        AppendUint16(data, 1);

        const auto fileSize = static_cast<std::uint32_t>(sizeof(IwiVersionHeader) + sizeof(iwi8::IwiHeader) + payload.size());
        for (auto picmip = 0u; picmip < 4u; picmip++)
            AppendUint32(data, fileSize);

        data.append(reinterpret_cast<const char*>(payload.data()), payload.size());
        return data;
    }

    void WriteZeroWaveletLevel(BitWriter& bits, const unsigned channelCount)
    {
        bits.WriteBits(0, 1); // No predictor delta.

        if (channelCount != 1)
        {
            bits.WriteBits(0, 1); // Blue/luminance parity and zero coefficients.
            bits.WriteBits(0x001, 3);
            bits.WriteBits(0x001, 3);
            bits.WriteBits(0x001, 3);

            if (channelCount >= 3)
            {
                for (auto channel = 1u; channel <= 2u; channel++)
                {
                    bits.WriteBits(0, 1); // Red/green parity and zero residual coefficients.
                    bits.WriteBits(0x003, 2);
                    bits.WriteBits(0x003, 2);
                    bits.WriteBits(0x003, 2);
                }
            }
        }

        if (channelCount != 3)
        {
            bits.WriteBits(0, 1); // Independent channel parity and zero coefficients.
            bits.WriteBits(0x001, 1);
            bits.WriteBits(0x001, 1);
            bits.WriteBits(0x001, 1);
        }
    }

    std::optional<IwiLoaderResult> Load(const std::string& data)
    {
        std::istringstream stream(data);
        return LoadIwi(stream);
    }

    TEST_CASE("IwiLoader: Can decode IWI6 wavelet luminance", "[image][iwi][wavelet]")
    {
        constexpr std::uint8_t PIXEL_VALUE = 0x40;

        BitWriter bits;
        bits.WriteBits(0, 1);     // No predictor delta.
        bits.WriteBits(0, 1);     // Even reconstruction parity.
        bits.WriteBits(0x001, 1); // Three zero alpha/luminance coefficients.
        bits.WriteBits(0x001, 1);
        bits.WriteBits(0x001, 1);

        std::vector<std::uint8_t> payload{PIXEL_VALUE};
        payload.insert(payload.end(), bits.Data().begin(), bits.Data().end());

        auto result = Load(MakeIwi6(iwi6::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE, payload));

        REQUIRE(result.has_value());
        REQUIRE(result->m_version == IwiVersion::IWI_6);
        REQUIRE(result->m_texture->GetFormat()->GetId() == ImageFormatId::R8);
        REQUIRE(result->m_texture->GetWidth() == 2);
        REQUIRE(result->m_texture->GetHeight() == 2);
        REQUIRE(result->m_texture->GetMipMapCount() == 2);

        const auto* largestMip = result->m_texture->GetBufferForMipLevel(0);
        for (auto pixel = 0u; pixel < 4u; pixel++)
            REQUIRE(largestMip[pixel] == PIXEL_VALUE);
        REQUIRE(result->m_texture->GetBufferForMipLevel(1)[0] == PIXEL_VALUE);
    }

    TEST_CASE("IwiLoader: Can decode IWI6 wavelet RGBA", "[image][iwi][wavelet]")
    {
        constexpr std::uint8_t BLUE = 0x10;
        constexpr std::uint8_t GREEN = 0x20;
        constexpr std::uint8_t RED = 0x30;
        constexpr std::uint8_t ALPHA = 0x40;

        BitWriter bits;
        bits.WriteBits(0, 1); // No predictor delta.

        bits.WriteBits(0, 1); // Blue parity and zero coefficients.
        bits.WriteBits(0x001, 3);
        bits.WriteBits(0x001, 3);
        bits.WriteBits(0x001, 3);

        for (auto channel = 0u; channel < 2u; channel++)
        {
            bits.WriteBits(0, 1); // Red/green parity and zero residual coefficients.
            bits.WriteBits(0x003, 2);
            bits.WriteBits(0x003, 2);
            bits.WriteBits(0x003, 2);
        }

        bits.WriteBits(0, 1); // Alpha parity and zero coefficients.
        bits.WriteBits(0x001, 1);
        bits.WriteBits(0x001, 1);
        bits.WriteBits(0x001, 1);

        std::vector<std::uint8_t> payload{BLUE, GREEN, RED, ALPHA};
        payload.insert(payload.end(), bits.Data().begin(), bits.Data().end());

        auto result = Load(MakeIwi6(iwi6::IwiFormat::IMG_FORMAT_WAVELET_RGBA, payload));

        REQUIRE(result.has_value());
        REQUIRE(result->m_texture->GetFormat()->GetId() == ImageFormatId::B8_G8_R8_A8);

        const auto* largestMip = result->m_texture->GetBufferForMipLevel(0);
        for (auto pixel = 0u; pixel < 4u; pixel++)
        {
            REQUIRE(largestMip[pixel * 4u] == BLUE);
            REQUIRE(largestMip[pixel * 4u + 1u] == GREEN);
            REQUIRE(largestMip[pixel * 4u + 2u] == RED);
            REQUIRE(largestMip[pixel * 4u + 3u] == ALPHA);
        }
    }

    TEST_CASE("IwiLoader: Can decode IWI6 wavelet RGB", "[image][iwi][wavelet]")
    {
        constexpr std::uint8_t BLUE = 0x10;
        constexpr std::uint8_t GREEN = 0x20;
        constexpr std::uint8_t RED = 0x30;

        BitWriter bits;
        bits.WriteBits(0, 1); // No predictor delta.

        bits.WriteBits(0, 1); // Blue parity and zero coefficients.
        bits.WriteBits(0x001, 3);
        bits.WriteBits(0x001, 3);
        bits.WriteBits(0x001, 3);

        for (auto channel = 0u; channel < 2u; channel++)
        {
            bits.WriteBits(0, 1); // Red/green parity and zero residual coefficients.
            bits.WriteBits(0x003, 2);
            bits.WriteBits(0x003, 2);
            bits.WriteBits(0x003, 2);
        }

        std::vector<std::uint8_t> payload{BLUE, GREEN, RED};
        payload.insert(payload.end(), bits.Data().begin(), bits.Data().end());

        auto result = Load(MakeIwi6(iwi6::IwiFormat::IMG_FORMAT_WAVELET_RGB, payload));

        REQUIRE(result.has_value());
        REQUIRE(result->m_texture->GetFormat()->GetId() == ImageFormatId::B8_G8_R8_X8);

        const auto* largestMip = result->m_texture->GetBufferForMipLevel(0);
        for (auto pixel = 0u; pixel < 4u; pixel++)
        {
            REQUIRE(largestMip[pixel * 4u] == BLUE);
            REQUIRE(largestMip[pixel * 4u + 1u] == GREEN);
            REQUIRE(largestMip[pixel * 4u + 2u] == RED);
            REQUIRE(largestMip[pixel * 4u + 3u] == 0xFF);
        }
    }

    TEST_CASE("IwiLoader: Can decode all IWI8 wavelet formats", "[image][iwi][wavelet]")
    {
        struct TestCase
        {
            iwi8::IwiFormat m_iwi_format;
            ImageFormatId m_image_format;
            std::vector<std::uint8_t> m_source_pixel;
            std::vector<std::uint8_t> m_expected_pixel;
        };

        const std::vector<TestCase> testCases{
            {iwi8::IwiFormat::IMG_FORMAT_WAVELET_RGBA,            ImageFormatId::B8_G8_R8_A8, {0x10, 0x20, 0x30, 0x40}, {0x10, 0x20, 0x30, 0x40}},
            {iwi8::IwiFormat::IMG_FORMAT_WAVELET_RGB,             ImageFormatId::B8_G8_R8_X8, {0x10, 0x20, 0x30},       {0x10, 0x20, 0x30, 0xFF}},
            {iwi8::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA, ImageFormatId::R8_A8,       {0x20, 0x40},             {0x20, 0x40}            },
            {iwi8::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE,       ImageFormatId::R8,          {0x30},                   {0x30}                  },
            {iwi8::IwiFormat::IMG_FORMAT_WAVELET_ALPHA,           ImageFormatId::A8,          {0x40},                   {0x40}                  },
        };

        for (const auto& testCase : testCases)
        {
            BitWriter bits;
            WriteZeroWaveletLevel(bits, static_cast<unsigned>(testCase.m_source_pixel.size()));

            auto payload = testCase.m_source_pixel;
            payload.insert(payload.end(), bits.Data().begin(), bits.Data().end());

            const auto result = Load(MakeIwi8(testCase.m_iwi_format, payload));

            REQUIRE(result.has_value());
            REQUIRE(result->m_version == IwiVersion::IWI_8);
            REQUIRE(result->m_texture->GetFormat()->GetId() == testCase.m_image_format);
            REQUIRE(result->m_texture->GetWidth() == 2);
            REQUIRE(result->m_texture->GetHeight() == 2);
            REQUIRE(result->m_texture->GetMipMapCount() == 2);

            const auto* largestMip = result->m_texture->GetBufferForMipLevel(0);
            for (auto pixel = 0u; pixel < 4u; pixel++)
            {
                for (auto channel = 0u; channel < testCase.m_expected_pixel.size(); channel++)
                    REQUIRE(largestMip[pixel * testCase.m_expected_pixel.size() + channel] == testCase.m_expected_pixel[channel]);
            }

            const auto* smallestMip = result->m_texture->GetBufferForMipLevel(1);
            for (auto channel = 0u; channel < testCase.m_expected_pixel.size(); channel++)
                REQUIRE(smallestMip[channel] == testCase.m_expected_pixel[channel]);
        }
    }

    TEST_CASE("IwiLoader: Rejects truncated IWI8 wavelet data", "[image][iwi][wavelet][malformed]")
    {
        const std::vector<std::uint8_t> payload{0x40};

        REQUIRE_FALSE(Load(MakeIwi8(iwi8::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE, payload)).has_value());
    }
} // namespace
