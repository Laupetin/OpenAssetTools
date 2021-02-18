#include "HeaderBlockUnion.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"
#include "Parsing/Header/Sequence/SequenceVariable.h"
#include "Utils/NameUtils.h"

HeaderBlockUnion::HeaderBlockUnion(std::string name, const bool isTypedef)
    : m_type_name(std::move(name)),
      m_union_definition(nullptr),
      m_custom_alignment(0),
      m_is_typedef(isTypedef),
      m_has_custom_align(false)
{
}

HeaderBlockType HeaderBlockUnion::GetType()
{
    return HeaderBlockType::UNION;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockUnion::GetTestsForBlock()
{
    static std::vector<sequence_t*> tests({
        new SequenceCloseBlock(true),
        new SequenceEnum(),
        new SequenceStruct(),
        new SequenceUnion(),
        new SequenceVariable()
    });

    return tests;
}

void HeaderBlockUnion::OnOpen(HeaderParserState* state)
{
    m_namespace = state->m_namespace.ToString();

    if (!m_type_name.empty())
        state->m_namespace.Push(m_type_name);
}

void HeaderBlockUnion::OnClose(HeaderParserState* state)
{
    if (!m_type_name.empty())
        state->m_namespace.Pop();

    auto isAnonymous = false;
    auto typeName = m_type_name;
    if (typeName.empty())
    {
        isAnonymous = true;
        typeName = NameUtils::GenerateRandomName();
    }

    auto unionDefinition = std::make_unique<UnionDefinition>(m_namespace, std::move(typeName), state->m_pack_value_supplier->GetCurrentPack());
    m_union_definition = unionDefinition.get();

    if (isAnonymous)
        unionDefinition->m_anonymous = true;

    for (auto& member : m_members)
        unionDefinition->m_members.emplace_back(std::move(member));

    state->AddDataType(std::move(unionDefinition));

    if (m_is_typedef)
        state->AddDataType(std::make_unique<TypedefDefinition>(m_namespace, m_variable_name, std::make_unique<TypeDeclaration>(m_union_definition)));
}

void HeaderBlockUnion::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block)
{
}

void HeaderBlockUnion::AddVariable(std::shared_ptr<Variable> variable)
{
    m_members.emplace_back(std::move(variable));
}

void HeaderBlockUnion::SetCustomAlignment(const int alignment)
{
    m_has_custom_align = true;
    m_custom_alignment = alignment;
}

void HeaderBlockUnion::Inherit(const StructDefinition* parentStruct)
{
    for (const auto& parentMember : parentStruct->m_members)
        AddVariable(parentMember);
}

void HeaderBlockUnion::SetBlockName(const TokenPos& nameTokenPos, std::string name)
{
    m_variable_name = std::move(name);
}

bool HeaderBlockUnion::IsDefiningVariable()
{
    return !m_is_typedef && !m_variable_name.empty();
}

DataDefinition* HeaderBlockUnion::GetVariableType()
{
    return m_union_definition;
}

std::string HeaderBlockUnion::GetVariableName()
{
    return m_variable_name;
}
