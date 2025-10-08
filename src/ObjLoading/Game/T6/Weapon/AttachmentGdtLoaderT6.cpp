#include "AttachmentGdtLoaderT6.h"

#include "AttachmentInfoStringLoaderT6.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class GdtLoaderAttachment final : public AssetCreator<AssetAttachment>
    {
    public:
        GdtLoaderAttachment(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
            : m_gdt(gdt),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                con::error("Failed to read attachment gdt entry: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        attachment::InfoStringLoaderT6 m_info_string_loader;
    };
} // namespace

namespace attachment
{
    std::unique_ptr<AssetCreator<AssetAttachment>> CreateGdtLoaderT6(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderAttachment>(memory, searchPath, gdt, zone);
    }
} // namespace attachment
