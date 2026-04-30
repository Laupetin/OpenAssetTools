#pragma once

#include "Persistence/IDataRepository.h"

#include <unordered_map>

class InMemoryRepository final : public IDataRepository
{
public:
    InMemoryRepository();
    ~InMemoryRepository() override;
    InMemoryRepository(const InMemoryRepository& other) = delete;
    InMemoryRepository(InMemoryRepository&& other) noexcept = default;
    InMemoryRepository& operator=(const InMemoryRepository& other) = delete;
    InMemoryRepository& operator=(InMemoryRepository&& other) noexcept = default;

    void Add(std::unique_ptr<EnumDefinition> enumsDefinition) override;
    void Add(std::unique_ptr<StructDefinition> structDefinition) override;
    void Add(std::unique_ptr<UnionDefinition> unionDefinition) override;
    void Add(std::unique_ptr<TypedefDefinition> typedefDefinition) override;
    void Add(std::unique_ptr<StructureInformation> structureInformation) override;
    void Add(std::unique_ptr<TypeInformation> typeInformation) override;
    void Add(std::unique_ptr<FastFileBlock> fastFileBlock) override;

    [[nodiscard]] const std::string& GetGameName() const override;
    void SetGame(std::string gameName) override;
    [[nodiscard]] WordSize GetWordSize() const override;
    void SetWordSize(WordSize wordSize) override;

    [[nodiscard]] const std::vector<EnumDefinition*>& GetAllEnums() const override;
    [[nodiscard]] const std::vector<StructDefinition*>& GetAllStructs() const override;
    [[nodiscard]] const std::vector<UnionDefinition*>& GetAllUnions() const override;
    [[nodiscard]] const std::vector<TypedefDefinition*>& GetAllTypedefs() const override;
    [[nodiscard]] const std::vector<StructureInformation*>& GetAllStructureInformation() const override;
    [[nodiscard]] const std::vector<const FastFileBlock*>& GetAllFastFileBlocks() const override;

    [[nodiscard]] DataDefinition* GetDataDefinitionByName(const std::string& name) const override;
    [[nodiscard]] StructureInformation* GetInformationFor(const DefinitionWithMembers* definitionWithMembers) const override;
    [[nodiscard]] TypeInformation* GetTypeInformationFor(const DataDefinition* definition) const override;
    [[nodiscard]] EnumMember* GetEnumMemberByName(const std::string& name) const override;
    [[nodiscard]] const FastFileBlock* GetFastFileBlockByName(const std::string& name) const override;

private:
    std::vector<EnumDefinition*> m_enums;
    std::vector<StructDefinition*> m_structs;
    std::vector<UnionDefinition*> m_unions;
    std::vector<TypedefDefinition*> m_typedefs;
    std::vector<StructureInformation*> m_structures_information;
    std::vector<TypeInformation*> m_types_information;
    std::vector<const FastFileBlock*> m_fast_file_blocks;
    std::unordered_map<std::string, DataDefinition*> m_data_definitions_by_name;
    std::unordered_map<std::string, EnumMember*> m_enum_members_by_name;
    std::unordered_map<std::string, const FastFileBlock*> m_fast_file_blocks_by_name;
    std::unordered_map<const DefinitionWithMembers*, StructureInformation*> m_structure_information_by_definition;
    std::unordered_map<const DataDefinition*, TypeInformation*> m_type_information_by_definition;
    std::string m_game_name;
    WordSize m_word_size;
};
