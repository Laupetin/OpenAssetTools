#include "LocalizeFileParser.h"

#include "Sequence/SequenceLocalizeFileConfig.h"
#include "Sequence/SequenceLocalizeFileConsumeEmptyLines.h"
#include "Sequence/SequenceLocalizeFileEndMarker.h"
#include "Sequence/SequenceLocalizeFileLanguageValue.h"
#include "Sequence/SequenceLocalizeFileNotes.h"
#include "Sequence/SequenceLocalizeFileReference.h"
#include "Sequence/SequenceLocalizeFileVersion.h"

LocalizeFileParser::LocalizeFileParser(SimpleLexer* lexer, GameLanguage language, ILocalizeFileDuplicationChecker& duplicationChecker)
    : AbstractParser(lexer, std::make_unique<LocalizeFileParserState>(language, duplicationChecker))
{
}

const std::vector<AbstractParser<SimpleParserValue, LocalizeFileParserState>::sequence_t*>& LocalizeFileParser::GetTestsForState()
{
    static std::vector<sequence_t*> tests({
        new SequenceLocalizeFileReference(),
        new SequenceLocalizeFileConfig(),
        new SequenceLocalizeFileNotes(),
        new SequenceLocalizeFileVersion(),
        new SequenceLocalizeFileEndMarker(),
        new SequenceLocalizeFileLanguageValue(),
        new SequenceLocalizeFileConsumeEmptyLines(),
    });

    static std::vector<sequence_t*> noTests({
        new SequenceLocalizeFileConsumeEmptyLines(),
    });

    return !m_state->m_end ? tests : noTests;
}

std::vector<CommonLocalizeEntry> LocalizeFileParser::GetParsedValues()
{
    return std::move(m_state->m_entries);
}
