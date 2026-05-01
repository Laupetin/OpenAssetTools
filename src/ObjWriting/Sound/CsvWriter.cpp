#include "CsvWriter.h"

CsvWriter::CsvWriter(std::ostream& stream)
    : m_stream(stream)
{
}

void CsvWriter::WriteHeader(const std::vector<std::string>& columns) const
{
    WriteRow(columns);
}

void CsvWriter::WriteRow(const std::vector<std::string>& values) const
{
    for (size_t i = 0; i < values.size(); ++i)
    {
        m_stream << Escape(values[i]);

        if (i < values.size() - 1)
        {
            m_stream << ",";
        }
    }
    m_stream << "\n";
}

template<typename T> void CsvWriter::WriteNumericRow(const std::vector<T>& values) const
{
    for (size_t i = 0; i < values.size(); ++i)
    {
        m_stream << values[i];

        if (i < values.size() - 1)
        {
            m_stream << ",";
        }
    }
    m_stream << "\n";
}

std::string CsvWriter::Escape(const std::string& value)
{
    bool needsQuotes = value.find_first_of(",\"\n") != std::string::npos;

    if (!needsQuotes)
        return value;

    std::string escaped = "\"";
    for (char c : value)
    {
        if (c == '"')
            escaped += "\"\"";
        else
            escaped += c;
    }
    escaped += "\"";

    return escaped;
}
