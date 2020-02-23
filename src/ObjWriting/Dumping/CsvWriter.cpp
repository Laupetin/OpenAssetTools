#include "CsvWriter.h"

#include <sstream>

const std::string CsvWriter::LINE_BREAK = "\n";

CsvWriter::CsvWriter(FileAPI::IFile* file)
{
    m_file = file;
    m_first_row = true;
    m_current_column = 0;
    m_column_count = 0;
}

void CsvWriter::WriteColumn(const std::string& value)
{
    if (m_current_column++ > 0)
        m_file->Printf(",");

    bool containsSeparator = false;
    bool containsQuote = false;
    for (const auto& c : value)
    {
        if (c == '"')
        {
            containsQuote = true;
            break;
        }

        if (c == SEPARATOR)
            containsSeparator = true;
    }

    if (containsQuote)
    {
        std::ostringstream str;

        for(const auto& c : value)
        {
            if (c == '"')
                str << "\"\"";
            else
                str << c;
        }

        m_file->Printf("\"%s\"", str.str().c_str());
    }
    else if (containsSeparator)
    {
        m_file->Printf("\"%s\"", value.c_str());
    }
    else
    {
        m_file->Printf("%s", value.c_str());
    }
}

void CsvWriter::NextRow()
{
    if (m_first_row)
    {
        m_first_row = false;
        m_column_count = m_current_column;
    }
    else
    {
        while(m_current_column < m_column_count)
        {
            m_file->Printf(",");
            m_current_column++;
        }
    }

    m_file->Printf("\n");
    m_current_column = 0;
}
