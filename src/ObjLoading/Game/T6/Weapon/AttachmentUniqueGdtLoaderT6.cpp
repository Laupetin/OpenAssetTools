#include "AttachmentUniqueGdtLoaderT6.h"

#include "AttachmentUniqueInfoStringLoaderT6.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class GdtLoaderAttachmentUnique final : public AssetCreator<AssetAttachmentUnique>
    {
    public:
        GdtLoaderAttachmentUnique(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
            : m_gdt(gdt),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* gdtEntry = m_gdt.GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT_UNIQUE, assetName);
            if (gdtEntry == nullptr)
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromGdtProperties(*gdtEntry))
            {
                std::cerr << std::format("Failed to read attachment unique gdt entry: \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        T6::attachment_unique::InfoStringLoader m_info_string_loader;
    };
} // namespace

namespace T6::attachment_unique
{
    std::unique_ptr<AssetCreator<AssetAttachmentUnique>> CreateGdtLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderAttachmentUnique>(memory, searchPath, gdt, zone);
    }
} // namespace T6::attachment_unique
