#include "ParserMultiInputStream.h"

#include <sstream>

ParserMultiInputStream::FileInfo::FileInfo(std::unique_ptr<std::istream> stream, std::string filePath)
    : m_owned_stream(std::move(stream)),
      m_stream(*m_owned_stream),
      m_file_path(std::move(filePath)),
      m_line_number(1)
{
}

ParserMultiInputStream::FileInfo::FileInfo(std::istream& stream, std::string filePath)
    : m_stream(stream),
      m_file_path(std::move(filePath)),
      m_line_number(1)
{
}

ParserMultiInputStream::ParserMultiInputStream(std::unique_ptr<std::istream> stream, std::string fileName, include_callback_t includeCallback)
    : m_include_callback(std::move(includeCallback))
{
    m_files.emplace(std::move(stream), std::move(fileName));
}

ParserMultiInputStream::ParserMultiInputStream(std::istream& stream, std::string fileName, include_callback_t includeCallback)
    : m_include_callback(std::move(includeCallback))
{
    m_files.emplace(stream, std::move(fileName));
}

ParserLine ParserMultiInputStream::NextLine()
{
    std::ostringstream str;
    auto hasLength = false;

    if (m_files.empty())
        return ParserLine();

    while (!m_files.empty())
    {
        auto& fileInfo = m_files.top();

        auto c = fileInfo.m_stream.get();
        while (c != EOF)
        {
            switch (c)
            {
            case '\r':
                c = fileInfo.m_stream.get();
                if (c == '\n')
                    return ParserLine(fileInfo.m_file_path, fileInfo.m_line_number++, str.str());
                str << '\r';
                hasLength = true;
                continue;

            case '\n':
                return ParserLine(fileInfo.m_file_path, fileInfo.m_line_number++, str.str());

            default:
                str << static_cast<char>(c);
                hasLength = true;
                break;
            }

            c = fileInfo.m_stream.get();
        }

        if (hasLength)
            return ParserLine(fileInfo.m_file_path, fileInfo.m_line_number, str.str());
        m_files.pop();
    }

    return ParserLine();
}

bool ParserMultiInputStream::IncludeFile(const std::string& filename)
{
    if (!m_include_callback)
        return false;

    auto newFile = m_include_callback(filename);
    if (!newFile)
        return false;

    m_files.emplace(std::move(newFile), filename);
    return true;
}

void ParserMultiInputStream::PopCurrentFile()
{
    if (!m_files.empty())
        m_files.pop();
}

bool ParserMultiInputStream::IsOpen() const
{
    return !m_files.empty();
}

bool ParserMultiInputStream::Eof() const
{
    return m_files.empty();
}
