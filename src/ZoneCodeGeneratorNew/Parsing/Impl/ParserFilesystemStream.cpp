#include "ParserFilesystemStream.h"

#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

const std::string ParserFilesystemStream::EMPTY_FILE_NAME;

ParserFilesystemStream::FileInfo::FileInfo(std::string filePath)
    : m_file_path(std::move(filePath)),
      m_stream(m_file_path),
      m_line_number(1)
{
}

ParserFilesystemStream::ParserFilesystemStream(std::string path)
{
    m_files.emplace(FileInfo(std::move(path)));
}

bool ParserFilesystemStream::IsOpen() const
{
    return !m_files.empty()
        && m_files.top().m_stream.is_open();
}

ParserLine ParserFilesystemStream::NextLine()
{
    std::ostringstream str;
    auto hasLength = false;

    if (m_files.empty())
        return ParserLine(EMPTY_FILE_NAME, 0, std::string());

    while(!m_files.empty())
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

        if(hasLength)
            return ParserLine(fileInfo.m_file_path, fileInfo.m_line_number, str.str());
        m_files.pop();
    }

    return ParserLine(EMPTY_FILE_NAME, 0, std::string());
}

bool ParserFilesystemStream::IncludeFile(const std::string& filename)
{
    if (m_files.empty())
        return false;
    
    const auto newFilePath = fs::path(m_files.top().m_file_path).remove_filename().concat(filename);

    FileInfo fileInfo(newFilePath.string());

    if (!fileInfo.m_stream.is_open())
        return false;

    m_files.emplace(std::move(fileInfo));
    return true;
}

bool ParserFilesystemStream::Eof() const
{
    return m_files.empty()
        || m_files.top().m_stream.eof();
}
