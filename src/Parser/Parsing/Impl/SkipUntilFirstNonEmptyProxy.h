#pragma once

#include "Parsing/IParserLineStream.h"

class SkipUntilFirstNonEmptyProxy final : public IParserLineStream
{
public:
    explicit SkipUntilFirstNonEmptyProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;

private:
    IParserLineStream* const m_stream;
    bool m_had_non_empty;
};
