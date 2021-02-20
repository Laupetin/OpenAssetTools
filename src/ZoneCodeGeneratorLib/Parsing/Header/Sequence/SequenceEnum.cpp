#include "SequenceEnum.h"

#include "Parsing/Header/Block/HeaderBlockEnum.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceEnum::SequenceEnum()
{
    const HeaderMatcherFactory create(this);

    AddLabeledMatchers(HeaderCommonMatchers::Typename(this), HeaderCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Optional(create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF)),
        create.Type(HeaderParserValueType::ENUM),
        create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
        create.Optional(create.And({
            create.Char(':'),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_PARENT_TYPE)
        })),
        create.Char('{')
    });
}

void SequenceEnum::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    auto isTypedef = result.PeekAndRemoveIfTag(TAG_TYPEDEF) == TAG_TYPEDEF;
    std::string name;
    const auto* parentType = BaseTypeDefinition::INT;

    if (result.HasNextCapture(CAPTURE_NAME))
        name = result.NextCapture(CAPTURE_NAME).IdentifierValue();

    if(result.HasNextCapture(CAPTURE_PARENT_TYPE))
    {
        const auto& typeNameToken = result.NextCapture(CAPTURE_PARENT_TYPE);
        const auto* foundTypeDefinition = state->FindType(typeNameToken.TypeNameValue());

        if (foundTypeDefinition == nullptr)
            throw ParsingException(typeNameToken.GetPos(), "Cannot find type");

        while (foundTypeDefinition->GetType() == DataDefinitionType::TYPEDEF)
            foundTypeDefinition = dynamic_cast<const TypedefDefinition*>(foundTypeDefinition)->m_type_declaration->m_type;

        if (foundTypeDefinition->GetType() != DataDefinitionType::BASE_TYPE)
            throw ParsingException(typeNameToken.GetPos(), "Enums can only have base types as parent type");

        parentType = dynamic_cast<const BaseTypeDefinition*>(foundTypeDefinition);
    }

    state->PushBlock(std::make_unique<HeaderBlockEnum>(name, parentType, isTypedef));
}
