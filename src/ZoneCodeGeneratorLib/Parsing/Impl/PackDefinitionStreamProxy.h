#pragma once

#include <stack>

#include "AbstractDirectiveStreamProxy.h"
#include "Parsing/IPackValueSupplier.h"
#include "Parsing/IParserLineStream.h"

class PackDefinitionStreamProxy final : public AbstractDirectiveStreamProxy, public IPackValueSupplier
{
public:
    static constexpr int DEFAULT_PACK = 8;

private:
    static constexpr const char* PRAGMA_PACK_DIRECTIVE = "pragma pack";
    static constexpr const char* PUSH_KEYWORD = "push";
    static constexpr const char* POP_KEYWORD = "pop";

    IParserLineStream* const m_stream;
    std::stack<int> m_current_pack;

    _NODISCARD bool MatchPackDirective(const ParserLine& line, unsigned directivePosition);
    _NODISCARD bool MatchDirectives(const ParserLine& line);

public:
    explicit PackDefinitionStreamProxy(IParserLineStream* stream);

    ParserLine NextLine() override;
    bool IncludeFile(const std::string& filename) override;
    void PopCurrentFile() override;
    _NODISCARD bool IsOpen() const override;
    _NODISCARD bool Eof() const override;

    int GetCurrentPack() override;
};
