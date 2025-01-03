#pragma once

#include "Asset/IAssetPostProcessor.h"
#include "Asset/ZoneDefinitionContext.h"
#include "Iwd/IwdCreator.h"

#include <filesystem>

class AbstractImageIwdPostProcessor : public IAssetPostProcessor
{
public:
    AbstractImageIwdPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                                  ISearchPath& searchPath,
                                  ZoneAssetCreationStateContainer& zoneStates,
                                  const std::filesystem::path& outDir);

    static bool AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition);

    void PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context) override;
    void FinalizeZone(AssetCreationContext& context) override;

private:
    void FindNextObjContainer(AssetCreationContext& context);

    const ZoneDefinitionContext& m_zone_definition;
    ISearchPath& m_search_path;
    IwdCreator& m_iwd_creator;
    const std::filesystem::path& m_out_dir;

    bool m_initialized;
    unsigned m_obj_container_index;
    IwdToCreate* m_current_iwd;
    unsigned m_current_iwd_start_index;
    unsigned m_current_iwd_end_index;
};

template<typename AssetType> class ImageIwdPostProcessor final : public AbstractImageIwdPostProcessor
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    ImageIwdPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                          ISearchPath& searchPath,
                          ZoneAssetCreationStateContainer& zoneStates,
                          const std::filesystem::path& outDir)
        : AbstractImageIwdPostProcessor(zoneDefinition, searchPath, zoneStates, outDir)
    {
    }

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    };
};
