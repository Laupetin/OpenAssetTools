#include "SndCurveReader.h"

#include "Parsing/Impl/AbstractParser.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"

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
    size_t m_knot_count;
    SndCurveReader::Result m_result;

    SndCurveParserState()
        : m_status(SndCurveParserStatus::EXPECT_MAGIC),
          m_knot_count(0u)
    {
    }
};

using snd_sequence_t = AbstractSequence<SimpleParserValue, SndCurveParserState>;

class SndCurveMagicSequence final : public snd_sequence_t
{
public:
    SndCurveMagicSequence()
    {
        const SimpleMatcherFactory create(this);
        AddMatchers({
            create.Keyword("SNDCURVE"),
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

        state->m_knot_count = static_cast<size_t>(knotCountToken.IntegerValue());
        state->m_result.m_knots.reserve(state->m_knot_count);
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
            return static_cast<double>(token.IntegerValue());

        return token.FloatingPointValue();
    }

protected:
    void ProcessMatch(SndCurveParserState* state, SequenceResult<SimpleParserValue>& result) const override
    {
        const auto& xToken = result.NextCapture(CAPTURE_X);
        const auto& yToken = result.NextCapture(CAPTURE_Y);

        if (state->m_result.m_knots.size() >= state->m_knot_count)
            throw ParsingException(xToken.GetPos(), "Too many knots");

        const auto xValue = GetValue(xToken);
        const auto yValue = GetValue(yToken);

        state->m_result.m_knots.push_back(SndCurveReader::Result::Knot{xValue, yValue});
    }
};

class SndCurveParser final : public AbstractParser<SimpleParserValue, SndCurveParserState>
{
public:
    explicit SndCurveParser(ILexer<SimpleParserValue>* lexer)
        : AbstractParser<SimpleParserValue, SndCurveParserState>(lexer, std::make_unique<SndCurveParserState>())
    {
    }

protected:
    const std::vector<sequence_t*>& GetTestsForState() override
    {
        switch (m_state->m_status)
        {
        case SndCurveParserStatus::EXPECT_MAGIC:
        {
            static std::vector<sequence_t*> expectMagicSequences{
                new SndCurveMagicSequence(),
            };
            return expectMagicSequences;
        }

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
    _NODISCARD SndCurveReader::Result& GetResult() const
    {
        return m_state->m_result;
    }

    _NODISCARD bool HasExpectedKnotCount() const
    {
        return m_state->m_knot_count == m_state->m_result.m_knots.size();
    }
};

SndCurveReader::SndCurveReader(std::istream& stream, const std::string& filename)
    : m_stream(stream),
      m_filename(filename)
{
}

std::unique_ptr<SndCurveReader::Result> SndCurveReader::Read() const
{
    ParserSingleInputStream parserStream(m_stream, m_filename);

    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_strings = false;
    lexerConfig.m_read_integer_numbers = true;
    lexerConfig.m_read_floating_point_numbers = true;

    SimpleLexer lexer(&parserStream, std::move(lexerConfig));
    SndCurveParser parser(&lexer);

    if (!parser.Parse())
    {
        std::cerr << "Failed to parse SndCurve \"" << m_filename << "\"\n";
        return nullptr;
    }

    if (!parser.HasExpectedKnotCount())
    {
        std::cerr << "Failed to load SndCurve: Actual knot count differs from expected: \"" << m_filename << "\"\n";
        return nullptr;
    }

    return std::make_unique<Result>(std::move(parser.GetResult()));
}
