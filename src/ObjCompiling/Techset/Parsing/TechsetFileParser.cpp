#include "TechsetFileParser.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace techset;

namespace
{
    class SequenceTechniqueTypeName final : public TechsetParser::sequence_t
    {
        static constexpr auto CAPTURE_TYPE_NAME = 1;

    public:
        SequenceTechniqueTypeName()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create.String().Capture(CAPTURE_TYPE_NAME),
                create.Char(':'),
            });
        }

    protected:
        void ProcessMatch(TechsetParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE_NAME);

            const auto maybeTechniqueTypeIndex = state->m_technique_type_names.GetTechniqueTypeByName(typeNameToken.StringValue());
            if (!maybeTechniqueTypeIndex.has_value())
                throw ParsingException(typeNameToken.GetPos(), "Unknown technique type name");

            state->m_current_technique_types.push_back(maybeTechniqueTypeIndex.value());
        }
    };

    class SequenceTechniqueName final : public TechsetParser::sequence_t
    {
        static constexpr auto CAPTURE_NAME = 1;

    public:
        SequenceTechniqueName()
        {
            const SimpleMatcherFactory create(this);

            AddMatchers({
                create
                    .Or({
                        create.Identifier(),
                        create.String(),
                    })
                    .Capture(CAPTURE_NAME),
                create.Char(';'),
            });
        }

    protected:
        void ProcessMatch(TechsetParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            assert(!state->m_current_technique_types.empty());

            const auto& techniqueNameToken = result.NextCapture(CAPTURE_NAME);
            const auto& techniqueName =
                techniqueNameToken.m_type == SimpleParserValueType::STRING ? techniqueNameToken.StringValue() : techniqueNameToken.IdentifierValue();

            for (const auto techniqueTypeIndex : state->m_current_technique_types)
                state->m_definition->m_technique_names[techniqueTypeIndex] = techniqueName;
            state->m_current_technique_types.clear();
        }
    };
} // namespace

namespace techset
{
    TechsetParser::TechsetParser(SimpleLexer& lexer, std::string techsetName, const CommonTechniqueTypeNames& techniqueTypeNames)
        : AbstractParser(&lexer, std::make_unique<TechsetParserState>(std::move(techsetName), techniqueTypeNames))
    {
    }

    const std::vector<TechsetParser::sequence_t*>& TechsetParser::GetTestsForState()
    {
        static std::vector<sequence_t*> allTests({
            new SequenceTechniqueTypeName(),
            new SequenceTechniqueName(),
        });
        static std::vector<sequence_t*> techniqueTypeNameOnlyTests({
            new SequenceTechniqueTypeName(),
        });

        return m_state->m_current_technique_types.empty() ? techniqueTypeNameOnlyTests : allTests;
    }

    std::unique_ptr<CommonTechset> TechsetParser::GetParsingResult() const
    {
        return std::move(m_state->m_definition);
    }
} // namespace techset
