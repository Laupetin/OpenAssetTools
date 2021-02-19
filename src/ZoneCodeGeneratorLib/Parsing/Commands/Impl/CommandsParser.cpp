#include "CommandsParser.h"

#include "Parsing/Commands/Sequence/SequenceAction.h"
#include "Parsing/Commands/Sequence/SequenceArrayCount.h"
#include "Parsing/Commands/Sequence/SequenceArraySize.h"
#include "Parsing/Commands/Sequence/SequenceAsset.h"
#include "Parsing/Commands/Sequence/SequenceBlock.h"
#include "Parsing/Commands/Sequence/SequenceCondition.h"
#include "Parsing/Commands/Sequence/SequenceCount.h"
#include "Parsing/Commands/Sequence/SequenceGame.h"
#include "Parsing/Commands/Sequence/SequenceName.h"
#include "Parsing/Commands/Sequence/SequenceReorder.h"
#include "Parsing/Commands/Sequence/SequenceReusable.h"
#include "Parsing/Commands/Sequence/SequenceScriptString.h"
#include "Parsing/Commands/Sequence/SequenceSetBlock.h"
#include "Parsing/Commands/Sequence/SequenceString.h"
#include "Parsing/Commands/Sequence/SequenceUse.h"

CommandsParser::CommandsParser(CommandsLexer* lexer, IDataRepository* targetRepository)
    : AbstractParser(lexer, std::make_unique<CommandsParserState>(targetRepository)),
      m_repository(targetRepository)
{
}

const std::vector<CommandsParser::sequence_t*>& CommandsParser::GetTestsForState()
{
    static std::vector<sequence_t*> tests({
        new SequenceAction(),
        new SequenceArrayCount(),
        new SequenceArraySize(),
        new SequenceAsset(),
        new SequenceBlock(),
        new SequenceCondition(),
        new SequenceCount(),
        new SequenceGame(),
        new SequenceName(),
        new SequenceReorder(),
        new SequenceReusable(),
        new SequenceScriptString(),
        new SequenceSetBlock(),
        new SequenceString(),
        new SequenceUse()
    });

    return tests;
}
