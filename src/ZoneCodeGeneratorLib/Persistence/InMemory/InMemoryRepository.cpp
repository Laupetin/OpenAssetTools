#include "InMemoryRepository.h"

InMemoryRepository::InMemoryRepository()
    : m_architecture(Architecture::UNKNOWN)
{
}

InMemoryRepository::~InMemoryRepository()
{
    for (auto* enumDefinition : m_enums)
        delete enumDefinition;
    for (auto* structDefinition : m_structs)
        delete structDefinition;
    for (auto* unionDefinition : m_unions)
        delete unionDefinition;
    for (auto* typedefDefinition : m_typedefs)
        delete typedefDefinition;
    for (auto* structureInformation : m_structures_information)
        delete structureInformation;
    for (auto* fastFileBlock : m_fast_file_blocks)
        delete fastFileBlock;
}

void InMemoryRepository::Add(std::unique_ptr<EnumDefinition> enumsDefinition)
{
    auto* raw = enumsDefinition.release();
    m_enums.push_back(raw);
    m_data_definitions_by_name[raw->m_name] = raw;

    for (const auto& enumMember : raw->m_members)
        m_enum_members_by_name[enumMember->m_name] = enumMember.get();
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

const std::string& InMemoryRepository::GetGameName() const
{
    return m_game_name;
}

void InMemoryRepository::SetGame(std::string gameName)
{
    m_game_name = std::move(gameName);
}

Architecture InMemoryRepository::GetArchitecture() const
{
    return m_architecture;
}

void InMemoryRepository::SetArchitecture(const Architecture architecture)
{
    m_architecture = architecture;
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

StructureInformation* InMemoryRepository::GetInformationFor(const DefinitionWithMembers* definitionWithMembers) const
{
    const auto foundEntry = m_structure_information_by_definition.find(definitionWithMembers);

    if (foundEntry != m_structure_information_by_definition.end())
        return foundEntry->second;

    return nullptr;
}

EnumMember* InMemoryRepository::GetEnumMemberByName(const std::string& name) const
{
    const auto foundEntry = m_enum_members_by_name.find(name);

    if (foundEntry != m_enum_members_by_name.end())
        return foundEntry->second;

    return nullptr;
}
