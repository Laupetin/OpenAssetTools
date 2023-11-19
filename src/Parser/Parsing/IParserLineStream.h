#pragma once

#include "Utils/ClassUtils.h"

#include <functional>
#include <memory>
#include <string>

class ParserLine
{
public:
    std::shared_ptr<std::string> m_filename;
    int m_line_number;
    std::string m_line;

    ParserLine();
    ParserLine(std::shared_ptr<std::string> filename, int lineNumber, std::string line);

    _NODISCARD bool IsEof() const;
};

class IParserLineStream
{
public:
    IParserLineStream() = default;
    virtual ~IParserLineStream() = default;

    IParserLineStream(const IParserLineStream& other) = default;
    IParserLineStream(IParserLineStream&& other) noexcept = default;
    IParserLineStream& operator=(const IParserLineStream& other) = default;
    IParserLineStream& operator=(IParserLineStream&& other) noexcept = default;

    virtual ParserLine NextLine() = 0;
    virtual bool IncludeFile(const std::string& filename) = 0;
    virtual void PopCurrentFile() = 0;
    _NODISCARD virtual bool IsOpen() const = 0;
    _NODISCARD virtual bool Eof() const = 0;
};
