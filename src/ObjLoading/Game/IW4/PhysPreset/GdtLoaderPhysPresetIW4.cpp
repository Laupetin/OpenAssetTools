#include "GdtLoaderPhysPresetIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysPresetIW4.h"

#include <format>
#include <iostream>

using namespace IW4;

GdtLoaderPhysPreset::GdtLoaderPhysPreset(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone)
    : m_memory(memory),
      m_gdt(gdt),
      m_zone(zone)
{
}

AssetCreationResult GdtLoaderPhysPreset::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_PHYS_PRESET, assetName);
    if (gdtEntry == nullptr)
        return AssetCreationResult::NoAction();

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cerr << std::format("Failed to read phys preset gdt entry: \"{}\"\n", assetName);
        return AssetCreationResult::Failure();
    }

    InfoStringLoaderPhysPreset infoStringLoader(m_memory, m_zone);
    return infoStringLoader.CreateAsset(assetName, infoString, context);
}
