#pragma once

#include <stack>
#include <fstream>

#include "Parsing/IParserLineStream.h"

class ParserFilesystemStream final : public IParserLineStream
{
    class FileInfo
    {
    public:
        std::shared_ptr<std::string> m_file_path;
        std::ifstream m_stream;
        int m_line_number;

        explicit FileInfo(std::string filePath);
    };
    std::stack<FileInfo> m_files;

public:
    explicit ParserFilesystemStream(const std::string& path);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
