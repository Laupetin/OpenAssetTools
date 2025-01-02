#pragma once

#include "Asset/IAssetPostProcessor.h"
#include "Asset/ZoneDefinitionContext.h"
#include "Image/IPak/IPakCreator.h"

#include <filesystem>

class AbstractImageIPakPostProcessor : public IAssetPostProcessor
{
public:
    AbstractImageIPakPostProcessor(const ZoneDefinitionContext& zoneDefinition, ISearchPath& searchPath, const std::filesystem::path& outDir);

    static bool AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition);

    void PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context) override;
    void FinalizeZone(AssetCreationContext& context) override;

private:
    void FindNextObjContainer(AssetCreationContext& context);

    const ZoneDefinitionContext& m_zone_definition;
    ISearchPath& m_search_path;
    const std::filesystem::path& m_out_dir;

    bool m_initialized;
    unsigned m_obj_container_index;
    IPakToCreate* m_current_ipak;
    unsigned m_current_ipak_start_index;
    unsigned m_current_ipak_end_index;
};

template<typename AssetType> class ImageIPakPostProcessor final : public AbstractImageIPakPostProcessor
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    ImageIPakPostProcessor(const ZoneDefinitionContext& zoneDefinition, ISearchPath& searchPath, const std::filesystem::path& outDir)
        : AbstractImageIPakPostProcessor(zoneDefinition, searchPath, outDir)
    {
    }

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    };
};
