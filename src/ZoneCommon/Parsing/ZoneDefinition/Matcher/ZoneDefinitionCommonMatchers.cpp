#include "ZoneDefinitionCommonMatchers.h"

#include <sstream>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

std::unique_ptr<ZoneDefinitionCommonMatchers::matcher_t> ZoneDefinitionCommonMatchers::AssetName(const supplier_t* labelSupplier)
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.And({
        create.Loop(create.Or({
            create.Identifier(),
            create.AnyCharBesides({',', '<', '>', '"', '\\', '*', '?', '|', ':'})
        })),
        create.Or({
            create.Type(SimpleParserValueType::NEW_LINE),
            create.Type(SimpleParserValueType::END_OF_FILE)
        }).NoConsume()
    }).Transform([](SimpleMatcherFactory::token_list_t& tokens)
    {
        std::ostringstream str;
        auto previousType = SimpleParserValueType::INVALID;
        auto previousCharacterWasSlash = false;

        for (const auto& token : tokens)
        {
            switch (token.get().m_type)
            {
            case SimpleParserValueType::IDENTIFIER:
                if (previousType == SimpleParserValueType::IDENTIFIER)
                    str << " ";
                str << token.get().IdentifierValue();
                break;

            case SimpleParserValueType::CHARACTER:
                if (token.get().CharacterValue() == '/')
                {
                    if (previousType != SimpleParserValueType::CHARACTER || !previousCharacterWasSlash)
                    {
                        str << "/";
                        previousCharacterWasSlash = true;
                    }
                }
                else
                {
                    str << token.get().CharacterValue();
                    previousCharacterWasSlash = false;
                }
                break;

            default:
                break;
            }

            previousType = token.get().m_type;
        }

        return SimpleParserValue::Identifier(tokens.front().get().GetPos(), new std::string(str.str()));
    });
}
