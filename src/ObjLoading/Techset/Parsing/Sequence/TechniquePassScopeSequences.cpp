#include "TechniquePassScopeSequences.h"

#include <cassert>
#include <sstream>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace techset;

namespace techset
{
    class SequenceEndPass final : public TechniqueParser::sequence_t
    {
    public:
        SequenceEndPass()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}')
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_pass == true);
            state->m_in_pass = false;
        }
    };

    class SequenceStateMap final : public TechniqueParser::sequence_t
    {
        static constexpr auto CAPTURE_STATE_MAP_NAME = 1;

    public:
        SequenceStateMap()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("stateMap"),
                create.String().Capture(CAPTURE_STATE_MAP_NAME),
                create.Char(';')
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_acceptor->AcceptStateMap(result.NextCapture(CAPTURE_STATE_MAP_NAME).StringValue());
        }
    };

    class SequenceShader final : public TechniqueParser::sequence_t
    {
        static constexpr auto TAG_VERTEX_SHADER = 1;
        static constexpr auto TAG_PIXEL_SHADER = 2;

        static constexpr auto CAPTURE_START = 1;
        static constexpr auto CAPTURE_VERSION = 2;
        static constexpr auto CAPTURE_VERSION_MAJOR = 3;
        static constexpr auto CAPTURE_VERSION_MINOR = 4;
        static constexpr auto CAPTURE_SHADER_NAME = 5;

    public:
        SequenceShader()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Or({
                    create.Keyword("vertexShader").Tag(TAG_VERTEX_SHADER),
                    create.Keyword("pixelShader").Tag(TAG_PIXEL_SHADER),
                }).Capture(CAPTURE_START),
                create.Or({
                    create.And({
                        create.Integer().Capture(CAPTURE_VERSION_MAJOR),
                        create.Char('.'),
                        create.Integer().Capture(CAPTURE_VERSION_MINOR),
                    }),
                    create.FloatingPoint().Capture(CAPTURE_VERSION), // This is dumb but cod devs made the format so cannot change it
                    create.String().Capture(CAPTURE_VERSION)
                }),
                create.String().Capture(CAPTURE_SHADER_NAME),
                create.Char('{')
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& firstToken = result.NextCapture(CAPTURE_START);

            // Don't care about shader version since it's stated in the shader bin anyway

            const auto& shaderNameToken = result.NextCapture(CAPTURE_SHADER_NAME);

            bool acceptorResult;
            std::string errorMessage;
            const auto shaderTag = result.NextTag();
            assert(shaderTag == TAG_VERTEX_SHADER || shaderTag == TAG_PIXEL_SHADER);
            if (shaderTag == TAG_VERTEX_SHADER)
            {
                acceptorResult = state->m_acceptor->AcceptVertexShader(shaderNameToken.StringValue(), errorMessage);
                state->m_current_shader = ShaderSelector::VERTEX_SHADER;
            }
            else
            {
                acceptorResult = state->m_acceptor->AcceptPixelShader(shaderNameToken.StringValue(), errorMessage);
                state->m_current_shader = ShaderSelector::PIXEL_SHADER;
            }

            state->m_in_shader = true;

            if (!acceptorResult)
                throw ParsingException(firstToken.GetPos(), std::move(errorMessage));
        }
    };

    class SequenceVertexStreamRouting final : public TechniqueParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_STREAM_DESTINATION_NAME = 2;
        static constexpr auto CAPTURE_STREAM_DESTINATION_INDEX = 3;
        static constexpr auto CAPTURE_STREAM_SOURCE_NAME = 4;
        static constexpr auto CAPTURE_STREAM_SOURCE_INDEX = 5;

    public:
        SequenceVertexStreamRouting()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Keyword("vertex").Capture(CAPTURE_FIRST_TOKEN),
                create.Char('.'),
                create.Identifier().Capture(CAPTURE_STREAM_DESTINATION_NAME),
                create.Optional(create.And({
                    create.Char('['),
                    create.Integer().Capture(CAPTURE_STREAM_DESTINATION_INDEX),
                    create.Char(']')
                })),

                create.Char('='),

                create.Keyword("code"),
                create.Char('.'),
                create.Identifier().Capture(CAPTURE_STREAM_SOURCE_NAME),
                create.Optional(create.And({
                    create.Char('['),
                    create.Integer().Capture(CAPTURE_STREAM_SOURCE_INDEX),
                    create.Char(']')
                })),

                create.Char(';')
            });
        }

        static std::string CreateRoutingString(SequenceResult<SimpleParserValue>& result, const int nameCapture, const int indexCapture)
        {
            if (result.HasNextCapture(indexCapture))
            {
                const auto& indexToken = result.NextCapture(indexCapture);
                if (indexToken.IntegerValue() < 0)
                    throw ParsingException(indexToken.GetPos(), "Index cannot be negative");

                std::ostringstream ss;
                ss << result.NextCapture(nameCapture).IdentifierValue() << '[' << indexToken.IntegerValue() << ']';
                return ss.str();
            }

            return result.NextCapture(nameCapture).IdentifierValue();
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& firstToken = result.NextCapture(CAPTURE_FIRST_TOKEN);
            const std::string destinationString = CreateRoutingString(result, CAPTURE_STREAM_DESTINATION_NAME, CAPTURE_STREAM_DESTINATION_INDEX);
            const std::string sourceString = CreateRoutingString(result, CAPTURE_STREAM_SOURCE_NAME, CAPTURE_STREAM_SOURCE_INDEX);

            std::string errorMessage;
            if (!state->m_acceptor->AcceptVertexStreamRouting(destinationString, sourceString, errorMessage))
                throw ParsingException(firstToken.GetPos(), std::move(errorMessage));
        }
    };
}

const std::vector<TechniqueParser::sequence_t*>& TechniquePassScopeSequences::GetSequences()
{
    static std::vector<TechniqueParser::sequence_t*> tests({
        new SequenceEndPass(),
        new SequenceStateMap(),
        new SequenceShader(),
        new SequenceVertexStreamRouting()
    });

    return tests;
}
