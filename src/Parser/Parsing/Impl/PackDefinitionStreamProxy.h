#pragma once

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IPackValueSupplier.h"
#include "Parsing/IParserLineStream.h"

#include <stack>

class PackDefinitionStreamProxy final : public AbstractDirectiveStreamProxy, public IPackValueSupplier
{
public:
    static constexpr int DEFAULT_PACK = 8;

private:
    static constexpr const char* PRAGMA_DIRECTIVE = "pragma";
    static constexpr const char* PACK_PRAGMA_COMMAND = "pack";
    static constexpr const char* PUSH_KEYWORD = "push";
    static constexpr const char* POP_KEYWORD = "pop";

    IParserLineStream* const m_stream;
    std::stack<int> m_current_pack;

    [[nodiscard]] bool MatchPackDirective(const ParserLine& line, size_t directiveStartPosition, size_t directiveEndPosition);
    [[nodiscard]] bool MatchDirectives(const ParserLine& line);

public:
    explicit PackDefinitionStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    [[nodiscard]] bool IsOpen() const override;
    [[nodiscard]] bool Eof() const override;

    [[nodiscard]] int GetCurrentPack() const override;
};
