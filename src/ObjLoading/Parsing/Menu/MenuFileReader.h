#pragma once

#include "Domain/MenuFeatureLevel.h"
#include "Domain/MenuParsingResult.h"
#include "MenuAssetZoneState.h"
#include "MenuFileParserState.h"
#include "Parsing/IParserLineStream.h"
#include "SearchPath/ISearchPath.h"
#include "SearchPath/SearchPathMultiInputStream.h"

#include <memory>
#include <string>
#include <vector>

namespace menu
{
    class MenuFileReader : public SearchPathMultiInputStream
    {
    public:
        MenuFileReader(std::istream& stream, std::string fileName, FeatureLevel featureLevel, ISearchPath& searchPath);

        void IncludeZoneState(const MenuAssetZoneState& zoneState);
        void SetPermissiveMode(bool usePermissiveMode);

        std::unique_ptr<ParsingResult> ReadMenuFile();

    private:
        bool OpenBaseStream(std::istream& stream);
        void SetupDefinesProxy();
        void SetupStreamProxies();

        bool IsValidEndState(const MenuFileParserState* state) const;
        std::unique_ptr<ParsingResult> CreateParsingResult(MenuFileParserState* state) const;

        const FeatureLevel m_feature_level;
        const std::string m_file_name;

        IParserLineStream* m_stream;
        std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;

        const MenuAssetZoneState* m_zone_state;
        bool m_permissive_mode;
    };
} // namespace menu
