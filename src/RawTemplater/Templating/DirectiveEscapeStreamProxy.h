#pragma once
#include "Parsing/Impl/AbstractDirectiveStreamProxy.h"

namespace templating
{
    class DirectiveEscapeStreamProxy final : public AbstractDirectiveStreamProxy
    {
    public:
        explicit DirectiveEscapeStreamProxy(IParserLineStream* stream);

        ParserLine NextLine() override;
        bool IncludeFile(const std::string& filename) override;
        void PopCurrentFile() override;
        _NODISCARD bool IsOpen() const override;
        _NODISCARD bool Eof() const override;

    private:
        IParserLineStream* const m_stream;
    };
} // namespace templating
