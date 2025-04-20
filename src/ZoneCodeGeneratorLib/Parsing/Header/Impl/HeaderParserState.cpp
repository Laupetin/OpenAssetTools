#include "HeaderParserState.h"

#include "Domain/Definition/EnumDefinition.h"
#include "Parsing/Header/Block/HeaderBlockNone.h"

#include <format>
#include <iostream>

HeaderParserState::HeaderParserState(const IPackValueSupplier* packValueSupplier)
    : m_pack_value_supplier(packValueSupplier)

{
    m_blocks.push(std::make_unique<HeaderBlockNone>());

    for (auto i = 0u; i < BaseTypeDefinition::ALL_BASE_TYPES_COUNT; i++)
        AddBaseDataType(BaseTypeDefinition::ALL_BASE_TYPES[i]);
}

void HeaderParserState::AddBaseDataType(const BaseTypeDefinition* baseType)
{
    m_definitions.insert(std::make_pair(baseType->GetFullName(), baseType));
}

IHeaderBlock* HeaderParserState::GetBlock() const
{
    return m_blocks.top().get();
}

void HeaderParserState::PushBlock(std::unique_ptr<IHeaderBlock> block)
{
    m_blocks.emplace(std::move(block));
    m_blocks.top()->OnOpen(this);
}

void HeaderParserState::PopBlock()
{
    // Leave at least one block on the stack which should be the only "none" block
    if (m_blocks.size() > 1)
    {
        std::unique_ptr<IHeaderBlock> poppedBlock = std::move(m_blocks.top());
        m_blocks.pop();
        poppedBlock->OnClose(this);
        m_blocks.top()->OnChildBlockClose(this, poppedBlock.get());
    }
}

void HeaderParserState::AddDataType(std::unique_ptr<DataDefinition> definition)
{
    if (definition->GetType() == DataDefinitionType::ENUM)
    {
        for (const auto& enumMember : dynamic_cast<EnumDefinition*>(definition.get())->m_members)
        {
            m_enum_members.insert(std::make_pair(enumMember->m_name, enumMember.get()));
        }
    }

    const DataDefinition* dataDefinition = definition.get();
    m_header_definitions.emplace_back(std::move(definition));
    m_definitions.insert(std::make_pair(dataDefinition->m_name, dataDefinition));
}

void HeaderParserState::AddForwardDeclaration(std::unique_ptr<ForwardDeclaration> forwardDeclaration)
{
    m_forward_declarations.insert(std::make_pair(forwardDeclaration->GetFullName(), std::move(forwardDeclaration)));
}

const DataDefinition* HeaderParserState::FindType(const std::string& typeName)
{
    const auto foundDefinitionEntry = m_definitions.find(typeName);

    if (foundDefinitionEntry != m_definitions.end())
        return foundDefinitionEntry->second;

    const auto foundForwardEntry = m_forward_declarations.find(typeName);

    if (foundForwardEntry != m_forward_declarations.end())
        return foundForwardEntry->second.get();

    return nullptr;
}

EnumMember* HeaderParserState::FindEnumMember(const std::string& enumMemberName)
{
    const auto foundEntry = m_enum_members.find(enumMemberName);

    if (foundEntry != m_enum_members.end())
        return foundEntry->second;

    return nullptr;
}

bool HeaderParserState::ResolveForwardDeclarations()
{
    for (auto& [_, forwardDeclaration] : m_forward_declarations)
    {
        const auto* dataDefinition = FindType(forwardDeclaration->GetFullName());

        if (dataDefinition == nullptr)
        {
            std::cerr << std::format("Forward declaration \"{}\" was not defined\n", forwardDeclaration->GetFullName());
            return false;
        }

        forwardDeclaration->m_definition = dataDefinition;
    }

    return true;
}

bool HeaderParserState::ReplaceForwardDeclarationsInStruct(StructDefinition* structDefinition)
{
    for (const auto& variable : structDefinition->m_members)
    {
        if (variable->m_type_declaration->m_type->GetType() == DataDefinitionType::FORWARD_DECLARATION)
            variable->m_type_declaration->m_type = dynamic_cast<const ForwardDeclaration*>(variable->m_type_declaration->m_type)->m_definition;
    }

    return true;
}

bool HeaderParserState::ReplaceForwardDeclarationsInTypedef(TypedefDefinition* typedefDefinition)
{
    if (typedefDefinition->m_type_declaration->m_type->GetType() == DataDefinitionType::FORWARD_DECLARATION)
        typedefDefinition->m_type_declaration->m_type = dynamic_cast<const ForwardDeclaration*>(typedefDefinition->m_type_declaration->m_type)->m_definition;

    return true;
}

bool HeaderParserState::ReplaceForwardDeclarationsInUnion(UnionDefinition* unionDefinition)
{
    for (const auto& variable : unionDefinition->m_members)
    {
        if (variable->m_type_declaration->m_type->GetType() == DataDefinitionType::FORWARD_DECLARATION)
            variable->m_type_declaration->m_type = dynamic_cast<const ForwardDeclaration*>(variable->m_type_declaration->m_type)->m_definition;
    }

    return true;
}

bool HeaderParserState::ReplaceForwardDeclarationsInDefinitions()
{
    for (auto& definition : m_header_definitions)
    {
        switch (definition->GetType())
        {
        case DataDefinitionType::STRUCT:
            if (!ReplaceForwardDeclarationsInStruct(dynamic_cast<StructDefinition*>(definition.get())))
                return false;
            break;

        case DataDefinitionType::TYPEDEF:
            if (!ReplaceForwardDeclarationsInTypedef(dynamic_cast<TypedefDefinition*>(definition.get())))
                return false;
            break;

        case DataDefinitionType::UNION:
            if (!ReplaceForwardDeclarationsInUnion(dynamic_cast<UnionDefinition*>(definition.get())))
                return false;
            break;

        default:
            break;
        }
    }

    return true;
}

bool HeaderParserState::MoveDefinitionsToRepository(IDataRepository* repository)
{
    for (auto& definition : m_header_definitions)
    {
        switch (definition->GetType())
        {
        case DataDefinitionType::ENUM:
            repository->Add(std::unique_ptr<EnumDefinition>(dynamic_cast<EnumDefinition*>(definition.release())));
            break;

        case DataDefinitionType::STRUCT:
            repository->Add(std::unique_ptr<StructDefinition>(dynamic_cast<StructDefinition*>(definition.release())));
            break;

        case DataDefinitionType::UNION:
            repository->Add(std::unique_ptr<UnionDefinition>(dynamic_cast<UnionDefinition*>(definition.release())));
            break;

        case DataDefinitionType::TYPEDEF:
            repository->Add(std::unique_ptr<TypedefDefinition>(dynamic_cast<TypedefDefinition*>(definition.release())));
            break;

        default:
            break;
        }
    }

    return true;
}

bool HeaderParserState::SaveToRepository(IDataRepository* repository)
{
    return ResolveForwardDeclarations() && ReplaceForwardDeclarationsInDefinitions() && MoveDefinitionsToRepository(repository);
}
