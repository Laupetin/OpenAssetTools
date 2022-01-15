#include "StringFileDumper.h"
#include <regex>

StringFileDumper::StringFileDumper(Zone* zone, std::ostream& stream)
    : AbstractTextDumper(stream),
      m_zone(zone),
      m_language_caps("ENGLISH"),
      m_wrote_header(false)
{
}

void StringFileDumper::SetLanguageName(std::string language)
{
    m_language_caps = std::move(language);
    for (auto& c : m_language_caps) c = toupper(c);
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
    m_stream << "// Dumped from fastfile \"" << m_zone->m_name << "\".\n";
    m_stream << "// In their original format the strings might have been separated in multiple files.\n";
    m_stream << "VERSION             \"1\"\n";
    m_stream << "CONFIG              \"" << m_config_file << "\"\n";
    m_stream << "FILENOTES           \"" << m_notes << "\"\n";

    m_wrote_header = true;
}

void StringFileDumper::WriteLocalizeEntry(const std::string& reference, const std::string& value)
{
    if (!m_wrote_header)
        WriteHeader();

    m_stream << "\n";
    m_stream << "REFERENCE           " << reference << "\n";

    auto escapedValue = std::regex_replace(value, std::regex("\n"), "\\n");
    escapedValue = std::regex_replace(escapedValue, std::regex("\r"), "\\r");

    const auto valueSpacing = std::string(15 - m_language_caps.length(), ' ');
    m_stream << "LANG_" << m_language_caps << valueSpacing << "\"" << escapedValue << "\"\n";
}

void StringFileDumper::Finalize()
{
    if (!m_wrote_header)
        WriteHeader();

    m_stream << "\nENDMARKER";
}
