#pragma once
#include <Csv/CsvStream.h>
#include <sstream>
#include <unordered_map>

class ParsedCsvRow
{
    std::unordered_map<std::string, size_t>& headers;
    std::vector<std::string> values;

public:
    explicit ParsedCsvRow(std::unordered_map<std::string, size_t>& headers, std::vector<std::string>& row);
    const std::string& GetValue(const std::string& header, bool required = false) const;
    
    template<typename T> T GetValueAs(const std::string& header, bool required = false) const
    {
        const auto& value = this->GetValue(header, required);
        if (!value.empty())
        {
            std::istringstream ss(value);
            T out{};
            ss >> out;
            return out;
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
    
    size_t Size() const;

    ParsedCsvRow operator[](size_t index) const;
};