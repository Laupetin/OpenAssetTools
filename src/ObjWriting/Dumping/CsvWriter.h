#pragma once

#include <string>
#include <ostream>

class CsvWriter
{
    static constexpr char SEPARATOR = ',';
    
    std::ostream& m_stream;
    unsigned m_column_count;
    unsigned m_current_column;
    bool m_first_row;

public:
    explicit CsvWriter(std::ostream& stream);

    void WriteColumn(const std::string& value);
    void NextRow();
};
