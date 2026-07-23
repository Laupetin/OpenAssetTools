#pragma once

#include "Parsing/IParserLineStream.h"

class CommentRemovingStreamProxy final : public IParserLineStream
{
    IParserLineStream* const m_stream;
    bool m_inside_multi_line_comment;
    bool m_next_line_is_comment;
    ParserLine m_multi_line_comment_start_line;
    size_t m_multi_line_comment_pos_in_start_line;

public:
    explicit CommentRemovingStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    [[nodiscard]] bool IsOpen() const override;
    [[nodiscard]] bool Eof() const override;
};
