#include "SequenceLoadMenu.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace menu;

SequenceLoadMenu::SequenceLoadMenu()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("loadMenu"),
        create.Char('{'),
        create.String().Capture(CAPTURE_MENU_NAME),
        create.Char('}'),
    });
}

void SequenceLoadMenu::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(!state->m_current_menu);

    const auto& menuNameToken = result.NextCapture(CAPTURE_MENU_NAME);

    if (menuNameToken.StringValue().empty())
        throw ParsingException(menuNameToken.GetPos(), "Invalid menu name");

    state->m_menus_to_load.emplace_back(menuNameToken.StringValue());
}
