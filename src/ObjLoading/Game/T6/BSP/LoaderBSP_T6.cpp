#include "LoaderBSP_T6.h"
#include "BSPCreator.h"
#include "BSPUtil.h"
#include "Linker/BSPLinker.h"

namespace
{
    using namespace BSP;

    class BSPLoader final : public AssetCreator<AssetGfxWorld>
    {
    public:
        BSPLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            // custom maps must have a map_gfx file
            std::string mapGfxFileName = "map_gfx.fbx";
            auto mapGfxFile = m_search_path.Open(BSPUtil::getFileNameForBSPAsset(mapGfxFileName));
            if (!mapGfxFile.IsOpen())
                return AssetCreationResult::NoAction();

            std::unique_ptr<BSPData> bsp = BSP::createBSPData(m_zone.m_name, m_search_path);
            if (bsp == nullptr)
                return AssetCreationResult::Failure();
            
            BSPLinker linker(m_memory, m_search_path, context);
            return linker.linkBSP(bsp.get());
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace

namespace BSP
{
    std::unique_ptr<AssetCreator<AssetGfxWorld>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<BSPLoader>(memory, searchPath, zone);
    }
} // namespace BSP
