#pragma once

#include <string>
#include <unordered_map>

#include "Domain/Information/StructureInformation.h"
#include "Persistence/IDataRepository.h"

class RenderingUsedType
{
public:
    bool m_members_loaded;
    const DataDefinition* m_type;
    StructureInformation* m_info;
    bool m_is_context_asset;

    bool m_non_runtime_reference_exists;
    bool m_non_embedded_reference_exists;
    bool m_array_reference_exists;
    bool m_pointer_array_reference_exists;
    bool m_pointer_array_reference_is_reusable;

    RenderingUsedType(const DataDefinition* type, StructureInformation* info);
};

class RenderingContext
{
    std::unordered_map<const DataDefinition*, std::unique_ptr<RenderingUsedType>> m_used_types_lookup;

    RenderingContext(std::string game, std::vector<const FastFileBlock*> fastFileBlocks);

    RenderingUsedType* AddUsedType(std::unique_ptr<RenderingUsedType> usedType);
    RenderingUsedType* GetBaseType(RenderingUsedType* usedType);
    void AddMembersToContext(StructureInformation* info);
    void MakeAsset(StructureInformation* asset);
    void CreateUsedTypeCollections();

public:
    std::string m_game;
    std::vector<const FastFileBlock*> m_blocks;

    StructureInformation* m_asset;

    std::vector<RenderingUsedType*> m_used_types;
    std::vector<RenderingUsedType*> m_used_structures;
    std::vector<RenderingUsedType*> m_referenced_assets;
    bool m_has_actions;

    const FastFileBlock* m_default_normal_block;
    const FastFileBlock* m_default_temp_block;

    static std::unique_ptr<RenderingContext> BuildContext(const IDataRepository* repository, StructureInformation* asset);
};
