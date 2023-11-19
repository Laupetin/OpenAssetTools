#include "HeaderCommonMatchers.h"

#include "HeaderMatcherFactory.h"

#include <sstream>
#include <type_traits>

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::Align(const supplier_t* labelSupplier)
{
    const HeaderMatcherFactory create(labelSupplier);

    return create.Or({create
                          .And({create.Type(HeaderParserValueType::DECLSPEC),
                                create.Char('('),
                                create.Type(HeaderParserValueType::ALIGN),
                                create.Char('('),
                                create.Integer(),
                                create.Char(')'),
                                create.Char(')')})
                          .Transform(
                              [](HeaderMatcherFactory::token_list_t& values)
                              {
                                  return HeaderParserValue::Integer(values[4].get().GetPos(), values[4].get().IntegerValue());
                              }),
                      create.And({create.Type(HeaderParserValueType::ALIGNAS), create.Char('('), create.Integer(), create.Char(')')})
                          .Transform(
                              [](HeaderMatcherFactory::token_list_t& values)
                              {
                                  return HeaderParserValue::Integer(values[2].get().GetPos(), values[2].get().IntegerValue());
                              })});
}

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::ArrayDef(const supplier_t* labelSupplier)
{
    const HeaderMatcherFactory create(labelSupplier);

    return create.And({create.Char('['), create.Or({create.Integer(), create.Identifier()}), create.Char(']')})
        .Transform(
            [](HeaderMatcherFactory::token_list_t& values)
            {
                if (values[1].get().m_type == HeaderParserValueType::INTEGER)
                    return HeaderParserValue::Integer(values[1].get().GetPos(), values[1].get().IntegerValue());

                return HeaderParserValue::Identifier(values[1].get().GetPos(), new std::string(values[1].get().IdentifierValue()));
            });
}

std::unique_ptr<HeaderCommonMatchers::matcher_t> HeaderCommonMatchers::Typename(const supplier_t* labelSupplier)
{
    static constexpr const char* BUILT_IN_TYPE_NAMES[]{"unsigned", "char", "short", "int", "long"};
    static_assert(std::extent<decltype(BUILT_IN_TYPE_NAMES)>::value
                  == static_cast<int>(HeaderParserValueType::BUILT_IN_LAST) - static_cast<int>(HeaderParserValueType::BUILT_IN_FIRST) + 1);

    const HeaderMatcherFactory create(labelSupplier);

    return create.Or(
        {create
             .And({create.Optional(create.Type(HeaderParserValueType::UNSIGNED)),
                   create.Or({create.Type(HeaderParserValueType::CHAR),
                              create.Type(HeaderParserValueType::SHORT),
                              create.Type(HeaderParserValueType::INT),
                              create.And({create.Type(HeaderParserValueType::LONG), create.Optional(create.Type(HeaderParserValueType::LONG))})})})
             .Transform(
                 [](HeaderMatcherFactory::token_list_t& values)
                 {
                     std::ostringstream str;
                     auto first = true;

                     for (const auto& token : values)
                     {
                         if (first)
                             first = false;
                         else
                             str << " ";
                         str << BUILT_IN_TYPE_NAMES[static_cast<int>(token.get().m_type) - static_cast<int>(HeaderParserValueType::BUILT_IN_FIRST)];
                     }

                     return HeaderParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                 }),
         create.And({create.Identifier(), create.OptionalLoop(create.And({create.Char(':'), create.Char(':'), create.Identifier()}))})
             .Transform(
                 [](HeaderMatcherFactory::token_list_t& values)
                 {
                     std::ostringstream str;
                     str << values[0].get().IdentifierValue();

                     for (auto i = 3u; i < values.size(); i += 3)
                         str << "::" << values[i].get().IdentifierValue();

                     return HeaderParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                 })});
}
