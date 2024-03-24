#include "Csv/ParsedCsv.h"

ParsedCsvRow::ParsedCsvRow(std::unordered_map<std::string, size_t>& headers, std::vector<std::string> row)
    : headers(headers),
      values(std::move(row))
{
}

std::string ParsedCsvRow::GetValue(const std::string& header, const bool required) const
{
    if (this->headers.find(header) == this->headers.end())
    {
        if (required)
            std::cerr << "ERROR: Required column \"" << header << "\" was not found\n";
        else
            std::cerr << "WARNING: Expected column \"" << header << "\" was not found\n";

        return {};
    }

    auto& value = this->values.at(this->headers[header]);
    if (required && value.empty())
    {
        std::cerr << "ERROR: Required column \"" << header << "\" does not have a value\n";
        return {};
    }

    return value;
}

float ParsedCsvRow::GetValueFloat(const std::string& header, const bool required) const
{
    const auto& value = this->GetValue(header, required);
    if (!value.empty())
    {
        std::istringstream ss(value);
        float out;
        ss >> out;
        return out;
    }

    return {};
}

ParsedCsv::ParsedCsv(const CsvInputStream& inputStream, const bool hasHeaders)
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
        const auto& headersRow = csvLines[0];
        for (auto i = 0u; i < headersRow.size(); i++)
        {
            this->headers[headersRow[i]] = i;
        }
    }

    for (auto i = hasHeaders ? 1u : 0u; i < csvLines.size(); i++)
    {
        auto& rowValues = csvLines[i];
        this->rows.emplace_back(this->headers, std::move(rowValues));
    }
}

size_t ParsedCsv::Size() const
{
    return this->rows.size();
}

ParsedCsvRow ParsedCsv::operator[](const size_t index) const
{
    return this->rows.at(index);
}
