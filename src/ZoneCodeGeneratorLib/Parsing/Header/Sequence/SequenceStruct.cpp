#include "SequenceStruct.h"

#include "Parsing/Header/Block/HeaderBlockStruct.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceStruct::SequenceStruct()
{
    const HeaderMatcherFactory create(this);

    AddLabeledMatchers(HeaderCommonMatchers::Align(this), HeaderCommonMatchers::LABEL_ALIGN);
    AddLabeledMatchers(HeaderCommonMatchers::Typename(this), HeaderCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Optional(create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF)),
        create.Optional(create.Type(HeaderParserValueType::CONST)),
        create.Type(HeaderParserValueType::STRUCT),
        create.Optional(create.Label(HeaderCommonMatchers::LABEL_ALIGN).Capture(CAPTURE_ALIGN)),
        create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
        create.Optional(create.And({
            create.Char(':'),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_PARENT_TYPE)
        })),
        create.Char('{')
    });
}

void SequenceStruct::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    auto isTypedef = result.PeekAndRemoveIfTag(TAG_TYPEDEF) == TAG_TYPEDEF;
    std::string name;

    if (result.HasNextCapture(CAPTURE_NAME))
        name = result.NextCapture(CAPTURE_NAME).IdentifierValue();

    auto structBlock = std::make_unique<HeaderBlockStruct>(std::move(name), isTypedef);

    if (result.HasNextCapture(CAPTURE_ALIGN))
        structBlock->SetCustomAlignment(result.NextCapture(CAPTURE_ALIGN).IntegerValue());

    if(result.HasNextCapture(CAPTURE_PARENT_TYPE))
    {
        const auto& parentTypeToken = result.NextCapture(CAPTURE_PARENT_TYPE);
        const auto* parentDefinition = state->FindType(parentTypeToken.TypeNameValue());

        if(parentDefinition == nullptr && !state->m_namespace.IsEmpty())
        {
            const auto fullTypeName = NamespaceBuilder::Combine(state->m_namespace.ToString(), parentTypeToken.TypeNameValue());
            parentDefinition = state->FindType(fullTypeName);
        }

        if (parentDefinition == nullptr)
            throw ParsingException(parentTypeToken.GetPos(), "Cannot find specified parent type");

        if(parentDefinition->GetType() != DataDefinitionType::STRUCT)
            throw ParsingException(parentTypeToken.GetPos(), "Parent type must be a struct");

        structBlock->Inherit(dynamic_cast<const StructDefinition*>(parentDefinition));
    }

    state->PushBlock(std::move(structBlock));
}
