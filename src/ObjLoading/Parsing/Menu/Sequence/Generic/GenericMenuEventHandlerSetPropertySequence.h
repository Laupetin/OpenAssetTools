#pragma once

#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerSet.h"
#include "Parsing/Menu/MenuFileParser.h"

#include <functional>
#include <string>

namespace menu
{
    class GenericMenuEventHandlerSetPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<std::unique_ptr<CommonEventHandlerSet>&(MenuFileParserState* state, const TokenPos& pos)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericMenuEventHandlerSetPropertySequence(std::string keywordName, callback_t setCallback);
    };
} // namespace menu
