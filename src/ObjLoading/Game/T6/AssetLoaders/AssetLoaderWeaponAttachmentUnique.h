#pragma once
#include "Game/T6/T6.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "SearchPath/ISearchPath.h"

namespace T6
{
    class AssetLoaderWeaponAttachmentUnique final : public BasicAssetLoader<ASSET_TYPE_ATTACHMENT_UNIQUE, WeaponAttachmentUnique>
    {
        static void LinkAttachmentUniqueFullSubStructs(WeaponAttachmentUniqueFull* attachmentUnique);
        static bool CalculateAttachmentUniqueFields(const std::string& assetName, WeaponAttachmentUniqueFull* attachmentUnique);

    public:
        static bool ExtractAttachmentsFromAssetName(const std::string& assetName, std::vector<eAttachment>& attachmentList);

        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
