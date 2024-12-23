#include "LocalizeFileParserState.h"

#include "Localize/LocalizeCommon.h"
#include "Utils/StringUtils.h"

LocalizeFileParserState::LocalizeFileParserState(const GameLanguage language, ILocalizeFileDuplicationChecker& duplicationChecker)
    : m_end(false),
      m_language(language),
      m_duplication_checker(duplicationChecker)
{
    m_language_name_caps = LocalizeCommon::GetNameOfLanguage(m_language);
    utils::MakeStringUpperCase(m_language_name_caps);
}
