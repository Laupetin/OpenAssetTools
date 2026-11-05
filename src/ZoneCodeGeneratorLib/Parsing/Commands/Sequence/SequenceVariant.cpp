#include "SequenceVariant.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_VARIANT_NAME = 1;
    static constexpr auto CAPTURE_WORD_SIZE = 2;

    static constexpr auto TAG_LITTLE_ENDIAN = 1;
    static constexpr auto TAG_BIG_ENDIAN = 2;
} // namespace

SequenceVariant::SequenceVariant()
{
    const CommandsMatcherFactory create(this);

    AddMatchers({
        create.Keyword("variant"),
        create.Identifier().Capture(CAPTURE_VARIANT_NAME),
        create.Integer().Capture(CAPTURE_WORD_SIZE),
        create.Or({
            create.Keyword("le").Tag(TAG_LITTLE_ENDIAN),
            create.Keyword("be").Tag(TAG_BIG_ENDIAN),
        }),
        create.Char(';'),
    });
}

void SequenceVariant::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    std::string variantName = result.NextCapture(CAPTURE_VARIANT_NAME).IdentifierValue();

    const auto& wordSizeToken = result.NextCapture(CAPTURE_WORD_SIZE);
    WordSize wordSize;
    switch (wordSizeToken.IntegerValue())
    {
    case 32:
        wordSize = WordSize::BITS_32;
        break;

    case 64:
        wordSize = WordSize::BITS_64;
        break;

    default:
        throw ParsingException(wordSizeToken.GetPos(), "Unknown word size");
    }

    std::endian endianness;
    switch (result.NextTag())
    {
    case TAG_LITTLE_ENDIAN:
        endianness = std::endian::little;
        break;

    case TAG_BIG_ENDIAN:
        endianness = std::endian::big;
        break;

    default:
        assert(false);
        endianness = std::endian::little;
        break;
    }

    state->AddGameVariant(std::move(variantName), wordSize, endianness);
}
