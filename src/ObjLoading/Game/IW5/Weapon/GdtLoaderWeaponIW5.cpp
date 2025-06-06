#include "GdtLoaderWeaponIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/ObjConstantsIW5.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderWeaponIW5.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

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

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateGdtWeaponLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderWeapon>(memory, searchPath, gdt, zone);
    }
} // namespace IW5
