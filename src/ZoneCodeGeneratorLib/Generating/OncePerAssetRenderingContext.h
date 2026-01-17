#pragma once

#include "BaseRenderingContext.h"
#include "Domain/Computations/MemberComputations.h"
#include "Domain/Information/StructureInformation.h"
#include "Persistence/IDataRepository.h"

#include <string>
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

class OncePerAssetRenderingContext : public BaseRenderingContext
{
public:
    static std::unique_ptr<OncePerAssetRenderingContext> BuildContext(const IDataRepository* repository, StructureInformation* asset);

    StructureInformation* m_asset;

    std::vector<RenderingUsedType*> m_used_types;
    std::vector<RenderingUsedType*> m_used_structures;
    std::vector<RenderingUsedType*> m_referenced_assets;
    bool m_has_actions;

private:
    OncePerAssetRenderingContext(std::string game, Architecture gameArchitecture, std::vector<const FastFileBlock*> fastFileBlocks);

    RenderingUsedType* AddUsedType(std::unique_ptr<RenderingUsedType> usedType);
    RenderingUsedType* GetBaseType(const IDataRepository* repository, MemberComputations* computations, RenderingUsedType* usedType);
    void AddMembersToContext(const IDataRepository* repository, StructureInformation* info);
    void ScanUsedTypeIfNeeded(const IDataRepository* repository, MemberComputations* computations, RenderingUsedType* usedType);
    void MakeAsset(const IDataRepository* repository, StructureInformation* asset);
    void CreateUsedTypeCollections();
    bool UsedTypeHasActions(const RenderingUsedType* usedType) const;

    std::unordered_map<const DataDefinition*, std::unique_ptr<RenderingUsedType>> m_used_types_lookup;
};
