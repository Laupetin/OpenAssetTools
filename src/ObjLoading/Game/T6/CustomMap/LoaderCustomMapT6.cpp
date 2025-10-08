#include "LoaderCustomMapT6.h"
#include "ProjectCreator.h"
#include "CustomMapLinker.h"

#include "Game/T6/T6.h"

#include <cstring>

using namespace T6;

namespace
{
    class CustomMapLoader final : public AssetCreator<AssetGfxWorld>
    {
    public:
        CustomMapLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            // custom maps must have a map_gfx file
            auto mapGfxFile = m_search_path.Open("custom_map/map_gfx.fbx");
            if (!mapGfxFile.IsOpen())
                return AssetCreationResult::NoAction();

            // create map info from the fbx file
            customMapInfo* mapInfo = ProjectCreator::createCustomMapInfo(m_zone.m_name, m_search_path);
            if (mapInfo == NULL)
                return AssetCreationResult::Failure();
            
            // linker will add all the assets needed
            CustomMapLinker* linker = new CustomMapLinker(m_memory, m_search_path, m_zone, context);
            bool result = linker->linkCustomMap(mapInfo);

            if (result)
            {
                auto gfxWorldAsset = context.LoadDependency<AssetGfxWorld>(mapInfo->bspName);
                _ASSERT(gfxWorldAsset != NULL);
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

namespace custom_map
{
    std::unique_ptr<AssetCreator<AssetGfxWorld>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<CustomMapLoader>(memory, searchPath, zone);
    }
} // namespace custom_map
