#include "LoaderBSP_T6.h"

#include "BSPCreator.h"
#include "BSPUtil.h"
#include "Linker/BSPLinker.h"

using namespace BSP;

namespace
{
    class BSPLoader final : public IAssetCreator
    {
    public:
        BSPLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        std::optional<asset_type_t> GetHandlingAssetType() const override
        {
            // don't handle any asset types
            return std::nullopt;
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            // BSP assets are added in the finalize zone step
            return AssetCreationResult::NoAction();
        }

        bool FinalizeZone(AssetCreationContext& context) override
        {
            const auto bsp = BSP::createBSPData(m_zone.m_name, m_search_path);
            if (bsp == nullptr)
                return false;

            BSPLinker linker(m_memory, m_search_path, context);
            const auto result = linker.linkBSP(*bsp);
            if (!result)
                con::error("BSP link has failed.");

            return result;
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace

namespace BSP
{
    std::unique_ptr<IAssetCreator> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<BSPLoader>(memory, searchPath, zone);
    }
} // namespace BSP
