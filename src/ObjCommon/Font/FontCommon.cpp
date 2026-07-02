#include "FontCommon.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace font
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        fs::path path(assetName);
        path.replace_extension(".json");
        return path.string();
    }

    bool IsPrintableLetter(const unsigned letter)
    {
        // Control characters
        if (letter < 0x20)
            return false;

        // Printable ascii characters
        if (letter < 0x7F)
            return true;

        // There are characters after this point that are printable as well
        // But they don't seem to play a role in cod fonts
        if (letter > 0xFF)
            return false;

        switch (letter)
        {
        case 0x7F:
        case 0x81:
        case 0x8D:
        case 0x8F:
        case 0x90:
        case 0x9D:
        case 0xA0:
        case 0xAD:
            return false;
        default:
            return true;
        }
    }

    std::string LetterToString(const unsigned letter)
    {
        // UTF-8 => 1 byte
        if (letter < 0x80)
            return std::string(1, static_cast<char>(letter));

        if (letter < 0x800)
        {
            // UTF-8 => 2 bytes
            std::string s(2, '\0');
            s[0] = static_cast<char>(0xC0 | (letter >> 6u));
            s[1] = static_cast<char>(0x80 | (letter & 0x3F));

            return s;
        }

        // UTF-8 => 3 bytes
        std::string s(3, '\0');
        s[0] = static_cast<char>(0xE0 | (letter >> 12u));
        s[1] = static_cast<char>(0x80 | ((letter >> 6u) & 0x3F));
        s[2] = static_cast<char>(0x80 | (letter & 0x3F));

        return s;
    }

    unsigned StringToLetter(const std::string& str)
    {
        const auto strSize = str.size();
        if (strSize < 1)
            return 0;

        const auto firstByte = static_cast<unsigned char>(str[0]);

        // UTF-8 => 1 byte
        if ((firstByte & 0x80) == 0)
            return firstByte;

        if (strSize < 2)
            return 0;
        const auto secondByte = static_cast<unsigned char>(str[1]);

        if ((firstByte & 0xE0) == 0xC0 && (secondByte & 0xC0) == 0x80)
        {
            // UTF-8 => 2 bytes
            return ((firstByte & 0x1F) << 6u) | (secondByte & 0x3F);
        }

        if (strSize < 3)
            return 0;
        const auto thirdByte = static_cast<unsigned char>(str[2]);

        if ((firstByte & 0xF0) == 0xE0 && (secondByte & 0xC0) == 0x80 && (thirdByte & 0xC0) == 0x80)
        {
            // UTF-8 => 3 bytes
            return ((firstByte & 0x0F) << 12u) | ((secondByte & 0x3F) << 6u) | (thirdByte & 0x3F);
        }

        // Unsupported
        return 0;
    }
} // namespace font
