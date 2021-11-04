#include "GenericExpressionPropertySequence.h"

#include <utility>

#include "Parsing/Menu/Matcher/MenuCommonMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

GenericExpressionPropertySequence::GenericExpressionPropertySequence(callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    AddLabeledMatchers(MenuCommonMatchers::Expression(this), MenuCommonMatchers::LABEL_EXPRESSION);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeyword(std::string keyword, callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    result->AddMatchers({
        create.KeywordIgnoreCase(std::move(keyword)),
        create.Label(MenuCommonMatchers::LABEL_EXPRESSION),
        create.Optional(create.Char(';'))
    });

    return std::move(result);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeywords(const std::initializer_list<std::string> keywords, callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    std::vector<std::unique_ptr<matcher_t>> keywordMatchers;
    for (auto keyword : keywords)
        keywordMatchers.emplace_back(create.KeywordIgnoreCase(std::move(keyword)));

    result->AddMatchers({
        create.And(std::move(keywordMatchers)),
        create.Label(MenuCommonMatchers::LABEL_EXPRESSION),
        create.Optional(create.Char(';'))
    });

    return std::move(result);
}

std::unique_ptr<GenericExpressionPropertySequence> GenericExpressionPropertySequence::WithKeywordAndBool(std::string keyword, callback_t setCallback)
{
    auto result = std::unique_ptr<GenericExpressionPropertySequence>(new GenericExpressionPropertySequence(std::move(setCallback)));

    const MenuMatcherFactory create(result.get());
    result->AddMatchers({
        create.KeywordIgnoreCase(std::move(keyword)),
        create.Or({
            create.And({
                create.KeywordIgnoreCase("when"),
                create.Char('('),
                create.Label(MenuCommonMatchers::LABEL_EXPRESSION).Capture(CAPTURE_VALUE),
                create.Char(')')
            }),
            create.Label(MenuCommonMatchers::LABEL_EXPRESSION)
        }),
        create.Optional(create.Char(';'))
    });

    return std::move(result);
}

void GenericExpressionPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        auto expression = MenuCommonMatchers::ProcessExpression(state, result);
        m_set_callback(state, std::move(expression));
    }
}
