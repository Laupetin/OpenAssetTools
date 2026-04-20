#include "WeaponWriterIW3.h"

#include "Weapon/AbstractWeaponWriter.h"
#include "ObjWriting.h"
#include "Game/IW3/Weapon/WeaponConstantsIW3.h"

#include <cassert>
#include <cmath>
#include <sstream>
#include <Dumping/AssetDumpingContext.h>
#include <algorithm>
#include <iomanip>

using namespace IW3;

namespace
{
    class WeaponWriter final : public ::weapon::AbstractBaseWriter, public weapon::IWriterIW3
    {
    public:
        explicit WeaponWriter(std::ostream& stream, AssetDumpingContext& context)
            : AbstractBaseWriter(stream, context)
        {
        }

        void WriteWeaponType(weapType_t weapType)
        {
            m_stream << "weaponType\\";
            switch (weapType)
            {
            case WEAPTYPE_BULLET:
                m_stream << "bullet\\";
                break;
            case WEAPTYPE_GRENADE:
                m_stream << "grenade\\";
                break;
            case WEAPTYPE_PROJECTILE:
                m_stream << "projectile\\";
                break;
            case WEAPTYPE_BINOCULARS:
                m_stream << "binoculars\\";
                break;
            };
        }

        void WriteWeaponClass(weapClass_t weapClass)
        {
            m_stream << "weaponClass\\";
            switch (weapClass)
            {
            case WEAPCLASS_RIFLE:
                m_stream << "rifle\\";
                break;
            case WEAPCLASS_MG:
                m_stream << "mg\\";
                break;
            case WEAPCLASS_SMG:
                m_stream << "smg\\";
                break;
            case WEAPCLASS_SPREAD:
                m_stream << "spread\\";
                break;
            case WEAPCLASS_PISTOL:
                m_stream << "pistol\\";
                break;
            case WEAPCLASS_GRENADE:
                m_stream << "grenade\\";
                break;
            case WEAPCLASS_ROCKETLAUNCHER:
                m_stream << "rocketlauncher\\";
                break;
            case WEAPCLASS_TURRET:
                m_stream << "turret\\";
                break;
            case WEAPCLASS_NON_PLAYER:
                m_stream << "non-player\\";
                break;
            case WEAPCLASS_ITEM:
                m_stream << "item\\";
                break;
            };
        }

        std::string ToUpper(const char* str)
        {
            if (!str)
                return "";

            std::string result(str);
            std::transform(result.begin(),
                           result.end(),
                           result.begin(),
                           [](unsigned char c)
                           {
                               return std::toupper(c);
                           });

            return result;
        }

        void WritePenetrateType(PenetrateType penetrateType)
        {
            m_stream << "penetrateType\\";
            switch (penetrateType)
            {
            case PENETRATE_TYPE_NONE:
                m_stream << "none\\";
                break;
            case PENETRATE_TYPE_SMALL:
                m_stream << "small\\";
                break;
            case PENETRATE_TYPE_MEDIUM:
                m_stream << "medium\\";
                break;
            case PENETRATE_TYPE_LARGE:
                m_stream << "large\\";
                break;
            };
        }

        void WriteImpactType(ImpactType impactType)
        {
            m_stream << "impactType\\";
            switch (impactType)
            {
            case IMPACT_TYPE_NONE:
                m_stream << "none\\";
                break;
            case IMPACT_TYPE_BULLET_SMALL:
                m_stream << "bullet_small\\";
                break;
            case IMPACT_TYPE_BULLET_LARGE:
                m_stream << "bullet_large\\";
                break;
            case IMPACT_TYPE_BULLET_AP:
                m_stream << "bullet_ap\\";
                break;
            case IMPACT_TYPE_SHOTGUN:
                m_stream << "shotgun\\";
                break;
            case IMPACT_TYPE_GRENADE_BOUNCE:
                m_stream << "grenade_bounce\\";
                break;
            case IMPACT_TYPE_GRENADE_EXPLODE:
                m_stream << "grenade_explode\\";
                break;
            case IMPACT_TYPE_ROCKET_EXPLODE:
                m_stream << "rocket_explode\\";
                break;
            case IMPACT_TYPE_PROJECTILE_DUD:
                m_stream << "projectile_dud\\";
                break;
            };
        }

        void WriteGuidedMissileType(guidedMissileType_t missileType)
        {
            m_stream << "guidedMissileType\\";
            switch (missileType)
            {
            case MISSILE_GUIDANCE_NONE:
                m_stream << "None\\";
                break;
            case MISSILE_GUIDANCE_SIDEWINDER:
                m_stream << "Sidewinder\\";
                break;
            case MISSILE_GUIDANCE_HELLFIRE:
                m_stream << "Hellfire\\";
                break;
            case MISSILE_GUIDANCE_JAVELIN:
                m_stream << "Javelin\\";
                break;
            };
        }

        void WriteInventoryType(weapInventoryType_t inventoryType)
        {
            m_stream << "inventoryType\\";
            switch (inventoryType)
            {
            case WEAPINVENTORY_PRIMARY:
                m_stream << "primary\\";
                break;
            case WEAPINVENTORY_OFFHAND:
                m_stream << "offhand\\";
                break;
            case WEAPINVENTORY_ITEM:
                m_stream << "item\\";
                break;
            case WEAPINVENTORY_ALTMODE:
                m_stream << "altmode\\";
                break;
            };
        }

        void WriteProjectileExplosionType(weapProjExposion_t projectileExpType)
        {
            m_stream << "projExplosionType\\";
            switch (projectileExpType)
            {
            case WEAPPROJEXP_GRENADE:
                m_stream << "grenade\\";
                break;
            case WEAPPROJEXP_ROCKET:
                m_stream << "rocket\\";
                break;
            case WEAPPROJEXP_FLASHBANG:
                m_stream << "flashbang\\";
                break;
            case WEAPPROJEXP_NONE:
                m_stream << "none\\";
                break;
            case WEAPPROJEXP_DUD:
                m_stream << "dud\\";
                break;
            case WEAPPROJEXP_SMOKE:
                m_stream << "smoke\\";
                break;
            case WEAPPROJEXP_HEAVY:
                m_stream << "heavy\\";
                break;
            };
        }

        void WriteFireType(weapFireType_t fireType)
        {
            m_stream << "fireType\\";
            switch (fireType)
            {
            case WEAPON_FIRETYPE_FULLAUTO:
                m_stream << "Full Auto\\";
                break;
            case WEAPON_FIRETYPE_SINGLESHOT:
                m_stream << "Single Shot\\";
                break;
            case WEAPON_FIRETYPE_BURSTFIRE2:
                m_stream << "2-Round Burst\\";
                break;
            case WEAPON_FIRETYPE_BURSTFIRE3:
                m_stream << "3-Round Burst\\";
                break;
            case WEAPON_FIRETYPE_BURSTFIRE4:
                m_stream << "4-Round Burst\\";
                break;
            };
        }

        void WritePlayerAnimType(playerAnimType_t playerAnimType)
        {
            m_stream << "playerAnimType\\";
            switch (playerAnimType)
            {
            case PLAYERANIMTYPE_NONE:
                m_stream << "none\\";
                break;
            case PLAYERANIMTYPE_OTHER:
                m_stream << "other\\";
                break;
            case PLAYERANIMTYPE_PISTOL:
                m_stream << "pistol\\";
                break;
            case PLAYERANIMTYPE_SMG:
                m_stream << "smg\\";
                break;
            case PLAYERANIMTYPE_AUTORIFLE:
                m_stream << "autorifle\\";
                break;
            case PLAYERANIMTYPE_MG:
                m_stream << "mg\\";
                break;
            case PLAYERANIMTYPE_SNIPER:
                m_stream << "sniper\\";
                break;
            case PLAYERANIMTYPE_ROCKETLAUNCHER:
                m_stream << "rocketlauncher\\";
                break;
            case PLAYERANIMTYPE_EXPLOSIVE:
                m_stream << "explosive\\";
                break;
            case PLAYERANIMTYPE_GRENADE:
                m_stream << "grenade\\";
                break;
            case PLAYERANIMTYPE_TURRET:
                m_stream << "turret\\";
                break;
            case PLAYERANIMTYPE_C4:
                m_stream << "c4\\";
                break;
            case PLAYERANIMTYPE_M203:
                m_stream << "m203\\";
                break;
            case PLAYERANIMTYPE_HOLD:
                m_stream << "hold\\";
                break;
            case PLAYERANIMTYPE_BRIEFCASE:
                m_stream << "briefcase\\";
                break;
            case PLAYERANIMTYPE_RIOTSHIELD:
                m_stream << "riotshield\\";
                break;
            case PLAYERANIMTYPE_LAPTOP:
                m_stream << "laptop\\";
                break;
            case PLAYERANIMTYPE_THROWINGKNIFE:
                m_stream << "throwingknife\\";
                break;
            };
        }

        void WriteOverlayReticle(weapOverlayReticle_t overlayReticle)
        {
            m_stream << "adsOverlayReticle\\";
            switch (overlayReticle)
            {
            case WEAPOVERLAYRETICLE_NONE:
                m_stream << "none\\";
                break;
            case WEAPOVERLAYRETICLE_CROSSHAIR:
                m_stream << "crosshair\\";
                break;
            };
        }

        void WriteIconRatioType(const char* propertyName, weaponIconRatioType_t ratioType)
        {
            m_stream << propertyName << "\\";
            switch (ratioType)
            {
            case WEAPON_ICON_RATIO_1TO1:
                m_stream << "1:1\\";
                break;
            case WEAPON_ICON_RATIO_2TO1:
                m_stream << "2:1\\";
                break;
            case WEAPON_ICON_RATIO_4TO1:
                m_stream << "4:1\\";
                break;
            };
        }

        void WriteAmmoCounterClipType(ammoCounterClipType_t clipType)
        {
            m_stream << "ammoCounterClip\\";
            switch (clipType)
            {
            case AMMO_COUNTER_CLIP_NONE:
                m_stream << "None\\";
                break;
            case AMMO_COUNTER_CLIP_MAGAZINE:
                m_stream << "Magazine\\";
                break;
            case AMMO_COUNTER_CLIP_SHORTMAGAZINE:
                m_stream << "ShortMagazine\\";
                break;
            case AMMO_COUNTER_CLIP_SHOTGUN:
                m_stream << "Shotgun\\";
                break;
            case AMMO_COUNTER_CLIP_ROCKET:
                m_stream << "Rocket\\";
                break;
            case AMMO_COUNTER_CLIP_BELTFED:
                m_stream << "Beltfed\\";
                break;
            case AMMO_COUNTER_CLIP_ALTWEAPON:
                m_stream << "AltWeapon\\";
                break;
            };
        }
        
        std::string FormatFloat(float value)
        {
            std::ostringstream ss;
            ss << value;

            return ss.str();
        }

        void WriteTimeProperty(const char* propertyName, int ms)
        {
            float seconds = ms / 1000.0f;

            m_stream << propertyName << "\\" << FormatFloat(seconds) << "\\";
        };

        void WriteWeapon(const WeaponDef& weapon) override
        {
            m_stream << "WEAPONFILE\\";

            m_stream << "modeName\\" << weapon.szModeName << "\\";
            WritePlayerAnimType(weapon.playerAnimType);
            m_stream << "altWeapon\\" << weapon.szAltWeaponName << "\\";
            WriteWeaponType(weapon.weapType);
            WriteWeaponClass(weapon.weapClass);
            WritePenetrateType(weapon.penetrateType);
            WriteImpactType(weapon.impactType);
            WriteInventoryType(weapon.inventoryType);
            m_stream << "maxSteeringAccel\\" << weapon.maxSteeringAccel << "\\";
            m_stream << "displayName\\" << weapon.szDisplayName << "\\";
            m_stream << "AIOverlayDescription\\\\";
            WriteFireType(weapon.fireType);
            m_stream << "twoHanded\\1\\";
            m_stream << "rifleBullet\\" << weapon.bRifleBullet << "\\";
            m_stream << "armorPiercing\\" << weapon.armorPiercing << "\\";
            m_stream << "boltAction\\" << weapon.bBoltAction << "\\";
            m_stream << "aimDownSight\\" << weapon.aimDownSight << "\\";
            m_stream << "rechamberWhileAds\\" << weapon.bRechamberWhileAds << "\\";
            m_stream << "noPartialReload\\" << weapon.bNoPartialReload << "\\";
            m_stream << "segmentedReload\\" << weapon.bSegmentedReload << "\\";
            m_stream << "adsFire\\" << weapon.adsFireOnly << "\\";
            m_stream << "noAdsWhenMagEmpty\\" << weapon.noAdsWhenMagEmpty << "\\";
            m_stream << "avoidDropCleanup\\" << weapon.avoidDropCleanup << "\\";
            m_stream << "enhanced\\" << weapon.enhanced << "\\";
            m_stream << "cancelAutoHolsterWhenEmpty\\" << weapon.cancelAutoHolsterWhenEmpty << "\\";
            m_stream << "suppressAmmoReserveDisplay\\" << weapon.suppressAmmoReserveDisplay << "\\";
            m_stream << "laserSightDuringNightvision\\" << weapon.laserSightDuringNightvision << "\\";
            m_stream << "blocksProne\\" << weapon.blocksProne << "\\";
            m_stream << "silenced\\" << weapon.silenced << "\\";
            m_stream << "autoAimRange\\" << weapon.autoAimRange << "\\";
            m_stream << "aimAssistRange\\" << weapon.aimAssistRange << "\\";
            m_stream << "aimAssistRangeAds\\" << weapon.aimAssistRangeAds << "\\";
            m_stream << "enemyCrosshairRange\\" << weapon.enemyCrosshairRange << "\\";
            m_stream << "crosshairColorChange\\" << weapon.crosshairColorChange << "\\";
            m_stream << "moveSpeedScale\\" << weapon.moveSpeedScale << "\\";
            m_stream << "adsMoveSpeedScale\\" << weapon.adsMoveSpeedScale << "\\";
            m_stream << "sprintDurationScale\\" << weapon.sprintDurationScale << "\\";
            m_stream << "gunMaxPitch\\" << weapon.fGunMaxPitch << "\\";
            m_stream << "gunMaxYaw\\" << weapon.fGunMaxYaw << "\\";
            m_stream << "lowAmmoWarningThreshold\\" << weapon.lowAmmoWarningThreshold << "\\";
            m_stream << "ammoName\\" << ToUpper(weapon.szAmmoName) << "\\";
            m_stream << "maxAmmo\\" << weapon.iMaxAmmo << "\\";
            m_stream << "startAmmo\\" << weapon.iStartAmmo << "\\";
            m_stream << "clipName\\" << weapon.szClipName << "\\";
            m_stream << "clipSize\\" << weapon.iClipSize << "\\";
            m_stream << "shotCount\\" << weapon.shotCount << "\\";
            m_stream << "dropAmmoMin\\" << weapon.iDropAmmoMin << "\\";
            m_stream << "dropAmmoMax\\" << weapon.iDropAmmoMax << "\\";
            m_stream << "reloadAmmoAdd\\" << weapon.iReloadAmmoAdd << "\\";
            m_stream << "reloadStartAdd\\" << weapon.iReloadStartAdd << "\\";
            m_stream << "damage\\" << weapon.damage << "\\";
            m_stream << "minDamage\\" << weapon.minDamage << "\\";
            m_stream << "meleeDamage\\" << weapon.iMeleeDamage << "\\";
            m_stream << "maxDamageRange\\" << weapon.fMaxDamageRange << "\\";
            m_stream << "minDamageRange\\" << weapon.fMinDamageRange << "\\";
            m_stream << "playerDamage\\" << weapon.playerDamage << "\\";
            m_stream << "locNone\\" << weapon.locationDamageMultipliers[0] << "\\";
            m_stream << "locHelmet\\" << weapon.locationDamageMultipliers[1] << "\\";
            m_stream << "locHead\\" << weapon.locationDamageMultipliers[2] << "\\";
            m_stream << "locNeck\\" << weapon.locationDamageMultipliers[3] << "\\";
            m_stream << "locTorsoUpper\\" << weapon.locationDamageMultipliers[4] << "\\";
            m_stream << "locTorsoLower\\" << weapon.locationDamageMultipliers[5] << "\\";
            m_stream << "locRightArmUpper\\" << weapon.locationDamageMultipliers[6] << "\\";
            m_stream << "locRightArmLower\\" << weapon.locationDamageMultipliers[7] << "\\";
            m_stream << "locRightHand\\" << weapon.locationDamageMultipliers[8] << "\\";
            m_stream << "locLeftArmUpper\\" << weapon.locationDamageMultipliers[9] << "\\";
            m_stream << "locLeftArmLower\\" << weapon.locationDamageMultipliers[10] << "\\";
            m_stream << "locLeftHand\\" << weapon.locationDamageMultipliers[11] << "\\";
            m_stream << "locRightLegUpper\\" << weapon.locationDamageMultipliers[12] << "\\";
            m_stream << "locRightLegLower\\" << weapon.locationDamageMultipliers[13] << "\\";
            m_stream << "locRightFoot\\" << weapon.locationDamageMultipliers[14] << "\\";
            m_stream << "locLeftLegUpper\\" << weapon.locationDamageMultipliers[15] << "\\";
            m_stream << "locLeftLegLower\\" << weapon.locationDamageMultipliers[16] << "\\";
            m_stream << "locLeftFoot\\" << weapon.locationDamageMultipliers[17] << "\\";
            m_stream << "locGun\\" << weapon.locationDamageMultipliers[18] << "\\";
            WriteTimeProperty("fireTime", weapon.iFireTime);
            WriteTimeProperty("fireDelay", weapon.iFireDelay);
            WriteTimeProperty("meleeTime", weapon.iMeleeTime);
            WriteTimeProperty("meleeChargeTime", weapon.meleeChargeTime);
            WriteTimeProperty("meleeDelay", weapon.iMeleeDelay);
            WriteTimeProperty("meleeChargeDelay", weapon.meleeChargeDelay);
            WriteTimeProperty("reloadTime", weapon.iReloadTime);
            WriteTimeProperty("reloadEmptyTime", weapon.iReloadEmptyTime);
            WriteTimeProperty("reloadStartTime", weapon.iReloadStartTime);
            WriteTimeProperty("reloadEndTime", weapon.iReloadEndTime);
            WriteTimeProperty("reloadAddTime", weapon.iReloadAddTime);
            WriteTimeProperty("reloadStartAddTime", weapon.iReloadStartAddTime);
            WriteTimeProperty("rechamberTime", weapon.iRechamberTime);
            WriteTimeProperty("rechamberBoltTime", weapon.iRechamberBoltTime);
            WriteTimeProperty("dropTime", weapon.iDropTime);
            WriteTimeProperty("raiseTime", weapon.iRaiseTime);
            WriteTimeProperty("altDropTime", weapon.iAltDropTime);
            WriteTimeProperty("altRaiseTime", weapon.iAltRaiseTime);
            WriteTimeProperty("quickDropTime", weapon.quickDropTime);
            WriteTimeProperty("quickRaiseTime", weapon.quickRaiseTime);
            WriteTimeProperty("firstRaiseTime", weapon.iFirstRaiseTime);
            WriteTimeProperty("emptyDropTime", weapon.iEmptyDropTime);
            WriteTimeProperty("emptyRaiseTime", weapon.iEmptyRaiseTime);
            WriteTimeProperty("sprintInTime", weapon.sprintInTime);
            WriteTimeProperty("sprintLoopTime", weapon.sprintLoopTime);
            WriteTimeProperty("sprintOutTime", weapon.sprintOutTime);
            WriteTimeProperty("nightVisionWearTime", weapon.nightVisionWearTime);
            WriteTimeProperty("nightVisionWearTimeFadeOutEnd", weapon.nightVisionWearTimeFadeOutEnd);
            WriteTimeProperty("nightVisionWearTimePowerUp", weapon.nightVisionWearTimePowerUp);
            WriteTimeProperty("nightVisionRemoveTime", weapon.nightVisionRemoveTime);
            WriteTimeProperty("nightVisionRemoveTimePowerDown", weapon.nightVisionRemoveTimePowerDown);
            WriteTimeProperty("nightVisionRemoveTimeFadeInStart", weapon.nightVisionRemoveTimeFadeInStart);
            m_stream << "standMoveF\\" << weapon.vStandMove[0] << "\\";
            m_stream << "standMoveR\\" << weapon.vStandMove[1] << "\\";
            m_stream << "standMoveU\\" << weapon.vStandMove[2] << "\\";
            m_stream << "standRotP\\" << weapon.vStandRot[0] << "\\";
            m_stream << "standRotY\\" << weapon.vStandRot[1] << "\\";
            m_stream << "standRotR\\" << weapon.vStandRot[2] << "\\";
            m_stream << "standMoveMinSpeed\\" << weapon.fStandMoveMinSpeed << "\\";
            m_stream << "standRotMinSpeed\\" << weapon.fStandRotMinSpeed << "\\";
            m_stream << "posMoveRate\\" << weapon.fPosMoveRate << "\\";
            m_stream << "posRotRate\\" << weapon.fPosRotRate << "\\";
            m_stream << "duckedMoveF\\" << weapon.vDuckedMove[0] << "\\";
            m_stream << "duckedMoveR\\" << weapon.vDuckedMove[1] << "\\";
            m_stream << "duckedMoveU\\" << weapon.vDuckedMove[2] << "\\";
            m_stream << "duckedRotP\\" << weapon.vDuckedRot[0] << "\\";
            m_stream << "duckedRotY\\" << weapon.vDuckedRot[1] << "\\";
            m_stream << "duckedRotR\\" << weapon.vDuckedRot[2] << "\\";
            m_stream << "duckedOfsF\\" << weapon.vDuckedOfs[0] << "\\";
            m_stream << "duckedOfsR\\" << weapon.vDuckedOfs[1] << "\\";
            m_stream << "duckedOfsU\\" << weapon.vDuckedOfs[2] << "\\";
            m_stream << "duckedMoveMinSpeed\\" << weapon.fDuckedMoveMinSpeed << "\\";
            m_stream << "duckedRotMinSpeed\\" << weapon.fDuckedRotMinSpeed << "\\";
            m_stream << "proneMoveF\\" << weapon.vProneMove[0] << "\\";
            m_stream << "proneMoveR\\" << weapon.vProneMove[1] << "\\";
            m_stream << "proneMoveU\\" << weapon.vProneMove[2] << "\\";
            m_stream << "proneRotP\\" << weapon.vProneRot[0] << "\\";
            m_stream << "proneRotY\\" << weapon.vProneRot[1] << "\\";
            m_stream << "proneRotR\\" << weapon.vProneRot[2] << "\\";
            m_stream << "proneOfsF\\" << weapon.vProneOfs[0] << "\\";
            m_stream << "proneOfsR\\" << weapon.vProneOfs[1] << "\\";
            m_stream << "proneOfsU\\" << weapon.vProneOfs[2] << "\\";
            m_stream << "posProneMoveRate\\" << weapon.fPosProneMoveRate << "\\";
            m_stream << "posProneRotRate\\" << weapon.fPosProneRotRate << "\\";
            m_stream << "proneMoveMinSpeed\\" << weapon.fProneMoveMinSpeed << "\\";
            m_stream << "proneRotMinSpeed\\" << weapon.fProneRotMinSpeed << "\\";
            m_stream << "hipIdleAmount\\" << weapon.fHipIdleAmount << "\\";
            m_stream << "adsIdleAmount\\" << weapon.fAdsIdleAmount << "\\";
            m_stream << "hipIdleSpeed\\" << weapon.hipIdleSpeed << "\\";
            m_stream << "adsIdleSpeed\\" << weapon.adsIdleSpeed << "\\";
            m_stream << "idleCrouchFactor\\" << weapon.fIdleCrouchFactor << "\\";
            m_stream << "idleProneFactor\\" << weapon.fIdleProneFactor << "\\";
            m_stream << "adsSpread\\" << weapon.fAdsSpread << "\\";
            m_stream << "adsAimPitch\\" << weapon.fAdsAimPitch << "\\";
            WriteTimeProperty("adsTransInTime", weapon.iAdsTransInTime);
            WriteTimeProperty("adsTransOutTime", weapon.iAdsTransOutTime);
            WriteTimeProperty("adsTransBlendTime", 0);
            WriteTimeProperty("adsReloadTransTime", 0);
            m_stream << "adsCrosshairInFrac\\" << weapon.fAdsCrosshairInFrac << "\\";
            m_stream << "adsCrosshairOutFrac\\" << weapon.fAdsCrosshairOutFrac << "\\";
            m_stream << "adsZoomFov\\" << weapon.fAdsZoomFov << "\\";
            m_stream << "adsZoomInFrac\\" << weapon.fAdsZoomInFrac << "\\";
            m_stream << "adsZoomOutFrac\\" << weapon.fAdsZoomOutFrac << "\\";
            m_stream << "adsBobFactor\\" << weapon.fAdsBobFactor << "\\";
            m_stream << "adsViewBobMult\\" << weapon.fAdsViewBobMult << "\\";
            m_stream << "adsViewErrorMin\\" << weapon.adsViewErrorMin << "\\";
            m_stream << "adsViewErrorMax\\" << weapon.adsViewErrorMax << "\\";
            m_stream << "hipSpreadStandMin\\" << weapon.fHipSpreadStandMin << "\\";
            m_stream << "hipSpreadDuckedMin\\" << weapon.fHipSpreadDuckedMin << "\\";
            m_stream << "hipSpreadProneMin\\" << weapon.fHipSpreadProneMin << "\\";
            m_stream << "hipSpreadMax\\" << weapon.hipSpreadStandMax << "\\";
            m_stream << "hipSpreadDuckedMax\\" << weapon.hipSpreadDuckedMax << "\\";
            m_stream << "hipSpreadProneMax\\" << weapon.hipSpreadProneMax << "\\";
            m_stream << "hipSpreadFireAdd\\" << weapon.fHipSpreadFireAdd << "\\";
            m_stream << "hipSpreadTurnAdd\\" << weapon.fHipSpreadTurnAdd << "\\";
            m_stream << "hipSpreadMoveAdd\\" << weapon.fHipSpreadMoveAdd << "\\";
            m_stream << "hipSpreadDecayRate\\" << weapon.fHipSpreadDecayRate << "\\";
            m_stream << "hipSpreadDuckedDecay\\" << weapon.fHipSpreadDuckedDecay << "\\";
            m_stream << "hipSpreadProneDecay\\" << weapon.fHipSpreadProneDecay << "\\";
            m_stream << "hipGunKickReducedKickBullets\\" << weapon.hipGunKickReducedKickBullets << "\\";
            m_stream << "hipGunKickReducedKickPercent\\" << weapon.hipGunKickReducedKickPercent << "\\";
            m_stream << "hipGunKickPitchMin\\" << weapon.fHipGunKickPitchMin << "\\";
            m_stream << "hipGunKickPitchMax\\" << weapon.fHipGunKickPitchMax << "\\";
            m_stream << "hipGunKickYawMin\\" << weapon.fHipGunKickYawMin << "\\";
            m_stream << "hipGunKickYawMax\\" << weapon.fHipGunKickYawMax << "\\";
            m_stream << "hipGunKickAccel\\" << weapon.fHipGunKickAccel << "\\";
            m_stream << "hipGunKickSpeedMax\\" << weapon.fHipGunKickSpeedMax << "\\";
            m_stream << "hipGunKickSpeedDecay\\" << weapon.fHipGunKickSpeedDecay << "\\";
            m_stream << "hipGunKickStaticDecay\\" << weapon.fHipGunKickStaticDecay << "\\";
            m_stream << "adsGunKickReducedKickBullets\\" << weapon.adsGunKickReducedKickBullets << "\\";
            m_stream << "adsGunKickReducedKickPercent\\" << weapon.adsGunKickReducedKickPercent << "\\";
            m_stream << "adsGunKickPitchMin\\" << weapon.fAdsGunKickPitchMin << "\\";
            m_stream << "adsGunKickPitchMax\\" << weapon.fAdsGunKickPitchMax << "\\";
            m_stream << "adsGunKickYawMin\\" << weapon.fAdsGunKickYawMin << "\\";
            m_stream << "adsGunKickYawMax\\" << weapon.fAdsGunKickYawMax << "\\";
            m_stream << "adsGunKickAccel\\" << weapon.fAdsGunKickAccel << "\\";
            m_stream << "adsGunKickSpeedMax\\" << weapon.fAdsGunKickSpeedMax << "\\";
            m_stream << "adsGunKickSpeedDecay\\" << weapon.fAdsGunKickSpeedDecay << "\\";
            m_stream << "adsGunKickStaticDecay\\" << weapon.fAdsGunKickStaticDecay << "\\";
            m_stream << "hipViewKickPitchMin\\" << weapon.fHipViewKickPitchMin << "\\";
            m_stream << "hipViewKickPitchMax\\" << weapon.fHipViewKickPitchMax << "\\";
            m_stream << "hipViewKickYawMin\\" << weapon.fHipViewKickYawMin << "\\";
            m_stream << "hipViewKickYawMax\\" << weapon.fHipViewKickYawMax << "\\";
            m_stream << "hipViewKickCenterSpeed\\" << weapon.fHipViewKickCenterSpeed << "\\";
            m_stream << "adsViewKickPitchMin\\" << weapon.fAdsViewKickPitchMin << "\\";
            m_stream << "adsViewKickPitchMax\\" << weapon.fAdsViewKickPitchMax << "\\";
            m_stream << "adsViewKickYawMin\\" << weapon.fAdsViewKickYawMin << "\\";
            m_stream << "adsViewKickYawMax\\" << weapon.fAdsViewKickYawMax << "\\";
            m_stream << "adsViewKickCenterSpeed\\" << weapon.fAdsViewKickCenterSpeed << "\\";
            m_stream << "swayMaxAngle\\" << weapon.swayMaxAngle << "\\";
            m_stream << "swayLerpSpeed\\" << weapon.swayLerpSpeed << "\\";
            m_stream << "swayPitchScale\\" << weapon.swayPitchScale << "\\";
            m_stream << "swayYawScale\\" << weapon.swayYawScale << "\\";
            m_stream << "swayHorizScale\\" << weapon.swayHorizScale << "\\";
            m_stream << "swayVertScale\\" << weapon.swayVertScale << "\\";
            m_stream << "swayShellShockScale\\" << weapon.swayShellShockScale << "\\";
            m_stream << "adsSwayMaxAngle\\" << weapon.adsSwayMaxAngle << "\\";
            m_stream << "adsSwayLerpSpeed\\" << weapon.adsSwayLerpSpeed << "\\";
            m_stream << "adsSwayPitchScale\\" << weapon.adsSwayPitchScale << "\\";
            m_stream << "adsSwayYawScale\\" << weapon.adsSwayYawScale << "\\";
            m_stream << "adsSwayHorizScale\\" << weapon.adsSwayHorizScale << "\\";
            m_stream << "adsSwayVertScale\\" << weapon.adsSwayVertScale << "\\";
            m_stream << "fightDist\\" << weapon.fightDist << "\\";
            m_stream << "maxDist\\" << weapon.maxDist << "\\";
            m_stream << "aiVsAiAccuracyGraph\\" << weapon.aiVsAiAccuracyGraphName << "\\";
            m_stream << "aiVsPlayerAccuracyGraph\\" << weapon.aiVsPlayerAccuracyGraphName << "\\";

            if (weapon.reticleCenter)
            {
                m_stream << "reticleCenter\\" << weapon.reticleCenter->info.name << "\\";
            }
            else
            {
                m_stream << "reticleCenter\\\\";
            }

            if (weapon.reticleSide)
            {
                m_stream << "reticleSide\\" << weapon.reticleSide->info.name << "\\";
            }
            else
            {
                m_stream << "reticleSide\\\\";
            }

            m_stream << "reticleCenterSize\\" << weapon.iReticleCenterSize << "\\";
            m_stream << "reticleSideSize\\" << weapon.iReticleSideSize << "\\";
            m_stream << "reticleMinOfs\\" << weapon.iReticleMinOfs << "\\";
            m_stream << "hipReticleSidePos\\" << weapon.fHipReticleSidePos << "\\";

            if (weapon.overlayMaterial)
            {
                m_stream << "adsOverlayShader\\" << weapon.overlayMaterial->info.name << "\\";
            }
            else
            {
                m_stream << "adsOverlayShader\\\\";
            }

            if (weapon.overlayMaterialLowRes)
            {
                m_stream << "adsOverlayShaderLowRes\\" << weapon.overlayMaterialLowRes->info.name << "\\";
            }
            else
            {
                m_stream << "adsOverlayShaderLowRes\\\\";
            }

            WriteOverlayReticle(weapon.overlayReticle);
            m_stream << "adsOverlayWidth\\" << weapon.overlayWidth << "\\";
            m_stream << "adsOverlayHeight\\" << weapon.overlayHeight << "\\";

            if (weapon.gunXModel[0])
            {
                m_stream << "gunModel\\" << weapon.gunXModel[0]->name << "\\";
            }
            else
            {
                m_stream << "gunModel\\\\";
            }

            for (int i = 1; i < 16; i++)
            {
                if (weapon.gunXModel[i])
                {
                    m_stream << "gunModel" << i + 1 << "\\" << weapon.gunXModel[i]->name << "\\";
                }
                else
                {
                    m_stream << "gunModel" << i + 1 << "\\\\";
                }
            }

            if (weapon.handXModel)
            {
                m_stream << "handModel\\" << weapon.handXModel->name << "\\";
            }
            else
            {
                m_stream << "handModel\\\\";
            }

            if (weapon.worldModel[0])
            {
                m_stream << "worldModel\\" << weapon.worldModel[0]->name << "\\";
            }
            else
            {
                m_stream << "guworldModelnModel\\\\";
            }

            for (int i = 1; i < 16; i++)
            {
                if (weapon.worldModel[i])
                {
                    m_stream << "worldModel" << i + 1 << "\\" << weapon.worldModel[i]->name << "\\";
                }
                else
                {
                    m_stream << "worldModel" << i + 1 << "\\\\";
                }
            }

            if (weapon.worldClipModel)
            {
                m_stream << "worldClipModel\\" << weapon.worldClipModel->name << "\\";
            }
            else
            {
                m_stream << "worldClipModel\\\\";
            }

            if (weapon.knifeModel)
            {
                m_stream << "knifeModel\\" << weapon.knifeModel->name << "\\";
            }
            else
            {
                m_stream << "knifeModel\\\\";
            }

            if (weapon.rocketModel)
            {
                //m_stream << "rocketModel\\" << weapon.rocketModel->name << "\\";
            }
            else
            {
                //m_stream << "rocketModel\\\\";
            }

            if (weapon.worldKnifeModel)
            {
                m_stream << "worldKnifeModel\\" << weapon.worldKnifeModel->name << "\\";
            }
            else
            {
                m_stream << "worldKnifeModel\\\\";
            }

            m_stream << "idleAnim\\" << weapon.szXAnims[1] << "\\";
            m_stream << "emptyIdleAnim\\" << weapon.szXAnims[2] << "\\";
            m_stream << "fireAnim\\" << weapon.szXAnims[3] << "\\";
            m_stream << "lastShotAnim\\" << weapon.szXAnims[5] << "\\";
            m_stream << "rechamberAnim\\" << weapon.szXAnims[4] << "\\";
            m_stream << "meleeAnim\\" << weapon.szXAnims[7] << "\\";
            m_stream << "meleeChargeAnim\\" << weapon.szXAnims[8] << "\\";
            m_stream << "reloadAnim\\" << weapon.szXAnims[9] << "\\";
            m_stream << "reloadEmptyAnim\\" << weapon.szXAnims[10] << "\\";
            m_stream << "reloadStartAnim\\" << weapon.szXAnims[11] << "\\";
            m_stream << "reloadEndAnim\\" << weapon.szXAnims[12] << "\\";
            m_stream << "raiseAnim\\" << weapon.szXAnims[13] << "\\";
            m_stream << "dropAnim\\" << weapon.szXAnims[15] << "\\";
            m_stream << "firstRaiseAnim\\" << weapon.szXAnims[14] << "\\";
            m_stream << "altRaiseAnim\\" << weapon.szXAnims[16] << "\\";
            m_stream << "altDropAnim\\" << weapon.szXAnims[17] << "\\";
            m_stream << "quickRaiseAnim\\" << weapon.szXAnims[18] << "\\";
            m_stream << "quickDropAnim\\" << weapon.szXAnims[19] << "\\";
            m_stream << "emptyRaiseAnim\\" << weapon.szXAnims[20] << "\\";
            m_stream << "emptyDropAnim\\" << weapon.szXAnims[21] << "\\";
            m_stream << "sprintInAnim\\" << weapon.szXAnims[22] << "\\";
            m_stream << "sprintLoopAnim\\" << weapon.szXAnims[23] << "\\";
            m_stream << "sprintOutAnim\\" << weapon.szXAnims[24] << "\\";
            m_stream << "nightVisionWearAnim\\" << weapon.szXAnims[25] << "\\";
            m_stream << "nightVisionRemoveAnim\\" << weapon.szXAnims[26] << "\\";
            m_stream << "adsFireAnim\\" << weapon.szXAnims[28] << "\\";
            m_stream << "adsLastShotAnim\\" << weapon.szXAnims[29] << "\\";
            m_stream << "adsRechamberAnim\\" << weapon.szXAnims[30] << "\\";
            m_stream << "adsUpAnim\\" << weapon.szXAnims[31] << "\\";
            m_stream << "adsDownAnim\\" << weapon.szXAnims[32] << "\\";
            if (weapon.viewFlashEffect)
            {
                m_stream << "viewFlashEffect\\" << weapon.viewFlashEffect->name << "\\";
            }
            else
            {
                m_stream << "viewFlashEffect\\\\";
            }

            if (weapon.worldFlashEffect)
            {
                m_stream << "worldFlashEffect\\" << weapon.worldFlashEffect->name << "\\";
            }
            else
            {
                m_stream << "worldFlashEffect\\\\";
            }

            if (weapon.viewShellEjectEffect)
            {
                m_stream << "viewShellEjectEffect\\" << weapon.viewShellEjectEffect->name << "\\";
            }
            else
            {
                m_stream << "viewShellEjectEffect\\\\";
            }

            if (weapon.worldShellEjectEffect)
            {
                m_stream << "worldShellEjectEffect\\" << weapon.worldShellEjectEffect->name << "\\";
            }
            else
            {
                m_stream << "worldShellEjectEffect\\\\";
            }

            if (weapon.viewLastShotEjectEffect)
            {
                m_stream << "viewLastShotEjectEffect\\" << weapon.viewLastShotEjectEffect->name << "\\";
            }
            else
            {
                m_stream << "viewLastShotEjectEffect\\\\";
            }

            if (weapon.worldLastShotEjectEffect)
            {
                m_stream << "worldLastShotEjectEffect\\" << weapon.worldLastShotEjectEffect->name << "\\";
            }
            else
            {
                m_stream << "worldLastShotEjectEffect\\\\";
            }

            m_stream << "worldClipDropEffect\\\\";

            if (weapon.pickupSound.name)
            {
                m_stream << "pickupSound\\" << weapon.pickupSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "pickupSound\\\\";
            }

            if (weapon.pickupSoundPlayer.name)
            {
                m_stream << "pickupSoundPlayer\\" << weapon.pickupSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "pickupSoundPlayer\\\\";
            }

            if (weapon.ammoPickupSound.name)
            {
                m_stream << "ammoPickupSound\\" << weapon.ammoPickupSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "ammoPickupSound\\\\";
            }

            if (weapon.ammoPickupSoundPlayer.name)
            {
                m_stream << "ammoPickupSoundPlayer\\" << weapon.ammoPickupSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "ammoPickupSoundPlayer\\\\";
            }

            if (weapon.fireSound.name)
            {
                m_stream << "fireSound\\" << weapon.fireSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "fireSound\\\\";
            }

            if (weapon.fireSoundPlayer.name)
            {
                m_stream << "fireSoundPlayer\\" << weapon.fireSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "fireSoundPlayer\\\\";
            }

            if (weapon.fireLastSound.name)
            {
                m_stream << "lastShotSound\\" << weapon.fireLastSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "lastShotSound\\\\";
            }

            if (weapon.fireLastSoundPlayer.name)
            {
                m_stream << "lastShotSoundPlayer\\" << weapon.fireLastSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "lastShotSoundPlayer\\\\";
            }

            if (weapon.emptyFireSound.name)
            {
                m_stream << "emptyFireSound\\" << weapon.emptyFireSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "emptyFireSound\\\\";
            }

            if (weapon.emptyFireSoundPlayer.name)
            {
                m_stream << "emptyFireSoundPlayer\\" << weapon.emptyFireSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "emptyFireSoundPlayer\\\\";
            }

            if (weapon.meleeSwipeSound.name)
            {
                m_stream << "meleeSwipeSound\\" << weapon.meleeSwipeSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "meleeSwipeSound\\\\";
            }

            if (weapon.meleeSwipeSoundPlayer.name)
            {
                m_stream << "meleeSwipeSoundPlayer\\" << weapon.meleeSwipeSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "meleeSwipeSoundPlayer\\\\";
            }

            if (weapon.meleeHitSound.name)
            {
                m_stream << "meleeHitSound\\" << weapon.meleeHitSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "meleeHitSound\\\\";
            }

            if (weapon.meleeMissSound.name)
            {
                m_stream << "meleeMissSound\\" << weapon.meleeMissSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "meleeMissSound\\\\";
            }

            if (weapon.rechamberSound.name)
            {
                m_stream << "rechamberSound\\" << weapon.rechamberSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "rechamberSound\\\\";
            }

            if (weapon.rechamberSoundPlayer.name)
            {
                m_stream << "rechamberSoundPlayer\\" << weapon.rechamberSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "rechamberSoundPlayer\\\\";
            }

            if (weapon.reloadSound.name)
            {
                m_stream << "reloadSound\\" << weapon.reloadSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadSound\\\\";
            }

            if (weapon.reloadSoundPlayer.name)
            {
                m_stream << "reloadSoundPlayer\\" << weapon.reloadSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadSoundPlayer\\\\";
            }

            if (weapon.reloadEmptySound.name)
            {
                m_stream << "reloadEmptySound\\" << weapon.reloadEmptySound.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadEmptySound\\\\";
            }

            if (weapon.reloadEmptySoundPlayer.name)
            {
                m_stream << "reloadEmptySoundPlayer\\" << weapon.reloadEmptySoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadEmptySoundPlayer\\\\";
            }

            if (weapon.reloadStartSound.name)
            {
                m_stream << "reloadStartSound\\" << weapon.reloadStartSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadStartSound\\\\";
            }

            if (weapon.reloadStartSoundPlayer.name)
            {
                m_stream << "reloadStartSoundPlayer\\" << weapon.reloadStartSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadStartSoundPlayer\\\\";
            }

            if (weapon.reloadEndSound.name)
            {
                m_stream << "reloadEndSound\\" << weapon.reloadEndSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadEndSound\\\\";
            }

            if (weapon.reloadEndSoundPlayer.name)
            {
                m_stream << "reloadEndSoundPlayer\\" << weapon.reloadEndSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "reloadEndSoundPlayer\\\\";
            }

            if (weapon.altSwitchSound.name)
            {
                m_stream << "altSwitchSound\\" << weapon.altSwitchSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "altSwitchSound\\\\";
            }

            if (weapon.altSwitchSoundPlayer.name)
            {
                m_stream << "altSwitchSoundPlayer\\" << weapon.altSwitchSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "altSwitchSoundPlayer\\\\";
            }

            if (weapon.raiseSound.name)
            {
                m_stream << "raiseSound\\" << weapon.raiseSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "raiseSound\\\\";
            }

            if (weapon.raiseSoundPlayer.name)
            {
                m_stream << "raiseSoundPlayer\\" << weapon.raiseSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "raiseSoundPlayer\\\\";
            }

            if (weapon.firstRaiseSound.name)
            {
                m_stream << "firstRaiseSound\\" << weapon.firstRaiseSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "firstRaiseSound\\\\";
            }

            if (weapon.firstRaiseSoundPlayer.name)
            {
                m_stream << "firstRaiseSoundPlayer\\" << weapon.firstRaiseSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "firstRaiseSoundPlayer\\\\";
            }

            if (weapon.putawaySound.name)
            {
                m_stream << "putawaySound\\" << weapon.putawaySound.name->soundName << "\\";
            }
            else
            {
                m_stream << "putawaySound\\\\";
            }

            if (weapon.putawaySoundPlayer.name)
            {
                m_stream << "putawaySoundPlayer\\" << weapon.putawaySoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "putawaySoundPlayer\\\\";
            }

            if (weapon.nightVisionWearSound.name)
            {
                m_stream << "nightVisionWearSound\\" << weapon.nightVisionWearSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "nightVisionWearSound\\\\";
            }

            if (weapon.nightVisionWearSoundPlayer.name)
            {
                m_stream << "nightVisionWearSoundPlayer\\" << weapon.nightVisionWearSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "nightVisionWearSoundPlayer\\\\";
            }

            if (weapon.nightVisionRemoveSound.name)
            {
                m_stream << "nightVisionRemoveSound\\" << weapon.nightVisionRemoveSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "nightVisionRemoveSound\\\\";
            }

            if (weapon.nightVisionRemoveSoundPlayer.name)
            {
                m_stream << "nightVisionRemoveSoundPlayer\\" << weapon.nightVisionRemoveSoundPlayer.name->soundName << "\\";
            }
            else
            {
                m_stream << "nightVisionRemoveSoundPlayer\\\\";
            }

            if (weapon.hudIcon)
            {
                m_stream << "hudIcon\\" << &weapon.hudIcon->info.name[1] << "\\";
            }
            else
            {
                m_stream << "hudIcon\\\\";
            }

            if (weapon.killIcon)
            {
                m_stream << "killIcon\\" << &weapon.killIcon->info.name[1] << "\\";
            }
            else
            {
                m_stream << "killIcon\\\\";
            }

            if (weapon.dpadIcon)
            {
                m_stream << "dpadIcon\\" << &weapon.dpadIcon->info.name[1] << "\\";
            }
            else
            {
                m_stream << "dpadIcon\\\\";
            }

            if (weapon.ammoCounterIcon)
            {
                m_stream << "ammoCounterIcon\\" << &weapon.ammoCounterIcon->info.name[1] << "\\";
            }
            else
            {
                m_stream << "ammoCounterIcon\\\\";
            }

            WriteIconRatioType("hudIconRatio", weapon.hudIconRatio);
            WriteIconRatioType("killIconRatio", weapon.killIconRatio);
            WriteIconRatioType("dpadIconRatio", weapon.dpadIconRatio);
            WriteIconRatioType("ammoCounterIconRatio", weapon.ammoCounterIconRatio);
            WriteAmmoCounterClipType(weapon.ammoCounterClip);

            m_stream << "flipKillIcon\\" << weapon.flipKillIcon << "\\";
            m_stream << "fireRumble\\" << weapon.fireRumble << "\\";
            m_stream << "meleeImpactRumble\\" << weapon.meleeImpactRumble << "\\";
            m_stream << "adsDofStart\\" << weapon.adsDofStart << "\\";
            m_stream << "adsDofEnd\\" << weapon.adsDofEnd << "\\";




            
            




            

          /*  if (weapon.bounceSound && weapon.bounceSound->name)
            {
                m_stream << "bounceSound\\" << weapon.bounceSound->name->soundName << "\\";
            }
            else
            {
                m_stream << "bounceSound\\\\";
            }

            if (weapon.projectileSound.name)
            {
                m_stream << "projectileSound\\" << weapon.projectileSound.name->soundName << "\\";
            }
            else
            {
                m_stream << "projectileSound\\\\";
            }
            
            m_stream << "maxSteeringAccel\\" << weapon.maxSteeringAccel << "\\";

            m_stream << "isHandModelOverridable\\\\";
            m_stream << "reloadShowRocketTime\\" << weapon.reloadShowRocketTime << "\\";
            m_stream << "reloadAddTime\\" << weapon.iReloadAddTime << "\\";
            m_stream << "dropTime\\" << weapon.iDropTime << "\\";
            m_stream << "raiseTime\\" << weapon.iRaiseTime << "\\";
            m_stream << "altDropTime\\" << weapon.iAltDropTime << "\\";
            m_stream << "altRaiseTime\\" << weapon.iAltRaiseTime << "\\";
            m_stream << "quickDropTime\\" << weapon.quickDropTime << "\\";
            m_stream << "quickRaiseTime\\" << weapon.quickRaiseTime << "\\";
            m_stream << "firstRaiseTime\\" << weapon.iFirstRaiseTime << "\\";
            m_stream << "emptyDropTime\\" << weapon.iEmptyDropTime << "\\";
            m_stream << "emptyRaiseTime\\" << weapon.iEmptyRaiseTime << "\\";
            m_stream << "sprintInTime\\" << weapon.sprintInTime << "\\";
            m_stream << "sprintLoopTime\\" << weapon.sprintLoopTime << "\\";
            m_stream << "sprintOutTime\\" << weapon.sprintOutTime << "\\";
            m_stream << "clipOnly\\" << weapon.bClipOnly << "\\";
            m_stream << "adsOverlayInterface\\" << weapon.overlayInterface << "\\";
            m_stream << "adsReloadTransTime\\\\";



            m_stream << "lowAmmoWarningThreshold\\" << weapon.lowAmmoWarningThreshold << "\\";
            m_stream << "ammoName\\" << weapon.szAmmoName << "\\";
            m_stream << "maxAmmo\\" << weapon.iMaxAmmo << "\\";
            m_stream << "startAmmo\\" << weapon.iStartAmmo << "\\";
            m_stream << "clipName\\" << weapon.szClipName << "\\";
            m_stream << "clipSize\\" << weapon.iClipSize << "\\";


            m_stream << "explosionRadius\\" << weapon.iExplosionRadius << "\\";
            m_stream << "explosionInnerDamage\\" << weapon.iExplosionInnerDamage << "\\";
            m_stream << "explosionOuterDamage\\" << weapon.iExplosionOuterDamage << "\\";
            m_stream << "damageConeAngle\\" << weapon.damageConeAngle << "\\";
            m_stream << "projectileSpeed\\" << weapon.iProjectileSpeed << "\\";
            m_stream << "projectileSpeedUp\\" << weapon.iProjectileSpeedUp << "\\";
            m_stream << "projectileActivateDist\\" << weapon.iProjectileActivateDist << "\\";
            m_stream << "projectileLifetime\\" << weapon.projLifetime << "\\";
            m_stream << "projectileModel\\" << weapon.projectileModel << "\\";
            WriteProjectileExplosionType(weapon.projExplosion);
            m_stream << "projExplosionEffect\\" << weapon.projExplosionEffect << "\\";
            m_stream << "projExplosionEffectForceNormalUp\\" << weapon.projExplosionEffectForceNormalUp << "\\";
            m_stream << "projExplosionSound\\" << weapon.projExplosionSound.name << "\\";
            m_stream << "projDudEffect\\" << weapon.projDudEffect << "\\";
            m_stream << "projDudSound\\" << weapon.projDudSound.name << "\\";
            m_stream << "projImpactExplode\\" << weapon.bProjImpactExplode << "\\";
            m_stream << "projTrailEffect\\" << weapon.projTrailEffect << "\\";
            m_stream << "projectileDLight\\\\";
            m_stream << "projectileRed\\" << weapon.vProjectileColor[0] << "\\";
            m_stream << "projectileGreen\\" << weapon.vProjectileColor[1] << "\\";
            m_stream << "projectileBlue\\" << weapon.vProjectileColor[2] << "\\";
            WriteGuidedMissileType(weapon.guidedMissileType);
            m_stream << "projIgnitionDelay\\" << weapon.projIgnitionDelay << "\\";
            m_stream << "projIgnitionEffect\\" << weapon.projIgnitionEffect << "\\";
            m_stream << "projIgnitionSound\\" << weapon.projIgnitionSound.name << "\\";
            m_stream << "destabilizationRateTime\\" << weapon.destabilizationRateTime << "\\";
            m_stream << "destabilizationCurvatureMax\\" << weapon.destabilizationCurvatureMax << "\\";
            m_stream << "destabilizeDistance\\" << weapon.destabilizeDistance << "\\";
            m_stream << "parallelDefaultBounce\\" << weapon.parallelBounce[0] << "\\";
            m_stream << "parallelBarkBounce\\" << weapon.parallelBounce[1] << "\\";
            m_stream << "parallelBrickBounce\\" << weapon.parallelBounce[2] << "\\";
            m_stream << "parallelCarpetBounce\\" << weapon.parallelBounce[3] << "\\";
            m_stream << "parallelClothBounce\\" << weapon.parallelBounce[4] << "\\";
            m_stream << "parallelConcreteBounce\\" << weapon.parallelBounce[5] << "\\";
            m_stream << "parallelDirtBounce\\" << weapon.parallelBounce[6] << "\\";
            m_stream << "parallelFleshBounce\\" << weapon.parallelBounce[7] << "\\";
            m_stream << "parallelFoliageBounce\\" << weapon.parallelBounce[8] << "\\";
            m_stream << "parallelGlassBounce\\" << weapon.parallelBounce[9] << "\\";
            m_stream << "parallelGrassBounce\\" << weapon.parallelBounce[10] << "\\";
            m_stream << "parallelGravelBounce\\" << weapon.parallelBounce[11] << "\\";
            m_stream << "parallelIceBounce\\" << weapon.parallelBounce[12] << "\\";
            m_stream << "parallelMetalBounce\\" << weapon.parallelBounce[13] << "\\";
            m_stream << "parallelMudBounce\\" << weapon.parallelBounce[14] << "\\";
            m_stream << "parallelPaperBounce\\" << weapon.parallelBounce[15] << "\\";
            m_stream << "parallelPlasterBounce\\" << weapon.parallelBounce[16] << "\\";
            m_stream << "parallelRockBounce\\" << weapon.parallelBounce[17] << "\\";
            m_stream << "parallelSandBounce\\" << weapon.parallelBounce[18] << "\\";
            m_stream << "parallelSnowBounce\\" << weapon.parallelBounce[19] << "\\";
            m_stream << "parallelWaterBounce\\" << weapon.parallelBounce[20] << "\\";
            m_stream << "parallelWoodBounce\\" << weapon.parallelBounce[21] << "\\";
            m_stream << "parallelAsphaltBounce\\" << weapon.parallelBounce[22] << "\\";
            m_stream << "parallelCeramicBounce\\" << weapon.parallelBounce[23] << "\\";
            m_stream << "parallelPlasticBounce\\" << weapon.parallelBounce[24] << "\\";
            m_stream << "parallelRubberBounce\\" << weapon.parallelBounce[25] << "\\";
            m_stream << "parallelCushionBounce\\" << weapon.parallelBounce[26] << "\\";
            m_stream << "parallelFruitBounce\\" << weapon.parallelBounce[27] << "\\";
            m_stream << "parallelPaintedMetalBounce\\" << weapon.parallelBounce[28] << "\\";
            m_stream << "perpendicularDefaultBounce\\" << weapon.perpendicularBounce[0] << "\\";
            m_stream << "perpendicularBarkBounce\\" << weapon.perpendicularBounce[1] << "\\";
            m_stream << "perpendicularBrickBounce\\" << weapon.perpendicularBounce[2] << "\\";
            m_stream << "perpendicularCarpetBounce\\" << weapon.perpendicularBounce[3] << "\\";
            m_stream << "perpendicularClothBounce\\" << weapon.perpendicularBounce[4] << "\\";
            m_stream << "perpendicularConcreteBounce\\" << weapon.perpendicularBounce[5] << "\\";
            m_stream << "perpendicularDirtBounce\\" << weapon.perpendicularBounce[6] << "\\";
            m_stream << "perpendicularFleshBounce\\" << weapon.perpendicularBounce[7] << "\\";
            m_stream << "perpendicularFoliageBounce\\" << weapon.perpendicularBounce[8] << "\\";
            m_stream << "perpendicularGlassBounce\\" << weapon.perpendicularBounce[9] << "\\";
            m_stream << "perpendicularGrassBounce\\" << weapon.perpendicularBounce[10] << "\\";
            m_stream << "perpendicularGravelBounce\\" << weapon.perpendicularBounce[11] << "\\";
            m_stream << "perpendicularIceBounce\\" << weapon.perpendicularBounce[12] << "\\";
            m_stream << "perpendicularMetalBounce\\" << weapon.perpendicularBounce[13] << "\\";
            m_stream << "perpendicularMudBounce\\" << weapon.perpendicularBounce[14] << "\\";
            m_stream << "perpendicularPaperBounce\\" << weapon.perpendicularBounce[15] << "\\";
            m_stream << "perpendicularPlasterBounce\\" << weapon.perpendicularBounce[16] << "\\";
            m_stream << "perpendicularRockBounce\\" << weapon.perpendicularBounce[17] << "\\";
            m_stream << "perpendicularSandBounce\\" << weapon.perpendicularBounce[18] << "\\";
            m_stream << "perpendicularSnowBounce\\" << weapon.perpendicularBounce[19] << "\\";
            m_stream << "perpendicularWaterBounce\\" << weapon.perpendicularBounce[20] << "\\";
            m_stream << "perpendicularWoodBounce\\" << weapon.perpendicularBounce[21] << "\\";
            m_stream << "perpendicularAsphaltBounce\\" << weapon.perpendicularBounce[22] << "\\";
            m_stream << "perpendicularCeramicBounce\\" << weapon.perpendicularBounce[23] << "\\";
            m_stream << "perpendicularPlasticBounce\\" << weapon.perpendicularBounce[24] << "\\";
            m_stream << "perpendicularRubberBounce\\" << weapon.perpendicularBounce[25] << "\\";
            m_stream << "perpendicularCushionBounce\\" << weapon.perpendicularBounce[26] << "\\";
            m_stream << "perpendicularFruitBounce\\" << weapon.perpendicularBounce[27] << "\\";
            m_stream << "perpendicularPaintedMetalBounce\\" << weapon.perpendicularBounce[28] << "\\";
            m_stream << "fireRumble\\" << weapon.fireRumble << "\\";
            m_stream << "meleeImpactRumble\\" << weapon.meleeImpactRumble << "\\";
            m_stream << "adsDofStart\\" << weapon.adsDofStart << "\\";
            m_stream << "adsDofEnd\\" << weapon.adsDofEnd << "\\";
            m_stream << "requireLockonToFire\\" << weapon.requireLockonToFire << "\\";*/

            int endPos = 0;
            for (int i = 0; i < 8; i++)
            {
                if (weapon.hideTags[i] != 0)
                    endPos = i;
            }

            m_stream << "hideTags\\";
            for (int i = 0; i < 8; i++)
            {
                if (weapon.hideTags[i] == 0)
                    continue; 

                if (weapon.hideTags[i] < m_context.m_zone.m_script_strings.Count())
                    m_stream << m_context.m_zone.m_script_strings[weapon.hideTags[i]];

                if (i != 7 && i < endPos)
                {
                    m_stream << " ";
                }
            }
            m_stream << "\n\\";

            endPos = 0;
            for (int i = 0; i < 16; i++)
            {
                if (weapon.notetrackSoundMapKeys[i] != 0)
                    endPos = i;
            }

            m_stream << "notetrackSoundMap\\";
            for (int i = 0; i < 16; i++)
            {
                if (weapon.notetrackSoundMapKeys[i] == 0)
                    continue; 

                if (weapon.notetrackSoundMapKeys[i] < m_context.m_zone.m_script_strings.Count()
                    && weapon.notetrackSoundMapValues[i] < m_context.m_zone.m_script_strings.Count())
                    m_stream << m_context.m_zone.m_script_strings[weapon.notetrackSoundMapKeys[i]] << " "
                             << m_context.m_zone.m_script_strings[weapon.notetrackSoundMapValues[i]];

                if (i != 15 && i < endPos)
                {
                    m_stream << "\n";
                }
            }

            return;
        }

    private:
    };
} // namespace

namespace weapon
{
    std::unique_ptr<IWriterIW3> CreateWeaponWriterIW3(std::ostream& stream, AssetDumpingContext& context)
    {
        return std::make_unique<WeaponWriter>(stream, context);
    }
} // namespace menu
