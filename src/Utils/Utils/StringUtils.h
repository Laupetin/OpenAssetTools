#pragma once
#include <string>

namespace utils
{
    std::string EscapeStringForQuotationMarks(const std::string_view& str);
    void EscapeStringForQuotationMarks(std::ostream& stream, const std::string_view& str);
    std::string UnescapeStringFromQuotationMarks(const std::string_view& str);
    void UnescapeStringFromQuotationMarks(std::ostream& stream, const std::string_view& str);
}
