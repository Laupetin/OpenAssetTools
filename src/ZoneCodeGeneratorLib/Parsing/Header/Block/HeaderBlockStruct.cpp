#include "HeaderBlockStruct.h"

#include "Parsing/Header/Sequence/SequenceCloseBlock.h"
#include "Parsing/Header/Sequence/SequenceEnum.h"
#include "Parsing/Header/Sequence/SequenceStruct.h"
#include "Parsing/Header/Sequence/SequenceUnion.h"
#include "Parsing/Header/Sequence/SequenceVariable.h"
#include "Utils/NameUtils.h"

HeaderBlockStruct::HeaderBlockStruct(std::string name, const bool isTypedef)
    : m_type_name(std::move(name)),
      m_struct_definition(nullptr),
      m_custom_alignment(0),
      m_is_typedef(isTypedef),
      m_has_custom_align(false),
      m_is_anonymous(false)
{
}

HeaderBlockType HeaderBlockStruct::GetType()
{
    return HeaderBlockType::STRUCT;
}

const std::vector<IHeaderBlock::sequence_t*>& HeaderBlockStruct::GetTestsForBlock()
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

void HeaderBlockStruct::OnOpen(HeaderParserState* state)
{
    m_namespace = state->m_namespace.ToString();

    if (m_type_name.empty())
    {
        m_is_anonymous = true;
        m_type_name = NameUtils::GenerateRandomName();
    }
    else
    {
        state->m_namespace.Push(m_type_name);
    }
}

void HeaderBlockStruct::OnClose(HeaderParserState* state)
{
    if (!m_is_anonymous)
        state->m_namespace.Pop();

    auto structDefinition = std::make_unique<StructDefinition>(m_namespace, m_type_name, state->m_pack_value_supplier->GetCurrentPack());
    m_struct_definition = structDefinition.get();

    if (m_is_anonymous)
        structDefinition->m_anonymous = true;

    for (auto& member : m_members)
        structDefinition->m_members.emplace_back(std::move(member));

    state->AddDataType(std::move(structDefinition));

    if (m_is_typedef)
        state->AddDataType(std::make_unique<TypedefDefinition>(m_namespace, m_variable_name, std::make_unique<TypeDeclaration>(m_struct_definition)));
}

void HeaderBlockStruct::OnChildBlockClose(HeaderParserState* state, IHeaderBlock* block)
{
    auto* variableDefining = dynamic_cast<IHeaderBlockVariableDefining*>(block);

    if (variableDefining && variableDefining->IsDefiningVariable())
        m_members.emplace_back(std::make_shared<Variable>(variableDefining->GetVariableName(), std::make_unique<TypeDeclaration>(variableDefining->GetVariableType())));
}

void HeaderBlockStruct::AddVariable(std::shared_ptr<Variable> variable)
{
    m_members.emplace_back(std::move(variable));
}

void HeaderBlockStruct::SetCustomAlignment(const int alignment)
{
    m_has_custom_align = true;
    m_custom_alignment = alignment;
}

void HeaderBlockStruct::Inherit(const StructDefinition* parentStruct)
{
    for (const auto& parentMember : parentStruct->m_members)
        AddVariable(parentMember);
}

void HeaderBlockStruct::SetBlockName(const TokenPos& nameTokenPos, std::string name)
{
    m_variable_name = std::move(name);
}

bool HeaderBlockStruct::IsDefiningVariable()
{
    return true;
}

DataDefinition* HeaderBlockStruct::GetVariableType()
{
    return m_struct_definition;
}

std::string HeaderBlockStruct::GetVariableName()
{
    return m_variable_name;
}
