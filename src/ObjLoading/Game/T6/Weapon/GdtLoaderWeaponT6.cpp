#include "GdtLoaderWeaponT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderWeaponT6.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

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
                std::cerr << std::format("Failed to read weapon gdt entry: \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        InfoStringLoaderWeapon m_info_string_loader;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateGdtWeaponLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderWeapon>(memory, searchPath, gdt, zone);
    }
} // namespace T6
