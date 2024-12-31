#include "GdtLoaderAttachmentT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderAttachmentT6.h"

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
                std::cerr << std::format("Failed to read attachment gdt entry: \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        IGdtQueryable& m_gdt;
        InfoStringLoaderAttachment m_info_string_loader;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetAttachment>> CreateGdtAttachmentLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone)
    {
        return std::make_unique<GdtLoaderAttachment>(memory, searchPath, gdt, zone);
    }
} // namespace T6
