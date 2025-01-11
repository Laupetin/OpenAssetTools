#include "StringFileDumper.h"

#include "Utils/StringUtils.h"

#include <regex>

StringFileDumper::StringFileDumper(const Zone& zone, std::ostream& stream)
    : AbstractTextDumper(stream),
      m_zone(zone),
      m_language_caps("ENGLISH"),
      m_wrote_header(false)
{
}

void StringFileDumper::SetLanguageName(std::string language)
{
    m_language_caps = std::move(language);
    for (auto& c : m_language_caps)
        c = toupper(c);
}

void StringFileDumper::SetConfigFile(std::string configFile)
{
    m_config_file = std::move(configFile);
}

void StringFileDumper::SetNotes(std::string notes)
{
    m_notes = std::move(notes);
}

void StringFileDumper::WriteHeader()
{
    m_stream << "// Dumped from fastfile \"" << m_zone.m_name << "\".\n";
    m_stream << "// In their original format the strings might have been separated in multiple files.\n";
    m_stream << "VERSION             \"1\"\n";
    m_stream << "CONFIG              \"" << m_config_file << "\"\n";
    m_stream << "FILENOTES           \"" << m_notes << "\"\n";

    m_wrote_header = true;
}

void StringFileDumper::WriteReference(const std::string& reference) const
{
    if (reference.find_first_not_of(utils::LETTERS_AL_NUM_UNDERSCORE) != std::string::npos)
    {
        m_stream << "REFERENCE           \"";

        utils::EscapeStringForQuotationMarks(m_stream, reference);

        m_stream << "\"\n";
    }
    else
        m_stream << "REFERENCE           " << reference << "\n";
}

void StringFileDumper::WriteLocalizeEntry(const std::string& reference, const std::string& value)
{
    if (!m_wrote_header)
        WriteHeader();

    m_stream << "\n";
    WriteReference(reference);

    const auto valueSpacing = std::string(15 - m_language_caps.length(), ' ');
    m_stream << "LANG_" << m_language_caps << valueSpacing << "\"";
    utils::EscapeStringForQuotationMarks(m_stream, value);
    m_stream << "\"\n";
}

void StringFileDumper::Finalize()
{
    if (!m_wrote_header)
        WriteHeader();

    m_stream << "\nENDMARKER";
}
