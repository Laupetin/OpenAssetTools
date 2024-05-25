#include "AssetDumperWeapon.h"

#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW5/InfoString/WeaponFields.h"
#include "Game/IW5/ObjConstantsIW5.h"
#include "Weapon/AccuracyGraphWriter.h"

#include <bit>
#include <cassert>
#include <cstring>
#include <sstream>
#include <type_traits>
#include <unordered_set>

using namespace IW5;

namespace IW5
{
    class InfoStringFromWeaponConverter final : public InfoStringFromStructConverter
    {
    public:
        InfoStringFromWeaponConverter(const WeaponFullDef* structure,
                                      const cspField_t* fields,
                                      const size_t fieldCount,
                                      std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback)),
              m_weapon(structure)
        {
        }

    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);
                break;

            case WFT_WEAPONCLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);
                break;

            case WFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);
                break;

            case WFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);
                break;

            case WFT_IMPACT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);
                break;

            case WFT_STANCE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);
                break;

            case WFT_PER_SURFACE_TYPE_SOUND:
            {
                const auto* perSurfaceTypeSound = *reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

                if (perSurfaceTypeSound && perSurfaceTypeSound->name)
                {
                    const std::string firstBounceSound(perSurfaceTypeSound->name->soundName);
                    const auto endOfBouncePrefix = firstBounceSound.rfind("_default");
                    assert(endOfBouncePrefix != std::string::npos);

                    if (endOfBouncePrefix != std::string::npos)
                    {
                        m_info_string.SetValueForKey(std::string(field.szName), firstBounceSound.substr(0, endOfBouncePrefix));
                    }
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");
                }
                else
                    m_info_string.SetValueForKey(std::string(field.szName), "");

                break;
            }

            case WFT_STICKINESS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);
                break;

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_PICKUP:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);
                break;

            case WFT_HIDETAGS:
            {
                const auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::hideTags)>; i++)
                {
                    const auto& str = m_get_scr_string(hideTags[i]);
                    if (!str.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << str;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

            case WFT_NOTETRACKSOUNDMAP:
            {
                const auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                const auto* values = &keys[std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>];
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>; i++)
                {
                    const auto& key = m_get_scr_string(keys[i]);
                    const auto& value = m_get_scr_string(values[i]);
                    if (!key.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << key;

                        if (!value.empty())
                            ss << " " << value;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

            case WFT_NOTETRACKRUMBLEMAP:
            {
                const auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                const auto* values = &keys[std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapKeys)>];
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapKeys)>; i++)
                {
                    const auto& key = m_get_scr_string(keys[i]);
                    const auto& value = m_get_scr_string(values[i]);
                    if (!key.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << key;

                        if (!value.empty())
                            ss << " " << value;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

            case WFT_ANIM_NAME:
                FillFromString(std::string(field.szName), field.iOffset);
                break;

            case WFT_ATTACHMENT:
                FillFromAttachments(std::string(field.szName));
                break;

            case WFT_ANIM_OVERRIDES:
                FillFromAnimOverrides(std::string(field.szName));
                break;

            case WFT_SOUND_OVERRIDES:
                FillFromSoundOverrides(std::string(field.szName));
                break;

            case WFT_FX_OVERRIDES:
                FillFromFxOverrides(std::string(field.szName));
                break;

            case WFT_RELOAD_OVERRIDES:
                FillFromReloadOverrides(std::string(field.szName));
                break;

            case WFT_NOTETRACK_OVERRIDES:
                FillFromNoteTrackOverrides(std::string(field.szName));
                break;

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    private:
        void FillFromAttachments(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (const auto& scope : m_weapon->scopes)
            {
                if (scope && scope->szInternalName)
                {
                    if (!first)
                        ss << "\n";
                    else
                        first = false;

                    ss << scope->szInternalName;
                }
            }

            for (const auto& underBarrel : m_weapon->underBarrels)
            {
                if (underBarrel && underBarrel->szInternalName)
                {
                    if (!first)
                        ss << "\n";
                    else
                        first = false;

                    ss << underBarrel->szInternalName;
                }
            }

            for (const auto& other : m_weapon->others)
            {
                if (other && other->szInternalName)
                {
                    if (!first)
                        ss << "\n";
                    else
                        first = false;

                    ss << other->szInternalName;
                }
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        [[nodiscard]] std::string GetNameForSingleWeaponAttachment(const WeaponAttachmentCombination& combination) const
        {
            // Only one attachment type can be set
            assert(combination.scope == 0 || (combination.underBarrel == 0 && combination.other == 0));
            assert(combination.underBarrel == 0 || (combination.scope == 0 && combination.other == 0));
            assert(combination.other == 0 || (combination.scope == 0 && std::popcount(combination.other) == 1));

            if (combination.scope > 0 && m_weapon->weapCompleteDef.scopes)
            {
                const auto attachment = m_weapon->weapCompleteDef.scopes[combination.scope - 1];
                if (attachment && attachment->szInternalName)
                    return attachment->szInternalName;
            }
            else if (combination.underBarrel > 0 && m_weapon->weapCompleteDef.underBarrels)
            {
                const auto attachment = m_weapon->weapCompleteDef.underBarrels[combination.underBarrel - 1];
                if (attachment && attachment->szInternalName)
                    return attachment->szInternalName;
            }
            else if (combination.other > 0 && m_weapon->weapCompleteDef.others)
            {
                const auto attachment = m_weapon->weapCompleteDef.others[std::countr_zero(combination.other)];
                if (attachment && attachment->szInternalName)
                    return attachment->szInternalName;
            }

            return {};
        }

        void FillFromAnimOverrides(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (auto i = 0u; i < m_weapon->weapCompleteDef.numAnimOverrides; i++)
            {
                const auto& animOverride = m_weapon->weapCompleteDef.animOverrides[i];

                if (!first)
                    ss << "\n";
                else
                    first = false;

                assert(animOverride.attachment1.fields);
                assert(animOverride.animTreeType < WEAP_ANIM_COUNT);

                if (animOverride.attachment1.fields)
                    ss << GetNameForSingleWeaponAttachment(animOverride.attachment1);
                else
                    ss << "none";

                ss << ' ';

                if (animOverride.attachment2.fields)
                    ss << GetNameForSingleWeaponAttachment(animOverride.attachment2);
                else
                    ss << "none";

                ss << ' ';

                if (animOverride.animTreeType < WEAP_ANIM_COUNT)
                    ss << weapAnimFilesNames[animOverride.animTreeType] << ' ';

                if (animOverride.overrideAnim && animOverride.overrideAnim[0])
                    ss << animOverride.overrideAnim;
                else
                    ss << "none";

                ss << ' ';

                if (animOverride.altmodeAnim && animOverride.altmodeAnim[0])
                    ss << animOverride.altmodeAnim;
                else
                    ss << "none";

                ss << ' ' << animOverride.animTime << ' ' << animOverride.altTime;
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        void FillFromSoundOverrides(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (auto i = 0u; i < m_weapon->weapCompleteDef.numSoundOverrides; i++)
            {
                const auto& soundOverride = m_weapon->weapCompleteDef.soundOverrides[i];

                if (!first)
                    ss << "\n";
                else
                    first = false;

                assert(soundOverride.soundType < SNDTYPE_PLAYER_COUNT);

                if (soundOverride.attachment1.fields)
                    ss << GetNameForSingleWeaponAttachment(soundOverride.attachment1);
                else
                    ss << "none";

                ss << ' ';

                if (soundOverride.attachment2.fields)
                    ss << GetNameForSingleWeaponAttachment(soundOverride.attachment2);
                else
                    ss << "none";

                ss << ' ';

                if (soundOverride.soundType < SNDTYPE_PLAYER_COUNT)
                    ss << soundOverrideTypeNames[soundOverride.soundType] << ' ';

                if (soundOverride.overrideSound.name && soundOverride.overrideSound.name->soundName && soundOverride.overrideSound.name->soundName[0])
                    ss << soundOverride.overrideSound.name->soundName;
                else
                    ss << "none";

                ss << ' ';

                if (soundOverride.altmodeSound.name && soundOverride.altmodeSound.name->soundName && soundOverride.altmodeSound.name->soundName[0])
                    ss << soundOverride.altmodeSound.name->soundName;
                else
                    ss << "none";
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        void FillFromFxOverrides(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (auto i = 0u; i < m_weapon->weapCompleteDef.numFxOverrides; i++)
            {
                const auto& fxOverride = m_weapon->weapCompleteDef.fxOverrides[i];

                if (!first)
                    ss << "\n";
                else
                    first = false;

                assert(fxOverride.fxType < FXTYPE_COUNT);

                if (fxOverride.attachment1.fields)
                    ss << GetNameForSingleWeaponAttachment(fxOverride.attachment1);
                else
                    ss << "none";

                ss << ' ';

                if (fxOverride.attachment2.fields)
                    ss << GetNameForSingleWeaponAttachment(fxOverride.attachment2);
                else
                    ss << "none";

                ss << ' ';

                if (fxOverride.fxType < FXTYPE_COUNT)
                    ss << fxOverrideTypeNames[fxOverride.fxType] << ' ';

                if (fxOverride.overrideFx && fxOverride.overrideFx->name && fxOverride.overrideFx->name[0])
                    ss << fxOverride.overrideFx->name;
                else
                    ss << "none";

                ss << ' ';

                if (fxOverride.altmodeFx && fxOverride.altmodeFx->name && fxOverride.altmodeFx->name[0])
                    ss << fxOverride.altmodeFx->name;
                else
                    ss << "none";
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        void FillFromReloadOverrides(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (auto i = 0u; i < m_weapon->weapCompleteDef.numReloadStateTimerOverrides; i++)
            {
                const auto& reloadOverride = m_weapon->weapCompleteDef.reloadOverrides[i];

                if (!first)
                    ss << "\n";
                else
                    first = false;

                if (reloadOverride.attachment.fields)
                    ss << GetNameForSingleWeaponAttachment(reloadOverride.attachment);
                else
                    ss << "none";

                ss << ' ' << reloadOverride.reloadAddTime << ' ' << reloadOverride.reloadStartAddTime;
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        void FillFromNoteTrackOverrides(const std::string& key)
        {
            std::stringstream ss;
            bool first = true;

            for (auto i = 0u; i < m_weapon->weapCompleteDef.numNotetrackOverrides; i++)
            {
                const auto& noteTrackOverrides = m_weapon->weapCompleteDef.notetrackOverrides[i];

                assert(noteTrackOverrides.notetrackSoundMapKeys || noteTrackOverrides.notetrackSoundMapValues);
                if (!noteTrackOverrides.notetrackSoundMapKeys || !noteTrackOverrides.notetrackSoundMapValues)
                    continue;

                const auto attachmentName = noteTrackOverrides.attachment.fields ? GetNameForSingleWeaponAttachment(noteTrackOverrides.attachment) : "none";
                for (auto j = 0u; j < 24u; j++)
                {
                    const auto& noteTrackKey = m_get_scr_string(noteTrackOverrides.notetrackSoundMapKeys[j]);
                    const auto& noteTrackValue = m_get_scr_string(noteTrackOverrides.notetrackSoundMapValues[j]);

                    if (noteTrackKey.empty() || noteTrackValue.empty())
                        continue;

                    if (!first)
                        ss << "\n";
                    else
                        first = false;

                    ss << attachmentName << ' ' << noteTrackKey << ' ' << noteTrackValue;
                }
            }

            m_info_string.SetValueForKey(key, ss.str());
        }

        const WeaponFullDef* m_weapon;
    };

    GenericGraph2D ConvertAccuracyGraph(const char* graphName, const vec2_t* originalKnots, const unsigned originalKnotCount)
    {
        GenericGraph2D graph;

        graph.name = graphName;
        graph.knots.resize(originalKnotCount);

        for (auto i = 0u; i < originalKnotCount; i++)
        {
            auto& knot = graph.knots[i];
            knot.x = originalKnots[i][0];
            knot.y = originalKnots[i][1];
        }

        return graph;
    }
} // namespace IW5

void AssetDumperWeapon::CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef)
{
    fullDef->weapCompleteDef = *weapon;

    if (weapon->weapDef)
    {
        fullDef->weapDef = *weapon->weapDef;
        fullDef->weapCompleteDef.weapDef = &fullDef->weapDef;
    }

    if (weapon->hideTags)
    {
        memcpy(fullDef->hideTags, weapon->hideTags, sizeof(scr_string_t) * std::extent_v<decltype(WeaponFullDef::hideTags)>);
        fullDef->weapCompleteDef.hideTags = fullDef->hideTags;
    }

    if (weapon->szXAnims)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::szXAnims)> == WEAP_ANIM_COUNT);
        memcpy(fullDef->szXAnims, weapon->szXAnims, sizeof(void*) * WEAP_ANIM_COUNT);
        fullDef->weapCompleteDef.szXAnims = fullDef->szXAnims;
    }

    if (fullDef->weapDef.gunXModel)
    {
        memcpy(fullDef->gunXModel, fullDef->weapDef.gunXModel, sizeof(void*) * std::extent_v<decltype(WeaponFullDef::gunXModel)>);
        fullDef->weapDef.gunXModel = fullDef->gunXModel;
    }

    if (fullDef->weapDef.szXAnimsRightHanded)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::szXAnimsRightHanded)> == WEAP_ANIM_COUNT);
        memcpy(fullDef->szXAnimsRightHanded, fullDef->weapDef.szXAnimsRightHanded, sizeof(void*) * WEAP_ANIM_COUNT);
        fullDef->weapDef.szXAnimsRightHanded = fullDef->szXAnimsRightHanded;
    }

    if (fullDef->weapDef.szXAnimsLeftHanded)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::szXAnimsLeftHanded)> == WEAP_ANIM_COUNT);
        memcpy(fullDef->szXAnimsLeftHanded, fullDef->weapDef.szXAnimsLeftHanded, sizeof(void*) * WEAP_ANIM_COUNT);
        fullDef->weapDef.szXAnimsLeftHanded = fullDef->szXAnimsLeftHanded;
    }

    if (fullDef->weapDef.notetrackSoundMapKeys)
    {
        memcpy(fullDef->notetrackSoundMapKeys,
               fullDef->weapDef.notetrackSoundMapKeys,
               sizeof(scr_string_t) * std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>);
        fullDef->weapDef.notetrackSoundMapKeys = fullDef->notetrackSoundMapKeys;
    }

    if (fullDef->weapDef.notetrackSoundMapValues)
    {
        memcpy(fullDef->notetrackSoundMapValues,
               fullDef->weapDef.notetrackSoundMapValues,
               sizeof(scr_string_t) * std::extent_v<decltype(WeaponFullDef::notetrackSoundMapValues)>);
        fullDef->weapDef.notetrackSoundMapValues = fullDef->notetrackSoundMapValues;
    }

    if (fullDef->weapDef.notetrackRumbleMapKeys)
    {
        memcpy(fullDef->notetrackRumbleMapKeys,
               fullDef->weapDef.notetrackRumbleMapKeys,
               sizeof(scr_string_t) * std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapKeys)>);
        fullDef->weapDef.notetrackRumbleMapKeys = fullDef->notetrackRumbleMapKeys;
    }

    if (fullDef->weapDef.notetrackRumbleMapValues)
    {
        memcpy(fullDef->notetrackRumbleMapValues,
               fullDef->weapDef.notetrackRumbleMapValues,
               sizeof(scr_string_t) * std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapValues)>);
        fullDef->weapDef.notetrackRumbleMapValues = fullDef->notetrackRumbleMapValues;
    }

    if (fullDef->weapDef.worldModel)
    {
        memcpy(fullDef->worldModel, fullDef->weapDef.worldModel, sizeof(void*) * std::extent_v<decltype(WeaponFullDef::worldModel)>);
        fullDef->weapDef.worldModel = fullDef->worldModel;
    }

    if (fullDef->weapDef.parallelBounce)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::parallelBounce)> == SURF_TYPE_COUNT);
        assert(sizeof(WeaponFullDef::parallelBounce) >= sizeof(float) * std::extent_v<decltype(WeaponFullDef::parallelBounce)>);
        memcpy(fullDef->parallelBounce, fullDef->weapDef.parallelBounce, sizeof(float) * std::extent_v<decltype(WeaponFullDef::parallelBounce)>);
        fullDef->weapDef.parallelBounce = fullDef->parallelBounce;
    }

    if (fullDef->weapDef.perpendicularBounce)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::perpendicularBounce)> == SURF_TYPE_COUNT);
        assert(sizeof(WeaponFullDef::perpendicularBounce) >= sizeof(float) * std::extent_v<decltype(WeaponFullDef::perpendicularBounce)>);
        memcpy(fullDef->perpendicularBounce, fullDef->weapDef.perpendicularBounce, sizeof(float) * std::extent_v<decltype(WeaponFullDef::perpendicularBounce)>);
        fullDef->weapDef.perpendicularBounce = fullDef->perpendicularBounce;
    }

    if (fullDef->weapDef.locationDamageMultipliers)
    {
        static_assert(std::extent_v<decltype(WeaponFullDef::locationDamageMultipliers)> == HITLOC_COUNT);
        assert(sizeof(WeaponFullDef::locationDamageMultipliers) >= sizeof(float) * std::extent_v<decltype(WeaponFullDef::locationDamageMultipliers)>);
        memcpy(fullDef->locationDamageMultipliers,
               fullDef->weapDef.locationDamageMultipliers,
               sizeof(float) * std::extent_v<decltype(WeaponFullDef::locationDamageMultipliers)>);
        fullDef->weapDef.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }

    if (fullDef->weapCompleteDef.scopes)
    {
        memcpy(fullDef->scopes, fullDef->weapCompleteDef.scopes, sizeof(void*) * std::extent_v<decltype(WeaponFullDef::scopes)>);
        fullDef->weapCompleteDef.scopes = fullDef->scopes;
    }

    if (fullDef->weapCompleteDef.underBarrels)
    {
        memcpy(fullDef->underBarrels, fullDef->weapCompleteDef.underBarrels, sizeof(void*) * std::extent_v<decltype(WeaponFullDef::underBarrels)>);
        fullDef->weapCompleteDef.underBarrels = fullDef->underBarrels;
    }

    if (fullDef->weapCompleteDef.others)
    {
        memcpy(fullDef->others, fullDef->weapCompleteDef.others, sizeof(void*) * std::extent_v<decltype(WeaponFullDef::others)>);
        fullDef->weapCompleteDef.others = fullDef->others;
    }
}

InfoString AssetDumperWeapon::CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset)
{
    const auto fullDef = std::make_unique<WeaponFullDef>();
    memset(fullDef.get(), 0, sizeof(WeaponFullDef));
    CopyToFullDef(asset->Asset(), fullDef.get());

    InfoStringFromWeaponConverter converter(fullDef.get(),
                                            weapon_fields,
                                            std::extent_v<decltype(weapon_fields)>,
                                            [asset](const scr_string_t scrStr) -> std::string
                                            {
                                                assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                    return "";

                                                return asset->m_zone->m_script_strings[scrStr];
                                            });

    return converter.Convert();
}

void AssetDumperWeapon::DumpAccuracyGraphs(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset)
{
    auto* accuracyGraphWriter = context.GetZoneAssetDumperState<AccuracyGraphWriter>();
    const auto weapon = asset->Asset();
    const auto* weapDef = weapon->weapDef;

    if (!weapDef)
        return;

    if (weapDef->aiVsAiAccuracyGraphName && weapDef->originalAiVsAiAccuracyGraphKnots
        && accuracyGraphWriter->ShouldDumpAiVsAiGraph(weapDef->aiVsAiAccuracyGraphName))
    {
        AccuracyGraphWriter::DumpAiVsAiGraph(
            context,
            ConvertAccuracyGraph(weapDef->aiVsAiAccuracyGraphName, weapDef->originalAiVsAiAccuracyGraphKnots, weapDef->originalAiVsAiAccuracyGraphKnotCount));
    }

    if (weapDef->aiVsPlayerAccuracyGraphName && weapDef->originalAiVsPlayerAccuracyGraphKnots
        && accuracyGraphWriter->ShouldDumpAiVsPlayerGraph(weapDef->aiVsPlayerAccuracyGraphName))
    {
        AccuracyGraphWriter::DumpAiVsPlayerGraph(context,
                                                 ConvertAccuracyGraph(weapDef->aiVsPlayerAccuracyGraphName,
                                                                      weapDef->originalAiVsPlayerAccuracyGraphKnots,
                                                                      weapDef->originalAiVsPlayerAccuracyGraphKnotCount));
    }
}

bool AssetDumperWeapon::ShouldDump(XAssetInfo<WeaponCompleteDef>* asset)
{
    return true;
}

void AssetDumperWeapon::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset)
{
    // TODO: only dump infostring fields when non-default

    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_WEAPON);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_WEAPON, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("weapons/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_WEAPON);
        stream.write(stringValue.c_str(), stringValue.size());
    }

    DumpAccuracyGraphs(context, asset);
}
