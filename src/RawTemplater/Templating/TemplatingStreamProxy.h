#pragma once

#include "Parsing/Impl/AbstractDirectiveStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Utils/ClassUtils.h"

namespace templating
{
    class ITemplaterControl
    {
    protected:
        ITemplaterControl() = default;

    public:
        virtual ~ITemplaterControl() = default;
        ITemplaterControl(const ITemplaterControl& other) = default;
        ITemplaterControl(ITemplaterControl&& other) noexcept = default;
        ITemplaterControl& operator=(const ITemplaterControl& other) = default;
        ITemplaterControl& operator=(ITemplaterControl&& other) noexcept = default;

        virtual bool AddSwitch(std::string switchName) = 0;
        virtual bool AddOptions(std::string optionsName, std::vector<std::string> optionValues) = 0;
        virtual bool SetFileName(const std::string& fileName) = 0;
        virtual bool SkipPass() = 0;
    };

    class TemplatingStreamProxy final : public AbstractDirectiveStreamProxy
    {
    public:
        TemplatingStreamProxy(IParserLineStream* stream, ITemplaterControl* templaterControl);

        void SetDefinesProxy(DefinesStreamProxy* definesProxy);

        ParserLine NextLine() override;
        bool IncludeFile(const std::string& filename) override;
        void PopCurrentFile() override;
        _NODISCARD bool IsOpen() const override;
        _NODISCARD bool Eof() const override;

    private:
        static constexpr const char* SWITCH_DIRECTIVE = "switch";
        static constexpr const char* OPTIONS_DIRECTIVE = "options";
        static constexpr const char* FILENAME_DIRECTIVE = "filename";
        static constexpr const char* SKIP_DIRECTIVE = "skip";

        _NODISCARD bool MatchSwitchDirective(const ParserLine& line, size_t directiveStartPosition, size_t directiveEndPosition) const;
        _NODISCARD bool MatchOptionsDirective(const ParserLine& line, size_t directiveStartPosition, size_t directiveEndPosition) const;
        _NODISCARD bool MatchFilenameDirective(const ParserLine& line, size_t directiveStartPosition, size_t directiveEndPosition) const;
        _NODISCARD bool MatchSkipDirective(const ParserLine& line, size_t directiveStartPosition, size_t directiveEndPosition) const;
        _NODISCARD bool MatchDirectives(const ParserLine& line) const;

        IParserLineStream* const m_stream;
        ITemplaterControl* const m_templater_control;
        DefinesStreamProxy* m_defines_proxy;
    };
} // namespace templating
