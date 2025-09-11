#include "Graph2DReader.h"

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Utils/Logging/Log.h"

#include <format>

namespace graph2d
{
    enum class SndCurveParserStatus
    {
        EXPECT_MAGIC,
        EXPECT_KNOT_COUNT,
        KNOTS
    };

    class SndCurveParserState
    {
    public:
        SndCurveParserStatus m_status;
        size_t m_expected_knot_count;
        GenericGraph2D m_result;

        SndCurveParserState()
            : m_status(SndCurveParserStatus::EXPECT_MAGIC),
              m_expected_knot_count(0u)
        {
        }
    };

    using snd_sequence_t = AbstractSequence<SimpleParserValue, SndCurveParserState>;

    class SndCurveMagicSequence final : public snd_sequence_t
    {
    public:
        explicit SndCurveMagicSequence(std::string magicWord)
        {
            const SimpleMatcherFactory create(this);
            AddMatchers({
                create.Keyword(std::move(magicWord)),
            });
        }

    protected:
        void ProcessMatch(SndCurveParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_status = SndCurveParserStatus::EXPECT_KNOT_COUNT;
        }
    };

    class SndCurveKnotCountSequence final : public snd_sequence_t
    {
        static constexpr auto CAPTURE_KNOT_COUNT = 1;

    public:
        SndCurveKnotCountSequence()
        {
            const SimpleMatcherFactory create(this);
            AddMatchers({
                create.Integer().Capture(CAPTURE_KNOT_COUNT),
            });
        }

    protected:
        void ProcessMatch(SndCurveParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_status = SndCurveParserStatus::KNOTS;

            const auto& knotCountToken = result.NextCapture(CAPTURE_KNOT_COUNT);
            if (knotCountToken.IntegerValue() < 0)
                throw ParsingException(knotCountToken.GetPos(), "Negative knot count is invalid");

            state->m_expected_knot_count = static_cast<size_t>(knotCountToken.IntegerValue());
            state->m_result.knots.reserve(state->m_expected_knot_count);
        }
    };

    class SndCurveKnotSequence final : public snd_sequence_t
    {
        static constexpr auto CAPTURE_X = 1;
        static constexpr auto CAPTURE_Y = 2;

    public:
        SndCurveKnotSequence()
        {
            const SimpleMatcherFactory create(this);
            AddMatchers({
                create
                    .Or({
                        create.FloatingPoint(),
                        create.Integer(),
                    })
                    .Capture(CAPTURE_X),

                create
                    .Or({
                        create.FloatingPoint(),
                        create.Integer(),
                    })
                    .Capture(CAPTURE_Y),
            });
        }

    private:
        static double GetValue(const SimpleParserValue& token)
        {
            if (token.m_type == SimpleParserValueType::INTEGER)
                return token.IntegerValue();

            return token.FloatingPointValue();
        }

    protected:
        void ProcessMatch(SndCurveParserState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            const auto& xToken = result.NextCapture(CAPTURE_X);
            const auto& yToken = result.NextCapture(CAPTURE_Y);

            if (state->m_result.knots.size() >= state->m_expected_knot_count)
                throw ParsingException(xToken.GetPos(), std::format("More knots than expected ({})", state->m_expected_knot_count));

            const auto xValue = GetValue(xToken);
            const auto yValue = GetValue(yToken);

            state->m_result.knots.emplace_back(xValue, yValue);
        }
    };

    class SndCurveParser final : public AbstractParser<SimpleParserValue, SndCurveParserState>
    {
    public:
        explicit SndCurveParser(ILexer<SimpleParserValue>* lexer, std::string magicWord)
            : AbstractParser(lexer, std::make_unique<SndCurveParserState>()),
              m_magic_sequence(std::move(magicWord)),
              m_magic_sequences({&m_magic_sequence})
        {
        }

    protected:
        const std::vector<sequence_t*>& GetTestsForState() override
        {
            switch (m_state->m_status)
            {
            case SndCurveParserStatus::EXPECT_MAGIC:
                return m_magic_sequences;

            case SndCurveParserStatus::EXPECT_KNOT_COUNT:
            {
                static std::vector<sequence_t*> expectKnotCountSequences{
                    new SndCurveKnotCountSequence(),
                };
                return expectKnotCountSequences;
            }

            case SndCurveParserStatus::KNOTS:
            {
                static std::vector<sequence_t*> knotsSequences{
                    new SndCurveKnotSequence(),
                };
                return knotsSequences;
            }
            }

            assert(false);
            throw std::runtime_error("Invalid parsing status");
        }

    public:
        _NODISCARD GenericGraph2D& GetResult() const
        {
            return m_state->m_result;
        }

        _NODISCARD bool HasExpectedKnotCount() const
        {
            return GetExpectedKnotCount() == GetActualKnotCount();
        }

        [[nodiscard]] size_t GetExpectedKnotCount() const
        {
            return m_state->m_expected_knot_count;
        }

        [[nodiscard]] size_t GetActualKnotCount() const
        {
            return m_state->m_result.knots.size();
        }

    private:
        SndCurveMagicSequence m_magic_sequence;
        std::vector<sequence_t*> m_magic_sequences;
    };

    std::unique_ptr<GenericGraph2D>
        Read(const std::string& graphTypeName, std::string graphMagicWord, std::istream& stream, const std::string& fileName, std::string graphName)
    {
        ParserSingleInputStream parserStream(stream, fileName);

        SimpleLexer::Config lexerConfig;
        lexerConfig.m_emit_new_line_tokens = false;
        lexerConfig.m_read_strings = false;
        lexerConfig.m_read_integer_numbers = true;
        lexerConfig.m_read_floating_point_numbers = true;

        SimpleLexer lexer(&parserStream, std::move(lexerConfig));
        SndCurveParser parser(&lexer, std::move(graphMagicWord));

        if (!parser.Parse())
        {
            con::error("Failed to parse {} \"{}\"", graphTypeName, graphName);
            return nullptr;
        }

        if (!parser.HasExpectedKnotCount())
        {
            con::error("Failed to load {} \"{}\": Actual knot count ({}) differs from expected ({})",
                       graphTypeName,
                       graphName,
                       parser.GetActualKnotCount(),
                       parser.GetExpectedKnotCount());
            return nullptr;
        }

        return std::make_unique<GenericGraph2D>(std::move(parser.GetResult()));
    }
} // namespace graph2d
