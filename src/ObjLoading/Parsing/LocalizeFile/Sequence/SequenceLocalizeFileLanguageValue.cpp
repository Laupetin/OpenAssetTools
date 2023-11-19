#include "SequenceLocalizeFileLanguageValue.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

#include <sstream>

SequenceLocalizeFileLanguageValue::SequenceLocalizeFileLanguageValue()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.KeywordPrefix("LANG_").Capture(CAPTURE_LANGUAGE_NAME),
        create.String().Capture(CAPTURE_ENTRY_VALUE),
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileLanguageValue::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    const auto& langToken = result.NextCapture(CAPTURE_LANGUAGE_NAME);
    const auto& valueToken = result.NextCapture(CAPTURE_ENTRY_VALUE);

    const auto langName = langToken.IdentifierValue().substr(std::char_traits<char>::length("LANG_"));
    const auto alreadyDefinedLanguage = state->m_current_reference_languages.find(langName);
    if (alreadyDefinedLanguage != state->m_current_reference_languages.end())
    {
        std::ostringstream str;
        str << "Value for reference \"" << state->m_current_reference << "\" already defined for language \"" << langToken.IdentifierValue() << "\"";
        throw ParsingException(langToken.GetPos(), str.str());
    }
    state->m_current_reference_languages.emplace(langName);

    if (langName == state->m_language_name_caps)
        state->m_entries.emplace_back(state->m_current_reference, valueToken.StringValue());
}
