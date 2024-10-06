#include "Csv/CsvHeaderRow.h"

CsvHeaderRow::CsvHeaderRow() = default;

bool CsvHeaderRow::Read(const CsvInputStream& inputStream)
{
    if (!m_header_row.empty())
        return false;

    return inputStream.NextRow(m_header_row);
}

const std::string& CsvHeaderRow::HeaderNameForColumn(const unsigned columnIndex) const
{
    return m_header_row[columnIndex];
}

bool CsvHeaderRow::RequireIndexForHeader(const std::string& headerName, unsigned& out) const
{
    const auto existingHeader = std::ranges::find(m_header_row, headerName);
    if (existingHeader == m_header_row.end())
        return false;

    out = std::distance(m_header_row.begin(), existingHeader);
    return true;
}

std::optional<unsigned> CsvHeaderRow::GetIndexForHeader(const std::string& headerName) const
{
    unsigned result;
    if (!RequireIndexForHeader(headerName, result))
        return std::nullopt;
    return result;
}
