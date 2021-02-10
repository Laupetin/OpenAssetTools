#pragma once

#include <vector>

#include "Parsing/IParserLineStream.h"

class MockParserLineStream final : public IParserLineStream
{
    static const std::string MOCK_FILENAME;

    const std::vector<std::string>& m_lines;
    unsigned m_line;
    std::vector<std::string> m_includes{};

public:
    explicit MockParserLineStream(const std::vector<std::string>& lines);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
