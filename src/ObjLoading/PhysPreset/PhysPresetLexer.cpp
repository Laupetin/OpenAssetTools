
// Generated from E:/Projects/VS/OpenAssetTools/src/ObjLoading/PhysPreset/PhysPreset.g4 by ANTLR 4.13.2

#include "PhysPresetLexer.h"

using namespace antlr4;

using namespace antlr4;

namespace
{

    struct PhysPresetLexerStaticData final
    {
        PhysPresetLexerStaticData(std::vector<std::string> ruleNames,
                                  std::vector<std::string> channelNames,
                                  std::vector<std::string> modeNames,
                                  std::vector<std::string> literalNames,
                                  std::vector<std::string> symbolicNames)
            : ruleNames(std::move(ruleNames)),
              channelNames(std::move(channelNames)),
              modeNames(std::move(modeNames)),
              literalNames(std::move(literalNames)),
              symbolicNames(std::move(symbolicNames)),
              vocabulary(this->literalNames, this->symbolicNames)
        {
        }

        PhysPresetLexerStaticData(const PhysPresetLexerStaticData&) = delete;
        PhysPresetLexerStaticData(PhysPresetLexerStaticData&&) = delete;
        PhysPresetLexerStaticData& operator=(const PhysPresetLexerStaticData&) = delete;
        PhysPresetLexerStaticData& operator=(PhysPresetLexerStaticData&&) = delete;

        std::vector<antlr4::dfa::DFA> decisionToDFA;
        antlr4::atn::PredictionContextCache sharedContextCache;
        const std::vector<std::string> ruleNames;
        const std::vector<std::string> channelNames;
        const std::vector<std::string> modeNames;
        const std::vector<std::string> literalNames;
        const std::vector<std::string> symbolicNames;
        const antlr4::dfa::Vocabulary vocabulary;
        antlr4::atn::SerializedATNView serializedATN;
        std::unique_ptr<antlr4::atn::ATN> atn;
    };

    ::antlr4::internal::OnceFlag physpresetlexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    static thread_local
#endif
        std::unique_ptr<PhysPresetLexerStaticData>
            physpresetlexerLexerStaticData = nullptr;

    void physpresetlexerLexerInitialize()
    {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
        if (physpresetlexerLexerStaticData != nullptr)
        {
            return;
        }
#else
        assert(physpresetlexerLexerStaticData == nullptr);
#endif
        auto staticData =
            std::make_unique<PhysPresetLexerStaticData>(std::vector<std::string>{"PHYSIC", "BACKSLASH", "FLOAT", "INTEGER", "IDENTIFIER", "WS", "DIGIT"},
                                                        std::vector<std::string>{"DEFAULT_TOKEN_CHANNEL", "HIDDEN"},
                                                        std::vector<std::string>{"DEFAULT_MODE"},
                                                        std::vector<std::string>{"", "'PHYSIC'", "'\\'"},
                                                        std::vector<std::string>{"", "PHYSIC", "BACKSLASH", "FLOAT", "INTEGER", "IDENTIFIER", "WS"});
        static const int32_t serializedATNSegment[] = {
            4,  0,  6,  74, 6,  -1, 2,  0,  7,  0,  2,  1,  7,  1,  2,  2,  7,  2,  2,  3,  7,   3,  2,  4,  7,  4,  2,  5,  7,  5,  2,  6,  7,   6,  1,
            0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1,  2,  3,  2,   26, 8,  2,  1,  2,  4,  2,  29, 8,  2,  11, 2,   12, 2,
            30, 1,  2,  1,  2,  5,  2,  35, 8,  2,  10, 2,  12, 2,  38, 9,  2,  1,  2,  3,  2,   41, 8,  2,  1,  2,  1,  2,  4,  2,  45, 8,  2,   11, 2,
            12, 2,  46, 3,  2,  49, 8,  2,  1,  3,  3,  3,  52, 8,  3,  1,  3,  4,  3,  55, 8,   3,  11, 3,  12, 3,  56, 1,  4,  1,  4,  5,  4,   61, 8,
            4,  10, 4,  12, 4,  64, 9,  4,  1,  5,  4,  5,  67, 8,  5,  11, 5,  12, 5,  68, 1,   5,  1,  5,  1,  6,  1,  6,  0,  0,  7,  1,  1,   3,  2,
            5,  3,  7,  4,  9,  5,  11, 6,  13, 0,  1,  0,  4,  3,  0,  65, 90, 95, 95, 97, 122, 5,  0,  46, 46, 48, 57, 65, 90, 95, 95, 97, 122, 3,  0,
            9,  10, 13, 13, 32, 32, 1,  0,  48, 57, 82, 0,  1,  1,  0,  0,  0,  0,  3,  1,  0,   0,  0,  0,  5,  1,  0,  0,  0,  0,  7,  1,  0,   0,  0,
            0,  9,  1,  0,  0,  0,  0,  11, 1,  0,  0,  0,  1,  15, 1,  0,  0,  0,  3,  22, 1,   0,  0,  0,  5,  48, 1,  0,  0,  0,  7,  51, 1,   0,  0,
            0,  9,  58, 1,  0,  0,  0,  11, 66, 1,  0,  0,  0,  13, 72, 1,  0,  0,  0,  15, 16,  5,  80, 0,  0,  16, 17, 5,  72, 0,  0,  17, 18,  5,  89,
            0,  0,  18, 19, 5,  83, 0,  0,  19, 20, 5,  73, 0,  0,  20, 21, 5,  67, 0,  0,  21,  2,  1,  0,  0,  0,  22, 23, 5,  92, 0,  0,  23,  4,  1,
            0,  0,  0,  24, 26, 5,  45, 0,  0,  25, 24, 1,  0,  0,  0,  25, 26, 1,  0,  0,  0,   26, 28, 1,  0,  0,  0,  27, 29, 3,  13, 6,  0,   28, 27,
            1,  0,  0,  0,  29, 30, 1,  0,  0,  0,  30, 28, 1,  0,  0,  0,  30, 31, 1,  0,  0,   0,  31, 32, 1,  0,  0,  0,  32, 36, 5,  46, 0,   0,  33,
            35, 3,  13, 6,  0,  34, 33, 1,  0,  0,  0,  35, 38, 1,  0,  0,  0,  36, 34, 1,  0,   0,  0,  36, 37, 1,  0,  0,  0,  37, 49, 1,  0,   0,  0,
            38, 36, 1,  0,  0,  0,  39, 41, 5,  45, 0,  0,  40, 39, 1,  0,  0,  0,  40, 41, 1,   0,  0,  0,  41, 42, 1,  0,  0,  0,  42, 44, 5,   46, 0,
            0,  43, 45, 3,  13, 6,  0,  44, 43, 1,  0,  0,  0,  45, 46, 1,  0,  0,  0,  46, 44,  1,  0,  0,  0,  46, 47, 1,  0,  0,  0,  47, 49,  1,  0,
            0,  0,  48, 25, 1,  0,  0,  0,  48, 40, 1,  0,  0,  0,  49, 6,  1,  0,  0,  0,  50,  52, 5,  45, 0,  0,  51, 50, 1,  0,  0,  0,  51,  52, 1,
            0,  0,  0,  52, 54, 1,  0,  0,  0,  53, 55, 3,  13, 6,  0,  54, 53, 1,  0,  0,  0,   55, 56, 1,  0,  0,  0,  56, 54, 1,  0,  0,  0,   56, 57,
            1,  0,  0,  0,  57, 8,  1,  0,  0,  0,  58, 62, 7,  0,  0,  0,  59, 61, 7,  1,  0,   0,  60, 59, 1,  0,  0,  0,  61, 64, 1,  0,  0,   0,  62,
            60, 1,  0,  0,  0,  62, 63, 1,  0,  0,  0,  63, 10, 1,  0,  0,  0,  64, 62, 1,  0,   0,  0,  65, 67, 7,  2,  0,  0,  66, 65, 1,  0,   0,  0,
            67, 68, 1,  0,  0,  0,  68, 66, 1,  0,  0,  0,  68, 69, 1,  0,  0,  0,  69, 70, 1,   0,  0,  0,  70, 71, 6,  5,  0,  0,  71, 12, 1,   0,  0,
            0,  72, 73, 7,  3,  0,  0,  73, 14, 1,  0,  0,  0,  11, 0,  25, 30, 36, 40, 46, 48,  51, 56, 62, 68, 1,  6,  0,  0};
        staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

        antlr4::atn::ATNDeserializer deserializer;
        staticData->atn = deserializer.deserialize(staticData->serializedATN);

        const size_t count = staticData->atn->getNumberOfDecisions();
        staticData->decisionToDFA.reserve(count);
        for (size_t i = 0; i < count; i++)
        {
            staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
        }
        physpresetlexerLexerStaticData = std::move(staticData);
    }

} // namespace

PhysPresetLexer::PhysPresetLexer(CharStream* input)
    : Lexer(input)
{
    PhysPresetLexer::initialize();
    _interpreter = new atn::LexerATNSimulator(
        this, *physpresetlexerLexerStaticData->atn, physpresetlexerLexerStaticData->decisionToDFA, physpresetlexerLexerStaticData->sharedContextCache);
}

PhysPresetLexer::~PhysPresetLexer()
{
    delete _interpreter;
}

std::string PhysPresetLexer::getGrammarFileName() const
{
    return "PhysPreset.g4";
}

const std::vector<std::string>& PhysPresetLexer::getRuleNames() const
{
    return physpresetlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& PhysPresetLexer::getChannelNames() const
{
    return physpresetlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& PhysPresetLexer::getModeNames() const
{
    return physpresetlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& PhysPresetLexer::getVocabulary() const
{
    return physpresetlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView PhysPresetLexer::getSerializedATN() const
{
    return physpresetlexerLexerStaticData->serializedATN;
}

const atn::ATN& PhysPresetLexer::getATN() const
{
    return *physpresetlexerLexerStaticData->atn;
}

void PhysPresetLexer::initialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    physpresetlexerLexerInitialize();
#else
    ::antlr4::internal::call_once(physpresetlexerLexerOnceFlag, physpresetlexerLexerInitialize);
#endif
}
