#pragma once

#include "Game/GameLanguage.h"
#include "Localize/LocalizeFile.h"
#include "Parsing/IParserLineStream.h"

#include <memory>
#include <string>
#include <vector>

class LocalizeFileReader
{
    std::string m_file_name;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    GameLanguage m_language;

    bool OpenBaseStream(std::istream& stream);
    void SetupStreamProxies();

public:
    LocalizeFileReader(std::istream& stream, std::string fileName, GameLanguage language);

    std::vector<LocalizeFileEntry> ReadLocalizeFile();
};
