#include "SequenceFullScreen.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

SequenceFullScreen::SequenceFullScreen()
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase("fullscreen"),
        create.Numeric().Capture(CAPTURE_VALUE)
    });
}

void SequenceFullScreen::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(state->m_current_menu);
    
    const auto nameValue = MenuMatcherFactory::TokenNumericIntValue(result.NextCapture(CAPTURE_VALUE));

    state->m_current_menu->m_fullscreen = nameValue > 0;
}
