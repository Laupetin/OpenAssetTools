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
            auto mapGfxFile = m_search_path.Open("custom_map/map_gfx.fbx");
            if (!mapGfxFile.IsOpen())
                return AssetCreationResult::NoAction();

            printf("Loading map data...\n");

            // create map info from the obj file
            customMapInfo* mapInfo = CustomMapInfo::createCustomMapInfo(m_zone.m_name, m_search_path);
            if (mapInfo == NULL)
                return AssetCreationResult::Failure();

            printf("Creating map from data...\n");
            
            // linker will add all the assets needed
            CustomMapLinker* linker = new CustomMapLinker(m_memory, m_search_path, m_zone, context);
            bool result = linker->linkCustomMap(mapInfo);

            //auto gfxWorldAsset = context.LoadDependency<AssetFootstepTable>("default_1st_person");
            //return AssetCreationResult::Success(gfxWorldAsset);

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

namespace T6
{
    std::unique_ptr<AssetCreator<AssetGfxWorld>> CreateCustomMapLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<CustomMapLoader>(memory, searchPath, zone);
    }
} // namespace T6
