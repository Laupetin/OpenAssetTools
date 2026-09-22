#pragma once

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Information/StructureInformation.h"
#include "PerVariantRenderingContext.h"
#include "Persistence/IDataRepository.h"

#include <unordered_map>

class RenderingUsedType
{
public:
    RenderingUsedType(const DataDefinition* type, StructureInformation* info);

    bool m_members_loaded;
    const DataDefinition* m_type;
    StructureInformation* m_info;
    bool m_is_context_asset;

    bool m_non_runtime_reference_exists;
    bool m_non_embedded_reference_exists;
    bool m_array_reference_exists;
    bool m_pointer_array_reference_exists;
    bool m_pointer_array_reference_is_reusable;
};

class PerAssetRenderingContext : public PerVariantRenderingContext
{
public:
    PerAssetRenderingContext(const IDataRepository* repository, StructureInformation* asset, const GameVariant* variant);

    StructureInformation* m_asset;

    std::vector<RenderingUsedType*> m_used_types;
    std::vector<RenderingUsedType*> m_used_structures;
    std::vector<RenderingUsedType*> m_referenced_assets;
    bool m_has_actions;

private:
    RenderingUsedType* AddUsedType(std::unique_ptr<RenderingUsedType> usedType);
    RenderingUsedType* GetBaseType(const IDataRepository* repository, const MemberComputations* computations, RenderingUsedType* usedType);
    void AddMembersToContext(const IDataRepository* repository, const StructureInformation* info);
    void ScanUsedTypeIfNeeded(const IDataRepository* repository, const MemberComputations* computations, RenderingUsedType* usedType);
    void MakeAsset(const IDataRepository* repository, StructureInformation* asset);
    void CreateUsedTypeCollections();
    static bool UsedTypeHasActions(const RenderingUsedType* usedType);

    std::unordered_map<const DataDefinition*, std::unique_ptr<RenderingUsedType>> m_used_types_lookup;
};
