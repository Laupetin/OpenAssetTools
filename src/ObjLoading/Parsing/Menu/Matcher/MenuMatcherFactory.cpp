#include "MenuMatcherFactory.h"

using namespace menu;

MenuMatcherFactory::MenuMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier)
    : SimpleMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::Text() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(Or({String(), Identifier()}));
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::Numeric() const
{
    return MatcherFactoryWrapper<SimpleParserValue>(Or({FloatingPoint(), Integer()}));
}

int MenuMatcherFactory::TokenNumericIntValue(const SimpleParserValue& value)
{
    if(value.m_type == SimpleParserValueType::FLOATING_POINT)
    {
        return static_cast<int>(value.FloatingPointValue());
    }

    return value.IntegerValue();
}

double MenuMatcherFactory::TokenNumericFloatingPointValue(const SimpleParserValue& value)
{
    if (value.m_type == SimpleParserValueType::INTEGER)
    {
        return static_cast<double>(value.IntegerValue());
    }

    return value.FloatingPointValue();
}

std::string& MenuMatcherFactory::TokenTextValue(const SimpleParserValue& value)
{
    if(value.m_type == SimpleParserValueType::IDENTIFIER)
    {
        return value.IdentifierValue();
    }

    return value.StringValue();
}
