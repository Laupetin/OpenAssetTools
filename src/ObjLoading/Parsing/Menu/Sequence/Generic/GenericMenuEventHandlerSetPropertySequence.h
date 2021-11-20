#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerSet.h"

namespace menu
{
    class GenericMenuEventHandlerSetPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<CommonEventHandlerSet> value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericMenuEventHandlerSetPropertySequence(std::string keywordName, callback_t setCallback);
    };
}
