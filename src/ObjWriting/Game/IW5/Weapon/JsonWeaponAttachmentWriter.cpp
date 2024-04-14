#include "JsonWeaponAttachmentWriter.h"

#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/Weapon/JsonWeaponAttachment.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace IW5;

namespace
{
    class JsonDumper
    {
    public:
        JsonDumper(AssetDumpingContext& context, std::ostream& stream)
            : m_stream(stream)
        {
        }

        void Dump(const WeaponAttachment* attachment) const
        {
            JsonWeaponAttachment jsonWeaponAttachment;
            CreateJsonAttachment(jsonWeaponAttachment, *attachment);
            json jRoot = jsonWeaponAttachment;

            jRoot["_type"] = "attachment";
            jRoot["_version"] = 1;

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static const char* AssetName(const char* input)
        {
            if (input && input[0] == ',')
                return &input[1];

            return input;
        }

        static void CreateJsonAttachment(JsonWeaponAttachment& jAttachment, const WeaponAttachment& attachment)
        {
#define CONVERT_XMODEL_ARRAY(propertyName, count)                                                                                                              \
    if (attachment.propertyName)                                                                                                                               \
    {                                                                                                                                                          \
        for (auto i = 0u; i < (count); i++)                                                                                                                    \
        {                                                                                                                                                      \
            const auto* model = attachment.propertyName[i];                                                                                                    \
            if (model && model->name)                                                                                                                          \
                jAttachment.propertyName.emplace_back(AssetName(model->name));                                                                                 \
        }                                                                                                                                                      \
    }

#define CONVERT_ATTRIBUTE(attributeJsonClass, attributeName)                                                                                                   \
    if (attachment.attributeName)                                                                                                                              \
    {                                                                                                                                                          \
        attributeJsonClass attribute;                                                                                                                          \
        Convert##attributeJsonClass(attribute, *attachment.attributeName);                                                                                     \
        jAttachment.attributeName = std::move(attribute);                                                                                                      \
    }

            if (attachment.szDisplayName)
                jAttachment.displayName = attachment.szDisplayName;

            jAttachment.type = attachment.type;
            jAttachment.weaponType = attachment.weaponType;
            jAttachment.weapClass = attachment.weapClass;

            CONVERT_XMODEL_ARRAY(worldModels, ATTACHMENT_WORLD_MODEL_COUNT)
            CONVERT_XMODEL_ARRAY(viewModels, ATTACHMENT_VIEW_MODEL_COUNT)
            CONVERT_XMODEL_ARRAY(reticleViewModels, ATTACHMENT_RETICLE_VIEW_MODEL_COUNT)

            CONVERT_ATTRIBUTE(JsonAttAmmoGeneral, ammoGeneral)
            CONVERT_ATTRIBUTE(JsonAttSight, sight)
            CONVERT_ATTRIBUTE(JsonAttReload, reload)
            CONVERT_ATTRIBUTE(JsonAttAddOns, addOns)
            CONVERT_ATTRIBUTE(JsonAttGeneral, general)
            CONVERT_ATTRIBUTE(JsonAttAimAssist, aimAssist)
            CONVERT_ATTRIBUTE(JsonAttAmmunition, ammunition)
            CONVERT_ATTRIBUTE(JsonAttDamage, damage)
            CONVERT_ATTRIBUTE(JsonAttLocationDamage, locationDamage)
            CONVERT_ATTRIBUTE(JsonAttIdleSettings, idleSettings)
            CONVERT_ATTRIBUTE(JsonAttADSSettings, adsSettings)
            CONVERT_ATTRIBUTE(JsonAttADSSettings, adsSettingsMain)
            CONVERT_ATTRIBUTE(JsonAttHipSpread, hipSpread)
            CONVERT_ATTRIBUTE(JsonAttGunKick, gunKick)
            CONVERT_ATTRIBUTE(JsonAttViewKick, viewKick)
            CONVERT_ATTRIBUTE(JsonAttADSOverlay, adsOverlay)
            CONVERT_ATTRIBUTE(JsonAttUI, ui)
            CONVERT_ATTRIBUTE(JsonAttRumbles, rumbles)
            CONVERT_ATTRIBUTE(JsonAttProjectile, projectile)

            jAttachment.ammunitionScale = attachment.ammunitionScale;
            jAttachment.damageScale = attachment.damageScale;
            jAttachment.damageScaleMin = attachment.damageScaleMin;
            jAttachment.stateTimersScale = attachment.stateTimersScale;
            jAttachment.fireTimersScale = attachment.fireTimersScale;
            jAttachment.idleSettingsScale = attachment.idleSettingsScale;
            jAttachment.adsSettingsScale = attachment.adsSettingsScale;
            jAttachment.adsSettingsScaleMain = attachment.adsSettingsScaleMain;
            jAttachment.hipSpreadScale = attachment.hipSpreadScale;
            jAttachment.gunKickScale = attachment.gunKickScale;
            jAttachment.viewKickScale = attachment.viewKickScale;
            jAttachment.viewCenterScale = attachment.viewCenterScale;
            jAttachment.loadIndex = attachment.loadIndex;
            jAttachment.hideIronSightsWithThisAttachment = attachment.hideIronSightsWithThisAttachment;
            jAttachment.shareAmmoWithAlt = attachment.shareAmmoWithAlt;
        }

        static void ConvertJsonAttAmmoGeneral(JsonAttAmmoGeneral& jAmmoGeneral, const AttAmmoGeneral& ammoGeneral)
        {
            jAmmoGeneral.penetrateType = ammoGeneral.penetrateType;
            jAmmoGeneral.penetrateMultiplier = ammoGeneral.penetrateMultiplier;
            jAmmoGeneral.impactType = ammoGeneral.impactType;
            jAmmoGeneral.fireType = ammoGeneral.fireType;

            if (ammoGeneral.tracerType && ammoGeneral.tracerType->name)
                jAmmoGeneral.tracerType = AssetName(ammoGeneral.tracerType->name);

            jAmmoGeneral.rifleBullet = ammoGeneral.rifleBullet;
            jAmmoGeneral.armorPiercing = ammoGeneral.armorPiercing;
        }

        static void ConvertJsonAttSight(JsonAttSight& jSight, const AttSight& sight)
        {
            jSight.aimDownSight = sight.aimDownSight;
            jSight.adsFire = sight.adsFire;
            jSight.rechamberWhileAds = sight.rechamberWhileAds;
            jSight.noAdsWhenMagEmpty = sight.noAdsWhenMagEmpty;
            jSight.canHoldBreath = sight.canHoldBreath;
            jSight.canVariableZoom = sight.canVariableZoom;
            jSight.hideRailWithThisScope = sight.hideRailWithThisScope;
        }

        static void ConvertJsonAttReload(JsonAttReload& jAttReload, const AttReload& reload)
        {
            jAttReload.noPartialReload = reload.noPartialReload;
            jAttReload.segmentedReload = reload.segmentedReload;
        }

        static void ConvertJsonAttAddOns(JsonAttAddOns& jAddOns, const AttAddOns& addOns)
        {
            jAddOns.motionTracker = addOns.motionTracker;
            jAddOns.silenced = addOns.silenced;
        }

        static void ConvertJsonAttGeneral(JsonAttGeneral& jGeneral, const AttGeneral& general)
        {
            jGeneral.boltAction = general.boltAction;
            jGeneral.inheritsPerks = general.inheritsPerks;
            jGeneral.enemyCrosshairRange = general.enemyCrosshairRange;

            if (general.reticleCenter && general.reticleCenter->info.name)
                jGeneral.reticleCenter = AssetName(general.reticleCenter->info.name);

            if (general.reticleSide && general.reticleSide->info.name)
                jGeneral.reticleSide = AssetName(general.reticleSide->info.name);

            jGeneral.reticleCenterSize = general.reticleCenterSize;
            jGeneral.reticleSideSize = general.reticleSideSize;
            jGeneral.moveSpeedScale = general.moveSpeedScale;
            jGeneral.adsMoveSpeedScale = general.adsMoveSpeedScale;
        }

        static void ConvertJsonAttAimAssist(JsonAttAimAssist& jAimAssist, const AttAimAssist& aimAssist)
        {
            jAimAssist.autoAimRange = aimAssist.autoAimRange;
            jAimAssist.aimAssistRange = aimAssist.aimAssistRange;
            jAimAssist.aimAssistRangeAds = aimAssist.aimAssistRangeAds;
        }

        static void ConvertJsonAttAmmunition(JsonAttAmmunition& jAmmunition, const AttAmmunition& ammunition)
        {
            jAmmunition.maxAmmo = ammunition.maxAmmo;
            jAmmunition.startAmmo = ammunition.startAmmo;
            jAmmunition.clipSize = ammunition.clipSize;
            jAmmunition.shotCount = ammunition.shotCount;
            jAmmunition.reloadAmmoAdd = ammunition.reloadAmmoAdd;
            jAmmunition.reloadStartAdd = ammunition.reloadStartAdd;
        }

        static void ConvertJsonAttDamage(JsonAttDamage& jDamage, const AttDamage& damage)
        {
            jDamage.damage = damage.damage;
            jDamage.minDamage = damage.minDamage;
            jDamage.meleeDamage = damage.meleeDamage;
            jDamage.maxDamageRange = damage.maxDamageRange;
            jDamage.minDamageRange = damage.minDamageRange;
            jDamage.playerDamage = damage.playerDamage;
            jDamage.minPlayerDamage = damage.minPlayerDamage;
        }

        static void ConvertJsonAttLocationDamage(JsonAttLocationDamage& jLocationDamage, const AttLocationDamage& locationDamage)
        {
            jLocationDamage.locNone = locationDamage.locNone;
            jLocationDamage.locHelmet = locationDamage.locHelmet;
            jLocationDamage.locHead = locationDamage.locHead;
            jLocationDamage.locNeck = locationDamage.locNeck;
            jLocationDamage.locTorsoUpper = locationDamage.locTorsoUpper;
            jLocationDamage.locTorsoLower = locationDamage.locTorsoLower;
            jLocationDamage.locRightArmUpper = locationDamage.locRightArmUpper;
            jLocationDamage.locRightArmLower = locationDamage.locRightArmLower;
            jLocationDamage.locRightHand = locationDamage.locRightHand;
            jLocationDamage.locLeftArmUpper = locationDamage.locLeftArmUpper;
            jLocationDamage.locLeftArmLower = locationDamage.locLeftArmLower;
            jLocationDamage.locLeftHand = locationDamage.locLeftHand;
            jLocationDamage.locRightLegUpper = locationDamage.locRightLegUpper;
            jLocationDamage.locRightLegLower = locationDamage.locRightLegLower;
            jLocationDamage.locRightFoot = locationDamage.locRightFoot;
            jLocationDamage.locLeftLegUpper = locationDamage.locLeftLegUpper;
            jLocationDamage.locLeftLegLower = locationDamage.locLeftLegLower;
            jLocationDamage.locLeftFoot = locationDamage.locLeftFoot;
            jLocationDamage.locGun = locationDamage.locGun;
        }

        static void ConvertJsonAttIdleSettings(JsonAttIdleSettings& jIdleSettings, const AttIdleSettings& idleSettings)
        {
            jIdleSettings.hipIdleAmount = idleSettings.hipIdleAmount;
            jIdleSettings.hipIdleSpeed = idleSettings.hipIdleSpeed;
            jIdleSettings.idleCrouchFactor = idleSettings.idleCrouchFactor;
            jIdleSettings.idleProneFactor = idleSettings.idleProneFactor;
            jIdleSettings.adsIdleLerpStartTime = idleSettings.adsIdleLerpStartTime;
            jIdleSettings.adsIdleLerpTime = idleSettings.adsIdleLerpTime;
        }

        static void ConvertJsonAttADSSettings(JsonAttADSSettings& jAdsSettings, const AttADSSettings& adsSettings)
        {
            jAdsSettings.adsSpread = adsSettings.adsSpread;
            jAdsSettings.adsAimPitch = adsSettings.adsAimPitch;
            jAdsSettings.adsTransInTime = adsSettings.adsTransInTime;
            jAdsSettings.adsTransOutTime = adsSettings.adsTransOutTime;
            jAdsSettings.adsReloadTransTime = adsSettings.adsReloadTransTime;
            jAdsSettings.adsCrosshairInFrac = adsSettings.adsCrosshairInFrac;
            jAdsSettings.adsCrosshairOutFrac = adsSettings.adsCrosshairOutFrac;
            jAdsSettings.adsZoomFov = adsSettings.adsZoomFov;
            jAdsSettings.adsZoomInFrac = adsSettings.adsZoomInFrac;
            jAdsSettings.adsZoomOutFrac = adsSettings.adsZoomOutFrac;
            jAdsSettings.adsBobFactor = adsSettings.adsBobFactor;
            jAdsSettings.adsViewBobMult = adsSettings.adsViewBobMult;
            jAdsSettings.adsViewErrorMin = adsSettings.adsViewErrorMin;
            jAdsSettings.adsViewErrorMax = adsSettings.adsViewErrorMax;
        }

        static void ConvertJsonAttHipSpread(JsonAttHipSpread& jHipSpread, const AttHipSpread& hipSpread)
        {
            jHipSpread.hipSpreadStandMin = hipSpread.hipSpreadStandMin;
            jHipSpread.hipSpreadDuckedMin = hipSpread.hipSpreadDuckedMin;
            jHipSpread.hipSpreadProneMin = hipSpread.hipSpreadProneMin;
            jHipSpread.hipSpreadMax = hipSpread.hipSpreadMax;
            jHipSpread.hipSpreadDuckedMax = hipSpread.hipSpreadDuckedMax;
            jHipSpread.hipSpreadProneMax = hipSpread.hipSpreadProneMax;
            jHipSpread.hipSpreadFireAdd = hipSpread.hipSpreadFireAdd;
            jHipSpread.hipSpreadTurnAdd = hipSpread.hipSpreadTurnAdd;
            jHipSpread.hipSpreadMoveAdd = hipSpread.hipSpreadMoveAdd;
            jHipSpread.hipSpreadDecayRate = hipSpread.hipSpreadDecayRate;
            jHipSpread.hipSpreadDuckedDecay = hipSpread.hipSpreadDuckedDecay;
            jHipSpread.hipSpreadProneDecay = hipSpread.hipSpreadProneDecay;
        }

        static void ConvertJsonAttGunKick(JsonAttGunKick& jGunKick, const AttGunKick& gunKick)
        {
            jGunKick.hipGunKickReducedKickBullets = gunKick.hipGunKickReducedKickBullets;
            jGunKick.hipGunKickReducedKickPercent = gunKick.hipGunKickReducedKickPercent;
            jGunKick.hipGunKickPitchMin = gunKick.hipGunKickPitchMin;
            jGunKick.hipGunKickPitchMax = gunKick.hipGunKickPitchMax;
            jGunKick.hipGunKickYawMin = gunKick.hipGunKickYawMin;
            jGunKick.hipGunKickYawMax = gunKick.hipGunKickYawMax;
            jGunKick.hipGunKickAccel = gunKick.hipGunKickAccel;
            jGunKick.hipGunKickSpeedMax = gunKick.hipGunKickSpeedMax;
            jGunKick.hipGunKickSpeedDecay = gunKick.hipGunKickSpeedDecay;
            jGunKick.hipGunKickStaticDecay = gunKick.hipGunKickStaticDecay;
            jGunKick.adsGunKickReducedKickBullets = gunKick.adsGunKickReducedKickBullets;
            jGunKick.adsGunKickReducedKickPercent = gunKick.adsGunKickReducedKickPercent;
            jGunKick.adsGunKickPitchMin = gunKick.adsGunKickPitchMin;
            jGunKick.adsGunKickPitchMax = gunKick.adsGunKickPitchMax;
            jGunKick.adsGunKickYawMin = gunKick.adsGunKickYawMin;
            jGunKick.adsGunKickYawMax = gunKick.adsGunKickYawMax;
            jGunKick.adsGunKickAccel = gunKick.adsGunKickAccel;
            jGunKick.adsGunKickSpeedMax = gunKick.adsGunKickSpeedMax;
            jGunKick.adsGunKickSpeedDecay = gunKick.adsGunKickSpeedDecay;
            jGunKick.adsGunKickStaticDecay = gunKick.adsGunKickStaticDecay;
        }

        static void ConvertJsonAttViewKick(JsonAttViewKick& jViewKick, const AttViewKick& viewKick)
        {
            jViewKick.hipViewKickPitchMin = viewKick.hipViewKickPitchMin;
            jViewKick.hipViewKickPitchMax = viewKick.hipViewKickPitchMax;
            jViewKick.hipViewKickYawMin = viewKick.hipViewKickYawMin;
            jViewKick.hipViewKickYawMax = viewKick.hipViewKickYawMax;
            jViewKick.hipViewKickCenterSpeed = viewKick.hipViewKickCenterSpeed;
            jViewKick.adsViewKickPitchMin = viewKick.adsViewKickPitchMin;
            jViewKick.adsViewKickPitchMax = viewKick.adsViewKickPitchMax;
            jViewKick.adsViewKickYawMin = viewKick.adsViewKickYawMin;
            jViewKick.adsViewKickYawMax = viewKick.adsViewKickYawMax;
            jViewKick.adsViewKickCenterSpeed = viewKick.adsViewKickCenterSpeed;
        }

        static void ConvertJsonAttADSOverlay(JsonAttADSOverlay& jAdsOverlay, const AttADSOverlay& adsOverlay)
        {
            if (adsOverlay.overlay.shader && adsOverlay.overlay.shader->info.name)
                jAdsOverlay.shader = AssetName(adsOverlay.overlay.shader->info.name);

            if (adsOverlay.overlay.shaderLowRes && adsOverlay.overlay.shaderLowRes->info.name)
                jAdsOverlay.shaderLowRes = AssetName(adsOverlay.overlay.shaderLowRes->info.name);

            if (adsOverlay.overlay.shaderEMP && adsOverlay.overlay.shaderEMP->info.name)
                jAdsOverlay.shaderEMP = AssetName(adsOverlay.overlay.shaderEMP->info.name);

            if (adsOverlay.overlay.shaderEMPLowRes && adsOverlay.overlay.shaderEMPLowRes->info.name)
                jAdsOverlay.shaderEMPLowRes = AssetName(adsOverlay.overlay.shaderEMPLowRes->info.name);

            jAdsOverlay.reticle = adsOverlay.overlay.reticle;
            jAdsOverlay.width = adsOverlay.overlay.width;
            jAdsOverlay.height = adsOverlay.overlay.height;
            jAdsOverlay.widthSplitscreen = adsOverlay.overlay.widthSplitscreen;
            jAdsOverlay.heightSplitscreen = adsOverlay.overlay.heightSplitscreen;
            jAdsOverlay.thermalScope = adsOverlay.thermalScope;
        }

        static void ConvertJsonAttUI(JsonAttUI& jUi, const AttUI& ui)
        {
            if (ui.dpadIcon && ui.dpadIcon->info.name)
                jUi.dpadIcon = AssetName(ui.dpadIcon->info.name);
            if (ui.ammoCounterIcon && ui.ammoCounterIcon->info.name)
                jUi.ammoCounterIcon = AssetName(ui.ammoCounterIcon->info.name);

            jUi.dpadIconRatio = ui.dpadIconRatio;
            jUi.ammoCounterIconRatio = ui.ammoCounterIconRatio;
            jUi.ammoCounterClip = ui.ammoCounterClip;
        }

        static void ConvertJsonAttRumbles(JsonAttRumbles& jRumbles, const AttRumbles& rumbles)
        {
            if (rumbles.fireRumble)
                jRumbles.fireRumble = rumbles.fireRumble;

            if (rumbles.meleeImpactRumble)
                jRumbles.meleeImpactRumble = rumbles.meleeImpactRumble;
        }

        static void ConvertJsonAttProjectile(JsonAttProjectile& jProjectile, const AttProjectile& projectile)
        {
            jProjectile.explosionRadius = projectile.explosionRadius;
            jProjectile.explosionInnerDamage = projectile.explosionInnerDamage;
            jProjectile.explosionOuterDamage = projectile.explosionOuterDamage;
            jProjectile.damageConeAngle = projectile.damageConeAngle;
            jProjectile.projectileSpeed = projectile.projectileSpeed;
            jProjectile.projectileSpeedUp = projectile.projectileSpeedUp;
            jProjectile.projectileActivateDist = projectile.projectileActivateDist;
            jProjectile.projectileLifetime = projectile.projectileLifetime;

            if (projectile.projectileModel && projectile.projectileModel->name)
                jProjectile.projectileModel = AssetName(projectile.projectileModel->name);

            jProjectile.projExplosionType = projectile.projExplosionType;

            if (projectile.projExplosionEffect && projectile.projExplosionEffect->name)
                jProjectile.projExplosionEffect = AssetName(projectile.projExplosionEffect->name);

            jProjectile.projExplosionEffectForceNormalUp = projectile.projExplosionEffectForceNormalUp;

            if (projectile.projExplosionSound.name && projectile.projExplosionSound.name->soundName)
                jProjectile.projExplosionSound = projectile.projExplosionSound.name->soundName;

            if (projectile.projDudEffect && projectile.projDudEffect->name)
                jProjectile.projDudEffect = AssetName(projectile.projDudEffect->name);

            if (projectile.projDudSound.name && projectile.projDudSound.name->soundName)
                jProjectile.projDudSound = projectile.projDudSound.name->soundName;

            jProjectile.projImpactExplode = projectile.projImpactExplode;
            jProjectile.destabilizationRateTime = projectile.destabilizationRateTime;
            jProjectile.destabilizationCurvatureMax = projectile.destabilizationCurvatureMax;
            jProjectile.destabilizeDistance = projectile.destabilizeDistance;

            if (projectile.projTrailEffect && projectile.projTrailEffect->name)
                jProjectile.projTrailEffect = AssetName(projectile.projTrailEffect->name);

            jProjectile.projIgnitionDelay = projectile.projIgnitionDelay;

            if (projectile.projIgnitionEffect && projectile.projIgnitionEffect->name)
                jProjectile.projIgnitionEffect = AssetName(projectile.projIgnitionEffect->name);

            if (projectile.projIgnitionSound.name && projectile.projIgnitionSound.name->soundName)
                jProjectile.projIgnitionSound = projectile.projIgnitionSound.name->soundName;
        }

        std::ostream& m_stream;
    };
} // namespace

namespace IW5
{
    void DumpWeaponAttachmentAsJson(std::ostream& stream, const WeaponAttachment* attachment, AssetDumpingContext& context)
    {
        const JsonDumper dumper(context, stream);
        dumper.Dump(attachment);
    }
} // namespace IW5
