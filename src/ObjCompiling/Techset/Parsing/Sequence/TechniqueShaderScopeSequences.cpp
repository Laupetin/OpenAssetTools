#include "TechniqueShaderScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

#include <cassert>

using namespace techset;

namespace techset
{
    class SequenceEndShader final : public TechniqueParser::sequence_t
    {
    public:
        SequenceEndShader()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Char('}'),
            });
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_shader == true);
            state->m_in_shader = false;
        }
    };

    class SequenceShaderArgument final : public TechniqueParser::sequence_t
    {
        static constexpr auto TAG_CONSTANT = 1;
        static constexpr auto TAG_SAMPLER = 2;
        static constexpr auto TAG_LITERAL = 3;
        static constexpr auto TAG_MATERIAL = 4;

        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_SHADER_ARGUMENT = 2;
        static constexpr auto CAPTURE_SHADER_INDEX = 3;
        static constexpr auto CAPTURE_CODE_ACCESSOR = 4;
        static constexpr auto CAPTURE_CODE_INDEX = 5;
        static constexpr auto CAPTURE_LITERAL_VALUE = 6;
        static constexpr auto CAPTURE_MATERIAL_HASH = 7;
        static constexpr auto CAPTURE_MATERIAL_NAME = 8;

        static std::unique_ptr<matcher_t> CodeMatchers(const SimpleMatcherFactory& create)
        {
            return create.And({
                create.Or({
                    create.Keyword("constant").Tag(TAG_CONSTANT),
                    create.Keyword("sampler").Tag(TAG_SAMPLER),
                }),
                create.Char('.'),
                create.Identifier().Capture(CAPTURE_CODE_ACCESSOR),
                create.OptionalLoop(create.And({
                    create.Char('.'),
                    create.Identifier().Capture(CAPTURE_CODE_ACCESSOR),
                })),
                create.Optional(create.And({
                    create.Char('['),
                    create.Integer().Capture(CAPTURE_CODE_INDEX),
                    create.Char(']'),
                })),
            });
        }

        static std::unique_ptr<matcher_t> LiteralValueMatchers(const SimpleMatcherFactory& create)
        {
            return create
                .Or({
                    create.FloatingPoint(),
                    create.Integer(),
                })
                .Capture(CAPTURE_LITERAL_VALUE);
        }

        static std::unique_ptr<matcher_t> LiteralMatchers(const SimpleMatcherFactory& create)
        {
            return create
                .And({
                    create.Keyword("float4"),
                    create.Char('('),
                    LiteralValueMatchers(create),
                    create.Char(','),
                    LiteralValueMatchers(create),
                    create.Char(','),
                    LiteralValueMatchers(create),
                    create.Char(','),
                    LiteralValueMatchers(create),
                    create.Char(')'),
                })
                .Tag(TAG_LITERAL);
        }

        static std::unique_ptr<matcher_t> MaterialMatchers(const SimpleMatcherFactory& create)
        {
            return create
                .And({
                    create.Keyword("material"),
                    create.Char('.'),

                    create.Or({
                        create.And({
                            create.Char('#'),
                            create.Integer().Capture(CAPTURE_MATERIAL_HASH),
                        }),
                        create.Identifier().Capture(CAPTURE_MATERIAL_NAME),
                    }),
                })
                .Tag(TAG_MATERIAL);
        }

    public:
        SequenceShaderArgument()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.Identifier().Capture(CAPTURE_FIRST_TOKEN).NoConsume(),
                create.Identifier().Capture(CAPTURE_SHADER_ARGUMENT),
                create.Optional(create.And({
                    create.Char('['),
                    create.Integer().Capture(CAPTURE_SHADER_INDEX),
                    create.Char(']'),
                })),
                create.Char('='),
                create.Or({
                    CodeMatchers(create),
                    LiteralMatchers(create),
                    MaterialMatchers(create),
                }),
                create.Char(';'),
            });
        }

        static void ProcessCodeArgument(const TechniqueParserState* state, SequenceResult<SimpleParserValue>& result, ShaderArgument arg, const bool isSampler)
        {
            std::vector<std::string> accessors;
            while (result.HasNextCapture(CAPTURE_CODE_ACCESSOR))
                accessors.emplace_back(result.NextCapture(CAPTURE_CODE_ACCESSOR).IdentifierValue());

            ShaderArgumentCodeSource source;
            if (result.HasNextCapture(CAPTURE_CODE_INDEX))
            {
                const auto& codeIndexToken = result.NextCapture(CAPTURE_CODE_INDEX);
                if (codeIndexToken.IntegerValue() < 0)
                    throw ParsingException(codeIndexToken.GetPos(), "Index cannot be negative");
                source = ShaderArgumentCodeSource(std::move(accessors), static_cast<size_t>(codeIndexToken.IntegerValue()));
            }
            else
                source = ShaderArgumentCodeSource(std::move(accessors));

            std::string errorMessage;
            if (!isSampler)
            {
                if (!state->m_acceptor->AcceptShaderConstantArgument(state->m_current_shader, std::move(arg), std::move(source), errorMessage))
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(errorMessage));
            }
            else
            {
                if (!state->m_acceptor->AcceptShaderSamplerArgument(state->m_current_shader, std::move(arg), std::move(source), errorMessage))
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(errorMessage));
            }
        }

        static void ProcessLiteralArgument(const TechniqueParserState* state, SequenceResult<SimpleParserValue>& result, ShaderArgument arg)
        {
            float value[4];
            for (float& i : value)
            {
                const auto& literalValueToken = result.NextCapture(CAPTURE_LITERAL_VALUE);

                if (literalValueToken.m_type == SimpleParserValueType::FLOATING_POINT)
                    i = static_cast<float>(literalValueToken.FloatingPointValue());
                else
                    i = static_cast<float>(literalValueToken.IntegerValue());
            }

            const ShaderArgumentLiteralSource source(value);
            std::string errorMessage;
            if (!state->m_acceptor->AcceptShaderLiteralArgument(state->m_current_shader, std::move(arg), source, errorMessage))
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(errorMessage));
        }

        static void ProcessMaterialArgument(const TechniqueParserState* state, SequenceResult<SimpleParserValue>& result, ShaderArgument arg)
        {
            std::string errorMessage;
            if (result.HasNextCapture(CAPTURE_MATERIAL_HASH))
            {
                ShaderArgumentMaterialSource source(static_cast<size_t>(result.NextCapture(CAPTURE_MATERIAL_HASH).IntegerValue()));
                if (!state->m_acceptor->AcceptShaderMaterialArgument(state->m_current_shader, std::move(arg), std::move(source), errorMessage))
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(errorMessage));
            }
            else
            {
                ShaderArgumentMaterialSource source(result.NextCapture(CAPTURE_MATERIAL_NAME).IdentifierValue());
                if (!state->m_acceptor->AcceptShaderMaterialArgument(state->m_current_shader, std::move(arg), std::move(source), errorMessage))
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(errorMessage));
            }
        }

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_in_shader == true);

            const auto& shaderArgumentNameToken = result.NextCapture(CAPTURE_SHADER_ARGUMENT);

            ShaderArgument arg;
            if (result.HasNextCapture(CAPTURE_SHADER_INDEX))
            {
                const auto& shaderArgumentIndexToken = result.NextCapture(CAPTURE_SHADER_INDEX);
                if (shaderArgumentIndexToken.IntegerValue() < 0)
                    throw ParsingException(shaderArgumentIndexToken.GetPos(), "Index cannot be negative");
                const auto index = static_cast<unsigned>(shaderArgumentIndexToken.IntegerValue());
                arg = ShaderArgument(shaderArgumentNameToken.IdentifierValue(), index);
            }
            else
                arg = ShaderArgument(shaderArgumentNameToken.IdentifierValue());

            const auto typeTag = result.NextTag();
            assert(typeTag == TAG_CONSTANT || typeTag == TAG_SAMPLER || typeTag == TAG_LITERAL || typeTag == TAG_MATERIAL);
            if (typeTag == TAG_CONSTANT || typeTag == TAG_SAMPLER)
                ProcessCodeArgument(state, result, std::move(arg), typeTag == TAG_SAMPLER);
            else if (typeTag == TAG_LITERAL)
                ProcessLiteralArgument(state, result, std::move(arg));
            else
                ProcessMaterialArgument(state, result, std::move(arg));
        }
    };
} // namespace techset

const std::vector<TechniqueParser::sequence_t*>& TechniqueShaderScopeSequences::GetSequences()
{
    static std::vector<TechniqueParser::sequence_t*> tests({
        new SequenceEndShader(),
        new SequenceShaderArgument(),
    });

    return tests;
}
