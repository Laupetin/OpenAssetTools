#pragma once

#include <stack>
#include <fstream>

#include "Parsing/IParserLineStream.h"

class ParserFilesystemStream final : public IParserLineStream
{
    static const std::string EMPTY_FILE_NAME;

    class FileInfo
    {
    public:
        std::string m_file_path;
        std::ifstream m_stream;
        int m_line_number;

        explicit FileInfo(std::string filePath);
    };
    std::stack<FileInfo> m_files;

public:
    explicit ParserFilesystemStream(std::string path);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
