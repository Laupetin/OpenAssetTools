#include "IwiWaveletDecoder.h"

#include "Image/ImageFormat.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <iterator>
#include <limits>
#include <vector>

namespace
{
    constexpr auto HUFFMAN_LOOKUP_BITS = 12u;
    constexpr auto HUFFMAN_LOOKUP_SIZE = 1u << HUFFMAN_LOOKUP_BITS;
    constexpr auto ESCAPE_VALUE = std::numeric_limits<std::int16_t>::min();

    struct HuffmanCodeword
    {
        std::uint16_t code;
        std::uint8_t bit_count;
        std::int16_t value;
    };

    struct HuffmanDecode
    {
        std::int16_t value;
        std::uint8_t bit_count;
    };

    // These are the unique, LSB-first codewords from the IW wavelet decoder's
    // three 4096-entry lookup tables. The full lookup tables are generated at
    // compile time below.
    // clang-format off
    constexpr auto BLUE_CODEWORDS = std::to_array<HuffmanCodeword>({
         {0x001, 3, 0},    {0x004, 5, 4},    {0x005, 5, 2},    {0x007, 5, 1},    {0x00A, 5, 3},    {0x014, 5, -4},
         {0x015, 5, -2},   {0x017, 5, -1},   {0x01A, 5, -3},   {0x000, 6, 12},   {0x002, 6, 10},   {0x003, 6, 7},
         {0x006, 6, 9},    {0x00B, 6, 6},    {0x018, 6, 11},   {0x01E, 6, 8},    {0x01F, 6, 5},    {0x020, 6, -12},
         {0x022, 6, -10},  {0x023, 6, -7},   {0x026, 6, -9},   {0x02B, 6, -6},   {0x038, 6, -11},  {0x03C, 6, ESCAPE_VALUE},
         {0x03E, 6, -8},   {0x03F, 6, -5},   {0x00F, 7, 13},   {0x012, 7, 19},   {0x016, 7, 18},   {0x01B, 7, 14},
         {0x028, 7, 21},   {0x02C, 7, 20},   {0x02D, 7, 16},   {0x02E, 7, 17},   {0x030, 7, 22},   {0x03D, 7, 15},
         {0x04F, 7, -13},  {0x052, 7, -19},  {0x056, 7, -18},  {0x05B, 7, -14},  {0x068, 7, -21},  {0x06C, 7, -20},
         {0x06D, 7, -16},  {0x06E, 7, -17},  {0x070, 7, -22},  {0x07D, 7, -15},  {0x008, 8, 34},   {0x00D, 8, 28},
         {0x00E, 8, 29},   {0x013, 8, 26},   {0x01D, 8, 27},   {0x02F, 8, 23},   {0x033, 8, 25},   {0x03B, 8, 24},
         {0x048, 8, 33},   {0x04C, 8, 32},   {0x05C, 8, 31},   {0x072, 8, 30},   {0x088, 8, -34},  {0x08D, 8, -28},
         {0x08E, 8, -29},  {0x093, 8, -26},  {0x09D, 8, -27},  {0x0AF, 8, -23},  {0x0B3, 8, -25},  {0x0BB, 8, -24},
         {0x0C8, 8, -33},  {0x0CC, 8, -32},  {0x0DC, 8, -31},  {0x0F2, 8, -30},  {0x00C, 9, 47},   {0x01C, 9, 46},
         {0x032, 9, 45},   {0x036, 9, 44},   {0x050, 9, 48},   {0x076, 9, 43},   {0x07B, 9, 37},   {0x090, 9, 49},
         {0x0CD, 9, 40},   {0x0CE, 9, 41},   {0x0D3, 9, 38},   {0x0DD, 9, 39},   {0x0EF, 9, 35},   {0x0F6, 9, 42},
         {0x0FB, 9, 36},   {0x10C, 9, -47},  {0x11C, 9, -46},  {0x132, 9, -45},  {0x136, 9, -44},  {0x150, 9, -48},
         {0x176, 9, -43},  {0x17B, 9, -37},  {0x190, 9, -49},  {0x1CD, 9, -40},  {0x1CE, 9, -41},  {0x1D3, 9, -38},
         {0x1DD, 9, -39},  {0x1EF, 9, -35},  {0x1F6, 9, -42},  {0x1FB, 9, -36},  {0x010, 10, 65},  {0x04D, 10, 56},
         {0x04E, 10, 57},  {0x05D, 10, 55},  {0x08C, 10, 62},  {0x09C, 10, 61},  {0x110, 10, 64},  {0x153, 10, 53},
         {0x15D, 10, 54},  {0x16F, 10, 50},  {0x173, 10, 52},  {0x19C, 10, 60},  {0x1B2, 10, 59},  {0x1B6, 10, 58},
         {0x1D0, 10, 63},  {0x1F3, 10, 51},  {0x210, 10, -65}, {0x24D, 10, -56}, {0x24E, 10, -57}, {0x25D, 10, -55},
         {0x28C, 10, -62}, {0x29C, 10, -61}, {0x310, 10, -64}, {0x353, 10, -53}, {0x35D, 10, -54}, {0x36F, 10, -50},
         {0x373, 10, -52}, {0x39C, 10, -60}, {0x3B2, 10, -59}, {0x3B6, 10, -58}, {0x3D0, 10, -63}, {0x3F3, 10, -51},
         {0x053, 11, 70},  {0x06F, 11, 66},  {0x073, 11, 69},  {0x0B2, 11, 77},  {0x0B6, 11, 75},  {0x0D0, 11, 81},
         {0x14E, 11, 73},  {0x18C, 11, 79},  {0x273, 11, 68},  {0x2B2, 11, 76},  {0x2B6, 11, 74},  {0x2D0, 11, 80},
         {0x2F3, 11, 67},  {0x34D, 11, 71},  {0x34E, 11, 72},  {0x38C, 11, 78},  {0x453, 11, -70}, {0x46F, 11, -66},
         {0x473, 11, -69}, {0x4B2, 11, -77}, {0x4B6, 11, -75}, {0x4D0, 11, -81}, {0x54E, 11, -73}, {0x58C, 11, -79},
         {0x673, 11, -68}, {0x6B2, 11, -76}, {0x6B6, 11, -74}, {0x6D0, 11, -80}, {0x6F3, 11, -67}, {0x74D, 11, -71},
         {0x74E, 11, -72}, {0x78C, 11, -78}, {0x0F3, 12, 85},  {0x14D, 12, 89},  {0x253, 12, 87},  {0x26F, 12, 83},
         {0x4F3, 12, 84},  {0x54D, 12, 88},  {0x653, 12, 86},  {0x66F, 12, 82},  {0x8F3, 12, -85}, {0x94D, 12, -89},
         {0xA53, 12, -87}, {0xA6F, 12, -83}, {0xCF3, 12, -84}, {0xD4D, 12, -88}, {0xE53, 12, -86}, {0xE6F, 12, -82},
    });
    static_assert(BLUE_CODEWORDS.size() == 180);

    constexpr auto RED_GREEN_CODEWORDS = std::to_array<HuffmanCodeword>({
         {0x003, 2, 0},    {0x002, 3, 1},    {0x006, 3, -1},   {0x001, 4, 2},    {0x009, 4, -2},   {0x004, 5, 4},
         {0x00D, 5, 3},    {0x014, 5, -4},   {0x01D, 5, -3},   {0x00C, 6, 6},    {0x010, 6, 7},    {0x015, 6, 5},
         {0x02C, 6, -6},   {0x030, 6, -7},   {0x035, 6, -5},   {0x018, 7, 10},   {0x01C, 7, 9},    {0x020, 7, 11},
         {0x025, 7, 8},    {0x058, 7, -10},  {0x05C, 7, -9},   {0x060, 7, -11},  {0x065, 7, -8},   {0x068, 7, ESCAPE_VALUE},
         {0x038, 8, 14},   {0x040, 8, 16},   {0x045, 8, 12},   {0x048, 8, 15},   {0x07C, 8, 13},   {0x0B8, 8, -14},
         {0x0C0, 8, -16},  {0x0C5, 8, -12},  {0x0C8, 8, -15},  {0x0FC, 8, -13},  {0x080, 9, 22},   {0x085, 9, 17},
         {0x088, 9, 21},   {0x0A8, 9, 20},   {0x0BC, 9, 18},   {0x0F8, 9, 19},   {0x180, 9, -22},  {0x185, 9, -17},
         {0x188, 9, -21},  {0x1A8, 9, -20},  {0x1BC, 9, -18},  {0x1F8, 9, -19},  {0x000, 10, 30},  {0x03C, 10, 25},
         {0x078, 10, 26},  {0x100, 10, 29},  {0x105, 10, 23},  {0x108, 10, 28},  {0x128, 10, 27},  {0x13C, 10, 24},
         {0x200, 10, -30}, {0x23C, 10, -25}, {0x278, 10, -26}, {0x300, 10, -29}, {0x305, 10, -23}, {0x308, 10, -28},
         {0x328, 10, -27}, {0x33C, 10, -24}, {0x005, 11, 31},  {0x008, 11, 37},  {0x028, 11, 35},  {0x178, 11, 33},
         {0x208, 11, 36},  {0x228, 11, 34},  {0x378, 11, 32},  {0x405, 11, -31}, {0x408, 11, -37}, {0x428, 11, -35},
         {0x578, 11, -33}, {0x608, 11, -36}, {0x628, 11, -34}, {0x778, 11, -32}, {0x205, 12, 39},  {0x605, 12, 38},
         {0xA05, 12, -39}, {0xE05, 12, -38},
    });
    static_assert(RED_GREEN_CODEWORDS.size() == 80);

    constexpr auto ALPHA_CODEWORDS = std::to_array<HuffmanCodeword>({
         {0x001, 1, 0},     {0x000, 4, ESCAPE_VALUE}, {0x002, 4, 1},     {0x00A, 4, -1},   {0x00C, 5, 2},    {0x01C, 5, -2},   {0x016, 6, 3},
         {0x018, 6, 4},     {0x036, 6, -3},           {0x038, 6, -4},    {0x004, 7, 7},    {0x02E, 7, 5},    {0x034, 7, 6},    {0x044, 7, -7},
         {0x06E, 7, -5},    {0x074, 7, -6},           {0x006, 8, 11},    {0x008, 8, 14},   {0x014, 8, 12},   {0x01E, 8, 9},    {0x048, 8, 15},
         {0x066, 8, 10},    {0x068, 8, 13},           {0x07E, 8, 8},     {0x086, 8, -11},  {0x088, 8, -14},  {0x094, 8, -12},  {0x09E, 8, -9},
         {0x0C8, 8, -15},   {0x0E6, 8, -10},          {0x0E8, 8, -13},   {0x0FE, 8, -8},   {0x028, 9, 23},   {0x046, 9, 19},   {0x054, 9, 20},
         {0x08E, 9, 17},    {0x0A4, 9, 22},           {0x0A8, 9, 24},    {0x0C6, 9, 18},   {0x0DE, 9, 16},   {0x0E4, 9, 21},   {0x128, 9, -23},
         {0x146, 9, -19},   {0x154, 9, -20},          {0x18E, 9, -17},   {0x1A4, 9, -22},  {0x1A8, 9, -24},  {0x1C6, 9, -18},  {0x1DE, 9, -16},
         {0x1E4, 9, -21},   {0x00E, 10, 29},          {0x024, 10, 37},   {0x026, 10, 31},  {0x04E, 10, 28},  {0x064, 10, 35},  {0x0BE, 10, 32},
         {0x0D4, 10, 33},   {0x124, 10, 36},          {0x126, 10, 30},   {0x13E, 10, 25},  {0x15E, 10, 26},  {0x164, 10, 34},  {0x1A6, 10, 127},
         {0x1CE, 10, 27},   {0x1D4, 10, 128},         {0x20E, 10, -29},  {0x224, 10, -37}, {0x226, 10, -31}, {0x24E, 10, -28}, {0x264, 10, -35},
         {0x2BE, 10, -32},  {0x2D4, 10, -33},         {0x324, 10, -36},  {0x326, 10, -30}, {0x33E, 10, -25}, {0x35E, 10, -26}, {0x364, 10, -34},
         {0x3A6, 10, -127}, {0x3CE, 10, -27},         {0x3D4, 10, -128}, {0x03E, 11, 41},  {0x05E, 11, 43},  {0x0A6, 11, 50},  {0x0CE, 11, 48},
         {0x10E, 11, 49},   {0x14E, 11, 64},          {0x1BE, 11, 39},   {0x23E, 11, 40},  {0x25E, 11, 42},  {0x2A6, 11, 47},  {0x2CE, 11, 44},
         {0x30E, 11, 46},   {0x34E, 11, 45},          {0x3BE, 11, 38},   {0x43E, 11, -41}, {0x45E, 11, -43}, {0x4A6, 11, -50}, {0x4CE, 11, -48},
         {0x50E, 11, -49},  {0x54E, 11, -64},         {0x5BE, 11, -39},  {0x63E, 11, -40}, {0x65E, 11, -42}, {0x6A6, 11, -47}, {0x6CE, 11, -44},
         {0x70E, 11, -46},  {0x74E, 11, -45},         {0x7BE, 11, -38},
    });
    static_assert(ALPHA_CODEWORDS.size() == 108);
    // clang-format on

    template<std::size_t CodewordCount>
    consteval std::array<HuffmanDecode, HUFFMAN_LOOKUP_SIZE> CreateHuffmanLookup(const std::array<HuffmanCodeword, CodewordCount>& codewords)
    {
        std::array<HuffmanDecode, HUFFMAN_LOOKUP_SIZE> result{};

        for (const auto codeword : codewords)
        {
            if (codeword.bit_count == 0 || codeword.bit_count > HUFFMAN_LOOKUP_BITS)
                throw "Invalid Huffman codeword bit count";

            const auto step = 1u << codeword.bit_count;
            if (codeword.code >= step)
                throw "Huffman codeword does not fit its bit count";

            for (auto index = static_cast<unsigned>(codeword.code); index < result.size(); index += step)
            {
                if (result[index].bit_count != 0)
                    throw "Overlapping Huffman codewords";

                result[index] = HuffmanDecode{codeword.value, codeword.bit_count};
            }
        }

        for (const auto entry : result)
        {
            if (entry.bit_count == 0)
                throw "Incomplete Huffman lookup";
        }

        return result;
    }

    constexpr auto BLUE_LOOKUP = CreateHuffmanLookup(BLUE_CODEWORDS);
    constexpr auto RED_GREEN_LOOKUP = CreateHuffmanLookup(RED_GREEN_CODEWORDS);
    constexpr auto ALPHA_LOOKUP = CreateHuffmanLookup(ALPHA_CODEWORDS);

    class WaveletBitReader
    {
    public:
        explicit WaveletBitReader(std::vector<std::uint8_t> data)
            : m_data(std::move(data))
        {
        }

        bool ReadRawByte(std::uint8_t& value)
        {
            if (m_bits_started || m_byte_offset >= m_data.size())
                return false;

            value = m_data[m_byte_offset++];
            return true;
        }

        bool ReadBits(const unsigned bitCount, unsigned& value)
        {
            StartBits();

            if (bitCount > 32 || m_bit_offset + bitCount > m_data.size() * 8u)
                return false;

            value = PeekBits(bitCount);
            m_bit_offset += bitCount;
            return true;
        }

        unsigned PeekBits(const unsigned bitCount)
        {
            StartBits();

            unsigned result = 0;
            const auto availableBitCount = m_data.size() * 8u;
            for (auto bitIndex = 0u; bitIndex < bitCount && m_bit_offset + bitIndex < availableBitCount; bitIndex++)
            {
                const auto sourceBit = m_bit_offset + bitIndex;
                result |= ((m_data[sourceBit / 8u] >> (sourceBit % 8u)) & 1u) << bitIndex;
            }

            return result;
        }

    private:
        void StartBits()
        {
            if (!m_bits_started)
            {
                m_bit_offset = m_byte_offset * 8u;
                m_bits_started = true;
            }
        }

        std::vector<std::uint8_t> m_data;
        std::size_t m_byte_offset = 0;
        std::size_t m_bit_offset = 0;
        bool m_bits_started = false;
    };

    struct WaveletLayout
    {
        const image::ImageFormat* output_format;
        unsigned channel_count;
        unsigned bytes_per_pixel;
    };

    WaveletLayout GetLayout(const image::IwiWaveletFormat format)
    {
        switch (format)
        {
        case image::IwiWaveletFormat::RGBA:
            return {&image::format::B8_G8_R8_A8, 4, 4};
        case image::IwiWaveletFormat::RGB:
            return {&image::format::B8_G8_R8_X8, 3, 4};
        case image::IwiWaveletFormat::LUMINANCE_ALPHA:
            return {&image::format::R8_A8, 2, 2};
        case image::IwiWaveletFormat::LUMINANCE:
            return {&image::format::R8, 1, 1};
        case image::IwiWaveletFormat::ALPHA:
            return {&image::format::A8, 1, 1};
        }

        return {nullptr, 0, 0};
    }

    bool DecodeValue(
        const std::array<HuffmanDecode, HUFFMAN_LOOKUP_SIZE>& lookup, const unsigned escapeBitCount, const int escapeBias, WaveletBitReader& reader, int& value)
    {
        const auto entry = lookup[reader.PeekBits(HUFFMAN_LOOKUP_BITS)];
        unsigned ignored;
        if (!entry.bit_count || !reader.ReadBits(entry.bit_count, ignored))
            return false;

        value = entry.value;
        if (entry.value == ESCAPE_VALUE)
        {
            unsigned escapedValue;
            if (!reader.ReadBits(escapeBitCount, escapedValue))
                return false;

            value = static_cast<int>(escapedValue) - escapeBias;
        }

        return true;
    }

    bool DecodeCoefficients(const std::array<HuffmanDecode, HUFFMAN_LOOKUP_SIZE>& lookup,
                            const unsigned escapeBitCount,
                            const int escapeBias,
                            WaveletBitReader& reader,
                            std::array<int, 3>& coefficients)
    {
        for (auto& coefficient : coefficients)
        {
            if (!DecodeValue(lookup, escapeBitCount, escapeBias, reader, coefficient))
                return false;
        }

        return true;
    }

    constexpr int ShiftRightOne(const int value)
    {
        return value >= 0 ? value / 2 : -((-value + 1) / 2);
    }

    void ReconstructChannel(const std::uint8_t* source,
                            std::uint8_t* topLeft,
                            const unsigned bytesPerPixel,
                            const unsigned stride,
                            const unsigned channel,
                            const unsigned parity,
                            const std::array<int, 3>& coefficients)
    {
        const auto base = 2 * source[channel];
        const auto horizontal = coefficients[0];
        const auto vertical = coefficients[1];
        const auto diagonal = coefficients[2];

        topLeft[channel] = static_cast<std::uint8_t>(parity + ShiftRightOne(diagonal + vertical + horizontal + base));
        topLeft[bytesPerPixel + channel] = static_cast<std::uint8_t>(ShiftRightOne(horizontal + base - diagonal - vertical));
        topLeft[stride + channel] = static_cast<std::uint8_t>(ShiftRightOne(vertical - diagonal + base - horizontal));
        topLeft[stride + bytesPerPixel + channel] = static_cast<std::uint8_t>(ShiftRightOne(base - horizontal - vertical + diagonal));
    }

    bool AddDeltaToMipmap(std::uint8_t* pixels, const unsigned pixelCount, const unsigned channelCount, const unsigned bytesPerPixel, WaveletBitReader& reader)
    {
        for (auto pixelIndex = 0u; pixelIndex < pixelCount; pixelIndex++)
        {
            for (auto channel = 0u; channel < channelCount; channel++)
            {
                int delta;
                if (!DecodeValue(ALPHA_LOOKUP, 9, 255, reader, delta))
                    return false;

                pixels[channel] = static_cast<std::uint8_t>(pixels[channel] + delta);
            }

            pixels += bytesPerPixel;
        }

        return true;
    }

    bool DecodeRawLevel(
        std::uint8_t* destination, const unsigned pixelCount, const unsigned channelCount, const unsigned bytesPerPixel, WaveletBitReader& reader)
    {
        for (auto pixelIndex = 0u; pixelIndex < pixelCount; pixelIndex++)
        {
            for (auto channel = 0u; channel < channelCount; channel++)
            {
                if (!reader.ReadRawByte(destination[channel]))
                    return false;
            }

            for (auto channel = channelCount; channel < bytesPerPixel; channel++)
                destination[channel] = 0xFF;

            destination += bytesPerPixel;
        }

        return true;
    }

    bool DecodeWaveletLevel(const std::uint8_t* source,
                            std::uint8_t* destination,
                            const unsigned width,
                            const unsigned height,
                            const unsigned channelCount,
                            const unsigned bytesPerPixel,
                            WaveletBitReader& reader)
    {
        if (width <= 1 || height <= 1)
            return DecodeRawLevel(destination, width * height, channelCount, bytesPerPixel, reader);

        if (!source)
            return false;

        unsigned needsMipDelta;
        if (!reader.ReadBits(1, needsMipDelta))
            return false;

        std::vector<std::uint8_t> adjustedSource;
        if (needsMipDelta)
        {
            const auto sourceSize = width * height / 4u * bytesPerPixel;
            adjustedSource.assign(source, source + sourceSize);
            if (!AddDeltaToMipmap(adjustedSource.data(), width * height / 4u, channelCount, bytesPerPixel, reader))
                return false;

            source = adjustedSource.data();
        }

        const auto stride = width * bytesPerPixel;
        for (auto y = 0u; y < height; y += 2)
        {
            for (auto x = 0u; x < width; x += 2)
            {
                const auto* sourcePixel = source + ((y / 2u) * (width / 2u) + x / 2u) * bytesPerPixel;
                auto* destinationPixel = destination + (y * width + x) * bytesPerPixel;

                std::array<int, 3> blueCoefficients{};
                if (channelCount != 1)
                {
                    unsigned parity;
                    if (!reader.ReadBits(1, parity) || !DecodeCoefficients(BLUE_LOOKUP, 9, 255, reader, blueCoefficients))
                        return false;

                    ReconstructChannel(sourcePixel, destinationPixel, bytesPerPixel, stride, 0, parity, blueCoefficients);

                    if (channelCount >= 3)
                    {
                        for (auto channel = 1u; channel <= 2u; channel++)
                        {
                            std::array<int, 3> coefficients{};
                            if (!reader.ReadBits(1, parity) || !DecodeCoefficients(RED_GREEN_LOOKUP, 10, 510, reader, coefficients))
                                return false;

                            for (auto coefficientIndex = 0u; coefficientIndex < coefficients.size(); coefficientIndex++)
                                coefficients[coefficientIndex] += blueCoefficients[coefficientIndex];

                            ReconstructChannel(sourcePixel, destinationPixel, bytesPerPixel, stride, channel, parity, coefficients);
                        }
                    }
                }

                if (channelCount == 3)
                {
                    destinationPixel[3] = 0xFF;
                    destinationPixel[bytesPerPixel + 3] = 0xFF;
                    destinationPixel[stride + 3] = 0xFF;
                    destinationPixel[stride + bytesPerPixel + 3] = 0xFF;
                }
                else
                {
                    unsigned parity;
                    std::array<int, 3> coefficients{};
                    if (!reader.ReadBits(1, parity) || !DecodeCoefficients(ALPHA_LOOKUP, 9, 255, reader, coefficients))
                        return false;

                    ReconstructChannel(sourcePixel, destinationPixel, bytesPerPixel, stride, channelCount - 1u, parity, coefficients);
                }
            }
        }

        return true;
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecodeIwiWavelet(
        std::istream& stream, const TextureType textureType, const unsigned width, const unsigned height, const bool hasMipMaps, const IwiWaveletFormat format)
    {
        if (textureType == TextureType::T_3D)
        {
            con::error("IWI wavelet volume textures are unsupported");
            return nullptr;
        }

        if (!width || !height || !std::has_single_bit(width) || !std::has_single_bit(height))
        {
            con::error("IWI wavelet dimensions must be powers of two");
            return nullptr;
        }

        if (!hasMipMaps && width > 1 && height > 1)
        {
            con::error("IWI wavelet textures larger than 1x1 require mipmaps");
            return nullptr;
        }

        const auto layout = GetLayout(format);
        if (!layout.output_format)
            return nullptr;

        std::vector<std::uint8_t> compressedData{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
        if (stream.bad())
        {
            con::error("Failed to read IWI wavelet data");
            return nullptr;
        }

        WaveletBitReader reader(std::move(compressedData));
        auto texture = Texture::CreateForType(textureType, layout.output_format, width, height, 1, hasMipMaps);
        texture->Allocate();

        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;
        for (auto mipLevel = mipMapCount - 1; mipLevel >= 0; mipLevel--)
        {
            for (auto face = 0; face < texture->GetFaceCount(); face++)
            {
                const auto* source = mipLevel + 1 < mipMapCount ? texture->GetBufferForMipLevel(mipLevel + 1, face) : nullptr;
                auto* destination = texture->GetBufferForMipLevel(mipLevel, face);
                const auto mipWidth = std::max(1u, width >> mipLevel);
                const auto mipHeight = std::max(1u, height >> mipLevel);

                if (!DecodeWaveletLevel(source, destination, mipWidth, mipHeight, layout.channel_count, layout.bytes_per_pixel, reader))
                {
                    con::error("Unexpected end of IWI wavelet data in mip level {}", mipLevel);
                    return nullptr;
                }
            }
        }

        return texture;
    }
} // namespace image
