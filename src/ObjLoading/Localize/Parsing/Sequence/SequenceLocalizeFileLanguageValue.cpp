#include "SequenceLocalizeFileLanguageValue.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Utils/Logging/Log.h"

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
        throw ParsingException(
            langToken.GetPos(),
            std::format("Value for reference \"{}\" already defined for language \"{}\"", state->m_current_reference, langToken.IdentifierValue()));
    }
    state->m_current_reference_languages.emplace(langName);

    if (langName == state->m_language_name_caps)
    {
        const auto& currentReference = state->m_current_reference;
        if (!state->m_duplication_checker.CheckLocalizeEntryForDuplicates(currentReference))
        {
            con::warn("Localize: a value for reference \"{}\" was already defined", currentReference);
        }

        state->m_entries.emplace_back(currentReference, valueToken.StringValue());
    }
}
