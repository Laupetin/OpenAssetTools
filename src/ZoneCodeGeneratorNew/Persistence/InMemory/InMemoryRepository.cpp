#include "InMemoryRepository.h"

void InMemoryRepository::Add(std::unique_ptr<EnumDefinition> enumsDefinition)
{
    auto* raw = enumsDefinition.release();
    m_enums.push_back(raw);
    m_data_definitions_by_name[raw->m_name] = raw;
}

void InMemoryRepository::Add(std::unique_ptr<StructDefinition> structDefinition)
{
    auto* raw = structDefinition.release();
    m_structs.push_back(raw);
    m_data_definitions_by_name[raw->m_name] = raw;
}

void InMemoryRepository::Add(std::unique_ptr<UnionDefinition> unionDefinition)
{
    auto* raw = unionDefinition.release();
    m_unions.push_back(raw);
    m_data_definitions_by_name[raw->m_name] = raw;
}

void InMemoryRepository::Add(std::unique_ptr<TypedefDefinition> typedefDefinition)
{
    auto* raw = typedefDefinition.release();
    m_typedefs.push_back(raw);
    m_data_definitions_by_name[raw->m_name] = raw;
}

void InMemoryRepository::Add(std::unique_ptr<StructureInformation> structureInformation)
{
    auto* raw = structureInformation.release();
    m_structures_information.push_back(raw);
    m_structure_information_by_definition[raw->m_definition] = raw;
}

void InMemoryRepository::Add(std::unique_ptr<FastFileBlock> fastFileBlock)
{
    m_fast_file_blocks.push_back(fastFileBlock.release());
}

const std::vector<EnumDefinition*>& InMemoryRepository::GetAllEnums() const
{
    return m_enums;
}

const std::vector<StructDefinition*>& InMemoryRepository::GetAllStructs() const
{
    return m_structs;
}

const std::vector<UnionDefinition*>& InMemoryRepository::GetAllUnions() const
{
    return m_unions;
}

const std::vector<TypedefDefinition*>& InMemoryRepository::GetAllTypedefs() const
{
    return m_typedefs;
}

const std::vector<StructureInformation*>& InMemoryRepository::GetAllStructureInformation() const
{
    return m_structures_information;
}

const std::vector<FastFileBlock*>& InMemoryRepository::GetAllFastFileBlocks() const
{
    return m_fast_file_blocks;
}

DataDefinition* InMemoryRepository::GetDataDefinitionByName(const std::string& name) const
{
    const auto foundEntry = m_data_definitions_by_name.find(name);

    if (foundEntry != m_data_definitions_by_name.end())
        return foundEntry->second;

    return nullptr;
}

StructureInformation* InMemoryRepository::GetInformationFor(DefinitionWithMembers* definitionWithMembers) const
{
    const auto foundEntry = m_structure_information_by_definition.find(definitionWithMembers);

    if (foundEntry != m_structure_information_by_definition.end())
        return foundEntry->second;

    return nullptr;
}
