#include "SequenceLocalizeFileLanguageValue.h"

#include <sstream>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileLanguageValue::SequenceLocalizeFileLanguageValue()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.KeywordPrefix("LANG_").Capture(CAPTURE_LANGUAGE_NAME),
        create.String().Capture(CAPTURE_ENTRY_VALUE),
        create.Type(SimpleParserValueType::NEW_LINE)
    });
}

std::string SequenceLocalizeFileLanguageValue::UnescapeValue(const std::string& value)
{
    std::ostringstream str;

    auto isEscaped = false;
    for(const auto c : value)
    {
        if(isEscaped)
        {
            switch(c)
            {
            case 'n':
                str << '\n';
                break;

            case 'r':
                str << '\r';
                break;

            default:
                str << c;
                break;
            }
            isEscaped = false;
        }
        else if(c == '\\')
        {
            isEscaped = true;
        }
        else
        {
            str << c;
        }
    }

    return str.str();
}

void SequenceLocalizeFileLanguageValue::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    const auto& langToken = result.NextCapture(CAPTURE_LANGUAGE_NAME);
    const auto& valueToken = result.NextCapture(CAPTURE_ENTRY_VALUE);

    const auto langName = langToken.IdentifierValue().substr(std::char_traits<char>::length("LANG_"));
    const auto alreadyDefinedLanguage = state->m_current_reference_languages.find(langName);
    if(alreadyDefinedLanguage != state->m_current_reference_languages.end())
    {
        std::ostringstream str;
        str << "Value for reference \"" << state->m_current_reference << "\" already defined for language \"" << langToken.IdentifierValue() << "\"";
        throw ParsingException(langToken.GetPos(), str.str());
    }
    state->m_current_reference_languages.emplace(langName);

    if(langName == state->m_language_name_caps)
        state->m_entries.emplace_back(state->m_current_reference, UnescapeValue(valueToken.StringValue()));
}
