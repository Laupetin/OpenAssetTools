#pragma once

#include "Asset/IAssetPostProcessor.h"
#include "Asset/ZoneDefinitionContext.h"
#include "Iwd/IwdCreator.h"
#include "SearchPath/IOutputPath.h"

namespace image
{
    class AbstractIwdPostProcessor : public IAssetPostProcessor
    {
    public:
        AbstractIwdPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                                 ISearchPath& searchPath,
                                 ZoneAssetCreationStateContainer& zoneStates,
                                 IOutputPath& outDir);

        static bool AppliesToZoneDefinition(const ZoneDefinitionContext& zoneDefinition);

        void PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context) override;
        void FinalizeZone(AssetCreationContext& context) override;

    private:
        void FindNextObjContainer();

        const ZoneDefinitionContext& m_zone_definition;
        ISearchPath& m_search_path;
        IwdCreator& m_iwd_creator;
        IOutputPath& m_out_dir;

        unsigned m_obj_container_index;
        IwdToCreate* m_current_iwd;
        unsigned m_current_iwd_start_index;
        unsigned m_current_iwd_end_index;
    };

    template<typename AssetType> class IwdPostProcessor final : public AbstractIwdPostProcessor
    {
    public:
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        IwdPostProcessor(const ZoneDefinitionContext& zoneDefinition, ISearchPath& searchPath, ZoneAssetCreationStateContainer& zoneStates, IOutputPath& outDir)
            : AbstractIwdPostProcessor(zoneDefinition, searchPath, zoneStates, outDir)
        {
        }

        [[nodiscard]] asset_type_t GetHandlingAssetType() const override
        {
            return AssetType::EnumEntry;
        }
    };
} // namespace image
