#include "CommonTechsetLoader.h"

#include "Parsing/IParserLineStream.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Techset/Parsing/TechsetFileParser.h"
#include "Techset/TechsetCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>
#include <memory>
#include <string>

namespace techset
{
    std::unique_ptr<CommonTechset>
        LoadCommonTechset(const std::string& assetName, const CommonTechniqueTypeNames& techniqueTypeNames, ISearchPath& searchPath, bool& failure)
    {
        failure = false;

        const auto fileName = GetFileNameForTechsetName(assetName);
        const auto techniqueFile = searchPath.Open(fileName);
        if (!techniqueFile.IsOpen())
            return nullptr;

        SimpleLexer::Config lexerConfig;
        lexerConfig.m_emit_new_line_tokens = false;
        lexerConfig.m_read_strings = true;
        lexerConfig.m_string_escape_sequences = false;
        lexerConfig.m_read_integer_numbers = false;
        lexerConfig.m_read_floating_point_numbers = false;

        ParserSingleInputStream baseStream(*techniqueFile.m_stream, fileName);
        CommentRemovingStreamProxy commentProxy(&baseStream);
        const auto lexer = std::make_unique<SimpleLexer>(&commentProxy, std::move(lexerConfig));

        const auto parser = std::make_unique<TechsetParser>(*lexer, assetName, techniqueTypeNames);

        const auto success = parser->Parse();
        if (success)
            return parser->GetParsingResult();

        con::error("Parsing techset file \"{}\" failed!", fileName);
        failure = true;
        return nullptr;
    }
} // namespace techset
