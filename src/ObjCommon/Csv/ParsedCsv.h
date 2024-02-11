#pragma once

#include "Csv/CsvStream.h"
#include "Utils/ClassUtils.h"

#include <sstream>
#include <unordered_map>

class ParsedCsvRow
{
    std::unordered_map<std::string, size_t>& headers;
    std::vector<std::string> values;

public:
    explicit ParsedCsvRow(std::unordered_map<std::string, size_t>& headers, std::vector<std::string> row);
    _NODISCARD std::string GetValue(const std::string& header, bool required = false) const;
    _NODISCARD float GetValueFloat(const std::string& header, bool required = false) const;

    template<typename T> T GetValueInt(const std::string& header, const bool required = false) const
    {
        const auto& value = this->GetValue(header, required);
        if (!value.empty())
        {
            std::istringstream ss(value);
            long long out;
            ss >> out;
            return static_cast<T>(out);
        }

        return {};
    }
};

class ParsedCsv
{
    std::unordered_map<std::string, size_t> headers;
    std::vector<ParsedCsvRow> rows;

public:
    explicit ParsedCsv(const CsvInputStream& inputStream, bool hasHeaders = true);

    _NODISCARD size_t Size() const;

    ParsedCsvRow operator[](size_t index) const;
};
