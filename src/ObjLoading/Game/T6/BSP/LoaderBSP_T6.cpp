#include "LoaderBSP_T6.h"
#include "BSPCreator.h"
#include "CustomMapLinker.h"

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
            auto mapGfxFile = m_search_path.Open("BSP/map_gfx.fbx");
            if (!mapGfxFile.IsOpen())
                return AssetCreationResult::NoAction();

            BSPData* BSP = BSP::createBSPData(m_zone.m_name, m_search_path);
            if (BSP == nullptr)
                return AssetCreationResult::Failure();
            
            CustomMapLinker linker(m_memory, m_search_path, m_zone, context);
            bool result = linker.linkCustomMap(BSP);

            if (result)
            {
                auto gfxWorldAsset = context.LoadDependency<AssetGfxWorld>(BSP->bspName);
                _ASSERT(gfxWorldAsset != nullptr);
                return AssetCreationResult::Success(gfxWorldAsset);
            }
            else
                return AssetCreationResult::Failure();
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
