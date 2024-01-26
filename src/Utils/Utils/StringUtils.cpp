#include "StringUtils.h"

#include <iostream>
#include <sstream>

namespace utils
{
    std::string EscapeStringForQuotationMarks(const std::string_view& str)
    {
        std::ostringstream ss;
        EscapeStringForQuotationMarks(ss, str);
        return ss.str();
    }

    void EscapeStringForQuotationMarks(std::ostream& stream, const std::string_view& str)
    {
        for (const auto& c : str)
        {
            switch (c)
            {
            case '\r':
                stream << "\\r";
                break;
            case '\n':
                stream << "\\n";
                break;
            case '\t':
                stream << "\\t";
                break;
            case '\f':
                stream << "\\f";
                break;
            case '"':
                stream << "\\\"";
                break;
            case '\\':
                stream << "\\\\";
                break;
            default:
                stream << c;
                break;
            }
        }
    }

    std::string UnescapeStringFromQuotationMarks(const std::string_view& str)
    {
        std::ostringstream ss;
        UnescapeStringFromQuotationMarks(ss, str);
        return ss.str();
    }

    void UnescapeStringFromQuotationMarks(std::ostream& stream, const std::string_view& str)
    {
        auto inEscape = false;
        for (const auto& c : str)
        {
            if (inEscape)
            {
                switch (c)
                {
                case 'r':
                    stream << "\r";
                    break;
                case 'n':
                    stream << "\n";
                    break;
                case 't':
                    stream << "\t";
                    break;
                case 'f':
                    stream << "\f";
                    break;
                case '"':
                    stream << "\"";
                    break;
                case '\\':
                    stream << "\\";
                    break;
                default:
                    stream << c;
                    break;
                }
                inEscape = false;
            }
            else if (c != '\\')
                stream << c;
            else
                inEscape = true;
        }
    }

    void MakeStringLowerCase(std::string& str)
    {
        for (auto& c : str)
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }

    void MakeStringUpperCase(std::string& str)
    {
        for (auto& c : str)
            c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }

    std::vector<std::string> StringSplit(const std::string& str, const char delim)
    {
        std::vector<std::string> strings{};
        std::istringstream stream(str);

        std::string s;
        while (std::getline(stream, s, delim))
        {
            strings.push_back(s);
        }

        return strings;
    }
} // namespace utils
