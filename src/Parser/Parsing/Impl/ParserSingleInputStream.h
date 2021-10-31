#pragma once

#include <istream>

#include "Parsing/IParserLineStream.h"

class ParserSingleInputStream final : public IParserLineStream
{
    std::istream& m_stream;
    std::string m_file_name;
    int m_line_number;

public:
    ParserSingleInputStream(std::istream& stream, std::string fileName);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};