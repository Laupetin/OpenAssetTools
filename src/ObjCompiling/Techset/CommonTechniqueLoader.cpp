#include "CommonTechniqueLoader.h"

#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/TechniqueFileParser.h"
#include "Techset/TechsetCommon.h"
#include "Utils/Logging/Log.h"

namespace techset
{
    std::unique_ptr<CommonTechnique> LoadCommonTechnique(const std::string& techniqueName,
                                                         const CommonCodeSourceInfos& codeSourceInfos,
                                                         const CommonStreamRoutingInfos& routingInfos,
                                                         CommonShaderArgCreator& commonShaderArgCreator,
                                                         ISearchPath& searchPath,
                                                         bool& failure)
    {
        failure = false;

        const auto fileName = GetFileNameForTechniqueName(techniqueName);
        const auto techniqueFile = searchPath.Open(fileName);
        if (!techniqueFile.IsOpen())
            return nullptr;

        SimpleLexer::Config lexerConfig;
        lexerConfig.m_emit_new_line_tokens = false;
        lexerConfig.m_read_strings = true;
        lexerConfig.m_string_escape_sequences = false;
        lexerConfig.m_read_integer_numbers = true;
        lexerConfig.m_read_floating_point_numbers = true;

        const auto baseStream = std::make_unique<ParserSingleInputStream>(*techniqueFile.m_stream, fileName);
        const auto commentProxy = std::make_unique<CommentRemovingStreamProxy>(baseStream.get());
        const auto lexer = std::make_unique<SimpleLexer>(commentProxy.get(), std::move(lexerConfig));

        const auto parser = std::make_unique<TechniqueParser>(*lexer, techniqueName, codeSourceInfos, routingInfos, commonShaderArgCreator);

        const auto success = parser->Parse();
        if (success)
            return parser->GetParsingResult();

        failure = true;
        con::error("Parsing technique file \"{}\" failed!", fileName);
        return nullptr;
    }
} // namespace techset
