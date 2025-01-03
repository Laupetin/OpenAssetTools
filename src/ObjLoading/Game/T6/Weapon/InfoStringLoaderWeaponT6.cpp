#include "InfoStringLoaderWeaponT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/Weapon/WeaponFields.h"
#include "Game/T6/Weapon/WeaponStrings.h"
#include "InfoStringLoaderAttachmentUniqueT6.h"
#include "Weapon/AccuracyGraphLoader.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);

            case WFT_WEAPONCLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);

            case WFT_OVERLAYRETICLE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case WFT_PENETRATE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);

            case WFT_IMPACT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);

            case WFT_STANCE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);

            case WFT_PROJ_EXPLOSION:
                return ConvertEnumInt(field.szName, value, field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);

            case WFT_OFFHAND_CLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);

            case WFT_OFFHAND_SLOT:
                return ConvertEnumInt(field.szName, value, field.iOffset, offhandSlotNames, std::extent_v<decltype(offhandSlotNames)>);

            case WFT_ANIMTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);

            case WFT_ACTIVE_RETICLE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);

            case WFT_GUIDED_MISSILE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);

            case WFT_BOUNCE_SOUND:
                return ConvertBounceSounds(field, value);

            case WFT_STICKINESS:
                return ConvertEnumInt(field.szName, value, field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);

            case WFT_ROTATETYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, rotateTypeNames, std::extent_v<decltype(rotateTypeNames)>);

            case WFT_OVERLAYINTERFACE:
                return ConvertEnumInt(field.szName, value, field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);

            case WFT_INVENTORYTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);

            case WFT_FIRETYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);

            case WFT_CLIPTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapClipTypeNames, std::extent_v<decltype(szWeapClipTypeNames)>);

            case WFT_AMMOCOUNTER_CLIPTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
            case WFT_ICONRATIO_INDICATOR:
                return ConvertEnumInt(field.szName, value, field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);

            case WFT_BARRELTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, barrelTypeNames, std::extent_v<decltype(barrelTypeNames)>);

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

            case WFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            default:
                assert(false);
                return false;
            }
        }

        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse hide tags as array\n";
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                std::cerr << std::format("Cannot have more than {} hide tags!\n", std::extent_v<decltype(WeaponFullDef::hideTags)>);
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (valueArray.size() < std::extent_v<decltype(WeaponFullDef::hideTags)>)
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

            auto currentHideTag = 0u;
            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto& currentValue = valueArray[currentHideTag];
                const auto scrString =
                    !currentValue.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                hideTags[currentHideTag] = scrString;
                m_registration.AddScriptString(scrString);
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

            assert(std::extent_v<decltype(bounceSoundSuffixes)> == SURF_TYPE_NUM);
            *bounceSound = m_memory.Alloc<const char*>(SURF_TYPE_NUM);
            for (auto i = 0u; i < SURF_TYPE_NUM; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];
                (*bounceSound)[i] = m_memory.Dup(currentBounceSound.c_str());
            }
            return true;
        }

        _NODISCARD bool ConvertNotetrackSoundMap(const cspField_t& field, const std::string& value)
        {
            std::vector<std::array<std::string, 2>> pairs;
            if (!ParseAsArray(value, pairs))
            {
                std::cerr << "Failed to parse notetracksoundmap as pairs\n";
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                std::cerr << "Cannot have more than " << std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)> << " notetracksoundmap entries!\n";
                return false;
            }

            auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto* values = &keys[std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>];
            auto currentEntryNum = 0u;

            if (pairs.size() < std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            for (; currentEntryNum < pairs.size(); currentEntryNum++)
            {
                const auto& currentValue = pairs[currentEntryNum];
                const auto keyScriptString = !currentValue[0].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[0])
                                                                      : m_zone_script_strings.AddOrGetScriptString(nullptr);
                const auto valueScriptString = !currentValue[1].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[1])
                                                                        : m_zone_script_strings.AddOrGetScriptString(nullptr);

                keys[currentEntryNum] = keyScriptString;
                m_registration.AddScriptString(keyScriptString);

                values[currentEntryNum] = valueScriptString;
                m_registration.AddScriptString(valueScriptString);
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
                *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* camo = m_context.LoadDependency<AssetWeaponCamo>(value);

            if (camo == nullptr)
            {
                std::cerr << std::format("Failed to load camo asset \"{}\"\n", value);
                return false;
            }

            m_registration.AddDependency(camo);
            *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = camo->Asset();

            return true;
        }

        _NODISCARD bool ConvertAttachments(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse attachments as array\n";
                return false;
            }

            auto** attachments = reinterpret_cast<WeaponAttachment**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            for (const auto& attachmentName : valueArray)
            {
                auto* attachmentAssetInfo = m_context.LoadDependency<AssetAttachment>(attachmentName);
                if (attachmentAssetInfo == nullptr)
                {
                    std::cerr << std::format("Failed to load attachment asset \"{}\"\n", attachmentName);
                    return false;
                }

                auto* attachmentAsset = attachmentAssetInfo->Asset();

                if (static_cast<unsigned>(attachmentAsset->attachmentType) >= ATTACHMENT_TYPE_COUNT)
                {
                    std::cerr << std::format(
                        "Invalid attachment type {} for attachment asset \"{}\"\n", static_cast<unsigned>(attachmentAsset->attachmentType), attachmentName);
                    return false;
                }

                if (attachments[attachmentAsset->attachmentType] != nullptr)
                {
                    std::cerr << std::format("Already loaded attachment with same type {}: \"{}\", \"{}\"\n",
                                             static_cast<unsigned>(attachmentAsset->attachmentType),
                                             attachments[attachmentAsset->attachmentType]->szInternalName,
                                             attachmentName);
                    return false;
                }

                attachments[attachmentAsset->attachmentType] = attachmentAsset;
                m_registration.AddDependency(attachmentAssetInfo);
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
                std::cerr << "Failed to parse attachment uniques as array\n";
                return false;
            }

            auto** attachmentUniques = reinterpret_cast<WeaponAttachmentUnique**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto attachmentCombinationIndex = std::extent_v<decltype(WeaponFullDef::attachments)>;

            for (const auto& attachmentUniqueName : valueArray)
            {
                auto* attachmentUniqueAssetInfo = m_context.LoadDependency<AssetAttachmentUnique>(attachmentUniqueName);
                if (attachmentUniqueAssetInfo == nullptr)
                {
                    std::cerr << std::format("Failed to load attachment unique asset \"{}\"\n", attachmentUniqueName);
                    return false;
                }

                auto* attachmentUniqueAsset = attachmentUniqueAssetInfo->Asset();

                if (HasMoreThanOneAttachmentSetInMask(attachmentUniqueAsset->combinedAttachmentTypeMask))
                {
                    if (attachmentCombinationIndex >= std::extent_v<decltype(WeaponFullDef::attachmentUniques)>)
                    {
                        std::cerr << std::format(
                            "Cannot have more than {} combined attachment attachment unique entries!\n",
                            (std::extent_v<decltype(WeaponFullDef::attachmentUniques)> - std::extent_v<decltype(WeaponFullDef::attachments)>));
                        return false;
                    }

                    attachmentUniques[attachmentCombinationIndex++] = attachmentUniqueAsset;
                    m_registration.AddDependency(attachmentUniqueAssetInfo);
                }
                else
                {
                    if (static_cast<unsigned>(attachmentUniqueAsset->attachmentType) >= ATTACHMENT_TYPE_COUNT)
                    {
                        std::cerr << std::format("Invalid attachment type {} for attachment unique asset \"{}\"\n",
                                                 static_cast<unsigned>(attachmentUniqueAsset->attachmentType),
                                                 attachmentUniqueName);
                        return false;
                    }

                    if (attachmentUniques[attachmentUniqueAsset->attachmentType] != nullptr)
                    {
                        std::cerr << std::format("Already loaded attachment unique with same type {}: \"{}\", \"{}\"\n",
                                                 static_cast<unsigned>(attachmentUniqueAsset->attachmentType),
                                                 attachmentUniques[attachmentUniqueAsset->attachmentType]->szInternalName,
                                                 attachmentUniqueName);
                        return false;
                    }

                    attachmentUniques[attachmentUniqueAsset->attachmentType] = attachmentUniqueAsset;
                    m_registration.AddDependency(attachmentUniqueAssetInfo);
                }
            }

            return true;
        }

        bool ConvertAnimName(const cspField_t& field, const std::string& value)
        {
            if (ConvertString(value, field.iOffset))
            {
                if (!value.empty())
                    m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetXAnim>(value));

                return true;
            }

            return false;
        }

    public:
        InfoStringToWeaponConverter(const InfoString& infoString,
                                    WeaponFullDef& weaponFullDef,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    AssetRegistration<AssetWeapon>& registration,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &weaponFullDef, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void ConvertAccuracyGraph(
        const GenericGraph2D& graph, vec2_t*& originalGraphKnots, int& originalGraphKnotCount, vec2_t*& graphKnots, int& graphKnotCount, MemoryManager& memory)
    {
        originalGraphKnotCount = static_cast<int>(graph.knots.size());
        originalGraphKnots = memory.Alloc<vec2_t>(originalGraphKnotCount);

        for (auto i = 0; i < originalGraphKnotCount; i++)
        {
            const auto& commonKnot = graph.knots[i];
            originalGraphKnots[i].x = static_cast<float>(commonKnot.x);
            originalGraphKnots[i].y = static_cast<float>(commonKnot.y);
        }

        graphKnots = originalGraphKnots;
        graphKnotCount = originalGraphKnotCount;
    }

    bool LoadAccuracyGraphs(WeaponFullDef& weaponFullDef, MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
    {
        auto& accuracyGraphLoader = context.GetZoneAssetLoaderState<AccuracyGraphLoader>();

        if (weaponFullDef.weapDef.aiVsAiAccuracyGraphName && weaponFullDef.weapDef.aiVsAiAccuracyGraphName[0])
        {
            const auto* graph = accuracyGraphLoader.LoadAiVsAiGraph(searchPath, weaponFullDef.weapDef.aiVsAiAccuracyGraphName);
            if (!graph)
                return false;

            ConvertAccuracyGraph(*graph,
                                 weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnots,
                                 weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnotCount,
                                 weaponFullDef.weapDef.aiVsAiAccuracyGraphKnots,
                                 weaponFullDef.weapDef.aiVsAiAccuracyGraphKnotCount,
                                 memory);
        }

        if (weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName && weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName[0])
        {
            const auto* graph = accuracyGraphLoader.LoadAiVsPlayerGraph(searchPath, weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName);
            if (!graph)
                return false;

            ConvertAccuracyGraph(*graph,
                                 weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnots,
                                 weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnotCount,
                                 weaponFullDef.weapDef.aiVsPlayerAccuracyGraphKnots,
                                 weaponFullDef.weapDef.aiVsPlayerAccuracyGraphKnotCount,
                                 memory);
        }

        return true;
    }

    void LinkWeaponFullDefSubStructs(WeaponFullDef& weapon)
    {
        weapon.weapVariantDef.weapDef = &weapon.weapDef;
        weapon.weapVariantDef.attachments = weapon.attachments;
        weapon.weapVariantDef.attachmentUniques = weapon.attachmentUniques;
        weapon.weapDef.gunXModel = weapon.gunXModel;
        weapon.weapVariantDef.szXAnims = weapon.szXAnims;
        weapon.weapVariantDef.hideTags = weapon.hideTags;
        weapon.weapDef.notetrackSoundMapKeys = weapon.notetrackSoundMapKeys;
        weapon.weapDef.notetrackSoundMapValues = weapon.notetrackSoundMapValues;
        weapon.weapDef.worldModel = weapon.worldModel;
        weapon.weapVariantDef.attachViewModel = weapon.attachViewModel;
        weapon.weapVariantDef.attachWorldModel = weapon.attachWorldModel;
        weapon.weapVariantDef.attachViewModelTag = weapon.attachViewModelTag;
        weapon.weapVariantDef.attachWorldModelTag = weapon.attachWorldModelTag;
        weapon.weapDef.parallelBounce = weapon.parallelBounce;
        weapon.weapDef.perpendicularBounce = weapon.perpendicularBounce;
        weapon.weapDef.locationDamageMultipliers = weapon.locationDamageMultipliers;
    }

    void CalculateWeaponFields(WeaponFullDef& weapon)
    {
        // iAttachments
        weapon.weapVariantDef.iAttachments = 0;
        for (auto i = 1u; i < sizeof(WeaponVariantDef::iAttachments) * 8; i++) // Bit for default attachment always 0
        {
            if (weapon.attachments[i])
                weapon.weapVariantDef.iAttachments |= 1 << i;
        }

        if (weapon.weapVariantDef.iAdsTransInTime <= 0)
            weapon.weapVariantDef.fOOPosAnimLength[0] = 0.0033333334f;
        else
            weapon.weapVariantDef.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weapon.weapVariantDef.iAdsTransInTime);

        if (weapon.weapVariantDef.iAdsTransOutTime <= 0)
            weapon.weapVariantDef.fOOPosAnimLength[1] = 0.0020000001f;
        else
            weapon.weapVariantDef.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weapon.weapVariantDef.iAdsTransOutTime);
    }

    bool IsStringOverride(const char* baseString, const char* overrideString)
    {
        if (overrideString == nullptr || overrideString[0] == '\0')
            return false;

        if (baseString == nullptr || baseString[0] == '\0')
            return true;

        return strcmp(baseString, overrideString) != 0;
    }

    bool IsFxOverride(const FxEffectDef* baseEffect, const FxEffectDef* overrideEffect)
    {
        if (overrideEffect == nullptr)
            return false;

        if (baseEffect == nullptr)
            return true;

        return strcmp(baseEffect->name, overrideEffect->name) != 0;
    }

    void HandleSoundOverride(WeaponAttachmentUnique& attachmentUnique, const char* snd1, const char* snd2, const eAttachmentOverrideSounds sndOverrideIndex)
    {
        if (IsStringOverride(snd1, snd2))
            attachmentUnique.soundOverrides |= 1 << static_cast<unsigned>(sndOverrideIndex);
    }

    void HandleFxOverride(WeaponAttachmentUnique& attachmentUnique,
                          const FxEffectDef* effect1,
                          const FxEffectDef* effect2,
                          const eAttachmentOverrideEffects fxOverrideIndex)
    {
        if (IsFxOverride(effect1, effect2))
            attachmentUnique.effectOverrides |= 1 << static_cast<unsigned>(fxOverrideIndex);
    }

    void CalculateAttachmentFields(const WeaponFullDef& weapon, unsigned attachmentIndex, WeaponAttachmentUnique& attachmentUnique)
    {
        for (auto& val : attachmentUnique.animationOverrides)
            val = 0;

        for (auto animIndex = 0u; animIndex < std::extent_v<decltype(WeaponFullDef::szXAnims)>; animIndex++)
        {
            if (IsStringOverride(weapon.szXAnims[animIndex], attachmentUnique.szXAnims[animIndex]))
                attachmentUnique.animationOverrides[animIndex / 32] |= 1 << (animIndex % 32);
        }

        attachmentUnique.soundOverrides = 0;
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireSound, attachmentUnique.fireSound, ATTACHMENT_OVERRIDE_SOUND_FIRE);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireSoundPlayer, attachmentUnique.fireSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_PLAYER);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireLoopSound, attachmentUnique.fireLoopSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP);
        HandleSoundOverride(
            attachmentUnique, weapon.weapDef.fireLoopSoundPlayer, attachmentUnique.fireLoopSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_PLAYER);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireLoopEndSound, attachmentUnique.fireLoopEndSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_END);
        HandleSoundOverride(
            attachmentUnique, weapon.weapDef.fireLoopEndSoundPlayer, attachmentUnique.fireLoopEndSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LOOP_END_PLAYER);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireStartSound, attachmentUnique.fireStartSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_START);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireStopSound, attachmentUnique.fireStopSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_STOP);
        HandleSoundOverride(
            attachmentUnique, weapon.weapDef.fireStartSoundPlayer, attachmentUnique.fireStartSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_START_PLAYER);
        HandleSoundOverride(
            attachmentUnique, weapon.weapDef.fireStopSoundPlayer, attachmentUnique.fireStopSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_STOP_PLAYER);
        HandleSoundOverride(attachmentUnique, weapon.weapDef.fireLastSound, attachmentUnique.fireLastSound, ATTACHMENT_OVERRIDE_SOUND_FIRE_LAST);
        HandleSoundOverride(
            attachmentUnique, weapon.weapDef.fireLastSoundPlayer, attachmentUnique.fireLastSoundPlayer, ATTACHMENT_OVERRIDE_SOUND_FIRE_LAST_PLAYER);

        attachmentUnique.effectOverrides = 0;
        HandleFxOverride(attachmentUnique, weapon.weapDef.viewFlashEffect, attachmentUnique.viewFlashEffect, ATTACHMENT_OVERRIDE_EFFECT_VIEW_FLASH);
        HandleFxOverride(attachmentUnique, weapon.weapDef.worldFlashEffect, attachmentUnique.worldFlashEffect, ATTACHMENT_OVERRIDE_EFFECT_WORLD_FLASH);

        attachmentUnique.childLink = 0;
        if (attachmentUnique.combinedAttachmentTypeMask == 0)
        {
            WeaponAttachmentUnique* lastSibling = nullptr;
            for (auto attachmentUniqueIndex = std::extent_v<decltype(WeaponFullDef::attachments)>;
                 attachmentUniqueIndex < std::extent_v<decltype(WeaponFullDef::attachmentUniques)>;
                 attachmentUniqueIndex++)
            {
                if (weapon.attachmentUniques[attachmentUniqueIndex] != nullptr
                    && weapon.attachmentUniques[attachmentUniqueIndex]->combinedAttachmentTypeMask
                           & (1 << static_cast<unsigned>(attachmentUnique.attachmentType))
                    && weapon.attachmentUniques[attachmentUniqueIndex]->attachmentType != attachmentUnique.attachmentType)
                {
                    std::vector<eAttachment> attachments;
                    if (InfoStringLoaderAttachmentUnique::ExtractAttachmentsFromAssetName(weapon.attachmentUniques[attachmentUniqueIndex]->szInternalName,
                                                                                          attachments)
                        && attachments.front() == attachmentUnique.attachmentType)
                    {
                        if (lastSibling == nullptr)
                        {
                            attachmentUnique.childLink = attachmentUniqueIndex;
                            lastSibling = weapon.attachmentUniques[attachmentUniqueIndex];
                        }
                        else
                        {
                            lastSibling->siblingLink = attachmentUniqueIndex;
                            lastSibling = weapon.attachmentUniques[attachmentUniqueIndex];
                        }
                    }
                }
            }
        }
    }

    void CalculateAttachmentFields(const WeaponFullDef& weapon)
    {
        for (auto attachmentUniqueIndex = 0u; attachmentUniqueIndex < std::extent_v<decltype(WeaponFullDef::attachmentUniques)>; attachmentUniqueIndex++)
        {
            if (weapon.attachmentUniques[attachmentUniqueIndex] == nullptr)
                continue;

            CalculateAttachmentFields(weapon, attachmentUniqueIndex, *weapon.attachmentUniques[attachmentUniqueIndex]);
        }
    }
} // namespace

InfoStringLoaderWeapon::InfoStringLoaderWeapon(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : m_memory(memory),
      m_search_path(searchPath),
      m_zone(zone)
{
}

AssetCreationResult InfoStringLoaderWeapon::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
{
    auto* weaponFullDef = m_memory.Alloc<WeaponFullDef>();
    weaponFullDef->weapVariantDef.szInternalName = m_memory.Dup(assetName.c_str());

    LinkWeaponFullDefSubStructs(*weaponFullDef);

    AssetRegistration<AssetWeapon> registration(assetName, &weaponFullDef->weapVariantDef);

    InfoStringToWeaponConverter converter(
        infoString, *weaponFullDef, m_zone.m_script_strings, m_memory, context, registration, weapon_fields, std::extent_v<decltype(weapon_fields)>);
    if (!converter.Convert())
    {
        std::cerr << std::format("Failed to parse weapon: \"{}\"\n", assetName);
        return AssetCreationResult::Failure();
    }

    CalculateWeaponFields(*weaponFullDef);
    CalculateAttachmentFields(*weaponFullDef);

    LoadAccuracyGraphs(*weaponFullDef, m_memory, m_search_path, context);

    return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
}
