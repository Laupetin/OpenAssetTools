#include "AssetLoaderWeapon.h"

#include "AssetLoaderWeaponAttachmentUnique.h"
#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/WeaponFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <cstring>
#include <iostream>

using namespace T6;

namespace T6
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cout << "Failed to parse hide tags as array\n";
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                std::cout << "Cannot have more than " << std::extent_v<decltype(WeaponFullDef::hideTags)> << " hide tags!\n";
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (valueArray.size() < std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                m_used_script_string_list.emplace(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            auto currentHideTag = 0u;
            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto& currentValue = valueArray[currentHideTag];
                const auto scrString =
                    !currentValue.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                hideTags[currentHideTag] = scrString;
                m_used_script_string_list.emplace(scrString);
            }

            for (; currentHideTag < std::extent_v<decltype(WeaponFullDef::hideTags)>; currentHideTag++)
            {
                hideTags[currentHideTag] = m_zone_script_strings.GetScriptString(nullptr);
            }

            return true;
        }

        _NODISCARD bool ConvertBounceSounds(const cspField_t& field, const std::string& value) const
        {
            auto*** bounceSound = reinterpret_cast<const char***>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            if (value.empty())
            {
                *bounceSound = nullptr;
                return true;
            }

            assert(std::extent<decltype(bounceSoundSuffixes)>::value == SURF_TYPE_NUM);
            *bounceSound = static_cast<const char**>(m_memory->Alloc(sizeof(const char*) * SURF_TYPE_NUM));
            for (auto i = 0u; i < SURF_TYPE_NUM; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];
                (*bounceSound)[i] = m_memory->Dup(currentBounceSound.c_str());
            }
            return true;
        }

        _NODISCARD bool ConvertNotetrackSoundMap(const cspField_t& field, const std::string& value)
        {
            std::vector<std::pair<std::string, std::string>> pairs;
            if (!ParseAsPairs(value, pairs))
            {
                std::cout << "Failed to parse notetracksoundmap as pairs\n";
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                std::cout << "Cannot have more than " << std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)> << " notetracksoundmap entries!\n";
                return false;
            }

            auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto* values = &keys[std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>];
            auto currentEntryNum = 0u;

            if (pairs.size() < std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                m_used_script_string_list.emplace(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            for (; currentEntryNum < pairs.size(); currentEntryNum++)
            {
                const auto& currentValue = pairs[currentEntryNum];
                const auto keyScriptString = !currentValue.first.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue.first)
                                                                         : m_zone_script_strings.AddOrGetScriptString(nullptr);
                const auto valueScriptString = !currentValue.second.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue.second)
                                                                            : m_zone_script_strings.AddOrGetScriptString(nullptr);

                keys[currentEntryNum] = keyScriptString;
                m_used_script_string_list.emplace(keyScriptString);

                values[currentEntryNum] = valueScriptString;
                m_used_script_string_list.emplace(valueScriptString);
            }

            for (; currentEntryNum < std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>; currentEntryNum++)
            {
                const auto emptyScr = m_zone_script_strings.GetScriptString(nullptr);
                keys[currentEntryNum] = emptyScr;
                values[currentEntryNum] = emptyScr;
            }

            return true;
        }

        _NODISCARD bool ConvertWeaponCamo(const cspField_t& field, const std::string& value)
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* camo = m_loading_manager->LoadDependency(ASSET_TYPE_WEAPON_CAMO, value);

            if (camo == nullptr)
            {
                std::cout << "Failed to load camo asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(camo);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = camo->m_ptr;

            return true;
        }

        _NODISCARD bool ConvertAttachments(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cout << "Failed to parse attachments as array" << std::endl;
                return false;
            }

            auto** attachments = reinterpret_cast<WeaponAttachment**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            for (const auto& attachmentName : valueArray)
            {
                auto* attachmentAssetInfo = m_loading_manager->LoadDependency(ASSET_TYPE_ATTACHMENT, attachmentName);
                if (attachmentAssetInfo == nullptr)
                {
                    std::cout << "Failed to load attachment asset \"" << attachmentName << "\"" << std::endl;
                    return false;
                }

                auto* attachmentAsset = static_cast<WeaponAttachment*>(attachmentAssetInfo->m_ptr);

                if (static_cast<unsigned>(attachmentAsset->attachmentType) >= ATTACHMENT_TYPE_COUNT)
                {
                    std::cout << "Invalid attachment type " << attachmentAsset->attachmentType << " for attachment asset \"" << attachmentName << "\""
                              << std::endl;
                    return false;
                }

                if (attachments[attachmentAsset->attachmentType] != nullptr)
                {
                    std::cout << "Already loaded attachment with same type " << attachmentAsset->attachmentType << ": \""
                              << attachments[attachmentAsset->attachmentType]->szInternalName << "\", \"" << attachmentName << "\"" << std::endl;
                    return false;
                }

                attachments[attachmentAsset->attachmentType] = attachmentAsset;
                m_dependencies.emplace(attachmentAssetInfo);
            }

            return true;
        }

        _NODISCARD static bool HasMoreThanOneAttachmentSetInMask(const int mask)
        {
            // Check if int has more than 1 bit set
            return (mask & (mask - 1)) != 0;
        }

        _NODISCARD bool ConvertAttachmentUniques(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cout << "Failed to parse attachment uniques as array" << std::endl;
                return false;
            }

            auto** attachmentUniques = reinterpret_cast<WeaponAttachmentUnique**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto attachmentCombinationIndex = std::extent_v<decltype(WeaponFullDef::attachments)>;

            for (const auto& attachmentUniqueName : valueArray)
            {
                auto* attachmentUniqueAssetInfo = m_loading_manager->LoadDependency(ASSET_TYPE_ATTACHMENT_UNIQUE, attachmentUniqueName);
                if (attachmentUniqueAssetInfo == nullptr)
                {
                    std::cout << "Failed to load attachment unique asset \"" << attachmentUniqueName << "\"" << std::endl;
                    return false;
                }

                auto* attachmentUniqueAsset = static_cast<WeaponAttachmentUnique*>(attachmentUniqueAssetInfo->m_ptr);

                if (HasMoreThanOneAttachmentSetInMask(attachmentUniqueAsset->combinedAttachmentTypeMask))
                {
                    if (attachmentCombinationIndex >= std::extent_v<decltype(WeaponFullDef::attachmentUniques)>)
                    {
                        std::cout << "Cannot have more than "
                                  << (std::extent_v<decltype(WeaponFullDef::attachmentUniques)> - std::extent_v<decltype(WeaponFullDef::attachments)>)
                                  << " combined attachment attachment unique entries!" << std::endl;
                        return false;
                    }

                    attachmentUniques[attachmentCombinationIndex++] = attachmentUniqueAsset;
                    m_dependencies.emplace(attachmentUniqueAssetInfo);
                }
                else
                {
                    if (static_cast<unsigned>(attachmentUniqueAsset->attachmentType) >= ATTACHMENT_TYPE_COUNT)
                    {
                        std::cout << "Invalid attachment type " << attachmentUniqueAsset->attachmentType << " for attachment unique asset \""
                                  << attachmentUniqueName << "\"" << std::endl;
                        return false;
                    }

                    if (attachmentUniques[attachmentUniqueAsset->attachmentType] != nullptr)
                    {
                        std::cout << "Already loaded attachment unique with same type " << attachmentUniqueAsset->attachmentType << ": \""
                                  << attachmentUniques[attachmentUniqueAsset->attachmentType]->szInternalName << "\", \"" << attachmentUniqueName << "\""
                                  << std::endl;
                        return false;
                    }

                    attachmentUniques[attachmentUniqueAsset->attachmentType] = attachmentUniqueAsset;
                    m_dependencies.emplace(attachmentUniqueAssetInfo);
                }
            }

            return true;
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                return ConvertEnumInt(value, field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);

            case WFT_WEAPONCLASS:
                return ConvertEnumInt(value, field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);

            case WFT_OVERLAYRETICLE:
                return ConvertEnumInt(value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case WFT_PENETRATE_TYPE:
                return ConvertEnumInt(value, field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);

            case WFT_IMPACT_TYPE:
                return ConvertEnumInt(value, field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);

            case WFT_STANCE:
                return ConvertEnumInt(value, field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);

            case WFT_PROJ_EXPLOSION:
                return ConvertEnumInt(value, field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);

            case WFT_OFFHAND_CLASS:
                return ConvertEnumInt(value, field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);

            case WFT_OFFHAND_SLOT:
                return ConvertEnumInt(value, field.iOffset, offhandSlotNames, std::extent_v<decltype(offhandSlotNames)>);

            case WFT_ANIMTYPE:
                return ConvertEnumInt(value, field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);

            case WFT_ACTIVE_RETICLE_TYPE:
                return ConvertEnumInt(value, field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);

            case WFT_GUIDED_MISSILE_TYPE:
                return ConvertEnumInt(value, field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);

            case WFT_BOUNCE_SOUND:
                return ConvertBounceSounds(field, value);

            case WFT_STICKINESS:
                return ConvertEnumInt(value, field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);

            case WFT_ROTATETYPE:
                return ConvertEnumInt(value, field.iOffset, rotateTypeNames, std::extent_v<decltype(rotateTypeNames)>);

            case WFT_OVERLAYINTERFACE:
                return ConvertEnumInt(value, field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);

            case WFT_INVENTORYTYPE:
                return ConvertEnumInt(value, field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);

            case WFT_FIRETYPE:
                return ConvertEnumInt(value, field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);

            case WFT_CLIPTYPE:
                return ConvertEnumInt(value, field.iOffset, szWeapClipTypeNames, std::extent_v<decltype(szWeapClipTypeNames)>);

            case WFT_AMMOCOUNTER_CLIPTYPE:
                return ConvertEnumInt(value, field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
            case WFT_ICONRATIO_INDICATOR:
                return ConvertEnumInt(value, field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);

            case WFT_BARRELTYPE:
                return ConvertEnumInt(value, field.iOffset, barrelTypeNames, std::extent_v<decltype(barrelTypeNames)>);

            case WFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case WFT_EXPLOSION_TAG:
                return ConvertScriptString(value, field.iOffset);

            case WFT_NOTETRACKSOUNDMAP:
                return ConvertNotetrackSoundMap(field, value);

            case WFT_WEAPON_CAMO:
                return ConvertWeaponCamo(field, value);

            case WFT_ATTACHMENTS:
                return ConvertAttachments(field, value);

            case WFT_ATTACHMENT_UNIQUES:
                return ConvertAttachmentUniques(field, value);

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToWeaponConverter(const InfoString& infoString,
                                    WeaponFullDef* weaponFullDef,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager* memory,
                                    IAssetLoadingManager* manager,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, weaponFullDef, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

void AssetLoaderWeapon::LinkWeaponFullDefSubStructs(WeaponFullDef* weapon)
{
    weapon->weapVariantDef.weapDef = &weapon->weapDef;
    weapon->weapVariantDef.attachments = weapon->attachments;
    weapon->weapVariantDef.attachmentUniques = weapon->attachmentUniques;
    weapon->weapDef.gunXModel = weapon->gunXModel;
    weapon->weapVariantDef.szXAnims = weapon->szXAnims;
    weapon->weapVariantDef.hideTags = weapon->hideTags;
    weapon->weapDef.notetrackSoundMapKeys = weapon->notetrackSoundMapKeys;
    weapon->weapDef.notetrackSoundMapValues = weapon->notetrackSoundMapValues;
    weapon->weapDef.worldModel = weapon->worldModel;
    weapon->weapVariantDef.attachViewModel = weapon->attachViewModel;
    weapon->weapVariantDef.attachWorldModel = weapon->attachWorldModel;
    weapon->weapVariantDef.attachViewModelTag = weapon->attachViewModelTag;
    weapon->weapVariantDef.attachWorldModelTag = weapon->attachWorldModelTag;
    weapon->weapDef.parallelBounce = weapon->parallelBounce;
    weapon->weapDef.perpendicularBounce = weapon->perpendicularBounce;
    weapon->weapDef.locationDamageMultipliers = weapon->locationDamageMultipliers;
}

void AssetLoaderWeapon::CalculateWeaponFields(WeaponFullDef* weapon)
{
    // iAttachments
    weapon->weapVariantDef.iAttachments = 0;
    for (auto i = 1u; i < sizeof(WeaponVariantDef::iAttachments) * 8; i++) // Bit for default attachment always 0
    {
        if (weapon->attachments[i])
            weapon->weapVariantDef.iAttachments |= 1 << i;
    }

    if (weapon->weapVariantDef.iAdsTransInTime <= 0)
        weapon->weapVariantDef.fOOPosAnimLength[0] = 0.0033333334f;
    else
        weapon->weapVariantDef.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weapon->weapVariantDef.iAdsTransInTime);

    if (weapon->weapVariantDef.iAdsTransOutTime <= 0)
        weapon->weapVariantDef.fOOPosAnimLength[1] = 0.0020000001f;
    else
        weapon->weapVariantDef.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weapon->weapVariantDef.iAdsTransOutTime);
}

bool AssetLoaderWeapon::IsStringOverride(const char* str1, const char* str2)
{
    if ((str1 == nullptr) != (str2 == nullptr))
        return true;

    if (str1 == nullptr)
        return false;

    return strcmp(str1, str2) != 0;
}

bool AssetLoaderWeapon::IsFxOverride(const FxEffectDef* effect1, const FxEffectDef* effect2)
{
    if ((effect1 == nullptr) != (effect2 == nullptr))
        return true;

    if (effect1 == nullptr)
        return false;

    return strcmp(effect1->name, effect2->name) != 0;
}

void AssetLoaderWeapon::HandleSoundOverride(WeaponAttachmentUnique* attachmentUnique,
                                            const char* snd1,
                                            const char* snd2,
                                            const eAttachmentOverrideSounds sndOverrideIndex)
{
    if (IsStringOverride(snd1, snd2))
        attachmentUnique->soundOverrides |= 1 << static_cast<unsigned>(sndOverrideIndex);
}

void AssetLoaderWeapon::HandleFxOverride(WeaponAttachmentUnique* attachmentUnique,
                                         const FxEffectDef* effect1,
                                         const FxEffectDef* effect2,
                                         const eAttachmentOverrideEffects fxOverrideIndex)
{
    if (IsFxOverride(effect1, effect2))
        attachmentUnique->effectOverrides |= 1 << static_cast<unsigned>(fxOverrideIndex);
}

void AssetLoaderWeapon::CalculateAttachmentFields(const WeaponFullDef* weapon, unsigned attachmentIndex, WeaponAttachmentUnique* attachmentUnique)
{
    for (auto& val : attachmentUnique->animationOverrides)
        val = 0;

    for (auto animIndex = 0u; animIndex < std::extent_v<decltype(WeaponFullDef::szXAnims)>; animIndex++)
    {
        if (IsStringOverride(weapon->szXAnims[animIndex], attachmentUnique->szXAnims[animIndex]))
            attachmentUnique->animationOverrides[animIndex / 32] |= 1 << (animIndex % 32);
    }

    attachmentUnique->soundOverrides = 0;
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireSound, attachmentUnique->fireSound, ATTACHMENT_OVERRIDE_SOUND_FIRE);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireSoundPlayer, attachmentUnique->fireSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_PLAYER);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireLoopSound, attachmentUnique->fireLoopSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP);
    HandleSoundOverride(
        attachmentUnique, weapon->weapDef.fireLoopSoundPlayer, attachmentUnique->fireLoopSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_PLAYER);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireLoopEndSound, attachmentUnique->fireLoopEndSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_END);
    HandleSoundOverride(
        attachmentUnique, weapon->weapDef.fireLoopEndSoundPlayer, attachmentUnique->fireLoopEndSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_END_PLAYER);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireStartSound, attachmentUnique->fireStartSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_START);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireStopSound, attachmentUnique->fireStopSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_STOP);
    HandleSoundOverride(
        attachmentUnique, weapon->weapDef.fireStartSoundPlayer, attachmentUnique->fireStartSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_START_PLAYER);
    HandleSoundOverride(
        attachmentUnique, weapon->weapDef.fireStopSoundPlayer, attachmentUnique->fireStopSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_STOP_PLAYER);
    HandleSoundOverride(attachmentUnique, weapon->weapDef.fireLastSound, attachmentUnique->fireLastSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LAST);
    HandleSoundOverride(
        attachmentUnique, weapon->weapDef.fireLastSoundPlayer, attachmentUnique->fireLastSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LAST_PLAYER);

    attachmentUnique->effectOverrides = 0;
    HandleFxOverride(attachmentUnique, weapon->weapDef.viewFlashEffect, attachmentUnique->viewFlashEffect, ATTACHMENT_OVERRIDE_EFFECT_VIEW_FLASH);
    HandleFxOverride(attachmentUnique, weapon->weapDef.worldFlashEffect, attachmentUnique->worldFlashEffect, ATTACHMENT_OVERRIDE_EFFECT_WORLD_FLASH);

    attachmentUnique->childLink = 0;
    if (attachmentUnique->combinedAttachmentTypeMask == 0)
    {
        WeaponAttachmentUnique* lastSibling = nullptr;
        for (auto attachmentUniqueIndex = std::extent_v<decltype(WeaponFullDef::attachments)>;
             attachmentUniqueIndex < std::extent_v<decltype(WeaponFullDef::attachmentUniques)>;
             attachmentUniqueIndex++)
        {
            if (weapon->attachmentUniques[attachmentUniqueIndex] != nullptr
                && weapon->attachmentUniques[attachmentUniqueIndex]->combinedAttachmentTypeMask & (1 << static_cast<unsigned>(attachmentUnique->attachmentType))
                && weapon->attachmentUniques[attachmentUniqueIndex]->attachmentType != attachmentUnique->attachmentType)
            {
                std::vector<eAttachment> attachments;
                if (AssetLoaderWeaponAttachmentUnique::ExtractAttachmentsFromAssetName(weapon->attachmentUniques[attachmentUniqueIndex]->szInternalName,
                                                                                       attachments)
                    && attachments.front() == attachmentUnique->attachmentType)
                {
                    if (lastSibling == nullptr)
                    {
                        attachmentUnique->childLink = attachmentUniqueIndex;
                        lastSibling = weapon->attachmentUniques[attachmentUniqueIndex];
                    }
                    else
                    {
                        lastSibling->siblingLink = attachmentUniqueIndex;
                        lastSibling = weapon->attachmentUniques[attachmentUniqueIndex];
                    }
                }
            }
        }
    }
}

void AssetLoaderWeapon::CalculateAttachmentFields(WeaponFullDef* weapon)
{
    for (auto attachmentUniqueIndex = 0u; attachmentUniqueIndex < std::extent_v<decltype(WeaponFullDef::attachmentUniques)>; attachmentUniqueIndex++)
    {
        if (weapon->attachmentUniques[attachmentUniqueIndex] == nullptr)
            continue;

        CalculateAttachmentFields(weapon, attachmentUniqueIndex, weapon->attachmentUniques[attachmentUniqueIndex]);
    }
}

bool AssetLoaderWeapon::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    auto* weaponFullDef = memory->Create<WeaponFullDef>();
    memset(weaponFullDef, 0, sizeof(WeaponFullDef));
    LinkWeaponFullDefSubStructs(weaponFullDef);

    InfoStringToWeaponConverter converter(
        infoString, weaponFullDef, zone->m_script_strings, memory, manager, weapon_fields, std::extent_v<decltype(weapon_fields)>);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse weapon: \"" << assetName << "\"" << std::endl;
        return true;
    }

    weaponFullDef->weapVariantDef.szInternalName = memory->Dup(assetName.c_str());

    // TODO: Load accuracy graph and flametable
    CalculateWeaponFields(weaponFullDef);
    CalculateAttachmentFields(weaponFullDef);

    manager->AddAsset(ASSET_TYPE_WEAPON, assetName, &weaponFullDef->weapVariantDef, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}

void* AssetLoaderWeapon::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* weaponFullDef = memory->Create<WeaponFullDef>();
    memset(weaponFullDef, 0, sizeof(WeaponFullDef));
    LinkWeaponFullDefSubStructs(weaponFullDef);
    CalculateWeaponFields(weaponFullDef);
    weaponFullDef->weapVariantDef.szInternalName = memory->Dup(assetName.c_str());
    return weaponFullDef;
}

bool AssetLoaderWeapon::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderWeapon::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_WEAPON, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read weapon gdt entry: \"" << assetName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderWeapon::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderWeapon::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "weapons/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON, *file.m_stream))
    {
        std::cout << "Failed to read weapon raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
