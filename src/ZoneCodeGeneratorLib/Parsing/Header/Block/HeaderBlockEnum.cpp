#include "HeaderBlockEnum.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnumMember.h"

HeaderBlockEnum::HeaderBlockEnum(std::string typeName, const BaseTypeDefinition* parentType, const bool isTypeDef)
    : m_type_name(std::move(typeName)),
      m_parent_type(parentType),
      m_is_typedef(isTypeDef),
      m_next_value(0),
      m_enum_definition(nullptr)
{
}

HeaderBlockType HeaderBlockEnum::GetType()
{
    return HeaderBlockType::ENUM;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockEnum::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({
        new SequenceCloseBlock(true),
        new SequenceEnumMember()
    });

    return tests;
}

void HeaderBlockEnum::OnOpen(HeaderParserState* state)
{
    m_namespace = state->m_namespace.ToString();

    if (!m_type_name.empty())
        state->m_namespace.Push(m_type_name);
}

void HeaderBlockEnum::OnClose(HeaderParserState* state)
{
    if (!m_type_name.empty())
        state->m_namespace.Pop();

    auto enumDefinition = std::make_unique<EnumDefinition>(m_namespace, m_type_name, m_parent_type);
    m_enum_definition = enumDefinition.get();
    for (auto& member : m_members)
        enumDefinition->m_members.emplace_back(std::move(member));

    state->AddDataType(std::move(enumDefinition));

    if (m_is_typedef)
        state->AddDataType(std::make_unique<TypedefDefinition>(m_namespace, m_variable_name, std::make_unique<TypeDeclaration>(m_enum_definition)));
}

void HeaderBlockEnum::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block)
{
}

void HeaderBlockEnum::AddEnumMember(std::unique_ptr<EnumMember> enumMember)
{
    m_next_value = enumMember->m_value + 1;
    m_members.emplace_back(std::move(enumMember));
}

EnumMember* HeaderBlockEnum::GetEnumMember(const std::string& name) const
{
    for (const auto& member : m_members)
    {
        if (member->m_name == name)
            return member.get();
    }

    return nullptr;
}

long long HeaderBlockEnum::GetNextEnumMemberValue() const
{
    return m_next_value;
}

void HeaderBlockEnum::SetBlockName(const TokenPos& nameTokenPos, std::string name)
{
    m_variable_name = std::move(name);
}

bool HeaderBlockEnum::IsDefiningVariable()
{
    return !m_is_typedef && !m_variable_name.empty();
}

DataDefinition* HeaderBlockEnum::GetVariableType()
{
    return m_enum_definition;
}

std::string HeaderBlockEnum::GetVariableName()
{
    return m_variable_name;
}
