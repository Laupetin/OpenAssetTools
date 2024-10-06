#pragma once
#include <string>
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

    void MakeStringLowerCase(std::string& str);
    void MakeStringUpperCase(std::string& str);

    void StringTrimL(std::string& str);
    void StringTrimR(std::string& str);
    void StringTrim(std::string& str);

    std::vector<std::string> StringSplit(const std::string& str, char delimiter);
} // namespace utils
