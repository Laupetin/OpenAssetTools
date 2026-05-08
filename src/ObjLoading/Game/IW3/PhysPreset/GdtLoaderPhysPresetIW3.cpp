#include "GdtLoaderPhysPresetIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/ObjConstantsIW3.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetIW3.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace IW3;

namespace
{
    class GdtLoaderPhysPreset final : public AssetCreator<AssetPhysPreset>
    {
    public:
        GdtLoaderPhysPreset(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone)
            : m_gdt(gdt),
              m_info_string_loader(memory, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(GDF_FILENAME_PHYS_PRESET, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                con::error("Failed to read phys preset gdt entry: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        phys_preset::InfoStringLoaderIW3 m_info_string_loader;
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW3::AssetPhysPreset>> CreateGdtLoaderIW3(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderPhysPreset>(memory, gdt, zone);
    }
} // namespace phys_preset
