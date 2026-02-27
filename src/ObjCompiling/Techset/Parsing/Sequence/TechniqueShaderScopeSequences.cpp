#include "TechniqueShaderScopeSequences.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Techset/CommonShaderArgCreator.h"

#include <cassert>

using namespace techset;

namespace techset
{
    class SequenceEndShader final : public TechniqueParser::sequence_t
    {
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

    public:
        SequenceEndShader()
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
            assert(state->m_current_shader);

            if (state->m_current_shader_type == CommonTechniqueShaderType::VERTEX)
                state->m_current_pass->m_vertex_shader = std::move(*state->m_current_shader);
            else
                state->m_current_pass->m_pixel_shader = std::move(*state->m_current_shader);

            state->m_current_shader = std::nullopt;

            auto leaveShaderResult = state->m_shader_arg_creator.LeaveShader();
            if (!leaveShaderResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), leaveShaderResult.error());
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

    protected:
        void ProcessMatch(TechniqueParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(state->m_current_shader);

            const auto& shaderArgumentNameToken = result.NextCapture(CAPTURE_SHADER_ARGUMENT);

            CommonShaderArgCreatorDestination destination;
            if (result.HasNextCapture(CAPTURE_SHADER_INDEX))
            {
                const auto& shaderArgumentIndexToken = result.NextCapture(CAPTURE_SHADER_INDEX);
                if (shaderArgumentIndexToken.IntegerValue() < 0)
                    throw ParsingException(shaderArgumentIndexToken.GetPos(), "Index cannot be negative");
                const auto index = static_cast<unsigned>(shaderArgumentIndexToken.IntegerValue());
                destination = CommonShaderArgCreatorDestination(shaderArgumentNameToken.IdentifierValue(), index);
            }
            else
                destination = CommonShaderArgCreatorDestination(shaderArgumentNameToken.IdentifierValue());

            const auto typeTag = result.NextTag();
            assert(typeTag == TAG_CONSTANT || typeTag == TAG_SAMPLER || typeTag == TAG_LITERAL || typeTag == TAG_MATERIAL);

            if (typeTag == TAG_CONSTANT || typeTag == TAG_SAMPLER)
            {
                ProcessCodeArgument(state, result, destination, typeTag == TAG_SAMPLER);
            }
            else if (typeTag == TAG_LITERAL)
            {
                ProcessLiteralArgument(state, result, destination);
            }
            else
            {
                ProcessMaterialArgument(state, result, destination);
            }
        }

    private:
        static void ProcessCodeArgument(const TechniqueParserState* state,
                                        SequenceResult<SimpleParserValue>& result,
                                        const CommonShaderArgCreatorDestination& destination,
                                        const bool isSampler)
        {
            const auto accessor = GetAccessorValue(result);

            union
            {
                CommonCodeConstSource constSource;
                CommonCodeSamplerSource samplerSource;
            };

            unsigned sourceIndex = 0u;

            if (isSampler)
            {
                const auto maybeSamplerSource = state->m_code_source_infos.GetCodeSamplerSourceForAccessor(accessor);
                if (!maybeSamplerSource.has_value())
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Unknown code sampler");

                samplerSource = *maybeSamplerSource;
            }
            else
            {
                const auto maybeConstSource = state->m_code_source_infos.GetCodeConstSourceForAccessor(accessor);
                if (!maybeConstSource.has_value())
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Unknown code constant");

                constSource = *maybeConstSource;
            }

            if (result.HasNextCapture(CAPTURE_CODE_INDEX))
            {
                if (isSampler)
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Code sampler is not an array");

                const auto& codeIndexToken = result.NextCapture(CAPTURE_CODE_INDEX);
                const auto indexIntValue = codeIndexToken.IntegerValue();
                if (indexIntValue < 0)
                    throw ParsingException(codeIndexToken.GetPos(), "Index cannot be negative");

                sourceIndex = static_cast<unsigned>(indexIntValue);

                size_t codeArraySize = state->m_code_source_infos.GetInfoForCodeConstSource(constSource)->arrayCount;

                if (codeArraySize == 0)
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Code constant is not an array");

                if (codeArraySize <= sourceIndex)
                {
                    throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(),
                                           std::format("Array overflow: Code array has size {}", codeArraySize));
                }
            }

            result::Expected<NoResult, std::string> shaderCreatorResult(NoResult{});
            if (isSampler)
                shaderCreatorResult = state->m_shader_arg_creator.AcceptShaderSamplerArgument(destination, samplerSource);
            else
                shaderCreatorResult = state->m_shader_arg_creator.AcceptShaderConstantArgument(destination, constSource, sourceIndex);

            if (!shaderCreatorResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(shaderCreatorResult.error()));
        }

        static std::string GetAccessorValue(SequenceResult<SimpleParserValue>& result)
        {
            std::ostringstream accessorStream;

            accessorStream << result.NextCapture(CAPTURE_CODE_ACCESSOR).IdentifierValue();
            while (result.HasNextCapture(CAPTURE_CODE_ACCESSOR))
                accessorStream << '.' << result.NextCapture(CAPTURE_CODE_ACCESSOR).IdentifierValue();

            return accessorStream.str();
        }

        static void ProcessLiteralArgument(const TechniqueParserState* state,
                                           SequenceResult<SimpleParserValue>& result,
                                           const CommonShaderArgCreatorDestination& destination)
        {
            std::array<float, 4> argValue;
            for (float& i : argValue)
            {
                const auto& literalValueToken = result.NextCapture(CAPTURE_LITERAL_VALUE);

                if (literalValueToken.m_type == SimpleParserValueType::FLOATING_POINT)
                    i = static_cast<float>(literalValueToken.FloatingPointValue());
                else
                    i = static_cast<float>(literalValueToken.IntegerValue());
            }

            auto shaderCreatorResult = state->m_shader_arg_creator.AcceptShaderLiteralArgument(destination, argValue);

            if (!shaderCreatorResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(shaderCreatorResult.error()));
        }

        static void ProcessMaterialArgument(const TechniqueParserState* state,
                                            SequenceResult<SimpleParserValue>& result,
                                            const CommonShaderArgCreatorDestination& destination)
        {
            result::Expected<NoResult, std::string> shaderCreatorResult(NoResult{});
            if (result.HasNextCapture(CAPTURE_MATERIAL_HASH))
            {
                shaderCreatorResult = state->m_shader_arg_creator.AcceptShaderMaterialArgument(
                    destination, static_cast<unsigned>(result.NextCapture(CAPTURE_MATERIAL_HASH).IntegerValue()));
            }
            else
            {
                const auto stringValue = result.NextCapture(CAPTURE_MATERIAL_NAME).IdentifierValue();
                shaderCreatorResult = state->m_shader_arg_creator.AcceptShaderMaterialArgument(destination, stringValue);
            }

            if (!shaderCreatorResult.has_value())
                throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(shaderCreatorResult.error()));
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
