#include "CsvStream.h"

#include "Utils/StringUtils.h"

#include <cstdlib>
#include <sstream>

namespace
{
    constexpr char CSV_SEPARATOR = ',';
}

CsvCell::CsvCell(std::string value)
    : m_value(std::move(value))
{
}

bool CsvCell::AsFloat(float& out) const
{
    const char* startPtr = m_value.c_str();
    char* endPtr;
    out = std::strtof(startPtr, &endPtr);

    if (endPtr == startPtr)
        return false;

    for (const auto* c = endPtr; *c; c++)
    {
        if (!isspace(*c))
            return false;
    }

    return true;
}

bool CsvCell::AsInt32(int32_t& out) const
{
    const char* startPtr = m_value.c_str();
    char* endPtr;
    out = std::strtol(startPtr, &endPtr, 0);

    if (endPtr == startPtr)
        return false;

    for (const auto* c = endPtr; *c; c++)
    {
        if (!isspace(*c))
            return false;
    }

    return true;
}

bool CsvCell::AsUInt32(uint32_t& out) const
{
    const char* startPtr = m_value.c_str();
    char* endPtr;
    out = std::strtoul(startPtr, &endPtr, 0);

    if (endPtr == startPtr)
        return false;

    for (const auto* c = endPtr; *c; c++)
    {
        if (!isspace(*c))
            return false;
    }

    return true;
}

CsvInputStream::CsvInputStream(std::istream& stream)
    : m_stream(stream)
{
}

bool CsvInputStream::NextRow(std::vector<CsvCell>& out) const
{
    if (!out.empty())
        out.clear();

    return EmitNextRow(
        [&out](std::string value)
        {
            out.emplace_back(std::move(value));
        });
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
    const auto startedWithEof = c == EOF;
    auto inQuotes = c == '"';
    std::ostringstream col;
    auto content = false;

    if (inQuotes)
        c = m_stream.get();

    while (c != EOF)
    {
        if (inQuotes)
        {
            if (c == '"')
            {
                c = m_stream.peek();
                if (c == '"')
                {
                    m_stream.get();
                    content = true;
                    col << '"';
                }
                else
                {
                    inQuotes = false;
                }
            }
            else if (isspace(c))
            {
                if (content)
                    col << static_cast<char>(c);
            }
            else
            {
                content = true;
                col << static_cast<char>(c);
            }
        }
        else if (c == CSV_SEPARATOR)
        {
            auto value = col.str();
            utils::StringTrimR(value);
            cb(std::move(value));
            col.clear();
            col.str(std::string());
            content = false;

            c = m_stream.peek();
            if (c == '"')
            {
                m_stream.get();
                inQuotes = true;
            }
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
        else if (isspace(c))
        {
            if (content)
                col << static_cast<char>(c);
        }
        else
        {
            content = true;
            col << static_cast<char>(c);
        }

        c = m_stream.get();
    }

    if (!startedWithEof)
    {
        auto value = col.str();
        utils::StringTrimR(value);
        cb(std::move(value));
    }

    return !startedWithEof;
}

namespace
{
    void InspectCsvValue(const std::string& value, bool& outContainsSeparator, bool& outContainsQuote, bool& outContainsNewLine)
    {
        for (const auto& c : value)
        {
            if (c == '"')
                outContainsQuote = true;
            else if (c == CSV_SEPARATOR)
                outContainsSeparator = true;
            else if (c == '\n')
                outContainsNewLine = true;
        }
    }
} // namespace

CsvOutputStream::CsvOutputStream(std::ostream& stream, const bool padColumns)
    : m_stream(stream),
      m_column_count(0),
      m_current_column(0),
      m_first_row(true),
      m_pad_columns(padColumns)
{
}

void CsvOutputStream::WriteColumn(const std::string& value)
{
    if (m_current_column++ > 0)
        m_stream << CSV_SEPARATOR;

    auto containsSeparator = false;
    auto containsQuote = false;
    auto containsNewLine = false;
    InspectCsvValue(value, containsSeparator, containsQuote, containsNewLine);

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
    else if (containsSeparator || containsNewLine)
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
    else if (m_pad_columns)
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
