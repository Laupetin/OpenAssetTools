#include "SequenceVariable.h"

#include "Domain/Definition/ArrayDeclarationModifier.h"
#include "Domain/Definition/PointerDeclarationModifier.h"
#include "Parsing/Header/Block/IHeaderBlockVariableHolder.h"
#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceVariable::SequenceVariable()
{
    const HeaderMatcherFactory create(this);

    AddLabeledMatchers(HeaderCommonMatchers::Align(this), HeaderCommonMatchers::LABEL_ALIGN);
    AddLabeledMatchers(HeaderCommonMatchers::ArrayDef(this), HeaderCommonMatchers::LABEL_ARRAY_DEF);
    AddLabeledMatchers(HeaderCommonMatchers::Typename(this), HeaderCommonMatchers::LABEL_TYPENAME);

    AddLabeledMatchers(
        {
            create.Optional(create.Type(HeaderParserValueType::CONST).Tag(TAG_CONST)),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
            create.OptionalLoop(create.Char('*').Tag(TAG_POINTER)),
            create.Identifier().Capture(CAPTURE_NAME),
            create.OptionalLoop(create.Label(HeaderCommonMatchers::LABEL_ARRAY_DEF).Capture(CAPTURE_ARRAY)),
            create.Optional(create.And({
                create.Char(':'),
                create.Integer().Capture(CAPTURE_BIT_SIZE)
            })),
            create.Char(';')
        }, LABEL_ARRAY_OF_POINTERS);

    AddLabeledMatchers(
        {
            create.Optional(create.Type(HeaderParserValueType::CONST).Tag(TAG_CONST)),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
            create.Char('('),
            create.Loop(create.Char('*').Tag(TAG_POINTER)),
            create.Identifier().Capture(CAPTURE_NAME),
            create.Char(')'),
            create.Loop(create.Label(HeaderCommonMatchers::LABEL_ARRAY_DEF).Capture(CAPTURE_ARRAY)),
            create.Char(';')
        }, LABEL_POINTER_TO_ARRAY);

    AddMatchers(create.Or({
        create.Label(LABEL_ARRAY_OF_POINTERS).Tag(TAG_ARRAY_OF_POINTERS),
        create.Label(LABEL_POINTER_TO_ARRAY).Tag(TAG_POINTER_TO_ARRAY)
    }));
}

void SequenceVariable::AddPointerDeclarationModifiers(SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const
{
    while (result.PeekAndRemoveIfTag(TAG_POINTER) == TAG_POINTER)
        typeDeclaration->m_declaration_modifiers.emplace_back(std::make_unique<PointerDeclarationModifier>());
}

void SequenceVariable::AddArrayDeclarationModifiers(HeaderParserState* state, SequenceResult<HeaderParserValue>& result, TypeDeclaration* typeDeclaration) const
{
    while (result.HasNextCapture(CAPTURE_ARRAY))
    {
        const auto& arrayToken = result.NextCapture(CAPTURE_ARRAY);

        if (arrayToken.m_type == HeaderParserValueType::INTEGER)
        {
            typeDeclaration->m_declaration_modifiers.emplace_back(std::make_unique<ArrayDeclarationModifier>(arrayToken.IntegerValue()));
        }
        else
        {
            auto* enumMember = state->FindEnumMember(arrayToken.IdentifierValue());

            if (enumMember == nullptr)
                throw ParsingException(arrayToken.GetPos(), "Unknown enum member");

            typeDeclaration->m_declaration_modifiers.emplace_back(std::make_unique<ArrayDeclarationModifier>(enumMember->m_value));
        }
    }
}

void SequenceVariable::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
    auto* variableHolder = dynamic_cast<IHeaderBlockVariableHolder*>(state->GetBlock());
    if (variableHolder == nullptr)
        throw ParsingException(TokenPos(), "Defining variable in non VariableHolder block");

    const auto modeTag = result.NextTag();
    assert(modeTag == TAG_ARRAY_OF_POINTERS || modeTag == TAG_POINTER_TO_ARRAY);

    const auto name = result.NextCapture(CAPTURE_NAME).IdentifierValue();

    const auto& typenameToken = result.NextCapture(CAPTURE_TYPE);
    const auto* type = state->FindType(typenameToken.TypeNameValue());
    if (type == nullptr && !state->m_namespace.IsEmpty())
    {
        const auto fullTypename = NamespaceBuilder::Combine(state->m_namespace.ToString(), typenameToken.TypeNameValue());
        type = state->FindType(fullTypename);
    }
    if (type == nullptr)
        throw ParsingException(typenameToken.GetPos(), "Unknown type");

    auto variable = std::make_shared<Variable>(name, std::make_unique<TypeDeclaration>(type));
    variable->m_type_declaration->m_is_const = result.PeekAndRemoveIfTag(TAG_CONST) == TAG_CONST;

    if (result.HasNextCapture(CAPTURE_ALIGN))
    {
        const auto& alignToken = result.NextCapture(CAPTURE_ALIGN);
        variable->m_alignment_override = static_cast<unsigned>(alignToken.IntegerValue());
        variable->m_has_alignment_override = true;
    }

    if(result.HasNextCapture(CAPTURE_BIT_SIZE))
    {
        const auto& bitSizeToken = result.NextCapture(CAPTURE_BIT_SIZE);
        variable->m_type_declaration->m_custom_bit_size = bitSizeToken.IntegerValue();

        if (variable->m_type_declaration->m_custom_bit_size <= 0)
            throw ParsingException(bitSizeToken.GetPos(), "Custom bit size must be greater than 0");

        variable->m_type_declaration->m_has_custom_bit_size = true;
    }

    if (modeTag == TAG_ARRAY_OF_POINTERS)
    {
        AddArrayDeclarationModifiers(state, result, variable->m_type_declaration.get());
        AddPointerDeclarationModifiers(result, variable->m_type_declaration.get());
    }
    else
    {
        AddPointerDeclarationModifiers(result, variable->m_type_declaration.get());
        AddArrayDeclarationModifiers(state, result, variable->m_type_declaration.get());
    }

    variableHolder->AddVariable(std::move(variable));
}
