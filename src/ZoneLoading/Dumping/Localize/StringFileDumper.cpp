#include "StringFileDumper.h"
#include <regex>

StringFileDumper::StringFileDumper(Zone* zone, FileAPI::File* file)
{
    m_zone = zone;
    m_file = file;

    m_config_file = "";
    m_notes = "";
    m_language_caps = "ENGLISH";

    m_wrote_header = false;
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
    m_file->Printf("// Dumped from fastfile \"%s\".\n", m_zone->m_name.c_str());
    m_file->Printf("// In their original format the strings might have been separated in multiple files.\n");
    m_file->Printf("VERSION             \"1\"\n");
    m_file->Printf("CONFIG              \"%s\"\n", m_config_file.c_str());
    m_file->Printf("FILENOTES           \"%s\"\n", m_notes.c_str());

    m_wrote_header = true;
}

void StringFileDumper::WriteLocalizeEntry(const std::string& reference, const std::string& value)
{
    if (!m_wrote_header)
        WriteHeader();

    m_file->Printf("\n");
    m_file->Printf("REFERENCE           %s\n", reference.c_str());

    const std::string escapedValue = std::regex_replace(value, std::regex("\n"), "\\n");

    const std::string valueSpacing = std::string(15 - m_language_caps.length(), ' ');
    m_file->Printf("LANG_%s%s\"%s\"\n", m_language_caps.c_str(), valueSpacing.c_str(), escapedValue.c_str());
}

void StringFileDumper::Finalize()
{
    if (!m_wrote_header)
        WriteHeader();

    m_file->Printf("\nENDMARKER");
}