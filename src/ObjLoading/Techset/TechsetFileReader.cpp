#include "TechsetFileReader.h"

#include <iostream>

#include "Parsing/TechsetFileParser.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"

using namespace techset;

TechsetFileReader::TechsetFileReader(std::istream& stream, std::string fileName, const char** validTechniqueTypeNames, const size_t validTechniqueTypeNameCount)
    : m_file_name(std::move(fileName)),
      m_valid_technique_type_names(validTechniqueTypeNames),
      m_valid_technique_type_name_count(validTechniqueTypeNameCount)
{
    m_base_stream = std::make_unique<ParserSingleInputStream>(stream, m_file_name);
    m_comment_proxy = std::make_unique<CommentRemovingStreamProxy>(m_base_stream.get());
}

std::unique_ptr<techset::TechsetDefinition> TechsetFileReader::ReadTechsetDefinition() const
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = true;
    lexerConfig.m_string_escape_sequences = false;
    lexerConfig.m_read_integer_numbers = false;
    lexerConfig.m_read_floating_point_numbers = false;
    const auto lexer = std::make_unique<SimpleLexer>(m_comment_proxy.get(), std::move(lexerConfig));

    const auto parser = std::make_unique<techset::TechsetParser>(lexer.get(), m_valid_technique_type_names, m_valid_technique_type_name_count);

    const auto success = parser->Parse();
    if (success)
        return parser->GetTechsetDefinition();

    std::cout << "Parsing techset file \"" << m_file_name << "\" failed!\n";
    return {};
}
