#include "WeaponWriterIW3.h"

#include "Weapon/AbstractWeaponWriter.h"
#include "ObjWriting.h"

#include <cassert>
#include <cmath>
#include <sstream>

using namespace IW3;

namespace
{
    class WeaponWriter final : public ::weapon::AbstractBaseWriter, public weapon::IWriterIW3
    {
    public:
        explicit WeaponWriter(std::ostream& stream)
            : AbstractBaseWriter(stream)
        {
        }

        void WriteWeaponType(weapType_t weapType)
        {
            m_stream << "weaponType:" << "\\";
            switch (type)
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
            m_stream << "weaponClass:" << "\\";
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

        void WriteImpactType(ImpactType impactType)
        {
            m_stream << "weaponClass:" << "\\";
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
            m_stream << "guidedMissileType:" << "\\";
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
            m_stream << "inventoryType:" << "\\";
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

        void WriteWeapon(const WeaponDef& weapon) override
        {
            m_stream << "WEAPONFILE\\";

            WriteWeaponType(weapon.weapType);
            WriteWeaponClass(weapon.weapClass);
            WriteImpactType(weapon.impactType);
            WriteGuidedMissileType(weapon.guidedMissileType);
            WriteInventoryType(weapon.inventoryType);
            
            m_stream << "maxSteeringAccel:" << "\\" << weapon.maxSteeringAccel << "\\";
            m_stream << "displayName:" << "\\" << weapon.szDisplayName << "\\";
            m_stream << "AIOverlayDescription:" << "\\" << weapon.AIOverlayDescription << "\\";
            m_stream << "modeName:" << "\\" << weapon.szModeName << "\\";
            m_stream << "playerAnimType:" << "\\" << weapon.playerAnimType << "\\";
            m_stream << "gunModel:" << "\\" << weapon.gunXModel[0] << "\\";
            m_stream << "gunModel2:" << "\\" << weapon.gunXModel[1] << "\\";
            m_stream << "gunModel3:" << "\\" << weapon.gunXModel[2] << "\\";
            m_stream << "gunModel4:" << "\\" << weapon.gunXModel[3] << "\\";
            m_stream << "gunModel5:" << "\\" << weapon.gunXModel[4] << "\\";
            m_stream << "gunModel6:" << "\\" << weapon.gunXModel[5] << "\\";
            m_stream << "gunModel7:" << "\\" << weapon.gunXModel[6] << "\\";
            m_stream << "gunModel8:" << "\\" << weapon.gunXModel[7] << "\\";
            m_stream << "gunModel9:" << "\\" << weapon.gunXModel[8] << "\\";
            m_stream << "gunModel10:" << "\\" << weapon.gunXModel[9] << "\\";
            m_stream << "gunModel11:" << "\\" << weapon.gunXModel[10] << "\\";
            m_stream << "gunModel12:" << "\\" << weapon.gunXModel[11] << "\\";
            m_stream << "gunModel13:" << "\\" << weapon.gunXModel[12] << "\\";
            m_stream << "gunModel14:" << "\\" << weapon.gunXModel[13] << "\\";
            m_stream << "gunModel15:" << "\\" << weapon.gunXModel[14] << "\\";
            m_stream << "gunModel16:" << "\\" << weapon.gunXModel[15] << "\\";
            m_stream << "handModel:" << "\\" << weapon.handXModel << "\\";
            m_stream << "isHandModelOverridable:" << "\\" << weapon.isHandModelOverridable << "\\";
            m_stream << "idleAnim:" << "\\" << weapon.idleAnim << "\\";
            m_stream << "emptyIdleAnim:" << "\\" << weapon.emptyIdleAnim << "\\";
            m_stream << "fireAnim:" << "\\" << weapon.fireAnim << "\\";
            m_stream << "lastShotAnim:" << "\\" << weapon.lastShotAnim << "\\";
            m_stream << "meleeAnim:" << "\\" << weapon.meleeAnim << "\\";
            m_stream << "meleeChargeAnim:" << "\\" << weapon.meleeChargeAnim << "\\";
            m_stream << "reloadAnim:" << "\\" << weapon.reloadAnim << "\\";
            m_stream << "raiseAnim:" << "\\" << weapon.raiseAnim << "\\";
            m_stream << "dropAnim:" << "\\" << weapon.dropAnim << "\\";
            m_stream << "firstRaiseAnim:" << "\\" << weapon.firstRaiseAnim << "\\";
            m_stream << "altRaiseAnim:" << "\\" << weapon.altRaiseAnim << "\\";
            m_stream << "altDropAnim:" << "\\" << weapon.altDropAnim << "\\";
            m_stream << "quickRaiseAnim:" << "\\" << weapon.quickRaiseAnim << "\\";
            m_stream << "quickDropAnim:" << "\\" << weapon.quickDropAnim << "\\";
            m_stream << "emptyRaiseAnim:" << "\\" << weapon.emptyRaiseAnim << "\\";
            m_stream << "emptyDropAnim:" << "\\" << weapon.emptyDropAnim << "\\";
            m_stream << "sprintInAnim:" << "\\" << weapon.sprintInAnim << "\\";
            m_stream << "sprintLoopAnim:" << "\\" << weapon.sprintLoopAnim << "\\";
            m_stream << "sprintOutAnim:" << "\\" << weapon.sprintOutAnim << "\\";
            m_stream << "nightVisionWearAnim:" << "\\" << weapon.nightVisionWearAnim << "\\";
            m_stream << "nightVisionRemoveAnim:" << "\\" << weapon.nightVisionRemoveAnim << "\\";
            m_stream << "adsFireAnim:" << "\\" << weapon.adsFireAnim << "\\";
            m_stream << "adsLastShotAnim:" << "\\" << weapon.adsLastShotAnim << "\\";
            m_stream << "adsUpAnim:" << "\\" << weapon.adsUpAnim << "\\";
            m_stream << "adsDownAnim:" << "\\" << weapon.adsDownAnim << "\\";
            m_stream << "moveSpeedScale:" << "\\" << weapon.moveSpeedScale << "\\";
            m_stream << "adsMoveSpeedScale:" << "\\" << weapon.adsMoveSpeedScale << "\\";
            m_stream << "sprintDurationScale:" << "\\" << weapon.sprintDurationScale << "\\";
            m_stream << "lowAmmoWarningThreshold:" << "\\" << weapon.lowAmmoWarningThreshold << "\\";
            m_stream << "damage:" << "\\" << weapon.damage << "\\";
            m_stream << "meleeDamage:" << "\\" << weapon.meleeDamage << "\\";
            m_stream << "fireDelay:" << "\\" << weapon.fireDelay << "\\";
            m_stream << "meleeDelay:" << "\\" << weapon.meleeDelay << "\\";
            m_stream << "meleeChargeDelay:" << "\\" << weapon.meleeChargeDelay << "\\";
            m_stream << "fireTime:" << "\\" << weapon.fireTime << "\\";
            m_stream << "meleeTime:" << "\\" << weapon.meleeTime << "\\";
            m_stream << "meleeChargeTime:" << "\\" << weapon.meleeChargeTime << "\\";
            m_stream << "reloadTime:" << "\\" << weapon.reloadTime << "\\";
            m_stream << "reloadShowRocketTime:" << "\\" << weapon.reloadShowRocketTime << "\\";
            m_stream << "reloadAddTime:" << "\\" << weapon.reloadAddTime << "\\";
            m_stream << "dropTime:" << "\\" << weapon.dropTime << "\\";
            m_stream << "raiseTime:" << "\\" << weapon.raiseTime << "\\";
            m_stream << "altDropTime:" << "\\" << weapon.altDropTime << "\\";
            m_stream << "altRaiseTime:" << "\\" << weapon.altRaiseTime << "\\";
            m_stream << "quickDropTime:" << "\\" << weapon.quickDropTime << "\\";
            m_stream << "quickRaiseTime:" << "\\" << weapon.quickRaiseTime << "\\";
            m_stream << "firstRaiseTime:" << "\\" << weapon.firstRaiseTime << "\\";
            m_stream << "emptyDropTime:" << "\\" << weapon.emptyDropTime << "\\";
            m_stream << "emptyRaiseTime:" << "\\" << weapon.emptyRaiseTime << "\\";
            m_stream << "sprintInTime:" << "\\" << weapon.sprintInTime << "\\";
            m_stream << "sprintLoopTime:" << "\\" << weapon.sprintLoopTime << "\\";
            m_stream << "sprintOutTime:" << "\\" << weapon.sprintOutTime << "\\";
            m_stream << "nightVisionWearTime:" << "\\" << weapon.nightVisionWearTime << "\\";
            m_stream << "nightVisionWearTimeFadeOutEnd:" << "\\" << weapon.nightVisionWearTimeFadeOutEnd << "\\";
            m_stream << "nightVisionWearTimePowerUp:" << "\\" << weapon.nightVisionWearTimePowerUp << "\\";
            m_stream << "nightVisionRemoveTime:" << "\\" << weapon.nightVisionRemoveTime << "\\";
            m_stream << "nightVisionRemoveTimePowerDown:" << "\\" << weapon.nightVisionRemoveTimePowerDown << "\\";
            m_stream << "nightVisionRemoveTimeFadeInStart:" << "\\" << weapon.nightVisionRemoveTimeFadeInStart << "\\";
            m_stream << "clipOnly:" << "\\" << weapon.clipOnly << "\\";
            m_stream << "adsTransInTime:" << "\\" << weapon.adsTransInTime << "\\";
            m_stream << "adsTransOutTime:" << "\\" << weapon.adsTransOutTime << "\\";
            m_stream << "adsZoomFov:" << "\\" << weapon.adsZoomFov << "\\";
            m_stream << "adsZoomGunFov:" << "\\" << weapon.adsZoomGunFov << "\\";
            m_stream << "adsZoomInFrac:" << "\\" << weapon.adsZoomInFrac << "\\";
            m_stream << "adsZoomOutFrac:" << "\\" << weapon.adsZoomOutFrac << "\\";
            m_stream << "adsOverlayShader:" << "\\" << weapon.adsOverlayShader << "\\";
            m_stream << "adsOverlayReticle:" << "\\" << weapon.adsOverlayReticle << "\\";
            m_stream << "adsOverlayInterface:" << "\\" << weapon.adsOverlayInterface << "\\";
            m_stream << "adsOverlayWidth:" << "\\" << weapon.adsOverlayWidth << "\\";
            m_stream << "adsOverlayHeight:" << "\\" << weapon.adsOverlayHeight << "\\";
            m_stream << "adsBobFactor:" << "\\" << weapon.adsBobFactor << "\\";
            m_stream << "adsViewBobMult:" << "\\" << weapon.adsViewBobMult << "\\";
            m_stream << "hipSpreadStandMin:" << "\\" << weapon.hipSpreadStandMin << "\\";
            m_stream << "hipSpreadDuckedMin:" << "\\" << weapon.hipSpreadDuckedMin << "\\";
            m_stream << "hipSpreadProneMin:" << "\\" << weapon.hipSpreadProneMin << "\\";
            m_stream << "hipSpreadMax:" << "\\" << weapon.hipSpreadMax << "\\";
            m_stream << "hipSpreadDuckedMax:" << "\\" << weapon.hipSpreadDuckedMax << "\\";
            m_stream << "hipSpreadProneMax:" << "\\" << weapon.hipSpreadProneMax << "\\";
            m_stream << "hipSpreadDecayRate:" << "\\" << weapon.hipSpreadDecayRate << "\\";
            m_stream << "hipSpreadFireAdd:" << "\\" << weapon.hipSpreadFireAdd << "\\";
            m_stream << "hipSpreadTurnAdd:" << "\\" << weapon.hipSpreadTurnAdd << "\\";
            m_stream << "hipSpreadMoveAdd:" << "\\" << weapon.hipSpreadMoveAdd << "\\";
            m_stream << "hipSpreadDuckedDecay:" << "\\" << weapon.hipSpreadDuckedDecay << "\\";
            m_stream << "hipSpreadProneDecay:" << "\\" << weapon.hipSpreadProneDecay << "\\";
            m_stream << "hipReticleSidePos:" << "\\" << weapon.hipReticleSidePos << "\\";
            m_stream << "adsSpread:" << "\\" << weapon.adsSpread << "\\";
            m_stream << "adsIdleAmount:" << "\\" << weapon.adsIdleAmount << "\\";
            m_stream << "hipIdleAmount:" << "\\" << weapon.hipIdleAmount << "\\";
            m_stream << "hipIdleSpeed:" << "\\" << weapon.hipIdleSpeed << "\\";
            m_stream << "adsIdleSpeed:" << "\\" << weapon.adsIdleSpeed << "\\";
            m_stream << "idleCrouchFactor:" << "\\" << weapon.idleCrouchFactor << "\\";
            m_stream << "idleProneFactor:" << "\\" << weapon.idleProneFactor << "\\";
            m_stream << "gunMaxPitch:" << "\\" << weapon.gunMaxPitch << "\\";
            m_stream << "gunMaxYaw:" << "\\" << weapon.gunMaxYaw << "\\";
            m_stream << "adsAimPitch:" << "\\" << weapon.adsAimPitch << "\\";
            m_stream << "adsCrosshairInFrac:" << "\\" << weapon.adsCrosshairInFrac << "\\";
            m_stream << "adsCrosshairOutFrac:" << "\\" << weapon.adsCrosshairOutFrac << "\\";
            m_stream << "adsReloadTransTime:" << "\\" << weapon.adsReloadTransTime << "\\";
            m_stream << "adsGunKickPitchMin:" << "\\" << weapon.adsGunKickPitchMin << "\\";
            m_stream << "adsGunKickPitchMax:" << "\\" << weapon.adsGunKickPitchMax << "\\";
            m_stream << "adsGunKickYawMin:" << "\\" << weapon.adsGunKickYawMin << "\\";
            m_stream << "adsGunKickYawMax:" << "\\" << weapon.adsGunKickYawMax << "\\";
            m_stream << "adsGunKickAccel:" << "\\" << weapon.adsGunKickAccel << "\\";
            m_stream << "adsGunKickSpeedMax:" << "\\" << weapon.adsGunKickSpeedMax << "\\";
            m_stream << "adsGunKickSpeedDecay:" << "\\" << weapon.adsGunKickSpeedDecay << "\\";
            m_stream << "adsGunKickStaticDecay:" << "\\" << weapon.adsGunKickStaticDecay << "\\";
            m_stream << "adsViewKickPitchMin:" << "\\" << weapon.adsViewKickPitchMin << "\\";
            m_stream << "adsViewKickPitchMax:" << "\\" << weapon.adsViewKickPitchMax << "\\";
            m_stream << "adsViewKickYawMin:" << "\\" << weapon.adsViewKickYawMin << "\\";
            m_stream << "adsViewKickYawMax:" << "\\" << weapon.adsViewKickYawMax << "\\";
            m_stream << "adsViewKickCenterSpeed:" << "\\" << weapon.adsViewKickCenterSpeed << "\\";
            m_stream << "hipGunKickPitchMin:" << "\\" << weapon.hipGunKickPitchMin << "\\";
            m_stream << "hipGunKickPitchMax:" << "\\" << weapon.hipGunKickPitchMax << "\\";
            m_stream << "hipGunKickYawMin:" << "\\" << weapon.hipGunKickYawMin << "\\";
            m_stream << "hipGunKickYawMax:" << "\\" << weapon.hipGunKickYawMax << "\\";
            m_stream << "hipGunKickAccel:" << "\\" << weapon.hipGunKickAccel << "\\";
            m_stream << "hipGunKickSpeedMax:" << "\\" << weapon.hipGunKickSpeedMax << "\\";
            m_stream << "hipGunKickSpeedDecay:" << "\\" << weapon.hipGunKickSpeedDecay << "\\";
            m_stream << "hipGunKickStaticDecay:" << "\\" << weapon.hipGunKickStaticDecay << "\\";
            m_stream << "hipViewKickPitchMin:" << "\\" << weapon.hipViewKickPitchMin << "\\";
            m_stream << "hipViewKickPitchMax:" << "\\" << weapon.hipViewKickPitchMax << "\\";
            m_stream << "hipViewKickYawMin:" << "\\" << weapon.hipViewKickYawMin << "\\";
            m_stream << "hipViewKickYawMax:" << "\\" << weapon.hipViewKickYawMax << "\\";
            m_stream << "hipViewKickCenterSpeed:" << "\\" << weapon.hipViewKickCenterSpeed << "\\";
            m_stream << "swayMaxAngle:" << "\\" << weapon.swayMaxAngle << "\\";
            m_stream << "swayLerpSpeed:" << "\\" << weapon.swayLerpSpeed << "\\";
            m_stream << "swayPitchScale:" << "\\" << weapon.swayPitchScale << "\\";
            m_stream << "swayYawScale:" << "\\" << weapon.swayYawScale << "\\";
            m_stream << "swayHorizScale:" << "\\" << weapon.swayHorizScale << "\\";
            m_stream << "swayVertScale:" << "\\" << weapon.swayVertScale << "\\";
            m_stream << "swayShellShockScale:" << "\\" << weapon.swayShellShockScale << "\\";
            m_stream << "adsSwayMaxAngle:" << "\\" << weapon.adsSwayMaxAngle << "\\";
            m_stream << "adsSwayLerpSpeed:" << "\\" << weapon.adsSwayLerpSpeed << "\\";
            m_stream << "adsSwayPitchScale:" << "\\" << weapon.adsSwayPitchScale << "\\";
            m_stream << "adsSwayYawScale:" << "\\" << weapon.adsSwayYawScale << "\\";
            m_stream << "adsSwayHorizScale:" << "\\" << weapon.adsSwayHorizScale << "\\";
            m_stream << "adsSwayVertScale:" << "\\" << weapon.adsSwayVertScale << "\\";
            m_stream << "hudIcon:" << "\\" << weapon.hudIcon << "\\";
            m_stream << "killIcon:" << "\\" << weapon.killIcon << "\\";
            m_stream << "dpadIcon:" << "\\" << weapon.dpadIcon << "\\";
            m_stream << "ammoCounterIcon:" << "\\" << weapon.ammoCounterIcon << "\\";
            m_stream << "hudIconRatio:" << "\\" << weapon.hudIconRatio << "\\";
            m_stream << "killIconRatio:" << "\\" << weapon.killIconRatio << "\\";
            m_stream << "dpadIconRatio:" << "\\" << weapon.dpadIconRatio << "\\";
            m_stream << "ammoCounterIconRatio:" << "\\" << weapon.ammoCounterIconRatio << "\\";
            m_stream << "ammoCounterClip:" << "\\" << weapon.ammoCounterClip << "\\";
            m_stream << "flipKillIcon:" << "\\" << weapon.flipKillIcon << "\\";
            m_stream << "worldModel:" << "\\" << weapon.worldModel[0] << "\\";
            m_stream << "worldModel2:" << "\\" << weapon.worldModel[1] << "\\";
            m_stream << "worldModel3:" << "\\" << weapon.worldModel[2] << "\\";
            m_stream << "worldModel4:" << "\\" << weapon.worldModel[3] << "\\";
            m_stream << "worldModel5:" << "\\" << weapon.worldModel[4] << "\\";
            m_stream << "worldModel6:" << "\\" << weapon.worldModel[5] << "\\";
            m_stream << "worldModel7:" << "\\" << weapon.worldModel[6] << "\\";
            m_stream << "worldModel8:" << "\\" << weapon.worldModel[7] << "\\";
            m_stream << "worldModel9:" << "\\" << weapon.worldModel[8] << "\\";
            m_stream << "worldModel10:" << "\\" << weapon.worldModel[9] << "\\";
            m_stream << "worldModel11:" << "\\" << weapon.worldModel[10] << "\\";
            m_stream << "worldModel12:" << "\\" << weapon.worldModel[11] << "\\";
            m_stream << "worldModel13:" << "\\" << weapon.worldModel[12] << "\\";
            m_stream << "worldModel14:" << "\\" << weapon.worldModel[13] << "\\";
            m_stream << "worldModel15:" << "\\" << weapon.worldModel[14] << "\\";
            m_stream << "worldModel16:" << "\\" << weapon.worldModel[15] << "\\";
            m_stream << "worldClipModel:" << "\\" << weapon.worldClipModel << "\\";
            m_stream << "rocketModel:" << "\\" << weapon.rocketModel << "\\";
            m_stream << "knifeModel:" << "\\" << weapon.knifeModel << "\\";
            m_stream << "worldKnifeModel:" << "\\" << weapon.worldKnifeModel << "\\";
            m_stream << "ammoName:" << "\\" << weapon.szAmmoName << "\\";
            m_stream << "maxAmmo:" << "\\" << weapon.iMaxAmmo << "\\";
            m_stream << "startAmmo:" << "\\" << weapon.iStartAmmo << "\\";
            m_stream << "clipName:" << "\\" << weapon.szClipName << "\\";
            m_stream << "clipSize:" << "\\" << weapon.iClipSize << "\\";
            m_stream << "dropAmmoMin:" << "\\" << weapon.iDropAmmoMin << "\\";
            m_stream << "dropAmmoMax:" << "\\" << weapon.iDropAmmoMax << "\\";
            m_stream << "viewFlashEffect:" << "\\" << weapon.viewFlashEffect << "\\";
            m_stream << "worldFlashEffect:" << "\\" << weapon.worldFlashEffect << "\\";
            m_stream << "shellEjectEffect:" << "\\" << weapon.shellEjectEffect << "\\";
            m_stream << "lastShotEjectEffect:" << "\\" << weapon.lastShotEjectEffect << "\\";
            m_stream << "worldClipDropEffect:" << "\\" << weapon.worldClipDropEffect << "\\";
            m_stream << "pickupSound:" << "\\" << weapon.pickupSound.name << "\\";
            m_stream << "pickupSoundPlayer:" << "\\" << weapon.pickupSoundPlayer.name << "\\";
            m_stream << "ammoPickupSound:" << "\\" << weapon.ammoPickupSound.name << "\\";
            m_stream << "ammoPickupSoundPlayer:" << "\\" << weapon.ammoPickupSoundPlayer.name << "\\";
            m_stream << "projectileSound:" << "\\" << weapon.projectileSound.name << "\\";
            m_stream << "fireSound:" << "\\" << weapon.fireSound.name << "\\";
            m_stream << "fireSoundPlayer:" << "\\" << weapon.fireSoundPlayer.name << "\\";
            m_stream << "lastShotSound:" << "\\" << weapon.lastShotSound << "\\";
            m_stream << "lastShotSoundPlayer:" << "\\" << weapon.lastShotSoundPlayer << "\\";
            m_stream << "emptyFireSound:" << "\\" << weapon.emptyFireSound.name << "\\";
            m_stream << "emptyFireSoundPlayer:" << "\\" << weapon.emptyFireSoundPlayer.name << "\\";
            m_stream << "meleeSwipeSound:" << "\\" << weapon.meleeSwipeSound.name << "\\";
            m_stream << "meleeSwipeSoundPlayer:" << "\\" << weapon.meleeSwipeSoundPlayer.name << "\\";
            m_stream << "rechamberSound:" << "\\" << weapon.rechamberSound.name << "\\";
            m_stream << "rechamberSoundPlayer:" << "\\" << weapon.rechamberSoundPlayer.name << "\\";
            m_stream << "reloadSound:" << "\\" << weapon.reloadSound.name << "\\";
            m_stream << "reloadSoundPlayer:" << "\\" << weapon.reloadSoundPlayer.name << "\\";
            m_stream << "reloadEmptySound:" << "\\" << weapon.reloadEmptySound.name << "\\";
            m_stream << "reloadEmptySoundPlayer:" << "\\" << weapon.reloadEmptySoundPlayer.name << "\\";
            m_stream << "reloadStartSound:" << "\\" << weapon.reloadStartSound.name << "\\";
            m_stream << "reloadStartSoundPlayer:" << "\\" << weapon.reloadStartSoundPlayer.name << "\\";
            m_stream << "reloadEndSound:" << "\\" << weapon.reloadEndSound.name << "\\";
            m_stream << "reloadEndSoundPlayer:" << "\\" << weapon.reloadEndSoundPlayer.name << "\\";
            m_stream << "altSwitchSound:" << "\\" << weapon.altSwitchSound.name << "\\";
            m_stream << "altSwitchSoundPlayer:" << "\\" << weapon.altSwitchSoundPlayer.name << "\\";
            m_stream << "raiseSound:" << "\\" << weapon.raiseSound.name << "\\";
            m_stream << "raiseSoundPlayer:" << "\\" << weapon.raiseSoundPlayer.name << "\\";
            m_stream << "firstRaiseSound:" << "\\" << weapon.firstRaiseSound.name << "\\";
            m_stream << "firstRaiseSoundPlayer:" << "\\" << weapon.firstRaiseSoundPlayer.name << "\\";
            m_stream << "putawaySound:" << "\\" << weapon.putawaySound.name << "\\";
            m_stream << "putawaySoundPlayer:" << "\\" << weapon.putawaySoundPlayer.name << "\\";
            m_stream << "bounceSound:" << "\\" << weapon.bounceSound << "\\";
            m_stream << "nightVisionWearSound:" << "\\" << weapon.nightVisionWearSound.name << "\\";
            m_stream << "nightVisionWearSoundPlayer:" << "\\" << weapon.nightVisionWearSoundPlayer.name << "\\";
            m_stream << "nightVisionRemoveSound:" << "\\" << weapon.nightVisionRemoveSound.name << "\\";
            m_stream << "nightVisionRemoveSoundPlayer:" << "\\" << weapon.nightVisionRemoveSoundPlayer.name << "\\";
            m_stream << "reticleCenter:" << "\\" << weapon.reticleCenter << "\\";
            m_stream << "reticleSide:" << "\\" << weapon.reticleSide << "\\";
            m_stream << "reticleCenterSize:" << "\\" << weapon.iReticleCenterSize << "\\";
            m_stream << "reticleSideSize:" << "\\" << weapon.iReticleSideSize << "\\";
            m_stream << "reticleMinOfs:" << "\\" << weapon.iReticleMinOfs << "\\";
            m_stream << "enemyCrosshairRange:" << "\\" << weapon.enemyCrosshairRange << "\\";
            m_stream << "crosshairColorChange:" << "\\" << weapon.crosshairColorChange << "\\";
            m_stream << "altWeapon:" << "\\" << weapon.altWeapon << "\\";
            m_stream << "boltAction:" << "\\" << weapon.boltAction << "\\";
            m_stream << "aimDownSight:" << "\\" << weapon.aimDownSight << "\\";
            m_stream << "rechamberWhileAds:" << "\\" << weapon.rechamberWhileAds << "\\";
            m_stream << "adsViewErrorMin:" << "\\" << weapon.adsViewErrorMin << "\\";
            m_stream << "adsViewErrorMax:" << "\\" << weapon.adsViewErrorMax << "\\";
            m_stream << "adsFire:" << "\\" << weapon.adsFire << "\\";
            m_stream << "noAdsWhenMagEmpty:" << "\\" << weapon.noAdsWhenMagEmpty << "\\";
            m_stream << "avoidDropCleanup:" << "\\" << weapon.avoidDropCleanup << "\\";
            m_stream << "cancelAutoHolsterWhenEmpty:" << "\\" << weapon.cancelAutoHolsterWhenEmpty << "\\";
            m_stream << "suppressAmmoReserveDisplay:" << "\\" << weapon.suppressAmmoReserveDisplay << "\\";
            m_stream << "blocksProne:" << "\\" << weapon.blocksProne << "\\";
            m_stream << "noPartialReload:" << "\\" << weapon.noPartialReload << "\\";
            m_stream << "reloadAmmoAdd:" << "\\" << weapon.reloadAmmoAdd << "\\";
            m_stream << "autoAimRange:" << "\\" << weapon.autoAimRange << "\\";
            m_stream << "aimAssistRange:" << "\\" << weapon.aimAssistRange << "\\";
            m_stream << "aimAssistRangeAds:" << "\\" << weapon.aimAssistRangeAds << "\\";
            m_stream << "fightDist:" << "\\" << weapon.fightDist << "\\";
            m_stream << "maxDist:" << "\\" << weapon.maxDist << "\\";
            m_stream << "aiVsAiAccuracyGraph:" << "\\" << weapon.aiVsAiAccuracyGraph << "\\";
            m_stream << "aiVsPlayerAccuracyGraph:" << "\\" << weapon.aiVsPlayerAccuracyGraph << "\\";
            m_stream << "standMoveF:" << "\\" << weapon.standMoveF << "\\";
            m_stream << "standMoveR:" << "\\" << weapon.standMoveR << "\\";
            m_stream << "standMoveU:" << "\\" << weapon.standMoveU << "\\";
            m_stream << "standRotP:" << "\\" << weapon.standRotP << "\\";
            m_stream << "standRotY:" << "\\" << weapon.standRotY << "\\";
            m_stream << "standRotR:" << "\\" << weapon.standRotR << "\\";
            m_stream << "duckedOfsF:" << "\\" << weapon.duckedOfsF << "\\";
            m_stream << "duckedOfsR:" << "\\" << weapon.duckedOfsR << "\\";
            m_stream << "duckedOfsU:" << "\\" << weapon.duckedOfsU << "\\";
            m_stream << "duckedMoveF:" << "\\" << weapon.duckedMoveF << "\\";
            m_stream << "duckedMoveR:" << "\\" << weapon.duckedMoveR << "\\";
            m_stream << "duckedMoveU:" << "\\" << weapon.duckedMoveU << "\\";
            m_stream << "duckedRotP:" << "\\" << weapon.duckedRotP << "\\";
            m_stream << "duckedRotY:" << "\\" << weapon.duckedRotY << "\\";
            m_stream << "duckedRotR:" << "\\" << weapon.duckedRotR << "\\";
            m_stream << "proneOfsF:" << "\\" << weapon.proneOfsF << "\\";
            m_stream << "proneOfsR:" << "\\" << weapon.proneOfsR << "\\";
            m_stream << "proneOfsU:" << "\\" << weapon.proneOfsU << "\\";
            m_stream << "proneMoveF:" << "\\" << weapon.proneMoveF << "\\";
            m_stream << "proneMoveR:" << "\\" << weapon.proneMoveR << "\\";
            m_stream << "proneMoveU:" << "\\" << weapon.proneMoveU << "\\";
            m_stream << "proneRotP:" << "\\" << weapon.proneRotP << "\\";
            m_stream << "proneRotY:" << "\\" << weapon.proneRotY << "\\";
            m_stream << "proneRotR:" << "\\" << weapon.proneRotR << "\\";
            m_stream << "posMoveRate:" << "\\" << weapon.posMoveRate << "\\";
            m_stream << "posProneMoveRate:" << "\\" << weapon.posProneMoveRate << "\\";
            m_stream << "standMoveMinSpeed:" << "\\" << weapon.standMoveMinSpeed << "\\";
            m_stream << "duckedMoveMinSpeed:" << "\\" << weapon.duckedMoveMinSpeed << "\\";
            m_stream << "proneMoveMinSpeed:" << "\\" << weapon.proneMoveMinSpeed << "\\";
            m_stream << "posRotRate:" << "\\" << weapon.posRotRate << "\\";
            m_stream << "posProneRotRate:" << "\\" << weapon.posProneRotRate << "\\";
            m_stream << "standRotMinSpeed:" << "\\" << weapon.standRotMinSpeed << "\\";
            m_stream << "duckedRotMinSpeed:" << "\\" << weapon.duckedRotMinSpeed << "\\";
            m_stream << "proneRotMinSpeed:" << "\\" << weapon.proneRotMinSpeed << "\\";
            m_stream << "explosionRadius:" << "\\" << weapon.explosionRadius << "\\";
            m_stream << "explosionInnerDamage:" << "\\" << weapon.explosionInnerDamage << "\\";
            m_stream << "explosionOuterDamage:" << "\\" << weapon.explosionOuterDamage << "\\";
            m_stream << "damageConeAngle:" << "\\" << weapon.damageConeAngle << "\\";
            m_stream << "projectileSpeed:" << "\\" << weapon.projectileSpeed << "\\";
            m_stream << "projectileSpeedUp:" << "\\" << weapon.projectileSpeedUp << "\\";
            m_stream << "projectileActivateDist:" << "\\" << weapon.projectileActivateDist << "\\";
            m_stream << "projectileLifetime:" << "\\" << weapon.projectileLifetime << "\\";
            m_stream << "projectileModel:" << "\\" << weapon.projectileModel << "\\";
            m_stream << "projExplosionType:" << "\\" << weapon.projExplosionType << "\\";
            m_stream << "projExplosionEffect:" << "\\" << weapon.projExplosionEffect << "\\";
            m_stream << "projExplosionEffectForceNormalUp:" << "\\" << weapon.projExplosionEffectForceNormalUp << "\\";
            m_stream << "projExplosionSound:" << "\\" << weapon.projExplosionSound << "\\";
            m_stream << "projDudEffect:" << "\\" << weapon.projDudEffect << "\\";
            m_stream << "projDudSound:" << "\\" << weapon.projDudSound << "\\";
            m_stream << "projImpactExplode:" << "\\" << weapon.projImpactExplode << "\\";
            m_stream << "projTrailEffect:" << "\\" << weapon.projTrailEffect << "\\";
            m_stream << "projectileDLight:" << "\\" << weapon.projectileDLight << "\\";
            m_stream << "projectileRed:" << "\\" << weapon.projectileRed << "\\";
            m_stream << "projectileGreen:" << "\\" << weapon.projectileGreen << "\\";
            m_stream << "projectileBlue:" << "\\" << weapon.projectileBlue << "\\";
            m_stream << "projIgnitionDelay:" << "\\" << weapon.projIgnitionDelay << "\\";
            m_stream << "projIgnitionEffect:" << "\\" << weapon.projIgnitionEffect << "\\";
            m_stream << "projIgnitionSound:" << "\\" << weapon.projIgnitionSound << "\\";
            m_stream << "destabilizationRateTime:" << "\\" << weapon.destabilizationRateTime << "\\";
            m_stream << "destabilizationCurvatureMax:" << "\\" << weapon.destabilizationCurvatureMax << "\\";
            m_stream << "destabilizeDistance:" << "\\" << weapon.destabilizeDistance << "\\";
            m_stream << "parallelDefaultBounce:" << "\\" << weapon.parallelDefaultBounce << "\\";
            m_stream << "parallelBarkBounce:" << "\\" << weapon.parallelBarkBounce << "\\";
            m_stream << "parallelBrickBounce:" << "\\" << weapon.parallelBrickBounce << "\\";
            m_stream << "parallelCarpetBounce:" << "\\" << weapon.parallelCarpetBounce << "\\";
            m_stream << "parallelClothBounce:" << "\\" << weapon.parallelClothBounce << "\\";
            m_stream << "parallelConcreteBounce:" << "\\" << weapon.parallelConcreteBounce << "\\";
            m_stream << "parallelDirtBounce:" << "\\" << weapon.parallelDirtBounce << "\\";
            m_stream << "parallelFleshBounce:" << "\\" << weapon.parallelFleshBounce << "\\";
            m_stream << "parallelFoliageBounce:" << "\\" << weapon.parallelFoliageBounce << "\\";
            m_stream << "parallelGlassBounce:" << "\\" << weapon.parallelGlassBounce << "\\";
            m_stream << "parallelGrassBounce:" << "\\" << weapon.parallelGrassBounce << "\\";
            m_stream << "parallelGravelBounce:" << "\\" << weapon.parallelGravelBounce << "\\";
            m_stream << "parallelIceBounce:" << "\\" << weapon.parallelIceBounce << "\\";
            m_stream << "parallelMetalBounce:" << "\\" << weapon.parallelMetalBounce << "\\";
            m_stream << "parallelMudBounce:" << "\\" << weapon.parallelMudBounce << "\\";
            m_stream << "parallelPaperBounce:" << "\\" << weapon.parallelPaperBounce << "\\";
            m_stream << "parallelPlasterBounce:" << "\\" << weapon.parallelPlasterBounce << "\\";
            m_stream << "parallelRockBounce:" << "\\" << weapon.parallelRockBounce << "\\";
            m_stream << "parallelSandBounce:" << "\\" << weapon.parallelSandBounce << "\\";
            m_stream << "parallelSnowBounce:" << "\\" << weapon.parallelSnowBounce << "\\";
            m_stream << "parallelWaterBounce:" << "\\" << weapon.parallelWaterBounce << "\\";
            m_stream << "parallelWoodBounce:" << "\\" << weapon.parallelWoodBounce << "\\";
            m_stream << "parallelAsphaltBounce:" << "\\" << weapon.parallelAsphaltBounce << "\\";
            m_stream << "parallelCeramicBounce:" << "\\" << weapon.parallelCeramicBounce << "\\";
            m_stream << "parallelPlasticBounce:" << "\\" << weapon.parallelPlasticBounce << "\\";
            m_stream << "parallelRubberBounce:" << "\\" << weapon.parallelRubberBounce << "\\";
            m_stream << "parallelCushionBounce:" << "\\" << weapon.parallelCushionBounce << "\\";
            m_stream << "parallelFruitBounce:" << "\\" << weapon.parallelFruitBounce << "\\";
            m_stream << "parallelPaintedMetalBounce:" << "\\" << weapon.parallelPaintedMetalBounce << "\\";
            m_stream << "perpendicularDefaultBounce:" << "\\" << weapon.perpendicularDefaultBounce << "\\";
            m_stream << "perpendicularBarkBounce:" << "\\" << weapon.perpendicularBarkBounce << "\\";
            m_stream << "perpendicularBrickBounce:" << "\\" << weapon.perpendicularBrickBounce << "\\";
            m_stream << "perpendicularCarpetBounce:" << "\\" << weapon.perpendicularCarpetBounce << "\\";
            m_stream << "perpendicularClothBounce:" << "\\" << weapon.perpendicularClothBounce << "\\";
            m_stream << "perpendicularConcreteBounce:" << "\\" << weapon.perpendicularConcreteBounce << "\\";
            m_stream << "perpendicularDirtBounce:" << "\\" << weapon.perpendicularDirtBounce << "\\";
            m_stream << "perpendicularFleshBounce:" << "\\" << weapon.perpendicularFleshBounce << "\\";
            m_stream << "perpendicularFoliageBounce:" << "\\" << weapon.perpendicularFoliageBounce << "\\";
            m_stream << "perpendicularGlassBounce:" << "\\" << weapon.perpendicularGlassBounce << "\\";
            m_stream << "perpendicularGrassBounce:" << "\\" << weapon.perpendicularGrassBounce << "\\";
            m_stream << "perpendicularGravelBounce:" << "\\" << weapon.perpendicularGravelBounce << "\\";
            m_stream << "perpendicularIceBounce:" << "\\" << weapon.perpendicularIceBounce << "\\";
            m_stream << "perpendicularMetalBounce:" << "\\" << weapon.perpendicularMetalBounce << "\\";
            m_stream << "perpendicularMudBounce:" << "\\" << weapon.perpendicularMudBounce << "\\";
            m_stream << "perpendicularPaperBounce:" << "\\" << weapon.perpendicularPaperBounce << "\\";
            m_stream << "perpendicularPlasterBounce:" << "\\" << weapon.perpendicularPlasterBounce << "\\";
            m_stream << "perpendicularRockBounce:" << "\\" << weapon.perpendicularRockBounce << "\\";
            m_stream << "perpendicularSandBounce:" << "\\" << weapon.perpendicularSandBounce << "\\";
            m_stream << "perpendicularSnowBounce:" << "\\" << weapon.perpendicularSnowBounce << "\\";
            m_stream << "perpendicularWaterBounce:" << "\\" << weapon.perpendicularWaterBounce << "\\";
            m_stream << "perpendicularWoodBounce:" << "\\" << weapon.perpendicularWoodBounce << "\\";
            m_stream << "perpendicularAsphaltBounce:" << "\\" << weapon.perpendicularAsphaltBounce << "\\";
            m_stream << "perpendicularCeramicBounce:" << "\\" << weapon.perpendicularCeramicBounce << "\\";
            m_stream << "perpendicularPlasticBounce:" << "\\" << weapon.perpendicularPlasticBounce << "\\";
            m_stream << "perpendicularRubberBounce:" << "\\" << weapon.perpendicularRubberBounce << "\\";
            m_stream << "perpendicularCushionBounce:" << "\\" << weapon.perpendicularCushionBounce << "\\";
            m_stream << "perpendicularFruitBounce:" << "\\" << weapon.perpendicularFruitBounce << "\\";
            m_stream << "perpendicularPaintedMetalBounce:" << "\\" << weapon.perpendicularPaintedMetalBounce << "\\";
            m_stream << "fireRumble:" << "\\" << weapon.fireRumble << "\\";
            m_stream << "meleeImpactRumble:" << "\\" << weapon.meleeImpactRumble << "\\";
            m_stream << "adsDofStart:" << "\\" << weapon.adsDofStart << "\\";
            m_stream << "adsDofEnd:" << "\\" << weapon.adsDofEnd << "\\";
            m_stream << "requireLockonToFire:" << "\\" << weapon.requireLockonToFire << "\\";
            m_stream << "notetrackSoundMap:" << "\\" << weapon.notetrackSoundMap << "\\";

            return;
        }

    private:
    };
} // namespace

namespace weapon
{
    std::unique_ptr<IWriterIW3> CreateWeaponWriterIW3(std::ostream& stream)
    {
        return std::make_unique<WeaponWriter>(stream);
    }
} // namespace menu
