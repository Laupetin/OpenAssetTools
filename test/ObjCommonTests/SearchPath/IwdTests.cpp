#include "OatTestPaths.h"
#include "SearchPath/IWD.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

namespace
{
    void WriteUint16(std::ostream& stream, const std::uint16_t value)
    {
        stream.put(static_cast<char>(value));
        stream.put(static_cast<char>(value >> 8u));
    }

    void WriteUint32(std::ostream& stream, const std::uint32_t value)
    {
        stream.put(static_cast<char>(value));
        stream.put(static_cast<char>(value >> 8u));
        stream.put(static_cast<char>(value >> 16u));
        stream.put(static_cast<char>(value >> 24u));
    }

    std::uint32_t CalculateCrc32(const std::string_view data)
    {
        auto crc = std::uint32_t{0xFFFFFFFFu};

        for (const auto character : data)
        {
            crc ^= static_cast<std::uint8_t>(character);
            for (auto bitIndex = 0u; bitIndex < 8u; bitIndex++)
                crc = (crc >> 1u) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
        }

        return ~crc;
    }

    void WriteIwd(const fs::path& iwdPath, const std::string_view entryName, const std::string_view contents)
    {
        constexpr auto LOCAL_FILE_HEADER_SIZE = 30u;
        constexpr auto CENTRAL_DIRECTORY_HEADER_SIZE = 46u;
        constexpr auto END_OF_CENTRAL_DIRECTORY_SIZE = 22u;

        const auto entryNameSize = static_cast<std::uint16_t>(entryName.size());
        const auto contentsSize = static_cast<std::uint32_t>(contents.size());
        const auto crc = CalculateCrc32(contents);
        const auto centralDirectoryOffset = LOCAL_FILE_HEADER_SIZE + entryNameSize + contentsSize;
        const auto centralDirectorySize = CENTRAL_DIRECTORY_HEADER_SIZE + entryNameSize;

        std::ofstream stream(iwdPath, std::ios::binary);
        REQUIRE(stream.is_open());

        // Local file header.
        WriteUint32(stream, 0x04034B50u);
        WriteUint16(stream, 20u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint32(stream, crc);
        WriteUint32(stream, contentsSize);
        WriteUint32(stream, contentsSize);
        WriteUint16(stream, entryNameSize);
        WriteUint16(stream, 0u);
        stream.write(entryName.data(), entryName.size());
        stream.write(contents.data(), contents.size());

        // Central directory entry.
        WriteUint32(stream, 0x02014B50u);
        WriteUint16(stream, 20u);
        WriteUint16(stream, 20u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint32(stream, crc);
        WriteUint32(stream, contentsSize);
        WriteUint32(stream, contentsSize);
        WriteUint16(stream, entryNameSize);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint32(stream, 0u);
        WriteUint32(stream, 0u);
        stream.write(entryName.data(), entryName.size());

        // End of central directory.
        WriteUint32(stream, 0x06054B50u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 0u);
        WriteUint16(stream, 1u);
        WriteUint16(stream, 1u);
        WriteUint32(stream, centralDirectorySize);
        WriteUint32(stream, centralDirectoryOffset);
        WriteUint16(stream, 0u);

        REQUIRE(stream.good());
        REQUIRE(stream.tellp() == static_cast<std::streampos>(centralDirectoryOffset + centralDirectorySize + END_OF_CENTRAL_DIRECTORY_SIZE));
    }

    TEST_CASE("Iwd: Reads a peeked entry through end of stream", "[searchpath][iwd]")
    {
        constexpr auto ENTRY_NAME = "test.txt";
        constexpr auto CONTENTS = "IWD stream test";

        const auto tempDirectory = oat::paths::GetTempDirectory("Iwd");
        fs::create_directories(tempDirectory);
        const auto iwdPath = tempDirectory / "test.iwd";
        WriteIwd(iwdPath, ENTRY_NAME, CONTENTS);

        const auto searchPath = iwd::LoadFromFile(iwdPath.string());
        REQUIRE(searchPath);

        const auto file = searchPath->Open(ENTRY_NAME);
        REQUIRE(file.IsOpen());

        auto& stream = *file.m_stream;
        REQUIRE(stream.peek() == CONTENTS[0]);

        std::array<char, std::string_view{CONTENTS}.size()> buffer{};
        stream.read(buffer.data(), buffer.size());
        REQUIRE(stream.gcount() == static_cast<std::streamsize>(buffer.size()));
        REQUIRE(std::string_view{buffer.data(), buffer.size()} == CONTENTS);
        REQUIRE(stream.get() == std::char_traits<char>::eof());
    }
} // namespace
