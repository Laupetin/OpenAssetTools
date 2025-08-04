#include "GdtLoaderPhysPresetT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetT6.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

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
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_PHYS_PRESET, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                std::cerr << std::format("Failed to read phys preset gdt entry: \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        T6::phys_preset::InfoStringLoader m_info_string_loader;
    };
} // namespace

namespace T6::phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateGdtLoader(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderPhysPreset>(memory, gdt, zone);
    }
} // namespace T6::phys_preset
