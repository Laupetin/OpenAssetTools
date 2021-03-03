#include "CsvWriter.h"

#include <sstream>

CsvWriter::CsvWriter(std::ostream& stream)
    : m_stream(stream),
      m_column_count(0),
      m_current_column(0),
      m_first_row(true)
{
}

void CsvWriter::WriteColumn(const std::string& value)
{
    if (m_current_column++ > 0)
        m_stream << ",";

    auto containsSeparator = false;
    auto containsQuote = false;
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

        for (const auto& c : value)
        {
            if (c == '"')
                str << "\"\"";
            else
                str << c;
        }

        m_stream << "\"" << str.str() << "\"";
    }
    else if (containsSeparator)
    {
        m_stream << "\"" << value << "\"";
    }
    else
    {
        m_stream << value;
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
        while (m_current_column < m_column_count)
        {
            m_stream << ",";
            m_current_column++;
        }
    }

    m_stream << "\n";
    m_current_column = 0;
}
