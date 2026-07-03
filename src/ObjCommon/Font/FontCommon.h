#pragma once

#include <string>

namespace font
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName);

    bool IsPrintableLetter(unsigned letter);

    std::string LetterToString(unsigned letter);
    unsigned StringToLetter(const std::string& str);
} // namespace font
