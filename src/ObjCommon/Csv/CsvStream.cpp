#include "CsvStream.h"

#include <sstream>

constexpr char CSV_SEPARATOR = ',';

CsvInputStream::CsvInputStream(std::istream& stream)
    : m_stream(stream)
{
}

bool CsvInputStream::NextRow(std::vector<std::string>& out) const
{
    if (!out.empty())
        out.clear();

    return EmitNextRow(
        [&out](std::string value)
        {
            out.emplace_back(std::move(value));
        });
}

bool CsvInputStream::NextRow(std::vector<const char*>& out, MemoryManager& memory) const
{
    if (!out.empty())
        out.clear();

    return EmitNextRow(
        [&out, &memory](const std::string& value)
        {
            out.emplace_back(memory.Dup(value.c_str()));
        });
}

bool CsvInputStream::EmitNextRow(const std::function<void(std::string)>& cb) const
{
    auto c = m_stream.get();
    const auto isEof = c == EOF;
    std::ostringstream col;
    while (c != EOF)
    {
        if (c == CSV_SEPARATOR)
        {
            cb(col.str());
            col.clear();
            col.str(std::string());
        }
        else if (c == '\r')
        {
            c = m_stream.get();
            if (c == '\n')
                break;
            col << '\r';
        }
        else if (c == '\n')
        {
            break;
        }
        else
        {
            col << static_cast<char>(c);
        }

        c = m_stream.get();
    }

    if (!isEof)
    {
        cb(col.str());
    }

    return !isEof;
}

CsvOutputStream::CsvOutputStream(std::ostream& stream)
    : m_stream(stream),
      m_column_count(0),
      m_current_column(0),
      m_first_row(true)
{
}

void CsvOutputStream::WriteColumn(const std::string& value)
{
    if (m_current_column++ > 0)
        m_stream << CSV_SEPARATOR;

    auto containsSeparator = false;
    auto containsQuote = false;
    for (const auto& c : value)
    {
        if (c == '"')
        {
            containsQuote = true;
            break;
        }

        if (c == CSV_SEPARATOR)
            containsSeparator = true;
    }

    if (containsQuote)
    {
        m_stream << "\"";
        for (const auto& c : value)
        {
            if (c == '"')
                m_stream << "\"\"";
            else
                m_stream << c;
        }

        m_stream << "\"";
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

void CsvOutputStream::NextRow()
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
            m_stream << CSV_SEPARATOR;
            m_current_column++;
        }
    }

    m_stream << "\n";
    m_current_column = 0;
}
