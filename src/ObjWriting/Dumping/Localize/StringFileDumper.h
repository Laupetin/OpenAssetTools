#pragma once

#include "Dumping/AbstractTextDumper.h"
#include "Zone/Zone.h"

class StringFileDumper : AbstractTextDumper
{
public:
    StringFileDumper(const Zone& zone, std::ostream& stream);

    void SetConfigFile(std::string configFile);
    void SetNotes(std::string notes);
    void SetLanguageName(std::string language);

    void WriteLocalizeEntry(const std::string& reference, const std::string& value);

    void Finalize();

private:
    void WriteHeader();
    void WriteReference(const std::string& reference) const;

    const Zone& m_zone;

    std::string m_config_file;
    std::string m_notes;
    std::string m_language_caps;

    bool m_wrote_header;
};
