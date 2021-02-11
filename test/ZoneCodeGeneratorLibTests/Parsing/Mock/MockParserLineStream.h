#pragma once

#include <vector>
#include <unordered_map>

#include "Parsing/IParserLineStream.h"

class MockParserLineStream final : public IParserLineStream
{
public:
    static const std::string MOCK_FILENAME;

private:
    class IncludePos
    {
    public:
        std::string m_filename;
        const std::vector<std::string>& m_lines;
        unsigned m_pos;

        IncludePos(std::string filename, const std::vector<std::string>& lines);
    };
    
    std::unordered_map<std::string, std::vector<std::string>> m_include_lines;
    std::vector<IncludePos> m_include_positions;

public:
    explicit MockParserLineStream(const std::vector<std::string>& lines);

    void AddIncludeLines(const std::string& filename, const std::vector<std::string>& lines);
    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
