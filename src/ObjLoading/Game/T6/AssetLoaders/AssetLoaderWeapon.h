#pragma once
#include "Game/T6/T6.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "SearchPath/ISearchPath.h"

namespace T6
{
    class AssetLoaderWeapon final : public BasicAssetLoader<ASSET_TYPE_WEAPON, WeaponVariantDef>
    {
        static void LinkWeaponFullDefSubStructs(WeaponFullDef* weapon);

        static bool IsStringOverride(const char* str1, const char* str2);
        static bool IsFxOverride(FxEffectDef* effect1, FxEffectDef* effect2);
        static void HandleSoundOverride(WeaponAttachmentUnique* attachmentUnique, const char* snd1, const char* snd2, eAttachmentOverrideSounds sndOverrideIndex);
        static void HandleFxOverride(WeaponAttachmentUnique* attachmentUnique, FxEffectDef* effect1, FxEffectDef* effect2, eAttachmentOverrideEffects fxOverrideIndex);

        static void CalculateWeaponFields(WeaponFullDef* weapon);
        static void CalculateAttachmentFields(WeaponFullDef* weapon, unsigned attachmentIndex, WeaponAttachmentUnique* attachmentUnique);
        static void CalculateAttachmentFields(WeaponFullDef* weapon);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
