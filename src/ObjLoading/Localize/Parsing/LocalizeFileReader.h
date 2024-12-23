#pragma once

#include "Game/GameLanguage.h"
#include "ILocalizeFileDuplicationChecker.h"
#include "Localize/CommonLocalizeEntry.h"
#include "Parsing/IParserLineStream.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class LocalizeFileReader
{
    std::string m_file_name;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    GameLanguage m_language;
    ILocalizeFileDuplicationChecker& m_duplication_checker;

    bool OpenBaseStream(std::istream& stream);
    void SetupStreamProxies();

public:
    LocalizeFileReader(std::istream& stream, std::string fileName, GameLanguage language, ILocalizeFileDuplicationChecker& duplicationChecker);

    bool ReadLocalizeFile(std::vector<CommonLocalizeEntry>& entries);
};
