#include "HeaderFileReader.h"

#include <iostream>

#include "Impl/HeaderLexer.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Impl/CommentRemovingStreamProxy.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/IncludingStreamProxy.h"
#include "Parsing/Impl/ParserFilesystemStream.h"

HeaderFileReader::HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename)
    : m_args(args),
      m_filename(std::move(filename)),
      m_stream(nullptr)
{
}

bool HeaderFileReader::OpenBaseStream()
{
    auto stream = std::make_unique<ParserFilesystemStream>(m_filename);
    if (!stream->IsOpen())
    {
        std::cout << "Could not open header file" << std::endl;
        return false;
    }

    m_stream = stream.get();
    m_open_streams.emplace_back(std::move(stream));
    return true;
}

void HeaderFileReader::SetupStreamProxies()
{
    auto commentProxy = std::make_unique<CommentRemovingStreamProxy>(m_stream);
    auto includeProxy = std::make_unique<IncludingStreamProxy>(commentProxy.get());
    auto definesProxy = std::make_unique<DefinesStreamProxy>(includeProxy.get());
    definesProxy->AddDefine(ZONE_CODE_GENERATOR_DEFINE_NAME, ZONE_CODE_GENERATOR_DEFINE_VALUE);

    m_stream = definesProxy.get();

    m_open_streams.emplace_back(std::move(commentProxy));
    m_open_streams.emplace_back(std::move(includeProxy));
    m_open_streams.emplace_back(std::move(definesProxy));
}

bool HeaderFileReader::ReadHeaderFile(IDataRepository* repository)
{
    std::cout << "Reading header file: " << m_filename << std::endl;

    if (!OpenBaseStream())
        return false;

    SetupStreamProxies();

    auto lexer = std::make_unique<HeaderLexer>(m_stream);

    try
    {
        /*while (true)
        {
            auto line = m_stream->NextLine();

            if (line.IsEof())
                break;

            std::cout << "Line " << line.m_filename.get() << ":" << line.m_line_number << ": " << line.m_line << "\n";
        }*/

        auto eof = false;
        while (!eof)
        {
            const auto& token = lexer->GetToken(0);

            switch (token.m_type)
            {
            case HeaderParserValueType::END_OF_FILE:
            case HeaderParserValueType::INVALID:
                eof = true;
                break;

            case HeaderParserValueType::CHARACTER:
                std::cout << "Token " << token.CharacterValue() << "\n";
                break;

            case HeaderParserValueType::IDENTIFIER:
                std::cout << "Token IDENTIFIER \"" << token.IdentifierValue() << "\"\n";
                break;

            case HeaderParserValueType::STRING:
                std::cout << "Token STRING \"" << token.StringValue() << "\"\n";
                break;

            case HeaderParserValueType::INTEGER:
                std::cout << "Token INTEGER " << token.IntegerValue() << "\n";
                break;

            case HeaderParserValueType::FLOATING_POINT:
                std::cout << "Token FLOATINGPOINT " << token.FloatingPointValue() << "\n";
                break;

            case HeaderParserValueType::DECLSPEC:
                std::cout << "Token DECLSPEC\n";
                break;

            case HeaderParserValueType::ALIGN:
                std::cout << "Token ALIGN\n";
                break;

            case HeaderParserValueType::ALIGNAS:
                std::cout << "Token ALIGNAS\n";
                break;

            case HeaderParserValueType::CONST:
                std::cout << "Token CONST\n";
                break;

            case HeaderParserValueType::ENUM:
                std::cout << "Token ENUM\n";
                break;

            case HeaderParserValueType::NAMESPACE:
                std::cout << "Token NAMESPACE\n";
                break;

            case HeaderParserValueType::STRUCT:
                std::cout << "Token STRUCT\n";
                break;

            case HeaderParserValueType::TYPEDEF:
                std::cout << "Token TYPEDEF\n";
                break;

            case HeaderParserValueType::UNION:
                std::cout << "Token UNION\n";
                break;

            default:
                std::cout << "Token UNKNOWN\n";
                break;
            }

            lexer->PopTokens(1);
        }
    }
    catch (const ParsingException& e)
    {
        std::cout << "Error: " << e.FullMessage() << std::endl;
    }

    return true;
}
