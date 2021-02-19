#include "SequenceGame.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceGame::SequenceGame()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("game"),
        create.Identifier().Capture(CAPTURE_GAME),
        create.Char(';')
    });
}

void SequenceGame::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    state->SetGame(result.NextCapture(CAPTURE_GAME).IdentifierValue());
}
