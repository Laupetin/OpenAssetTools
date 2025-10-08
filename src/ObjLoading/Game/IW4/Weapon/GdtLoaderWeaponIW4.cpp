#include "GdtLoaderWeaponIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderWeaponIW4.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW4;

namespace
{
    class GdtLoaderWeapon final : public AssetCreator<AssetWeapon>
    {
    public:
        GdtLoaderWeapon(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
            : m_gdt(gdt),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_WEAPON, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                con::error("Failed to read weapon gdt entry: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        weapon::InfoStringLoaderIW4 m_info_string_loader;
    };
} // namespace

namespace weapon
{
    std::unique_ptr<AssetCreator<IW4::AssetWeapon>> CreateGdtLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderWeapon>(memory, searchPath, gdt, zone);
    }
} // namespace weapon
