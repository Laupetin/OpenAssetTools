
// Generated from E:/Projects/VS/OpenAssetTools/src/ObjLoading/PhysPreset/PhysPreset.g4 by ANTLR 4.13.2

#include "PhysPresetParser.h"

#include "PhysPresetListener.h"

using namespace antlrcpp;

using namespace antlr4;

namespace
{

    struct PhysPresetParserStaticData final
    {
        PhysPresetParserStaticData(std::vector<std::string> ruleNames, std::vector<std::string> literalNames, std::vector<std::string> symbolicNames)
            : ruleNames(std::move(ruleNames)),
              literalNames(std::move(literalNames)),
              symbolicNames(std::move(symbolicNames)),
              vocabulary(this->literalNames, this->symbolicNames)
        {
        }

        PhysPresetParserStaticData(const PhysPresetParserStaticData&) = delete;
        PhysPresetParserStaticData(PhysPresetParserStaticData&&) = delete;
        PhysPresetParserStaticData& operator=(const PhysPresetParserStaticData&) = delete;
        PhysPresetParserStaticData& operator=(PhysPresetParserStaticData&&) = delete;

        std::vector<antlr4::dfa::DFA> decisionToDFA;
        antlr4::atn::PredictionContextCache sharedContextCache;
        const std::vector<std::string> ruleNames;
        const std::vector<std::string> literalNames;
        const std::vector<std::string> symbolicNames;
        const antlr4::dfa::Vocabulary vocabulary;
        antlr4::atn::SerializedATNView serializedATN;
        std::unique_ptr<antlr4::atn::ATN> atn;
    };

    ::antlr4::internal::OnceFlag physpresetParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    static thread_local
#endif
        std::unique_ptr<PhysPresetParserStaticData>
            physpresetParserStaticData = nullptr;

    void physpresetParserInitialize()
    {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
        if (physpresetParserStaticData != nullptr)
        {
            return;
        }
#else
        assert(physpresetParserStaticData == nullptr);
#endif
        auto staticData =
            std::make_unique<PhysPresetParserStaticData>(std::vector<std::string>{"physPreset", "pair", "key", "value"},
                                                         std::vector<std::string>{"", "'PHYSIC'", "'\\'"},
                                                         std::vector<std::string>{"", "PHYSIC", "BACKSLASH", "FLOAT", "INTEGER", "IDENTIFIER", "WS"});
        static const int32_t serializedATNSegment[] = {
            4, 1,  6,  27, 2, 0, 7, 0, 2,  1,  7, 1, 2, 2, 7,  2,  2, 3, 7, 3, 1,  0,  1, 0, 5, 0, 11, 8,  0, 10, 0, 12, 0,  14, 9, 0,
            1, 0,  1,  0,  1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1,  2,  1, 2, 1, 3, 1,  3,  1, 3, 0, 0, 4,  0,  2, 4,  6, 0,  1,  1,  0, 3,
            5, 23, 0,  8,  1, 0, 0, 0, 2,  17, 1, 0, 0, 0, 4,  22, 1, 0, 0, 0, 6,  24, 1, 0, 0, 0, 8,  12, 5, 1,  0, 0,  9,  11, 3, 2,
            1, 0,  10, 9,  1, 0, 0, 0, 11, 14, 1, 0, 0, 0, 12, 10, 1, 0, 0, 0, 12, 13, 1, 0, 0, 0, 13, 15, 1, 0,  0, 0,  14, 12, 1, 0,
            0, 0,  15, 16, 5, 0, 0, 1, 16, 1,  1, 0, 0, 0, 17, 18, 5, 2, 0, 0, 18, 19, 3, 4, 2, 0, 19, 20, 5, 2,  0, 0,  20, 21, 3, 6,
            3, 0,  21, 3,  1, 0, 0, 0, 22, 23, 5, 5, 0, 0, 23, 5,  1, 0, 0, 0, 24, 25, 7, 0, 0, 0, 25, 7,  1, 0,  0, 0,  1,  12};
        staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

        antlr4::atn::ATNDeserializer deserializer;
        staticData->atn = deserializer.deserialize(staticData->serializedATN);

        const size_t count = staticData->atn->getNumberOfDecisions();
        staticData->decisionToDFA.reserve(count);
        for (size_t i = 0; i < count; i++)
        {
            staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
        }
        physpresetParserStaticData = std::move(staticData);
    }

} // namespace

PhysPresetParser::PhysPresetParser(TokenStream* input)
    : PhysPresetParser(input, antlr4::atn::ParserATNSimulatorOptions())
{
}

PhysPresetParser::PhysPresetParser(TokenStream* input, const antlr4::atn::ParserATNSimulatorOptions& options)
    : Parser(input)
{
    PhysPresetParser::initialize();
    _interpreter = new atn::ParserATNSimulator(
        this, *physpresetParserStaticData->atn, physpresetParserStaticData->decisionToDFA, physpresetParserStaticData->sharedContextCache, options);
}

PhysPresetParser::~PhysPresetParser()
{
    delete _interpreter;
}

const atn::ATN& PhysPresetParser::getATN() const
{
    return *physpresetParserStaticData->atn;
}

std::string PhysPresetParser::getGrammarFileName() const
{
    return "PhysPreset.g4";
}

const std::vector<std::string>& PhysPresetParser::getRuleNames() const
{
    return physpresetParserStaticData->ruleNames;
}

const dfa::Vocabulary& PhysPresetParser::getVocabulary() const
{
    return physpresetParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView PhysPresetParser::getSerializedATN() const
{
    return physpresetParserStaticData->serializedATN;
}

//----------------- PhysPresetContext ------------------------------------------------------------------

PhysPresetParser::PhysPresetContext::PhysPresetContext(ParserRuleContext* parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState)
{
}

tree::TerminalNode* PhysPresetParser::PhysPresetContext::PHYSIC()
{
    return getToken(PhysPresetParser::PHYSIC, 0);
}

tree::TerminalNode* PhysPresetParser::PhysPresetContext::EOF()
{
    return getToken(PhysPresetParser::EOF, 0);
}

std::vector<PhysPresetParser::PairContext*> PhysPresetParser::PhysPresetContext::pair()
{
    return getRuleContexts<PhysPresetParser::PairContext>();
}

PhysPresetParser::PairContext* PhysPresetParser::PhysPresetContext::pair(size_t i)
{
    return getRuleContext<PhysPresetParser::PairContext>(i);
}

size_t PhysPresetParser::PhysPresetContext::getRuleIndex() const
{
    return PhysPresetParser::RulePhysPreset;
}

void PhysPresetParser::PhysPresetContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->enterPhysPreset(this);
}

void PhysPresetParser::PhysPresetContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->exitPhysPreset(this);
}

PhysPresetParser::PhysPresetContext* PhysPresetParser::physPreset()
{
    PhysPresetContext* _localctx = _tracker.createInstance<PhysPresetContext>(_ctx, getState());
    enterRule(_localctx, 0, PhysPresetParser::RulePhysPreset);
    size_t _la = 0;

#if __cplusplus > 201703L
    auto onExit = finally(
        [=, this]
        {
#else
    auto onExit = finally(
        [=]
        {
#endif
            exitRule();
        });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(8);
        match(PhysPresetParser::PHYSIC);
        setState(12);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == PhysPresetParser::BACKSLASH)
        {
            setState(9);
            pair();
            setState(14);
            _errHandler->sync(this);
            _la = _input->LA(1);
        }
        setState(15);
        match(PhysPresetParser::EOF);
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

//----------------- PairContext ------------------------------------------------------------------

PhysPresetParser::PairContext::PairContext(ParserRuleContext* parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState)
{
}

std::vector<tree::TerminalNode*> PhysPresetParser::PairContext::BACKSLASH()
{
    return getTokens(PhysPresetParser::BACKSLASH);
}

tree::TerminalNode* PhysPresetParser::PairContext::BACKSLASH(size_t i)
{
    return getToken(PhysPresetParser::BACKSLASH, i);
}

PhysPresetParser::KeyContext* PhysPresetParser::PairContext::key()
{
    return getRuleContext<PhysPresetParser::KeyContext>(0);
}

PhysPresetParser::ValueContext* PhysPresetParser::PairContext::value()
{
    return getRuleContext<PhysPresetParser::ValueContext>(0);
}

size_t PhysPresetParser::PairContext::getRuleIndex() const
{
    return PhysPresetParser::RulePair;
}

void PhysPresetParser::PairContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->enterPair(this);
}

void PhysPresetParser::PairContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->exitPair(this);
}

PhysPresetParser::PairContext* PhysPresetParser::pair()
{
    PairContext* _localctx = _tracker.createInstance<PairContext>(_ctx, getState());
    enterRule(_localctx, 2, PhysPresetParser::RulePair);

#if __cplusplus > 201703L
    auto onExit = finally(
        [=, this]
        {
#else
    auto onExit = finally(
        [=]
        {
#endif
            exitRule();
        });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(17);
        match(PhysPresetParser::BACKSLASH);
        setState(18);
        key();
        setState(19);
        match(PhysPresetParser::BACKSLASH);
        setState(20);
        value();
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

//----------------- KeyContext ------------------------------------------------------------------

PhysPresetParser::KeyContext::KeyContext(ParserRuleContext* parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState)
{
}

tree::TerminalNode* PhysPresetParser::KeyContext::IDENTIFIER()
{
    return getToken(PhysPresetParser::IDENTIFIER, 0);
}

size_t PhysPresetParser::KeyContext::getRuleIndex() const
{
    return PhysPresetParser::RuleKey;
}

void PhysPresetParser::KeyContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->enterKey(this);
}

void PhysPresetParser::KeyContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->exitKey(this);
}

PhysPresetParser::KeyContext* PhysPresetParser::key()
{
    KeyContext* _localctx = _tracker.createInstance<KeyContext>(_ctx, getState());
    enterRule(_localctx, 4, PhysPresetParser::RuleKey);

#if __cplusplus > 201703L
    auto onExit = finally(
        [=, this]
        {
#else
    auto onExit = finally(
        [=]
        {
#endif
            exitRule();
        });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(22);
        match(PhysPresetParser::IDENTIFIER);
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

PhysPresetParser::ValueContext::ValueContext(ParserRuleContext* parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState)
{
}

tree::TerminalNode* PhysPresetParser::ValueContext::FLOAT()
{
    return getToken(PhysPresetParser::FLOAT, 0);
}

tree::TerminalNode* PhysPresetParser::ValueContext::INTEGER()
{
    return getToken(PhysPresetParser::INTEGER, 0);
}

tree::TerminalNode* PhysPresetParser::ValueContext::IDENTIFIER()
{
    return getToken(PhysPresetParser::IDENTIFIER, 0);
}

size_t PhysPresetParser::ValueContext::getRuleIndex() const
{
    return PhysPresetParser::RuleValue;
}

void PhysPresetParser::ValueContext::enterRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->enterValue(this);
}

void PhysPresetParser::ValueContext::exitRule(tree::ParseTreeListener* listener)
{
    auto parserListener = dynamic_cast<PhysPresetListener*>(listener);
    if (parserListener != nullptr)
        parserListener->exitValue(this);
}

PhysPresetParser::ValueContext* PhysPresetParser::value()
{
    ValueContext* _localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
    enterRule(_localctx, 6, PhysPresetParser::RuleValue);
    size_t _la = 0;

#if __cplusplus > 201703L
    auto onExit = finally(
        [=, this]
        {
#else
    auto onExit = finally(
        [=]
        {
#endif
            exitRule();
        });
    try
    {
        enterOuterAlt(_localctx, 1);
        setState(24);
        _la = _input->LA(1);
        if (!((((_la & ~0x3fULL) == 0) && ((1ULL << _la) & 56) != 0)))
        {
            _errHandler->recoverInline(this);
        }
        else
        {
            _errHandler->reportMatch(this);
            consume();
        }
    }
    catch (RecognitionException& e)
    {
        _errHandler->reportError(this, e);
        _localctx->exception = std::current_exception();
        _errHandler->recover(this, _localctx->exception);
    }

    return _localctx;
}

void PhysPresetParser::initialize()
{
#if ANTLR4_USE_THREAD_LOCAL_CACHE
    physpresetParserInitialize();
#else
    ::antlr4::internal::call_once(physpresetParserOnceFlag, physpresetParserInitialize);
#endif
}
