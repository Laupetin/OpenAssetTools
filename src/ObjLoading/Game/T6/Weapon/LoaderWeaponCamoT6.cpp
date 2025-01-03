#include "LoaderWeaponCamoT6.h"

#include "Game/T6/T6.h"
#include "JsonWeaponCamoLoaderT6.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class WeaponCamoLoader final : public AssetCreator<AssetWeaponCamo>
    {
    public:
        WeaponCamoLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(std::format("camo/{}.json", assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* weaponCamo = m_memory.Alloc<WeaponCamo>();
            weaponCamo->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetWeaponCamo> registration(assetName, weaponCamo);
            if (!LoadWeaponCamoAsJson(*file.m_stream, *weaponCamo, m_memory, context, registration))
            {
                std::cerr << std::format("Failed to load weapon camo \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetWeaponCamo>> CreateWeaponCamoLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<WeaponCamoLoader>(memory, searchPath);
    }
} // namespace T6
