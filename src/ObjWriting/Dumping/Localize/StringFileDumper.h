#pragma once

#include <ostream>

#include "Zone/Zone.h"

class StringFileDumper
{
    Zone* m_zone;
    std::ostream& m_stream;

    std::string m_config_file;
    std::string m_notes;
    std::string m_language_caps;

    bool m_wrote_header;

    void WriteHeader();

public:
    StringFileDumper(Zone* zone, std::ostream& stream);

    void SetConfigFile(std::string configFile);
    void SetNotes(std::string notes);
    void SetLanguageName(std::string language);

    void WriteLocalizeEntry(const std::string& reference, const std::string& value);

    void Finalize();
};
