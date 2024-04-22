#include "JsonWeaponAttachmentLoader.h"

#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/Weapon/JsonWeaponAttachment.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace IW5;

namespace
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream,
                   MemoryManager& memory,
                   IAssetLoadingManager& manager,
                   std::vector<XAssetInfoGeneric*>& dependencies,
                   std::vector<IndirectAssetReference>& indirectAssetReferences)
            : m_stream(stream),
              m_memory(memory),
              m_manager(manager),
              m_dependencies(dependencies),
              m_indirect_asset_references(indirectAssetReferences)

        {
        }

        bool Load(WeaponAttachment& attachment) const
        {
            const auto jRoot = json::parse(m_stream);
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_version").get_to(version);

            if (type != "attachment" || version != 1u)
            {
                std::cerr << "Tried to load attachment \"" << attachment.szInternalName << "\" but did not find expected type attachment of version 1\n";
                return false;
            }

            const auto jAttachment = jRoot.get<JsonWeaponAttachment>();
            return CreateWeaponAttachmentFromJson(jAttachment, attachment);
        }

    private:
        static void PrintError(const WeaponAttachment& attachment, const std::string& message)
        {
            std::cerr << "Cannot load attachment \"" << attachment.szInternalName << "\": " << message << "\n";
        }

        bool CreateWeaponAttachmentFromJson(const JsonWeaponAttachment& jAttachment, WeaponAttachment& attachment) const
        {
#define CONVERT_XMODEL_ARRAY(propertyName, count)                                                                                                              \
    CreateXModelArrayFromJson(jAttachment.propertyName, attachment.propertyName, #propertyName, count, attachment);

#define CONVERT_ATTRIBUTE(attributeClass, attributeName)                                                                                                       \
    if (jAttachment.attributeName)                                                                                                                             \
    {                                                                                                                                                          \
        using AttributeType = std::remove_pointer_t<decltype(attachment.attributeName)>;                                                                       \
        attachment.attributeName = m_memory.Alloc<AttributeType>();                                                                                            \
        if (!Create##attributeClass##FromJson(jAttachment.attributeName.value(), *attachment.attributeName, attachment))                                       \
            return false;                                                                                                                                      \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
        attachment.attributeName = nullptr;

            attachment.szDisplayName = m_memory.Dup(jAttachment.displayName.c_str());
            attachment.type = jAttachment.type;
            attachment.weaponType = jAttachment.weaponType;
            attachment.weapClass = jAttachment.weapClass;

            CONVERT_XMODEL_ARRAY(worldModels, ATTACHMENT_WORLD_MODEL_COUNT)
            CONVERT_XMODEL_ARRAY(viewModels, ATTACHMENT_VIEW_MODEL_COUNT)
            CONVERT_XMODEL_ARRAY(reticleViewModels, ATTACHMENT_RETICLE_VIEW_MODEL_COUNT)

            CONVERT_ATTRIBUTE(AttAmmoGeneral, ammoGeneral)
            CONVERT_ATTRIBUTE(AttSight, sight)
            CONVERT_ATTRIBUTE(AttReload, reload)
            CONVERT_ATTRIBUTE(AttAddOns, addOns)
            CONVERT_ATTRIBUTE(AttGeneral, general)
            CONVERT_ATTRIBUTE(AttAimAssist, aimAssist)
            CONVERT_ATTRIBUTE(AttAmmunition, ammunition)
            CONVERT_ATTRIBUTE(AttDamage, damage)
            CONVERT_ATTRIBUTE(AttLocationDamage, locationDamage)
            CONVERT_ATTRIBUTE(AttIdleSettings, idleSettings)
            CONVERT_ATTRIBUTE(AttADSSettings, adsSettings)
            CONVERT_ATTRIBUTE(AttADSSettings, adsSettingsMain)
            CONVERT_ATTRIBUTE(AttHipSpread, hipSpread)
            CONVERT_ATTRIBUTE(AttGunKick, gunKick)
            CONVERT_ATTRIBUTE(AttViewKick, viewKick)
            CONVERT_ATTRIBUTE(AttADSOverlay, adsOverlay)
            CONVERT_ATTRIBUTE(AttUI, ui)
            CONVERT_ATTRIBUTE(AttRumbles, rumbles)
            CONVERT_ATTRIBUTE(AttProjectile, projectile)

            attachment.ammunitionScale = jAttachment.ammunitionScale;
            attachment.damageScale = jAttachment.damageScale;
            attachment.damageScaleMin = jAttachment.damageScaleMin;
            attachment.stateTimersScale = jAttachment.stateTimersScale;
            attachment.fireTimersScale = jAttachment.fireTimersScale;
            attachment.idleSettingsScale = jAttachment.idleSettingsScale;
            attachment.adsSettingsScale = jAttachment.adsSettingsScale;
            attachment.adsSettingsScaleMain = jAttachment.adsSettingsScaleMain;
            attachment.hipSpreadScale = jAttachment.hipSpreadScale;
            attachment.gunKickScale = jAttachment.gunKickScale;
            attachment.viewKickScale = jAttachment.viewKickScale;
            attachment.viewCenterScale = jAttachment.viewCenterScale;
            attachment.loadIndex = jAttachment.loadIndex;
            attachment.hideIronSightsWithThisAttachment = jAttachment.hideIronSightsWithThisAttachment;
            attachment.shareAmmoWithAlt = jAttachment.shareAmmoWithAlt;

            return true;
        }

        bool CreateTracerFromJson(const std::string& assetName, TracerDef*& tracerPtr, const WeaponAttachment& attachment) const
        {
            auto* tracer = m_manager.LoadDependency<AssetTracer>(assetName);
            if (!tracer)
            {
                PrintError(attachment, std::format("Could not find tracer {}", assetName));
                return false;
            }
            m_dependencies.push_back(tracer);
            tracerPtr = tracer->Asset();

            return true;
        }

        bool CreateMaterialFromJson(const std::string& assetName, Material*& materialPtr, const WeaponAttachment& attachment) const
        {
            auto* material = m_manager.LoadDependency<AssetMaterial>(assetName);
            if (!material)
            {
                PrintError(attachment, std::format("Could not find material {}", assetName));
                return false;
            }
            m_dependencies.push_back(material);
            materialPtr = material->Asset();

            return true;
        }

        bool CreateFxFromJson(const std::string& assetName, FxEffectDef*& fxPtr, const WeaponAttachment& attachment) const
        {
            auto* fx = m_manager.LoadDependency<AssetFx>(assetName);
            if (!fx)
            {
                PrintError(attachment, std::format("Could not find fx {}", assetName));
                return false;
            }
            m_dependencies.push_back(fx);
            fxPtr = fx->Asset();

            return true;
        }

        bool CreateSoundFromJson(const std::string& assetName, SndAliasCustom& sndAliasCustom, const WeaponAttachment& attachment) const
        {
            auto sound = m_manager.LoadIndirectAssetReference<AssetSound>(assetName);
            m_indirect_asset_references.push_back(std::move(sound));
            sndAliasCustom.name = m_memory.Alloc<snd_alias_list_name>();
            sndAliasCustom.name->soundName = m_memory.Dup(assetName.c_str());

            return true;
        }

        bool CreateXModelFromJson(const std::string& assetName, XModel*& xmodelPtr, const WeaponAttachment& attachment) const
        {
            auto* xmodel = m_manager.LoadDependency<AssetXModel>(assetName);
            if (!xmodel)
            {
                PrintError(attachment, std::format("Could not find xmodel {}", assetName));
                return false;
            }
            m_dependencies.push_back(xmodel);
            xmodelPtr = xmodel->Asset();

            return true;
        }

        bool CreateXModelArrayFromJson(const std::vector<std::string>& jXmodelArray,
                                       XModel**& xmodelArray,
                                       const char* propertyName,
                                       size_t propertyCount,
                                       const WeaponAttachment& attachment) const
        {
            if (!jXmodelArray.empty())
            {
                const auto arraySize = jXmodelArray.size();
                if (arraySize > propertyCount)
                {
                    PrintError(attachment, std::format("{} size cannot exceed {}", propertyName, propertyCount));
                    return false;
                }
                xmodelArray = m_memory.Alloc<XModel*>(propertyCount);
                memset(xmodelArray, 0, sizeof(void*) * propertyCount);

                for (auto i = 0u; i < arraySize; i++)
                {
                    if (!CreateXModelFromJson(jXmodelArray[i], xmodelArray[i], attachment))
                        return false;
                }
            }
            else
            {
                xmodelArray = nullptr;
            }

            return true;
        }

        bool CreateAttAmmoGeneralFromJson(const JsonAttAmmoGeneral& jAmmoGeneral, AttAmmoGeneral& ammoGeneral, const WeaponAttachment& attachment) const
        {
            ammoGeneral.penetrateType = jAmmoGeneral.penetrateType;
            ammoGeneral.penetrateMultiplier = jAmmoGeneral.penetrateMultiplier;
            ammoGeneral.impactType = jAmmoGeneral.impactType;
            ammoGeneral.fireType = jAmmoGeneral.fireType;

            if (jAmmoGeneral.tracerType)
            {
                if (!CreateTracerFromJson(jAmmoGeneral.tracerType.value(), ammoGeneral.tracerType, attachment))
                    return false;
            }
            else
                ammoGeneral.tracerType = nullptr;

            ammoGeneral.rifleBullet = jAmmoGeneral.rifleBullet;
            ammoGeneral.armorPiercing = jAmmoGeneral.armorPiercing;

            return true;
        }

        static bool CreateAttSightFromJson(const JsonAttSight& jSight, AttSight& sight, const WeaponAttachment& attachment)
        {
            sight.aimDownSight = jSight.aimDownSight;
            sight.adsFire = jSight.adsFire;
            sight.rechamberWhileAds = jSight.rechamberWhileAds;
            sight.noAdsWhenMagEmpty = jSight.noAdsWhenMagEmpty;
            sight.canHoldBreath = jSight.canHoldBreath;
            sight.canVariableZoom = jSight.canVariableZoom;
            sight.hideRailWithThisScope = jSight.hideRailWithThisScope;

            return true;
        }

        static bool CreateAttReloadFromJson(const JsonAttReload& jReload, AttReload& reload, const WeaponAttachment& attachment)
        {
            reload.noPartialReload = jReload.noPartialReload;
            reload.segmentedReload = jReload.segmentedReload;

            return true;
        }

        static bool CreateAttAddOnsFromJson(const JsonAttAddOns& jAddOns, AttAddOns& addOns, const WeaponAttachment& attachment)
        {
            addOns.motionTracker = jAddOns.motionTracker;
            addOns.silenced = jAddOns.silenced;

            return true;
        }

        bool CreateAttGeneralFromJson(const JsonAttGeneral& jGeneral, AttGeneral& general, const WeaponAttachment& attachment) const
        {
            general.boltAction = jGeneral.boltAction;
            general.inheritsPerks = jGeneral.inheritsPerks;
            general.enemyCrosshairRange = jGeneral.enemyCrosshairRange;

            if (jGeneral.reticleCenter)
            {
                if (!CreateMaterialFromJson(jGeneral.reticleCenter.value(), general.reticleCenter, attachment))
                    return false;
            }
            else
                general.reticleCenter = nullptr;

            if (jGeneral.reticleSide)
            {
                if (!CreateMaterialFromJson(jGeneral.reticleSide.value(), general.reticleSide, attachment))
                    return false;
            }
            else
                general.reticleSide = nullptr;

            general.reticleCenterSize = jGeneral.reticleCenterSize;
            general.reticleSideSize = jGeneral.reticleSideSize;
            general.moveSpeedScale = jGeneral.moveSpeedScale;
            general.adsMoveSpeedScale = jGeneral.adsMoveSpeedScale;

            return true;
        }

        static bool CreateAttAimAssistFromJson(const JsonAttAimAssist& jAimAssist, AttAimAssist& aimAssist, const WeaponAttachment& attachment)
        {
            aimAssist.autoAimRange = jAimAssist.autoAimRange;
            aimAssist.aimAssistRange = jAimAssist.aimAssistRange;
            aimAssist.aimAssistRangeAds = jAimAssist.aimAssistRangeAds;

            return true;
        }

        static bool CreateAttAmmunitionFromJson(const JsonAttAmmunition& jAmmunition, AttAmmunition& ammunition, const WeaponAttachment& attachment)
        {
            ammunition.maxAmmo = jAmmunition.maxAmmo;
            ammunition.startAmmo = jAmmunition.startAmmo;
            ammunition.clipSize = jAmmunition.clipSize;
            ammunition.shotCount = jAmmunition.shotCount;
            ammunition.reloadAmmoAdd = jAmmunition.reloadAmmoAdd;
            ammunition.reloadStartAdd = jAmmunition.reloadStartAdd;

            return true;
        }

        static bool CreateAttDamageFromJson(const JsonAttDamage& jDamage, AttDamage& damage, const WeaponAttachment& attachment)
        {
            damage.damage = jDamage.damage;
            damage.minDamage = jDamage.minDamage;
            damage.meleeDamage = jDamage.meleeDamage;
            damage.maxDamageRange = jDamage.maxDamageRange;
            damage.minDamageRange = jDamage.minDamageRange;
            damage.playerDamage = jDamage.playerDamage;
            damage.minPlayerDamage = jDamage.minPlayerDamage;

            return true;
        }

        static bool
            CreateAttLocationDamageFromJson(const JsonAttLocationDamage& jLocationDamage, AttLocationDamage& locationDamage, const WeaponAttachment& attachment)
        {
            locationDamage.locNone = jLocationDamage.locNone;
            locationDamage.locHelmet = jLocationDamage.locHelmet;
            locationDamage.locHead = jLocationDamage.locHead;
            locationDamage.locNeck = jLocationDamage.locNeck;
            locationDamage.locTorsoUpper = jLocationDamage.locTorsoUpper;
            locationDamage.locTorsoLower = jLocationDamage.locTorsoLower;
            locationDamage.locRightArmUpper = jLocationDamage.locRightArmUpper;
            locationDamage.locRightArmLower = jLocationDamage.locRightArmLower;
            locationDamage.locRightHand = jLocationDamage.locRightHand;
            locationDamage.locLeftArmUpper = jLocationDamage.locLeftArmUpper;
            locationDamage.locLeftArmLower = jLocationDamage.locLeftArmLower;
            locationDamage.locLeftHand = jLocationDamage.locLeftHand;
            locationDamage.locRightLegUpper = jLocationDamage.locRightLegUpper;
            locationDamage.locRightLegLower = jLocationDamage.locRightLegLower;
            locationDamage.locRightFoot = jLocationDamage.locRightFoot;
            locationDamage.locLeftLegUpper = jLocationDamage.locLeftLegUpper;
            locationDamage.locLeftLegLower = jLocationDamage.locLeftLegLower;
            locationDamage.locLeftFoot = jLocationDamage.locLeftFoot;
            locationDamage.locGun = jLocationDamage.locGun;

            return true;
        }

        static bool CreateAttIdleSettingsFromJson(const JsonAttIdleSettings& jIdleSettings, AttIdleSettings& idleSettings, const WeaponAttachment& attachment)
        {
            idleSettings.hipIdleAmount = jIdleSettings.hipIdleAmount;
            idleSettings.hipIdleSpeed = jIdleSettings.hipIdleSpeed;
            idleSettings.idleCrouchFactor = jIdleSettings.idleCrouchFactor;
            idleSettings.idleProneFactor = jIdleSettings.idleProneFactor;
            idleSettings.adsIdleLerpStartTime = jIdleSettings.adsIdleLerpStartTime;
            idleSettings.adsIdleLerpTime = jIdleSettings.adsIdleLerpTime;

            return true;
        }

        static bool CreateAttADSSettingsFromJson(const JsonAttADSSettings& jAdsSettings, AttADSSettings& adsSettings, const WeaponAttachment& attachment)
        {
            adsSettings.adsSpread = jAdsSettings.adsSpread;
            adsSettings.adsAimPitch = jAdsSettings.adsAimPitch;
            adsSettings.adsTransInTime = jAdsSettings.adsTransInTime;
            adsSettings.adsTransOutTime = jAdsSettings.adsTransOutTime;
            adsSettings.adsReloadTransTime = jAdsSettings.adsReloadTransTime;
            adsSettings.adsCrosshairInFrac = jAdsSettings.adsCrosshairInFrac;
            adsSettings.adsCrosshairOutFrac = jAdsSettings.adsCrosshairOutFrac;
            adsSettings.adsZoomFov = jAdsSettings.adsZoomFov;
            adsSettings.adsZoomInFrac = jAdsSettings.adsZoomInFrac;
            adsSettings.adsZoomOutFrac = jAdsSettings.adsZoomOutFrac;
            adsSettings.adsBobFactor = jAdsSettings.adsBobFactor;
            adsSettings.adsViewBobMult = jAdsSettings.adsViewBobMult;
            adsSettings.adsViewErrorMin = jAdsSettings.adsViewErrorMin;
            adsSettings.adsViewErrorMax = jAdsSettings.adsViewErrorMax;

            return true;
        }

        static bool CreateAttHipSpreadFromJson(const JsonAttHipSpread& jHipSpread, AttHipSpread& hipSpread, const WeaponAttachment& attachment)
        {
            hipSpread.hipSpreadStandMin = jHipSpread.hipSpreadStandMin;
            hipSpread.hipSpreadDuckedMin = jHipSpread.hipSpreadDuckedMin;
            hipSpread.hipSpreadProneMin = jHipSpread.hipSpreadProneMin;
            hipSpread.hipSpreadMax = jHipSpread.hipSpreadMax;
            hipSpread.hipSpreadDuckedMax = jHipSpread.hipSpreadDuckedMax;
            hipSpread.hipSpreadProneMax = jHipSpread.hipSpreadProneMax;
            hipSpread.hipSpreadFireAdd = jHipSpread.hipSpreadFireAdd;
            hipSpread.hipSpreadTurnAdd = jHipSpread.hipSpreadTurnAdd;
            hipSpread.hipSpreadMoveAdd = jHipSpread.hipSpreadMoveAdd;
            hipSpread.hipSpreadDecayRate = jHipSpread.hipSpreadDecayRate;
            hipSpread.hipSpreadDuckedDecay = jHipSpread.hipSpreadDuckedDecay;
            hipSpread.hipSpreadProneDecay = jHipSpread.hipSpreadProneDecay;

            return true;
        }

        static bool CreateAttGunKickFromJson(const JsonAttGunKick& jGunKick, AttGunKick& gunKick, const WeaponAttachment& attachment)
        {
            gunKick.hipGunKickReducedKickBullets = jGunKick.hipGunKickReducedKickBullets;
            gunKick.hipGunKickReducedKickPercent = jGunKick.hipGunKickReducedKickPercent;
            gunKick.hipGunKickPitchMin = jGunKick.hipGunKickPitchMin;
            gunKick.hipGunKickPitchMax = jGunKick.hipGunKickPitchMax;
            gunKick.hipGunKickYawMin = jGunKick.hipGunKickYawMin;
            gunKick.hipGunKickYawMax = jGunKick.hipGunKickYawMax;
            gunKick.hipGunKickAccel = jGunKick.hipGunKickAccel;
            gunKick.hipGunKickSpeedMax = jGunKick.hipGunKickSpeedMax;
            gunKick.hipGunKickSpeedDecay = jGunKick.hipGunKickSpeedDecay;
            gunKick.hipGunKickStaticDecay = jGunKick.hipGunKickStaticDecay;
            gunKick.adsGunKickReducedKickBullets = jGunKick.adsGunKickReducedKickBullets;
            gunKick.adsGunKickReducedKickPercent = jGunKick.adsGunKickReducedKickPercent;
            gunKick.adsGunKickPitchMin = jGunKick.adsGunKickPitchMin;
            gunKick.adsGunKickPitchMax = jGunKick.adsGunKickPitchMax;
            gunKick.adsGunKickYawMin = jGunKick.adsGunKickYawMin;
            gunKick.adsGunKickYawMax = jGunKick.adsGunKickYawMax;
            gunKick.adsGunKickAccel = jGunKick.adsGunKickAccel;
            gunKick.adsGunKickSpeedMax = jGunKick.adsGunKickSpeedMax;
            gunKick.adsGunKickSpeedDecay = jGunKick.adsGunKickSpeedDecay;
            gunKick.adsGunKickStaticDecay = jGunKick.adsGunKickStaticDecay;

            return true;
        }

        static bool CreateAttViewKickFromJson(const JsonAttViewKick& jViewKick, AttViewKick& viewKick, const WeaponAttachment& attachment)
        {
            viewKick.hipViewKickPitchMin = jViewKick.hipViewKickPitchMin;
            viewKick.hipViewKickPitchMax = jViewKick.hipViewKickPitchMax;
            viewKick.hipViewKickYawMin = jViewKick.hipViewKickYawMin;
            viewKick.hipViewKickYawMax = jViewKick.hipViewKickYawMax;
            viewKick.hipViewKickCenterSpeed = jViewKick.hipViewKickCenterSpeed;
            viewKick.adsViewKickPitchMin = jViewKick.adsViewKickPitchMin;
            viewKick.adsViewKickPitchMax = jViewKick.adsViewKickPitchMax;
            viewKick.adsViewKickYawMin = jViewKick.adsViewKickYawMin;
            viewKick.adsViewKickYawMax = jViewKick.adsViewKickYawMax;
            viewKick.adsViewKickCenterSpeed = jViewKick.adsViewKickCenterSpeed;

            return true;
        }

        bool CreateAttADSOverlayFromJson(const JsonAttADSOverlay& jAdsOverlay, AttADSOverlay& adsOverlay, const WeaponAttachment& attachment) const
        {
            if (jAdsOverlay.shader)
            {
                if (!CreateMaterialFromJson(jAdsOverlay.shader.value(), adsOverlay.overlay.shader, attachment))
                    return false;
            }
            else
                adsOverlay.overlay.shader = nullptr;

            if (jAdsOverlay.shaderLowRes)
            {
                if (!CreateMaterialFromJson(jAdsOverlay.shaderLowRes.value(), adsOverlay.overlay.shaderLowRes, attachment))
                    return false;
            }
            else
                adsOverlay.overlay.shaderLowRes = nullptr;

            if (jAdsOverlay.shaderEMP)
            {
                if (!CreateMaterialFromJson(jAdsOverlay.shaderEMP.value(), adsOverlay.overlay.shaderEMP, attachment))
                    return false;
            }
            else
                adsOverlay.overlay.shaderEMP = nullptr;

            if (jAdsOverlay.shaderEMPLowRes)
            {
                if (!CreateMaterialFromJson(jAdsOverlay.shaderEMPLowRes.value(), adsOverlay.overlay.shaderEMPLowRes, attachment))
                    return false;
            }
            else
                adsOverlay.overlay.shaderEMPLowRes = nullptr;

            adsOverlay.overlay.reticle = jAdsOverlay.reticle;
            adsOverlay.overlay.width = jAdsOverlay.width;
            adsOverlay.overlay.height = jAdsOverlay.height;
            adsOverlay.overlay.widthSplitscreen = jAdsOverlay.widthSplitscreen;
            adsOverlay.overlay.heightSplitscreen = jAdsOverlay.heightSplitscreen;
            adsOverlay.thermalScope = jAdsOverlay.thermalScope;

            return true;
        }

        bool CreateAttUIFromJson(const JsonAttUI& jUi, AttUI& ui, const WeaponAttachment& attachment) const
        {
            if (jUi.dpadIcon)
            {
                if (!CreateMaterialFromJson(jUi.dpadIcon.value(), ui.dpadIcon, attachment))
                    return false;
            }
            else
                ui.dpadIcon = nullptr;

            if (jUi.ammoCounterIcon)
            {
                if (!CreateMaterialFromJson(jUi.ammoCounterIcon.value(), ui.ammoCounterIcon, attachment))
                    return false;
            }
            else
                ui.ammoCounterIcon = nullptr;

            ui.dpadIconRatio = jUi.dpadIconRatio;
            ui.ammoCounterIconRatio = jUi.ammoCounterIconRatio;
            ui.ammoCounterClip = jUi.ammoCounterClip;

            return true;
        }

        bool CreateAttRumblesFromJson(const JsonAttRumbles& jRumbles, AttRumbles& rumbles, const WeaponAttachment& attachment) const
        {
            if (jRumbles.fireRumble)
                rumbles.fireRumble = m_memory.Dup(jRumbles.fireRumble.value().c_str());
            else
                rumbles.fireRumble = nullptr;

            if (jRumbles.meleeImpactRumble)
                rumbles.meleeImpactRumble = m_memory.Dup(jRumbles.meleeImpactRumble.value().c_str());
            else
                rumbles.meleeImpactRumble = nullptr;

            return true;
        }

        bool CreateAttProjectileFromJson(const JsonAttProjectile& jProjectile, AttProjectile& projectile, const WeaponAttachment& attachment) const
        {
            projectile.explosionRadius = jProjectile.explosionRadius;
            projectile.explosionInnerDamage = jProjectile.explosionInnerDamage;
            projectile.explosionOuterDamage = jProjectile.explosionOuterDamage;
            projectile.damageConeAngle = jProjectile.damageConeAngle;
            projectile.projectileSpeed = jProjectile.projectileSpeed;
            projectile.projectileSpeedUp = jProjectile.projectileSpeedUp;
            projectile.projectileActivateDist = jProjectile.projectileActivateDist;
            projectile.projectileLifetime = jProjectile.projectileLifetime;

            if (jProjectile.projectileModel)
            {
                if (!CreateXModelFromJson(jProjectile.projectileModel.value(), projectile.projectileModel, attachment))
                    return false;
            }
            else
                projectile.projectileModel = nullptr;

            projectile.projExplosionType = jProjectile.projExplosionType;

            if (jProjectile.projExplosionEffect)
            {
                if (!CreateFxFromJson(jProjectile.projExplosionEffect.value(), projectile.projExplosionEffect, attachment))
                    return false;
            }
            else
                projectile.projExplosionEffect = nullptr;

            projectile.projExplosionEffectForceNormalUp = jProjectile.projExplosionEffectForceNormalUp;

            if (jProjectile.projExplosionSound)
            {
                if (!CreateSoundFromJson(jProjectile.projExplosionSound.value(), projectile.projExplosionSound, attachment))
                    return false;
            }
            else
                projectile.projExplosionSound.name = nullptr;

            if (jProjectile.projDudEffect)
            {
                if (!CreateFxFromJson(jProjectile.projDudEffect.value(), projectile.projDudEffect, attachment))
                    return false;
            }
            else
                projectile.projDudEffect = nullptr;

            if (jProjectile.projDudSound)
            {
                if (!CreateSoundFromJson(jProjectile.projDudSound.value(), projectile.projDudSound, attachment))
                    return false;
            }
            else
                projectile.projDudSound.name = nullptr;

            projectile.projImpactExplode = jProjectile.projImpactExplode;
            projectile.destabilizationRateTime = jProjectile.destabilizationRateTime;
            projectile.destabilizationCurvatureMax = jProjectile.destabilizationCurvatureMax;
            projectile.destabilizeDistance = jProjectile.destabilizeDistance;

            if (jProjectile.projTrailEffect)
            {
                if (!CreateFxFromJson(jProjectile.projTrailEffect.value(), projectile.projTrailEffect, attachment))
                    return false;
            }
            else
                projectile.projTrailEffect = nullptr;

            projectile.projIgnitionDelay = jProjectile.projIgnitionDelay;

            if (jProjectile.projIgnitionEffect)
            {
                if (!CreateFxFromJson(jProjectile.projIgnitionEffect.value(), projectile.projIgnitionEffect, attachment))
                    return false;
            }
            else
                projectile.projIgnitionEffect = nullptr;

            if (jProjectile.projIgnitionSound)
            {
                if (!CreateSoundFromJson(jProjectile.projIgnitionSound.value(), projectile.projIgnitionSound, attachment))
                    return false;
            }
            else
                projectile.projIgnitionSound.name = nullptr;

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
        IAssetLoadingManager& m_manager;
        std::vector<XAssetInfoGeneric*>& m_dependencies;
        std::vector<IndirectAssetReference>& m_indirect_asset_references;
    };
} // namespace

namespace IW5
{
    bool LoadWeaponAttachmentAsJson(std::istream& stream,
                                    WeaponAttachment& attachment,
                                    MemoryManager* memory,
                                    IAssetLoadingManager* manager,
                                    std::vector<XAssetInfoGeneric*>& dependencies,
                                    std::vector<IndirectAssetReference>& indirectAssetReferences)
    {
        const JsonLoader loader(stream, *memory, *manager, dependencies, indirectAssetReferences);

        return loader.Load(attachment);
    }
} // namespace IW5
