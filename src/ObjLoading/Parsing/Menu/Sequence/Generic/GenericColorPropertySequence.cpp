#include "GenericColorPropertySequence.h"

#include <utility>

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

GenericColorPropertySequence::GenericColorPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)),
        create.Numeric().Capture(CAPTURE_R),
        create.Numeric().Capture(CAPTURE_G),
        create.Numeric().Capture(CAPTURE_B),
        create.Numeric().Capture(CAPTURE_A)
    });
}

void GenericColorPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        CommonColor color{};
        color.r = MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_R));
        color.g = MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_G));
        color.b = MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_B));
        color.a = MenuMatcherFactory::TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_A));

        m_set_callback(state, color);
    }
}
