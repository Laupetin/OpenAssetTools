#pragma once

#include "Parsing/IParserLineStream.h"

class CommentRemovingStreamProxy final : IParserLineStream
{
public:
    std::string NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    bool IsOpen() const override;
    _NODISCARD bool Eof() const override;
};
