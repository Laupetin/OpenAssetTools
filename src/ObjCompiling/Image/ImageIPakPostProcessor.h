#pragma once

#include "Asset/IAssetPostProcessor.h"

#include <filesystem>

class AbstractImageIPakPostProcessor : public IAssetPostProcessor
{
public:
    AbstractImageIPakPostProcessor(ISearchPath& searchPath, const std::filesystem::path& outDir);

    void PostProcessAsset(XAssetInfoGeneric& assetInfo, AssetCreationContext& context) override;
    void FinalizeZone(AssetCreationContext& context) override;

private:
    ISearchPath& m_search_path;
    const std::filesystem::path& m_out_dir;
};

template<typename AssetType> class ImageIPakPostProcessor final : public AbstractImageIPakPostProcessor
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    ImageIPakPostProcessor(ISearchPath& searchPath, const std::filesystem::path& outDir)
        : AbstractImageIPakPostProcessor(searchPath, outDir)
    {
    }

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    };
};
