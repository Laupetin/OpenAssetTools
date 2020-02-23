#pragma once
#include "Utils/FileAPI.h"

class CsvWriter
{
    static constexpr char SEPARATOR = ',';
    static const std::string LINE_BREAK;

    FileAPI::IFile* m_file;
    unsigned m_column_count;
    unsigned m_current_column;
    bool m_first_row;

public:
    explicit CsvWriter(FileAPI::IFile* file);

    void WriteColumn(const std::string& value);
    void NextRow();
};
