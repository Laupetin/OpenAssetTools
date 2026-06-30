#include "WeaponInfoStringLoaderT5.h"

#include "Game/T5/InfoString/InfoStringToStructConverter.h"
#include "Game/T5/T5.h"
#include "Game/T5/Weapon/WeaponFields.h"
#include "Game/T5/Weapon/WeaponStrings.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Weapon/WeaponCommon.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T5;

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

            case WFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case WFT_EXPLOSION_TAG:
                return ConvertScriptString(value, field.iOffset);

            case WFT_NOTETRACKSOUNDMAP:
                return ConvertNotetrackSoundMap(field, value);

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
                con::error("Failed to parse hide tags as array");
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                con::error("Cannot have more than {} hide tags!", std::extent_v<decltype(WeaponFullDef::hideTags)>);
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

        [[nodiscard]] bool ConvertBounceSounds(const cspField_t& field, const std::string& value) const
        {
            auto*** bounceSound = reinterpret_cast<const char***>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            if (value.empty())
            {
                *bounceSound = nullptr;
                return true;
            }

            *bounceSound = m_memory.Alloc<const char*>(SURF_TYPE_NUM);
            for (auto i = 0u; i < SURF_TYPE_NUM; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];
                (*bounceSound)[i] = m_memory.Dup(currentBounceSound.c_str());
            }
            return true;
        }

        [[nodiscard]] bool ConvertNotetrackSoundMap(const cspField_t& field, const std::string& value)
        {
            std::vector<std::array<std::string, 2>> pairs;
            if (!ParseAsArray(value, pairs))
            {
                con::error("Failed to parse notetracksoundmap as pairs");
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                con::error("Cannot have more than {} notetracksoundmap entries!", std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>);
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

    bool LoadAccuracyGraph(const std::string& graphName,
                           vec2_t*& originalGraphKnots,
                           int& originalGraphKnotCount,
                           vec2_t*& graphKnots,
                           int& graphKnotCount,
                           AssetCreationContext& context)
    {
        auto* accuracyGraphAsset = context.LoadSubAsset<SubAssetAccuracyGraph>(graphName);
        if (!accuracyGraphAsset)
            return false;

        const auto* accuracyGraph = accuracyGraphAsset->Asset();

        assert(accuracyGraphAsset->m_dependencies.empty());
        assert(accuracyGraphAsset->m_used_script_strings.empty());
        assert(accuracyGraphAsset->m_indirect_asset_references.empty());

        originalGraphKnots = accuracyGraph->graphKnots;
        originalGraphKnotCount = accuracyGraph->graphKnotCount;

        graphKnots = accuracyGraph->graphKnots;
        graphKnotCount = accuracyGraph->graphKnotCount;

        return true;
    }

    bool LoadAccuracyGraphs(WeaponFullDef& weaponFullDef, AssetCreationContext& context)
    {
        if (weaponFullDef.weapDef.aiVsAiAccuracyGraphName && weaponFullDef.weapDef.aiVsAiAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsAiAccuracyGraph(weaponFullDef.weapDef.aiVsAiAccuracyGraphName),
                                   weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnots,
                                   weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnotCount,
                                   weaponFullDef.weapDef.aiVsAiAccuracyGraphKnots,
                                   weaponFullDef.weapDef.aiVsAiAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        if (weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName && weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsPlayerAccuracyGraph(weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName),
                                   weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnots,
                                   weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnotCount,
                                   weaponFullDef.weapDef.aiVsPlayerAccuracyGraphKnots,
                                   weaponFullDef.weapDef.aiVsPlayerAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        return true;
    }

    bool LoadFlameTable(const char* flameTableName, FlameTable*& flameTablePtr, AssetRegistration<AssetWeapon>& registration, AssetCreationContext& context)
    {
        if (flameTableName && flameTableName[0] != '\0')
        {
            auto* flameTableAsset = context.LoadSubAsset<SubAssetFlameTable>(flameTableName);
            if (!flameTableAsset)
                return false;

            for (auto* dependency : flameTableAsset->m_dependencies)
                registration.AddDependency(dependency);

            assert(flameTableAsset->m_used_script_strings.empty());
            assert(flameTableAsset->m_indirect_asset_references.empty());

            flameTablePtr = flameTableAsset->Asset();
        }

        return true;
    }

    bool LoadFlameTables(WeaponFullDef& weaponFullDef, AssetRegistration<AssetWeapon>& registration, AssetCreationContext& context)
    {
        return LoadFlameTable(weaponFullDef.weapDef.flameTableFirstPerson, weaponFullDef.weapDef.flameTableFirstPersonPtr, registration, context)
               && LoadFlameTable(weaponFullDef.weapDef.flameTableThirdPerson, weaponFullDef.weapDef.flameTableThirdPersonPtr, registration, context);
    }

    void LinkWeaponFullDefSubStructs(WeaponFullDef& weapon)
    {
        weapon.weapVariantDef.weapDef = &weapon.weapDef;
        weapon.weapDef.gunXModel = weapon.gunXModel;
        weapon.weapVariantDef.szXAnims = weapon.szXAnims;
        weapon.weapVariantDef.hideTags = weapon.hideTags;
        weapon.weapDef.notetrackSoundMapKeys = weapon.notetrackSoundMapKeys;
        weapon.weapDef.notetrackSoundMapValues = weapon.notetrackSoundMapValues;
        weapon.weapDef.worldModel = weapon.worldModel;
        weapon.weapDef.parallelBounce = weapon.parallelBounce;
        weapon.weapDef.perpendicularBounce = weapon.perpendicularBounce;
        weapon.weapDef.locationDamageMultipliers = weapon.locationDamageMultipliers;
    }

    bool IsDefaultWeapon(const WeaponFullDef& weapon)
    {
        return strcmp(weapon.weapVariantDef.szInternalName, "defaultweapon") == 0 || strcmp(weapon.weapVariantDef.szInternalName, "defaultweapon_mp") == 0;
    }

    void SetWeaponDefaults(WeaponFullDef& weapon)
    {
        if (IsDefaultWeapon(weapon))
            return;

        if (!weapon.weapDef.viewLastShotEjectEffect)
            weapon.weapDef.viewLastShotEjectEffect = weapon.weapDef.viewShellEjectEffect;
        if (!weapon.weapDef.worldLastShotEjectEffect)
            weapon.weapDef.worldLastShotEjectEffect = weapon.weapDef.worldShellEjectEffect;
        if (!weapon.weapDef.raiseSound)
            weapon.weapDef.raiseSound = "wpn_default_raise";
        if (!weapon.weapDef.putawaySound)
            weapon.weapDef.putawaySound = "wpn_default_putaway";
        if (!weapon.weapDef.pickupSound)
            weapon.weapDef.pickupSound = "wpn_default_pickup";
        if (!weapon.weapDef.ammoPickupSound)
            weapon.weapDef.ammoPickupSound = "wpn_default_ammo_pickup";
        if (!weapon.weapDef.emptyFireSound)
            weapon.weapDef.emptyFireSound = "wpn_default_no_ammo";
    }

    void SetupTransitionTimes(WeaponFullDef& weapon)
    {
        if (weapon.weapVariantDef.iAdsTransInTime <= 0)
            weapon.weapVariantDef.fOOPosAnimLength[0] = 1.0f / 300.0f; // 0.0033333334f;
        else
            weapon.weapVariantDef.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weapon.weapVariantDef.iAdsTransInTime);

        if (weapon.weapVariantDef.iAdsTransOutTime <= 0)
            weapon.weapVariantDef.fOOPosAnimLength[1] = 1.0f / 500.0f; // 0.0020000001f
        else
            weapon.weapVariantDef.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weapon.weapVariantDef.iAdsTransOutTime);
    }

    void CheckWeaponDamageRanges(WeaponFullDef& weapon)
    {
        if (strcmp(weapon.weapVariantDef.szInternalName, "none") == 0)
            return;

        if (weapon.weapDef.fMaxDamageRange <= 0.0)
            weapon.weapDef.fMaxDamageRange = 999999.0f;
        if (weapon.weapDef.fMinDamageRange <= 0.0)
            weapon.weapDef.fMinDamageRange = 999999.12f; // oddly specific number, no clue
    }

    void CheckCrosshairValues(WeaponFullDef& weapon)
    {
        if (weapon.weapDef.enemyCrosshairRange > 15000.0f)
            con::warn("Weapon {}: Enemy crosshair ranges should be less than 15000", weapon.weapVariantDef.szInternalName);
    }

    void CheckProjectileValues(WeaponFullDef& weapon)
    {
        if (weapon.weapDef.weapType != WEAPTYPE_PROJECTILE)
            return;

        if (weapon.weapDef.iProjectileSpeed <= 0)
            con::warn("Weapon {}: Projectile speed must be greater than 0.0", weapon.weapVariantDef.szDisplayName);

        if (weapon.weapDef.destabilizationCurvatureMax >= 1000000000.0f || weapon.weapDef.destabilizationCurvatureMax < 0.0f)
            con::warn("Weapon {}: Destabilization angle must be between 0 and 45 degrees", weapon.weapVariantDef.szDisplayName);

        if (weapon.weapDef.destabilizationRateTime < 0.0f)
            con::warn("Weapon {}: Destabilization rate time must be non-negative", weapon.weapVariantDef.szDisplayName);
    }

    void CheckSharedAmmoValues(WeaponFullDef& weapon)
    {
        if (weapon.weapDef.sharedAmmo)
        {
            if (weapon.weapVariantDef.szAmmoName)
                utils::MakeStringLowerCase(const_cast<char*>(weapon.weapVariantDef.szAmmoName));

            if (weapon.weapVariantDef.szClipName)
                utils::MakeStringLowerCase(const_cast<char*>(weapon.weapVariantDef.szClipName));
        }
        else
        {
            weapon.weapVariantDef.szAmmoName = "";
            weapon.weapVariantDef.szClipName = "";
        }
    }
} // namespace

namespace weapon
{
    InfoStringLoaderT5::InfoStringLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderT5::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context) const
    {
        auto* weaponFullDef = m_memory.Alloc<WeaponFullDef>();
        weaponFullDef->weapVariantDef.szInternalName = m_memory.Dup(assetName.c_str());

        LinkWeaponFullDefSubStructs(*weaponFullDef);

        AssetRegistration<AssetWeapon> registration(assetName, &weaponFullDef->weapVariantDef);

        InfoStringToWeaponConverter converter(
            infoString, *weaponFullDef, m_zone.m_script_strings, m_memory, context, registration, weapon_fields, std::extent_v<decltype(weapon_fields)>);
        if (!converter.Convert())
        {
            con::error("Failed to parse weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        if (!LoadAccuracyGraphs(*weaponFullDef, context))
        {
            con::error("Failed to load accuracy tables of weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        if (!LoadFlameTables(*weaponFullDef, registration, context))
        {
            con::error("Failed to load flame tables of weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        SetWeaponDefaults(*weaponFullDef);
        SetupTransitionTimes(*weaponFullDef);
        CheckWeaponDamageRanges(*weaponFullDef);
        CheckCrosshairValues(*weaponFullDef);
        CheckProjectileValues(*weaponFullDef);
        CheckSharedAmmoValues(*weaponFullDef);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace weapon
