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
        [[nodiscard]] bool IsOpen() const override;
        [[nodiscard]] bool Eof() const override;

    private:
        IParserLineStream* const m_stream;
    };
} // namespace templating
