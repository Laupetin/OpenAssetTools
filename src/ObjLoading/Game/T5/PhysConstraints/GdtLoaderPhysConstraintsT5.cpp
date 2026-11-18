#include "GdtLoaderPhysConstraintsT5.h"

#include "Game/T5/ObjConstantsT5.h"
#include "Game/T5/T5.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderPhysConstraintsT5.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T5;

namespace
{
    class GdtLoaderPhysConstraints final : public AssetCreator<AssetPhysConstraints>
    {
    public:
        GdtLoaderPhysConstraints(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
            : m_gdt(gdt),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(GDF_FILENAME_PHYS_CONSTRAINTS, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                con::error("Failed to read phys constraints gdt entry: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        phys_constraints::InfoStringLoaderT5 m_info_string_loader;
    };
} // namespace

namespace phys_constraints
{
    std::unique_ptr<AssetCreator<AssetPhysConstraints>> CreateGdtLoaderT5(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderPhysConstraints>(memory, searchPath, gdt, zone);
    }
} // namespace phys_constraints
