#pragma once

#include <istream>
#include <stack>
#include <memory>
#include <functional>

#include "Parsing/IParserLineStream.h"

class ParserMultiInputStream final : public IParserLineStream
{
public:
    using include_callback_t = std::function<std::unique_ptr<std::istream>(const std::string& filename, const std::string& sourceFile)>;

private:
    class FileInfo
    {
    public:
        std::unique_ptr<std::istream> m_owned_stream;
        std::istream& m_stream;
        std::string m_file_path;
        int m_line_number;

        FileInfo(std::unique_ptr<std::istream> stream, std::string filePath);
        FileInfo(std::istream& stream, std::string filePath);
    };

    const include_callback_t m_include_callback;
    std::stack<FileInfo> m_files;

public:
    ParserMultiInputStream(std::unique_ptr<std::istream> stream, std::string fileName, include_callback_t includeCallback);
    ParserMultiInputStream(std::istream& stream, std::string fileName, include_callback_t includeCallback);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};