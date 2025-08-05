#pragma once

#include "Asset/IAssetPostProcessor.h"
#include "Asset/ZoneDefinitionContext.h"
#include "Image/IPak/IPakCreator.h"
#include "SearchPath/IOutputPath.h"

namespace image
{
    class AbstractIPakPostProcessor : public IAssetPostProcessor
    {
    public:
        AbstractIPakPostProcessor(const ZoneDefinitionContext& zoneDefinition,
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
        IPakCreator& m_ipak_creator;
        IOutputPath& m_out_dir;

        unsigned m_obj_container_index;
        IPakToCreate* m_current_ipak;
        unsigned m_current_ipak_start_index;
        unsigned m_current_ipak_end_index;
    };

    template<typename AssetType> class IPakPostProcessor final : public AbstractIPakPostProcessor
    {
    public:
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        IPakPostProcessor(const ZoneDefinitionContext& zoneDefinition,
                          ISearchPath& searchPath,
                          ZoneAssetCreationStateContainer& zoneStates,
                          IOutputPath& outDir)
            : AbstractIPakPostProcessor(zoneDefinition, searchPath, zoneStates, outDir)
        {
        }

        [[nodiscard]] asset_type_t GetHandlingAssetType() const override
        {
            return AssetType::EnumEntry;
        }
    };
} // namespace image
