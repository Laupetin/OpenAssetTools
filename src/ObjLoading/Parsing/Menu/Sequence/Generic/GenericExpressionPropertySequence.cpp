#include "GenericExpressionPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericExpressionPropertySequence::GenericExpressionPropertySequence(callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuExpressionMatchers expressionMatchers;
    AddLabeledMatchers(expressionMatchers.Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeyword(std::string keyword, callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    result->AddMatchers({create.KeywordIgnoreCase(std::move(keyword)).Capture(CAPTURE_FIRST_TOKEN),
                         create.Label(MenuExpressionMatchers::LABEL_EXPRESSION),
                         create.Optional(create.Char(';'))});

    return std::move(result);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeywords(const std::initializer_list<std::string> keywords,
                                                                                                   callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    std::vector<std::unique_ptr<matcher_t>> keywordMatchers;
    for (auto keyword : keywords)
        keywordMatchers.emplace_back(create.KeywordIgnoreCase(std::move(keyword)));

    result->AddMatchers({create.And(std::move(keywordMatchers)).Capture(CAPTURE_FIRST_TOKEN),
                         create.Label(MenuExpressionMatchers::LABEL_EXPRESSION),
                         create.Optional(create.Char(';'))});

    return std::move(result);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeywordAndBool(std::string keyword, callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    result->AddMatchers({create.KeywordIgnoreCase(std::move(keyword)).Capture(CAPTURE_FIRST_TOKEN),
                         create.Optional(create.KeywordIgnoreCase("when")),
                         create.Label(MenuExpressionMatchers::LABEL_EXPRESSION),
                         create.Optional(create.Char(';'))});

    return std::move(result);
}

void GenericExpressionPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        const MenuExpressionMatchers expressionMatchers(state);
        auto expression = expressionMatchers.ProcessExpression(result);
        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), std::move(expression));
    }
}
