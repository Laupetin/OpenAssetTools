#include "TechniquePassScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

#include <cassert>
#include <sstream>

using namespace techset;

namespace techset
{
    class SequenceEndPass final : public TechniqueParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

    public:
        SequenceEndPass()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}').Capture(CAPTURE_FIRST_TOKEN),
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_pass);
            assert(state->m_technique);

            auto finalizeResult = state->m_shader_arg_creator.FinalizePass(*state->m_technique, *state->m_current_pass);
            if (!finalizeResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), finalizeResult.error());

            state->m_current_pass->m_vertex_declaration.SortRoutingEntries();
            state->m_technique->m_passes.emplace_back(std::move(*state->m_current_pass));
            state->m_current_pass = std::nullopt;
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
                create.Char(';'),
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_pass);

            state->m_current_pass->m_state_map = result.NextCapture(CAPTURE_STATE_MAP_NAME).StringValue();
        }
    };

    class SequenceShader final : public TechniqueParser::sequence_t
    {
        static constexpr auto TAG_VERTEX_SHADER = 1;
        static constexpr auto TAG_PIXEL_SHADER = 2;

        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_VERSION = 2;
        static constexpr auto CAPTURE_VERSION_MAJOR = 3;
        static constexpr auto CAPTURE_VERSION_MINOR = 4;
        static constexpr auto CAPTURE_SHADER_NAME = 5;

    public:
        SequenceShader()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create
                    .Or({
                        create.Keyword("vertexShader").Tag(TAG_VERTEX_SHADER),
                        create.Keyword("pixelShader").Tag(TAG_PIXEL_SHADER),
                    })
                    .Capture(CAPTURE_FIRST_TOKEN),
                create.Or({
                    create.And({
                        create.Integer().Capture(CAPTURE_VERSION_MAJOR),
                        create.Char('.'),
                        create.Integer().Capture(CAPTURE_VERSION_MINOR),
                    }),
                    create.FloatingPoint().Capture(CAPTURE_VERSION), // This is dumb but cod devs made the format so cannot change it
                    create.String().Capture(CAPTURE_VERSION),
                }),
                create.String().Capture(CAPTURE_SHADER_NAME),
                create.Char('{'),
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            // Don't care about shader version since it's stated in the shader bin anyway

            const auto& shaderNameToken = result.NextCapture(CAPTURE_SHADER_NAME);

            const auto shaderTag = result.NextTag();
            assert(shaderTag == TAG_VERTEX_SHADER || shaderTag == TAG_PIXEL_SHADER);

            if (shaderTag == TAG_VERTEX_SHADER)
                state->m_current_shader_type = CommonTechniqueShaderType::VERTEX;
            else
                state->m_current_shader_type = CommonTechniqueShaderType::PIXEL;

            state->m_current_shader = CommonTechniqueShader(state->m_current_shader_type, shaderNameToken.StringValue());

            auto enterResult = state->m_shader_arg_creator.EnterShader(state->m_current_shader->m_type, state->m_current_shader->m_name);
            if (!enterResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), enterResult.error());
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
                    create.Char(']'),
                })),

                create.Char('='),

                create.Keyword("code"),
                create.Char('.'),
                create.Identifier().Capture(CAPTURE_STREAM_SOURCE_NAME),
                create.Optional(create.And({
                    create.Char('['),
                    create.Integer().Capture(CAPTURE_STREAM_SOURCE_INDEX),
                    create.Char(']'),
                })),

                create.Char(';'),
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
            assert(state->m_current_pass);

            const auto& firstToken = result.NextCapture(CAPTURE_FIRST_TOKEN);

            const std::string destinationString = CreateRoutingString(result, CAPTURE_STREAM_DESTINATION_NAME, CAPTURE_STREAM_DESTINATION_INDEX);
            const auto maybeDestination = state->m_routing_infos.GetDestinationByName(destinationString);

            if (!maybeDestination.has_value())
                throw ParsingException(firstToken.GetPos(), "Unknown routing destination");

            const std::string sourceString = CreateRoutingString(result, CAPTURE_STREAM_SOURCE_NAME, CAPTURE_STREAM_SOURCE_INDEX);
            const auto maybeSource = state->m_routing_infos.GetSourceByName(sourceString);

            if (!maybeSource.has_value())
                throw ParsingException(firstToken.GetPos(), "Unknown routing source");

            state->m_current_pass->m_vertex_declaration.m_routing.emplace_back(*maybeSource, *maybeDestination);
        }
    };
} // namespace techset

const std::vector<TechniqueParser::sequence_t*>& TechniquePassScopeSequences::GetSequences()
{
    static std::vector<TechniqueParser::sequence_t*> tests({
        new SequenceEndPass(),
        new SequenceStateMap(),
        new SequenceShader(),
        new SequenceVertexStreamRouting(),
    });

    return tests;
}
