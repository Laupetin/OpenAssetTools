#include "FxImpactTableLoaderIW3.h"

#include "FxImpactTable/FxImpactTableCommon.h"
#include "Game/IW3/IW3.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace IW3;

namespace
{
    class FxImpactTableLoader : public AssetCreator<AssetImpactFx>
    {
    public:
        FxImpactTableLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = fx_impact_table::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            // TODO: Implement csv sound alias list loader

            return AssetCreationResult::NoAction();
        }

    private:
        ISearchPath& m_search_path;
    };
} // namespace

namespace fx_impact_table
{
    std::unique_ptr<AssetCreator<AssetImpactFx>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<FxImpactTableLoader>(memory, searchPath, zone);
    }
} // namespace fx_impact_table
