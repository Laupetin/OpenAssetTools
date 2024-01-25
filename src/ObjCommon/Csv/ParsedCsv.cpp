#include "Csv/ParsedCsv.h"

ParsedCsvRow::ParsedCsvRow(std::unordered_map<std::string, size_t>& headers, std::vector<std::string>& row)
    : headers(headers),
      values(row)
{
}

const std::string& ParsedCsvRow::GetValue(const std::string& header, bool required) const
{
    if (this->headers.find(header) == this->headers.end())
    {
        if (required)
            std::cerr << "ERROR: Required column \"" << header << "\" was not found";
        else
            std::cerr << "WARNING: Expected column \"" << header << "\" was not found";
        return nullptr;
    }

    auto& value = this->values.at(this->headers[header]);
    if (required && value.empty())
    {
        std::cerr << "ERROR: Required column \"" << header << "\" does not have a value";
        return nullptr;
    }

    return value;
}

ParsedCsv::ParsedCsv(const CsvInputStream& inputStream, bool hasHeaders)
{
    std::vector<std::vector<std::string>> csvLines;
    std::vector<std::string> currentLine;

    while (inputStream.NextRow(currentLine))
    {
        csvLines.emplace_back(std::move(currentLine));
        currentLine = std::vector<std::string>();
    }

    if (hasHeaders)
    {
        auto& headersRow = csvLines[0];
        for (auto i = 0u; i < headersRow.size(); i++)
        {
            this->headers[headersRow[i]] = i;
        }
    }

    for (auto i = hasHeaders ? 1u : 0u; i < csvLines.size(); i++)
    {
        auto& rowValues = csvLines[i];
        this->rows.push_back(ParsedCsvRow(this->headers, rowValues));
    }
}

size_t ParsedCsv::Size() const
{
    return this->rows.size();
}

ParsedCsvRow ParsedCsv::operator[](size_t index) const
{
    return this->rows.at(index);
}
