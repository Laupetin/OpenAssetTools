#pragma once
#include <iostream>
#include <string>
#include <vector>

class CsvInputStream
{
    std::istream& m_stream;

public:
    explicit CsvInputStream(std::istream& stream);

    bool NextRow(std::vector<std::string>& out) const;
};

class CsvOutputStream
{
    std::ostream& m_stream;
    unsigned m_column_count;
    unsigned m_current_column;
    bool m_first_row;

public:
    explicit CsvOutputStream(std::ostream& stream);

    void WriteColumn(const std::string& value);
    void NextRow();
};