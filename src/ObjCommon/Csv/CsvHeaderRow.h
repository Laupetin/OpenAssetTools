#pragma once

#include "Csv/CsvStream.h"

#include <optional>
#include <string>
#include <vector>

class CsvHeaderRow
{
public:
    CsvHeaderRow();

    bool Read(const CsvInputStream& inputStream);

    const std::string& HeaderNameForColumn(unsigned columnIndex) const;
    bool RequireIndexForHeader(const std::string& headerName, unsigned& out) const;
    [[nodiscard]] std::optional<unsigned> GetIndexForHeader(const std::string& headerName) const;

private:
    std::vector<std::string> m_header_row;
};
