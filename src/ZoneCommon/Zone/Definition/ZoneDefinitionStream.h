#pragma once

#include "Game/IGame.h"
#include "Parsing/IParserLineStream.h"
#include "SearchPath/ISearchPath.h"
#include "SearchPath/SearchPathMultiInputStream.h"
#include "ZoneDefinition.h"

#include <iostream>
#include <memory>
#include <optional>

class ZoneDefinitionInputStream : public SearchPathMultiInputStream
{
public:
    ZoneDefinitionInputStream(std::istream& stream, std::string targetName, std::string fileName, ISearchPath& searchPath);

    void SetPreviouslySetGame(GameId game);
    std::unique_ptr<ZoneDefinition> ReadDefinition();

private:
    bool OpenBaseStream(std::istream& stream);
    void SetupStreamProxies();

    std::string m_target_name;
    std::string m_file_name;
    IParserLineStream* m_stream;
    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    std::optional<GameId> m_previously_set_game;
};

class ZoneDefinitionOutputStream
{
public:
    explicit ZoneDefinitionOutputStream(std::ostream& stream);

    void EmptyLine() const;
    void WriteComment(const std::string& comment) const;
    void WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const;
    void WriteEntry(const std::string& entryKey, const std::string& entryValue) const;

private:
    std::ostream& m_stream;
};
