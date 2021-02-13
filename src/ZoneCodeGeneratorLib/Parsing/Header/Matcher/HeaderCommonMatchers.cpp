#include "HeaderCommonMatchers.h"

#include <sstream>

#include "HeaderMatcherFactory.h"

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::Align(const supplier_t* labelSupplier)
{
    const HeaderMatcherFactory create(labelSupplier);

    return create.And({
        create.Type(HeaderParserValueType::DECLSPEC),
        create.Char('('),
        create.Type(HeaderParserValueType::ALIGN),
        create.Char('('),
        create.Integer(),
        create.Char(')'),
        create.Char(')')
    }).Transform([](HeaderMatcherFactory::token_list_t& values)
    {
        return HeaderParserValue::Integer(values[4].get().GetPos(), values[4].get().IntegerValue());
    });
}

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::ArrayDef(const supplier_t* labelSupplier)
{
    const HeaderMatcherFactory create(labelSupplier);

    return create.And({
        create.Char('['),
        create.Or({
            create.Integer(),
            create.Identifier()
        }),
        create.Char(']')
    }).Transform([](HeaderMatcherFactory::token_list_t& values)
    {
        if(values[1].get().m_type == HeaderParserValueType::INTEGER)
            return HeaderParserValue::Integer(values[1].get().GetPos(), values[1].get().IntegerValue());

        return HeaderParserValue::Identifier(values[1].get().GetPos(), new std::string(values[1].get().IdentifierValue()));
    });
}

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::Typename(const supplier_t* labelSupplier)
{
    const HeaderMatcherFactory create(labelSupplier);

    return create.And({
        create.Identifier(),
        create.OptionalLoop(create.And({
            create.Char(':'),
            create.Char(':'),
            create.Identifier()
        }))
    }).Transform([](HeaderMatcherFactory::token_list_t& values)
    {
        std::ostringstream str;
        str << values[0].get().IdentifierValue();

        for (auto i = 3u; i < values.size(); i += 3)
            str << "::" << values[i].get().IdentifierValue();

        return HeaderParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
    }).Build();
}
