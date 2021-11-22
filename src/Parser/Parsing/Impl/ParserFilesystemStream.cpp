#include "ParserFilesystemStream.h"

#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

ParserFilesystemStream::FileInfo::FileInfo(std::string filePath)
    : m_file_path(std::make_shared<std::string>(std::move(filePath))),
      m_stream(*m_file_path),
      m_line_number(1)
{
}

ParserFilesystemStream::ParserFilesystemStream(const std::string& path)
{
    const auto absolutePath = absolute(fs::path(path));
    m_files.emplace(FileInfo(absolutePath.string()));
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
        return ParserLine();

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

    return ParserLine();
}

bool ParserFilesystemStream::IncludeFile(const std::string& filename)
{
    if (m_files.empty())
        return false;
    
    auto newFilePath = fs::path(*m_files.top().m_file_path);
    newFilePath.remove_filename().concat(filename);
    newFilePath = absolute(newFilePath);

    FileInfo fileInfo(newFilePath.string());

    if (!fileInfo.m_stream.is_open())
        return false;

    m_files.emplace(std::move(fileInfo));
    return true;
}

void ParserFilesystemStream::PopCurrentFile()
{
    if(!m_files.empty())
        m_files.pop();
}

bool ParserFilesystemStream::Eof() const
{
    return m_files.empty()
        || m_files.top().m_stream.eof();
}
