#pragma once

#include "Parsing/IParserLineStream.h"

#include <functional>
#include <istream>
#include <memory>
#include <stack>

class IInclusionCallback
{
public:
    virtual ~IInclusionCallback() = default;

    virtual std::unique_ptr<std::istream> OpenIncludedFile(const std::string& filename, const std::string& sourceFile) = 0;
};

class ParserMultiInputStream final : public IParserLineStream
{
public:
    ParserMultiInputStream(std::unique_ptr<std::istream> stream, std::string fileName, IInclusionCallback& includeCallback);
    ParserMultiInputStream(std::istream& stream, std::string fileName, IInclusionCallback& includeCallback);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;

private:
    class FileInfo
    {
    public:
        std::unique_ptr<std::istream> m_owned_stream;
        std::istream& m_stream;
        std::shared_ptr<std::string> m_file_path;
        int m_line_number;

        FileInfo(std::unique_ptr<std::istream> stream, std::string filePath);
        FileInfo(std::istream& stream, std::string filePath);
    };

    IInclusionCallback& m_include_callback;
    std::stack<FileInfo> m_files;
};
