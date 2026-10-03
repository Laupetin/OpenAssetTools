#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace utils
{
#define M_LETTERS_AL_NUM "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    static constexpr const char* LETTERS_AL_NUM = M_LETTERS_AL_NUM;
    static constexpr const char* LETTERS_AL_NUM_UNDERSCORE = M_LETTERS_AL_NUM "_";

    std::string EscapeStringForQuotationMarks(const std::string_view& str);
    void EscapeStringForQuotationMarks(std::ostream& stream, const std::string_view& str);
    std::string UnescapeStringFromQuotationMarks(const std::string_view& str);
    void UnescapeStringFromQuotationMarks(std::ostream& stream, const std::string_view& str);

    void MakeStringLowerCase(char* str);
    void MakeStringLowerCase(std::string& str);
    void MakeStringUpperCase(char* str);
    void MakeStringUpperCase(std::string& str);

    constexpr char CharToLowerAscii(const char c)
    {
        return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c;
    }

    constexpr bool StringEqualsIgnoreCase(const std::string_view lhs, const std::string_view rhs)
    {
        return std::ranges::equal(lhs,
                                  rhs,
                                  [](const char a, const char b)
                                  {
                                      return CharToLowerAscii(a) == CharToLowerAscii(b);
                                  });
    }

    void StringTrimL(std::string& str);
    void StringTrimR(std::string& str);
    void StringTrim(std::string& str);

    std::vector<std::string> StringSplit(const std::string& str, char delimiter);
} // namespace utils
