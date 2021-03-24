#include "AssetDumperWeapon.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include <type_traits>

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"

using namespace IW4;

cspField_t AssetDumperWeapon::weapon_fields[]
{
    {"displayName", offsetof(WeaponFullDef, weapCompleteDef.szDisplayName), CSPFT_STRING},
    {"AIOverlayDescription", offsetof(WeaponFullDef, weapDef.szOverlayName), CSPFT_STRING},
    {"modeName", offsetof(WeaponFullDef, weapDef.szModeName), CSPFT_STRING},
    {"playerAnimType", offsetof(WeaponFullDef, weapDef.playerAnimType), WFT_ANIMTYPE},
    {"gunModel", offsetof(WeaponFullDef, gunXModel[0]), CSPFT_XMODEL},
    {"gunModel2", offsetof(WeaponFullDef, gunXModel[1]), CSPFT_XMODEL},
    {"gunModel3", offsetof(WeaponFullDef, gunXModel[2]), CSPFT_XMODEL},
    {"gunModel4", offsetof(WeaponFullDef, gunXModel[3]), CSPFT_XMODEL},
    {"gunModel5", offsetof(WeaponFullDef, gunXModel[4]), CSPFT_XMODEL},
    {"gunModel6", offsetof(WeaponFullDef, gunXModel[5]), CSPFT_XMODEL},
    {"gunModel7", offsetof(WeaponFullDef, gunXModel[6]), CSPFT_XMODEL},
    {"gunModel8", offsetof(WeaponFullDef, gunXModel[7]), CSPFT_XMODEL},
    {"gunModel9", offsetof(WeaponFullDef, gunXModel[8]), CSPFT_XMODEL},
    {"gunModel10", offsetof(WeaponFullDef, gunXModel[9]), CSPFT_XMODEL},
    {"gunModel11", offsetof(WeaponFullDef, gunXModel[10]), CSPFT_XMODEL},
    {"gunModel12", offsetof(WeaponFullDef, gunXModel[11]), CSPFT_XMODEL},
    {"gunModel13", offsetof(WeaponFullDef, gunXModel[12]), CSPFT_XMODEL},
    {"gunModel14", offsetof(WeaponFullDef, gunXModel[13]), CSPFT_XMODEL},
    {"gunModel15", offsetof(WeaponFullDef, gunXModel[14]), CSPFT_XMODEL},
    {"gunModel16", offsetof(WeaponFullDef, gunXModel[15]), CSPFT_XMODEL},
    {"handModel", offsetof(WeaponFullDef, weapDef.handXModel), CSPFT_XMODEL},
    {"hideTags", offsetof(WeaponFullDef, hideTags), WFT_HIDETAGS},
    {"notetrackSoundMap", offsetof(WeaponFullDef, notetrackSoundMapKeys), WFT_NOTETRACKSOUNDMAP},
    {"notetrackRumbleMap", offsetof(WeaponFullDef, notetrackRumbleMapKeys), WFT_NOTETRACKRUMBLEMAP},
    {"idleAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_IDLE]), CSPFT_STRING},
    {"emptyIdleAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_IDLE]), CSPFT_STRING},
    {"fireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRE]), CSPFT_STRING},
    {"holdFireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_HOLD_FIRE]), CSPFT_STRING},
    {"lastShotAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LASTSHOT]), CSPFT_STRING},
    {"detonateAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DETONATE]), CSPFT_STRING},
    {"rechamberAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RECHAMBER]), CSPFT_STRING},
    {"meleeAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE]), CSPFT_STRING},
    {"meleeChargeAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE_CHARGE]), CSPFT_STRING},
    {"reloadAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD]), CSPFT_STRING},
    {"reloadEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_EMPTY]), CSPFT_STRING},
    {"reloadStartAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_START]), CSPFT_STRING},
    {"reloadEndAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_END]), CSPFT_STRING},
    {"raiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RAISE]), CSPFT_STRING},
    {"dropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DROP]), CSPFT_STRING},
    {"firstRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRST_RAISE]), CSPFT_STRING},
    {"breachRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_BREACH_RAISE]), CSPFT_STRING},
    {"altRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ALT_RAISE]), CSPFT_STRING},
    {"altDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ALT_DROP]), CSPFT_STRING},
    {"quickRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_QUICK_RAISE]), CSPFT_STRING},
    {"quickDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_QUICK_DROP]), CSPFT_STRING},
    {"emptyRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_RAISE]), CSPFT_STRING},
    {"emptyDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_DROP]), CSPFT_STRING},
    {"sprintInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_IN]), CSPFT_STRING},
    {"sprintLoopAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_LOOP]), CSPFT_STRING},
    {"sprintOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_OUT]), CSPFT_STRING},
    {"stunnedAnimStart", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_STUNNED_START]), CSPFT_STRING},
    {"stunnedAnimLoop", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_STUNNED_LOOP]), CSPFT_STRING},
    {"stunnedAnimEnd", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_STUNNED_END]), CSPFT_STRING},
    {"nightVisionWearAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_NIGHTVISION_WEAR]), CSPFT_STRING},
    {"nightVisionRemoveAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_NIGHTVISION_REMOVE]), CSPFT_STRING},
    {"adsFireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_FIRE]), CSPFT_STRING},
    {"adsLastShotAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_LASTSHOT]), CSPFT_STRING},
    {"adsRechamberAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_RECHAMBER]), CSPFT_STRING},
    {"adsUpAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_UP]), CSPFT_STRING},
    {"adsDownAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_DOWN]), CSPFT_STRING},
    {"idleAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_IDLE]), CSPFT_STRING},
    {"emptyIdleAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_EMPTY_IDLE]), CSPFT_STRING},
    {"fireAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_FIRE]), CSPFT_STRING},
    {"holdFireAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_HOLD_FIRE]), CSPFT_STRING},
    {"lastShotAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_LASTSHOT]), CSPFT_STRING},
    {"detonateAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_DETONATE]), CSPFT_STRING},
    {"rechamberAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RECHAMBER]), CSPFT_STRING},
    {"meleeAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_MELEE]), CSPFT_STRING},
    {"meleeChargeAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_MELEE_CHARGE]), CSPFT_STRING},
    {"reloadAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RELOAD]), CSPFT_STRING},
    {"reloadEmptyAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RELOAD_EMPTY]), CSPFT_STRING},
    {"reloadStartAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RELOAD_START]), CSPFT_STRING},
    {"reloadEndAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RELOAD_END]), CSPFT_STRING},
    {"raiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_RAISE]), CSPFT_STRING},
    {"dropAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_DROP]), CSPFT_STRING},
    {"firstRaiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_FIRST_RAISE]), CSPFT_STRING},
    {"breachRaiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_BREACH_RAISE]), CSPFT_STRING},
    {"altRaiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ALT_RAISE]), CSPFT_STRING},
    {"altDropAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ALT_DROP]), CSPFT_STRING},
    {"quickRaiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_QUICK_RAISE]), CSPFT_STRING},
    {"quickDropAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_QUICK_DROP]), CSPFT_STRING},
    {"emptyRaiseAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_EMPTY_RAISE]), CSPFT_STRING},
    {"emptyDropAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_EMPTY_DROP]), CSPFT_STRING},
    {"sprintInAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_SPRINT_IN]), CSPFT_STRING},
    {"sprintLoopAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_SPRINT_LOOP]), CSPFT_STRING},
    {"sprintOutAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_SPRINT_OUT]), CSPFT_STRING},
    {"stunnedAnimStartR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_STUNNED_START]), CSPFT_STRING},
    {"stunnedAnimLoopR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_STUNNED_LOOP]), CSPFT_STRING},
    {"stunnedAnimEndR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_STUNNED_END]), CSPFT_STRING},
    {"nightVisionWearAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_NIGHTVISION_WEAR]), CSPFT_STRING},
    {"nightVisionRemoveAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_NIGHTVISION_REMOVE]), CSPFT_STRING},
    {"adsFireAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ADS_FIRE]), CSPFT_STRING},
    {"adsLastShotAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ADS_LASTSHOT]), CSPFT_STRING},
    {"adsRechamberAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ADS_RECHAMBER]), CSPFT_STRING},
    {"adsUpAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ADS_UP]), CSPFT_STRING},
    {"adsDownAnimR", offsetof(WeaponFullDef, szXAnimsRightHanded[WEAP_ANIM_ADS_DOWN]), CSPFT_STRING},
    {"idleAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_IDLE]), CSPFT_STRING},
    {"emptyIdleAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_EMPTY_IDLE]), CSPFT_STRING},
    {"fireAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_FIRE]), CSPFT_STRING},
    {"holdFireAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_HOLD_FIRE]), CSPFT_STRING},
    {"lastShotAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_LASTSHOT]), CSPFT_STRING},
    {"detonateAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_DETONATE]), CSPFT_STRING},
    {"rechamberAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RECHAMBER]), CSPFT_STRING},
    {"meleeAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_MELEE]), CSPFT_STRING},
    {"meleeChargeAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_MELEE_CHARGE]), CSPFT_STRING},
    {"reloadAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RELOAD]), CSPFT_STRING},
    {"reloadEmptyAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RELOAD_EMPTY]), CSPFT_STRING},
    {"reloadStartAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RELOAD_START]), CSPFT_STRING},
    {"reloadEndAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RELOAD_END]), CSPFT_STRING},
    {"raiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_RAISE]), CSPFT_STRING},
    {"dropAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_DROP]), CSPFT_STRING},
    {"firstRaiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_FIRST_RAISE]), CSPFT_STRING},
    {"breachRaiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_BREACH_RAISE]), CSPFT_STRING},
    {"altRaiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ALT_RAISE]), CSPFT_STRING},
    {"altDropAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ALT_DROP]), CSPFT_STRING},
    {"quickRaiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_QUICK_RAISE]), CSPFT_STRING},
    {"quickDropAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_QUICK_DROP]), CSPFT_STRING},
    {"emptyRaiseAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_EMPTY_RAISE]), CSPFT_STRING},
    {"emptyDropAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_EMPTY_DROP]), CSPFT_STRING},
    {"sprintInAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_SPRINT_IN]), CSPFT_STRING},
    {"sprintLoopAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_SPRINT_LOOP]), CSPFT_STRING},
    {"sprintOutAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_SPRINT_OUT]), CSPFT_STRING},
    {"stunnedAnimStartL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_STUNNED_START]), CSPFT_STRING},
    {"stunnedAnimLoopL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_STUNNED_LOOP]), CSPFT_STRING},
    {"stunnedAnimEndL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_STUNNED_END]), CSPFT_STRING},
    {"nightVisionWearAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_NIGHTVISION_WEAR]), CSPFT_STRING},
    {"nightVisionRemoveAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_NIGHTVISION_REMOVE]), CSPFT_STRING},
    {"adsFireAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ADS_FIRE]), CSPFT_STRING},
    {"adsLastShotAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ADS_LASTSHOT]), CSPFT_STRING},
    {"adsRechamberAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ADS_RECHAMBER]), CSPFT_STRING},
    {"adsUpAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ADS_UP]), CSPFT_STRING},
    {"adsDownAnimL", offsetof(WeaponFullDef, szXAnimsLeftHanded[WEAP_ANIM_ADS_DOWN]), CSPFT_STRING},
    {"script", offsetof(WeaponFullDef, weapDef.szScript), CSPFT_STRING},
    {"weaponType", offsetof(WeaponFullDef, weapDef.weapType), WFT_WEAPONTYPE},
    {"weaponClass", offsetof(WeaponFullDef, weapDef.weapClass), WFT_WEAPONCLASS},
    {"penetrateType", offsetof(WeaponFullDef, weapDef.penetrateType), WFT_PENETRATE_TYPE},
    {"penetrateMultiplier", offsetof(WeaponFullDef, weapCompleteDef.penetrateMultiplier), CSPFT_FLOAT},
    {"impactType", offsetof(WeaponFullDef, weapCompleteDef.impactType), WFT_IMPACT_TYPE},
    {"inventoryType", offsetof(WeaponFullDef, weapDef.inventoryType), WFT_INVENTORYTYPE},
    {"fireType", offsetof(WeaponFullDef, weapDef.fireType), WFT_FIRETYPE},
    {"offhandClass", offsetof(WeaponFullDef, weapDef.offhandClass), WFT_OFFHAND_CLASS},
    {"viewFlashEffect", offsetof(WeaponFullDef, weapDef.viewFlashEffect), CSPFT_FX},
    {"worldFlashEffect", offsetof(WeaponFullDef, weapDef.worldFlashEffect), CSPFT_FX},
    {"pickupSound", offsetof(WeaponFullDef, weapDef.pickupSound), CSPFT_SOUND},
    {"pickupSoundPlayer", offsetof(WeaponFullDef, weapDef.pickupSoundPlayer), CSPFT_SOUND},
    {"ammoPickupSound", offsetof(WeaponFullDef, weapDef.ammoPickupSound), CSPFT_SOUND},
    {"ammoPickupSoundPlayer", offsetof(WeaponFullDef, weapDef.ammoPickupSoundPlayer), CSPFT_SOUND},
    {"projectileSound", offsetof(WeaponFullDef, weapDef.projectileSound), CSPFT_SOUND},
    {"pullbackSound", offsetof(WeaponFullDef, weapDef.pullbackSound), CSPFT_SOUND},
    {"pullbackSoundPlayer", offsetof(WeaponFullDef, weapDef.pullbackSoundPlayer), CSPFT_SOUND},
    {"fireSound", offsetof(WeaponFullDef, weapDef.fireSound), CSPFT_SOUND},
    {"fireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireSoundPlayer), CSPFT_SOUND},
    {"fireSoundPlayerAkimbo", offsetof(WeaponFullDef, weapDef.fireSoundPlayerAkimbo), CSPFT_SOUND},
    {"loopFireSound", offsetof(WeaponFullDef, weapDef.fireLoopSound), CSPFT_SOUND},
    {"loopFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireLoopSoundPlayer), CSPFT_SOUND},
    {"stopFireSound", offsetof(WeaponFullDef, weapDef.fireStopSound), CSPFT_SOUND},
    {"stopFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireStopSoundPlayer), CSPFT_SOUND},
    {"lastShotSound", offsetof(WeaponFullDef, weapDef.fireLastSound), CSPFT_SOUND},
    {"lastShotSoundPlayer", offsetof(WeaponFullDef, weapDef.fireLastSoundPlayer), CSPFT_SOUND},
    {"emptyFireSound", offsetof(WeaponFullDef, weapDef.emptyFireSound), CSPFT_SOUND},
    {"emptyFireSoundPlayer", offsetof(WeaponFullDef, weapDef.emptyFireSoundPlayer), CSPFT_SOUND},
    {"meleeSwipeSound", offsetof(WeaponFullDef, weapDef.meleeSwipeSound), CSPFT_SOUND},
    {"meleeSwipeSoundPlayer", offsetof(WeaponFullDef, weapDef.meleeSwipeSoundPlayer), CSPFT_SOUND},
    {"meleeHitSound", offsetof(WeaponFullDef, weapDef.meleeHitSound), CSPFT_SOUND},
    {"meleeMissSound", offsetof(WeaponFullDef, weapDef.meleeMissSound), CSPFT_SOUND},
    {"rechamberSound", offsetof(WeaponFullDef, weapDef.rechamberSound), CSPFT_SOUND},
    {"rechamberSoundPlayer", offsetof(WeaponFullDef, weapDef.rechamberSoundPlayer), CSPFT_SOUND},
    {"reloadSound", offsetof(WeaponFullDef, weapDef.reloadSound), CSPFT_SOUND},
    {"reloadSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadSoundPlayer), CSPFT_SOUND},
    {"reloadEmptySound", offsetof(WeaponFullDef, weapDef.reloadEmptySound), CSPFT_SOUND},
    {"reloadEmptySoundPlayer", offsetof(WeaponFullDef, weapDef.reloadEmptySoundPlayer), CSPFT_SOUND},
    {"reloadStartSound", offsetof(WeaponFullDef, weapDef.reloadStartSound), CSPFT_SOUND},
    {"reloadStartSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadStartSoundPlayer), CSPFT_SOUND},
    {"reloadEndSound", offsetof(WeaponFullDef, weapDef.reloadEndSound), CSPFT_SOUND},
    {"reloadEndSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadEndSoundPlayer), CSPFT_SOUND},
    {"detonateSound", offsetof(WeaponFullDef, weapDef.detonateSound), CSPFT_SOUND},
    {"detonateSoundPlayer", offsetof(WeaponFullDef, weapDef.detonateSoundPlayer), CSPFT_SOUND},
    {"nightVisionWearSound", offsetof(WeaponFullDef, weapDef.nightVisionWearSound), CSPFT_SOUND},
    {"nightVisionWearSoundPlayer", offsetof(WeaponFullDef, weapDef.nightVisionWearSoundPlayer), CSPFT_SOUND},
    {"nightVisionRemoveSound", offsetof(WeaponFullDef, weapDef.nightVisionRemoveSound), CSPFT_SOUND},
    {"nightVisionRemoveSoundPlayer", offsetof(WeaponFullDef, weapDef.nightVisionRemoveSoundPlayer), CSPFT_SOUND},
    {"raiseSound", offsetof(WeaponFullDef, weapDef.raiseSound), CSPFT_SOUND},
    {"raiseSoundPlayer", offsetof(WeaponFullDef, weapDef.raiseSoundPlayer), CSPFT_SOUND},
    {"firstRaiseSound", offsetof(WeaponFullDef, weapDef.firstRaiseSound), CSPFT_SOUND},
    {"firstRaiseSoundPlayer", offsetof(WeaponFullDef, weapDef.firstRaiseSoundPlayer), CSPFT_SOUND},
    {"altSwitchSound", offsetof(WeaponFullDef, weapDef.altSwitchSound), CSPFT_SOUND},
    {"altSwitchSoundPlayer", offsetof(WeaponFullDef, weapDef.altSwitchSoundPlayer), CSPFT_SOUND},
    {"putawaySound", offsetof(WeaponFullDef, weapDef.putawaySound), CSPFT_SOUND},
    {"putawaySoundPlayer", offsetof(WeaponFullDef, weapDef.putawaySoundPlayer), CSPFT_SOUND},
    {"scanSound", offsetof(WeaponFullDef, weapDef.scanSound), CSPFT_SOUND},
    {"bounceSound", offsetof(WeaponFullDef, weapDef.bounceSound), WFT_BOUNCE_SOUND},
    {"viewShellEjectEffect", offsetof(WeaponFullDef, weapDef.viewShellEjectEffect), CSPFT_FX},
    {"worldShellEjectEffect", offsetof(WeaponFullDef, weapDef.worldShellEjectEffect), CSPFT_FX},
    {"viewLastShotEjectEffect", offsetof(WeaponFullDef, weapDef.viewLastShotEjectEffect), CSPFT_FX},
    {"worldLastShotEjectEffect", offsetof(WeaponFullDef, weapDef.worldLastShotEjectEffect), CSPFT_FX},
    {"reticleCenter", offsetof(WeaponFullDef, weapDef.reticleCenter), CSPFT_MATERIAL},
    {"reticleSide", offsetof(WeaponFullDef, weapDef.reticleSide), CSPFT_MATERIAL},
    {"reticleCenterSize", offsetof(WeaponFullDef, weapDef.iReticleCenterSize), CSPFT_INT},
    {"reticleSideSize", offsetof(WeaponFullDef, weapDef.iReticleSideSize), CSPFT_INT},
    {"reticleMinOfs", offsetof(WeaponFullDef, weapDef.iReticleMinOfs), CSPFT_INT},
    {"activeReticleType", offsetof(WeaponFullDef, weapDef.activeReticleType), WFT_ACTIVE_RETICLE_TYPE},
    {"standMoveF", offsetof(WeaponFullDef, weapDef.vStandMove[0]), CSPFT_FLOAT},
    {"standMoveR", offsetof(WeaponFullDef, weapDef.vStandMove[1]), CSPFT_FLOAT},
    {"standMoveU", offsetof(WeaponFullDef, weapDef.vStandMove[2]), CSPFT_FLOAT},
    {"standRotP", offsetof(WeaponFullDef, weapDef.vStandRot[0]), CSPFT_FLOAT},
    {"standRotY", offsetof(WeaponFullDef, weapDef.vStandRot[1]), CSPFT_FLOAT},
    {"standRotR", offsetof(WeaponFullDef, weapDef.vStandRot[2]), CSPFT_FLOAT},
    {"strafeMoveF", offsetof(WeaponFullDef, weapDef.strafeMove[0]), CSPFT_FLOAT},
    {"strafeMoveR", offsetof(WeaponFullDef, weapDef.strafeMove[1]), CSPFT_FLOAT},
    {"strafeMoveU", offsetof(WeaponFullDef, weapDef.strafeMove[2]), CSPFT_FLOAT},
    {"strafeRotP", offsetof(WeaponFullDef, weapDef.strafeRot[0]), CSPFT_FLOAT},
    {"strafeRotY", offsetof(WeaponFullDef, weapDef.strafeRot[1]), CSPFT_FLOAT},
    {"strafeRotR", offsetof(WeaponFullDef, weapDef.strafeRot[2]), CSPFT_FLOAT},
    {"duckedOfsF", offsetof(WeaponFullDef, weapDef.vDuckedOfs[0]), CSPFT_FLOAT},
    {"duckedOfsR", offsetof(WeaponFullDef, weapDef.vDuckedOfs[1]), CSPFT_FLOAT},
    {"duckedOfsU", offsetof(WeaponFullDef, weapDef.vDuckedOfs[2]), CSPFT_FLOAT},
    {"duckedMoveF", offsetof(WeaponFullDef, weapDef.vDuckedMove[0]), CSPFT_FLOAT},
    {"duckedMoveR", offsetof(WeaponFullDef, weapDef.vDuckedMove[1]), CSPFT_FLOAT},
    {"duckedMoveU", offsetof(WeaponFullDef, weapDef.vDuckedMove[2]), CSPFT_FLOAT},
    {"duckedRotP", offsetof(WeaponFullDef, weapDef.vDuckedRot[0]), CSPFT_FLOAT},
    {"duckedRotY", offsetof(WeaponFullDef, weapDef.vDuckedRot[1]), CSPFT_FLOAT},
    {"duckedRotR", offsetof(WeaponFullDef, weapDef.vDuckedRot[2]), CSPFT_FLOAT},
    {"proneOfsF", offsetof(WeaponFullDef, weapDef.vProneOfs[0]), CSPFT_FLOAT},
    {"proneOfsR", offsetof(WeaponFullDef, weapDef.vProneOfs[1]), CSPFT_FLOAT},
    {"proneOfsU", offsetof(WeaponFullDef, weapDef.vProneOfs[2]), CSPFT_FLOAT},
    {"proneMoveF", offsetof(WeaponFullDef, weapDef.vProneMove[0]), CSPFT_FLOAT},
    {"proneMoveR", offsetof(WeaponFullDef, weapDef.vProneMove[1]), CSPFT_FLOAT},
    {"proneMoveU", offsetof(WeaponFullDef, weapDef.vProneMove[2]), CSPFT_FLOAT},
    {"proneRotP", offsetof(WeaponFullDef, weapDef.vProneRot[0]), CSPFT_FLOAT},
    {"proneRotY", offsetof(WeaponFullDef, weapDef.vProneRot[1]), CSPFT_FLOAT},
    {"proneRotR", offsetof(WeaponFullDef, weapDef.vProneRot[2]), CSPFT_FLOAT},
    {"posMoveRate", offsetof(WeaponFullDef, weapDef.fPosMoveRate), CSPFT_FLOAT},
    {"posProneMoveRate", offsetof(WeaponFullDef, weapDef.fPosProneMoveRate), CSPFT_FLOAT},
    {"standMoveMinSpeed", offsetof(WeaponFullDef, weapDef.fStandMoveMinSpeed), CSPFT_FLOAT},
    {"duckedMoveMinSpeed", offsetof(WeaponFullDef, weapDef.fDuckedMoveMinSpeed), CSPFT_FLOAT},
    {"proneMoveMinSpeed", offsetof(WeaponFullDef, weapDef.fProneMoveMinSpeed), CSPFT_FLOAT},
    {"posRotRate", offsetof(WeaponFullDef, weapDef.fPosRotRate), CSPFT_FLOAT},
    {"posProneRotRate", offsetof(WeaponFullDef, weapDef.fPosProneRotRate), CSPFT_FLOAT},
    {"standRotMinSpeed", offsetof(WeaponFullDef, weapDef.fStandRotMinSpeed), CSPFT_FLOAT},
    {"duckedRotMinSpeed", offsetof(WeaponFullDef, weapDef.fDuckedRotMinSpeed), CSPFT_FLOAT},
    {"proneRotMinSpeed", offsetof(WeaponFullDef, weapDef.fProneRotMinSpeed), CSPFT_FLOAT},
    {"worldModel", offsetof(WeaponFullDef, worldModel[0]), CSPFT_XMODEL},
    {"worldModel2", offsetof(WeaponFullDef, worldModel[1]), CSPFT_XMODEL},
    {"worldModel3", offsetof(WeaponFullDef, worldModel[2]), CSPFT_XMODEL},
    {"worldModel4", offsetof(WeaponFullDef, worldModel[3]), CSPFT_XMODEL},
    {"worldModel5", offsetof(WeaponFullDef, worldModel[4]), CSPFT_XMODEL},
    {"worldModel6", offsetof(WeaponFullDef, worldModel[5]), CSPFT_XMODEL},
    {"worldModel7", offsetof(WeaponFullDef, worldModel[6]), CSPFT_XMODEL},
    {"worldModel8", offsetof(WeaponFullDef, worldModel[7]), CSPFT_XMODEL},
    {"worldModel9", offsetof(WeaponFullDef, worldModel[8]), CSPFT_XMODEL},
    {"worldModel10", offsetof(WeaponFullDef, worldModel[9]), CSPFT_XMODEL},
    {"worldModel11", offsetof(WeaponFullDef, worldModel[10]), CSPFT_XMODEL},
    {"worldModel12", offsetof(WeaponFullDef, worldModel[11]), CSPFT_XMODEL},
    {"worldModel13", offsetof(WeaponFullDef, worldModel[12]), CSPFT_XMODEL},
    {"worldModel14", offsetof(WeaponFullDef, worldModel[13]), CSPFT_XMODEL},
    {"worldModel15", offsetof(WeaponFullDef, worldModel[14]), CSPFT_XMODEL},
    {"worldModel16", offsetof(WeaponFullDef, worldModel[15]), CSPFT_XMODEL},
    {"worldClipModel", offsetof(WeaponFullDef, weapDef.worldClipModel), CSPFT_XMODEL},
    {"rocketModel", offsetof(WeaponFullDef, weapDef.rocketModel), CSPFT_XMODEL},
    {"knifeModel", offsetof(WeaponFullDef, weapDef.knifeModel), CSPFT_XMODEL},
    {"worldKnifeModel", offsetof(WeaponFullDef, weapDef.worldKnifeModel), CSPFT_XMODEL},
    {"hudIcon", offsetof(WeaponFullDef, weapDef.hudIcon), CSPFT_MATERIAL},
    {"hudIconRatio", offsetof(WeaponFullDef, weapDef.hudIconRatio), WFT_ICONRATIO_HUD},
    {"pickupIcon", offsetof(WeaponFullDef, weapDef.pickupIcon), CSPFT_MATERIAL},
    {"pickupIconRatio", offsetof(WeaponFullDef, weapDef.pickupIconRatio), WFT_ICONRATIO_PICKUP},
    {"ammoCounterIcon", offsetof(WeaponFullDef, weapDef.ammoCounterIcon), CSPFT_MATERIAL},
    {"ammoCounterIconRatio", offsetof(WeaponFullDef, weapDef.ammoCounterIconRatio), WFT_ICONRATIO_AMMOCOUNTER},
    {"ammoCounterClip", offsetof(WeaponFullDef, weapDef.ammoCounterClip), WFT_AMMOCOUNTER_CLIPTYPE},
    {"startAmmo", offsetof(WeaponFullDef, weapDef.iStartAmmo), CSPFT_INT},
    {"shareAmmo", offsetof(WeaponFullDef, weapDef.sharedAmmo), CSPFT_BOOL},
    {"ammoName", offsetof(WeaponFullDef, weapDef.szAmmoName), CSPFT_STRING},
    {"clipName", offsetof(WeaponFullDef, weapDef.szClipName), CSPFT_STRING},
    {"maxAmmo", offsetof(WeaponFullDef, weapDef.iMaxAmmo), CSPFT_INT},
    {"clipSize", offsetof(WeaponFullDef, weapCompleteDef.iClipSize), CSPFT_INT},
    {"shotCount", offsetof(WeaponFullDef, weapDef.shotCount), CSPFT_INT},
    {"sharedAmmoCapName", offsetof(WeaponFullDef, weapDef.szSharedAmmoCapName), CSPFT_STRING},
    {"sharedAmmoCap", offsetof(WeaponFullDef, weapDef.iSharedAmmoCap), CSPFT_INT},
    {"damage", offsetof(WeaponFullDef, weapDef.damage), CSPFT_INT},
    {"playerDamage", offsetof(WeaponFullDef, weapDef.playerDamage), CSPFT_INT},
    {"meleeDamage", offsetof(WeaponFullDef, weapDef.iMeleeDamage), CSPFT_INT},
    {"minDamage", offsetof(WeaponFullDef, weapDef.minDamage), CSPFT_INT},
    {"minPlayerDamage", offsetof(WeaponFullDef, weapDef.minPlayerDamage), CSPFT_INT},
    {"maxDamageRange", offsetof(WeaponFullDef, weapDef.fMaxDamageRange), CSPFT_FLOAT},
    {"minDamageRange", offsetof(WeaponFullDef, weapDef.fMinDamageRange), CSPFT_FLOAT},
    {"destabilizationRateTime", offsetof(WeaponFullDef, weapDef.destabilizationRateTime), CSPFT_FLOAT},
    {"destabilizationCurvatureMax", offsetof(WeaponFullDef, weapDef.destabilizationCurvatureMax), CSPFT_FLOAT},
    {"destabilizeDistance", offsetof(WeaponFullDef, weapDef.destabilizeDistance), CSPFT_INT},
    {"fireDelay", offsetof(WeaponFullDef, weapDef.iFireDelay), CSPFT_MILLISECONDS},
    {"meleeDelay", offsetof(WeaponFullDef, weapDef.iMeleeDelay), CSPFT_MILLISECONDS},
    {"meleeChargeDelay", offsetof(WeaponFullDef, weapDef.meleeChargeDelay), CSPFT_MILLISECONDS},
    {"fireTime", offsetof(WeaponFullDef, weapCompleteDef.iFireTime), CSPFT_MILLISECONDS},
    {"rechamberTime", offsetof(WeaponFullDef, weapDef.iRechamberTime), CSPFT_MILLISECONDS},
    {"rechamberTimeOneHanded", offsetof(WeaponFullDef, weapDef.rechamberTimeOneHanded), CSPFT_MILLISECONDS},
    {"rechamberBoltTime", offsetof(WeaponFullDef, weapDef.iRechamberBoltTime), CSPFT_MILLISECONDS},
    {"holdFireTime", offsetof(WeaponFullDef, weapDef.iHoldFireTime), CSPFT_MILLISECONDS},
    {"detonateTime", offsetof(WeaponFullDef, weapDef.iDetonateTime), CSPFT_MILLISECONDS},
    {"detonateDelay", offsetof(WeaponFullDef, weapDef.iDetonateDelay), CSPFT_MILLISECONDS},
    {"meleeTime", offsetof(WeaponFullDef, weapDef.iMeleeTime), CSPFT_MILLISECONDS},
    {"meleeChargeTime", offsetof(WeaponFullDef, weapDef.meleeChargeTime), CSPFT_MILLISECONDS},
    {"reloadTime", offsetof(WeaponFullDef, weapDef.iReloadTime), CSPFT_MILLISECONDS},
    {"reloadShowRocketTime", offsetof(WeaponFullDef, weapDef.reloadShowRocketTime), CSPFT_MILLISECONDS},
    {"reloadEmptyTime", offsetof(WeaponFullDef, weapDef.iReloadEmptyTime), CSPFT_MILLISECONDS},
    {"reloadAddTime", offsetof(WeaponFullDef, weapDef.iReloadAddTime), CSPFT_MILLISECONDS},
    {"reloadStartTime", offsetof(WeaponFullDef, weapDef.iReloadStartTime), CSPFT_MILLISECONDS},
    {"reloadStartAddTime", offsetof(WeaponFullDef, weapDef.iReloadStartAddTime), CSPFT_MILLISECONDS},
    {"reloadEndTime", offsetof(WeaponFullDef, weapDef.iReloadEndTime), CSPFT_MILLISECONDS},
    {"dropTime", offsetof(WeaponFullDef, weapDef.iDropTime), CSPFT_MILLISECONDS},
    {"raiseTime", offsetof(WeaponFullDef, weapDef.iRaiseTime), CSPFT_MILLISECONDS},
    {"altDropTime", offsetof(WeaponFullDef, weapDef.iAltDropTime), CSPFT_MILLISECONDS},
    {"altRaiseTime", offsetof(WeaponFullDef, weapCompleteDef.iAltRaiseTime), CSPFT_MILLISECONDS},
    {"quickDropTime", offsetof(WeaponFullDef, weapDef.quickDropTime), CSPFT_MILLISECONDS},
    {"quickRaiseTime", offsetof(WeaponFullDef, weapDef.quickRaiseTime), CSPFT_MILLISECONDS},
    {"firstRaiseTime", offsetof(WeaponFullDef, weapCompleteDef.iFirstRaiseTime), CSPFT_MILLISECONDS},
    {"breachRaiseTime", offsetof(WeaponFullDef, weapDef.iBreachRaiseTime), CSPFT_MILLISECONDS},
    {"emptyRaiseTime", offsetof(WeaponFullDef, weapDef.iEmptyRaiseTime), CSPFT_MILLISECONDS},
    {"emptyDropTime", offsetof(WeaponFullDef, weapDef.iEmptyDropTime), CSPFT_MILLISECONDS},
    {"sprintInTime", offsetof(WeaponFullDef, weapDef.sprintInTime), CSPFT_MILLISECONDS},
    {"sprintLoopTime", offsetof(WeaponFullDef, weapDef.sprintLoopTime), CSPFT_MILLISECONDS},
    {"sprintOutTime", offsetof(WeaponFullDef, weapDef.sprintOutTime), CSPFT_MILLISECONDS},
    {"stunnedTimeBegin", offsetof(WeaponFullDef, weapDef.stunnedTimeBegin), CSPFT_MILLISECONDS},
    {"stunnedTimeLoop", offsetof(WeaponFullDef, weapDef.stunnedTimeLoop), CSPFT_MILLISECONDS},
    {"stunnedTimeEnd", offsetof(WeaponFullDef, weapDef.stunnedTimeEnd), CSPFT_MILLISECONDS},
    {"nightVisionWearTime", offsetof(WeaponFullDef, weapDef.nightVisionWearTime), CSPFT_MILLISECONDS},
    {"nightVisionWearTimeFadeOutEnd", offsetof(WeaponFullDef, weapDef.nightVisionWearTimeFadeOutEnd), CSPFT_MILLISECONDS},
    {"nightVisionWearTimePowerUp", offsetof(WeaponFullDef, weapDef.nightVisionWearTimePowerUp), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTime", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTime), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimePowerDown", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTimePowerDown), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimeFadeInStart", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTimeFadeInStart), CSPFT_MILLISECONDS},
    {"fuseTime", offsetof(WeaponFullDef, weapDef.fuseTime), CSPFT_MILLISECONDS},
    {"aifuseTime", offsetof(WeaponFullDef, weapDef.aiFuseTime), CSPFT_MILLISECONDS},
    {"lockonSupported", offsetof(WeaponFullDef, weapDef.lockonSupported), CSPFT_BOOL},
    {"requireLockonToFire", offsetof(WeaponFullDef, weapDef.requireLockonToFire), CSPFT_BOOL},
    {"bigExplosion", offsetof(WeaponFullDef, weapDef.bigExplosion), CSPFT_BOOL},
    {"noAdsWhenMagEmpty", offsetof(WeaponFullDef, weapDef.noAdsWhenMagEmpty), CSPFT_BOOL},
    {"inheritsPerks", offsetof(WeaponFullDef, weapDef.inheritsPerks), CSPFT_BOOL},
    {"avoidDropCleanup", offsetof(WeaponFullDef, weapDef.avoidDropCleanup), CSPFT_BOOL},
    {"autoAimRange", offsetof(WeaponFullDef, weapDef.autoAimRange), CSPFT_FLOAT},
    {"aimAssistRange", offsetof(WeaponFullDef, weapDef.aimAssistRange), CSPFT_FLOAT},
    {"aimAssistRangeAds", offsetof(WeaponFullDef, weapDef.aimAssistRangeAds), CSPFT_FLOAT},
    {"aimPadding", offsetof(WeaponFullDef, weapDef.aimPadding), CSPFT_FLOAT},
    {"enemyCrosshairRange", offsetof(WeaponFullDef, weapDef.enemyCrosshairRange), CSPFT_FLOAT},
    {"crosshairColorChange", offsetof(WeaponFullDef, weapDef.crosshairColorChange), CSPFT_BOOL},
    {"moveSpeedScale", offsetof(WeaponFullDef, weapDef.moveSpeedScale), CSPFT_FLOAT},
    {"adsMoveSpeedScale", offsetof(WeaponFullDef, weapDef.adsMoveSpeedScale), CSPFT_FLOAT},
    {"sprintDurationScale", offsetof(WeaponFullDef, weapDef.sprintDurationScale), CSPFT_FLOAT},
    {"idleCrouchFactor", offsetof(WeaponFullDef, weapDef.fIdleCrouchFactor), CSPFT_FLOAT},
    {"idleProneFactor", offsetof(WeaponFullDef, weapDef.fIdleProneFactor), CSPFT_FLOAT},
    {"gunMaxPitch", offsetof(WeaponFullDef, weapDef.fGunMaxPitch), CSPFT_FLOAT},
    {"gunMaxYaw", offsetof(WeaponFullDef, weapDef.fGunMaxYaw), CSPFT_FLOAT},
    {"swayMaxAngle", offsetof(WeaponFullDef, weapDef.swayMaxAngle), CSPFT_FLOAT},
    {"swayLerpSpeed", offsetof(WeaponFullDef, weapDef.swayLerpSpeed), CSPFT_FLOAT},
    {"swayPitchScale", offsetof(WeaponFullDef, weapDef.swayPitchScale), CSPFT_FLOAT},
    {"swayYawScale", offsetof(WeaponFullDef, weapDef.swayYawScale), CSPFT_FLOAT},
    {"swayHorizScale", offsetof(WeaponFullDef, weapDef.swayHorizScale), CSPFT_FLOAT},
    {"swayVertScale", offsetof(WeaponFullDef, weapDef.swayVertScale), CSPFT_FLOAT},
    {"swayShellShockScale", offsetof(WeaponFullDef, weapDef.swayShellShockScale), CSPFT_FLOAT},
    {"adsSwayMaxAngle", offsetof(WeaponFullDef, weapDef.adsSwayMaxAngle), CSPFT_FLOAT},
    {"adsSwayLerpSpeed", offsetof(WeaponFullDef, weapDef.adsSwayLerpSpeed), CSPFT_FLOAT},
    {"adsSwayPitchScale", offsetof(WeaponFullDef, weapDef.adsSwayPitchScale), CSPFT_FLOAT},
    {"adsSwayYawScale", offsetof(WeaponFullDef, weapDef.adsSwayYawScale), CSPFT_FLOAT},
    {"adsSwayHorizScale", offsetof(WeaponFullDef, weapDef.adsSwayHorizScale), CSPFT_FLOAT},
    {"adsSwayVertScale", offsetof(WeaponFullDef, weapDef.adsSwayVertScale), CSPFT_FLOAT},
    {"rifleBullet", offsetof(WeaponFullDef, weapDef.bRifleBullet), CSPFT_BOOL},
    {"armorPiercing", offsetof(WeaponFullDef, weapDef.armorPiercing), CSPFT_BOOL},
    {"boltAction", offsetof(WeaponFullDef, weapDef.bBoltAction), CSPFT_BOOL},
    {"aimDownSight", offsetof(WeaponFullDef, weapDef.aimDownSight), CSPFT_BOOL},
    {"rechamberWhileAds", offsetof(WeaponFullDef, weapDef.bRechamberWhileAds), CSPFT_BOOL},
    {"bBulletExplosiveDamage", offsetof(WeaponFullDef, weapDef.bBulletExplosiveDamage), CSPFT_BOOL},
    {"adsViewErrorMin", offsetof(WeaponFullDef, weapDef.adsViewErrorMin), CSPFT_FLOAT},
    {"adsViewErrorMax", offsetof(WeaponFullDef, weapDef.adsViewErrorMax), CSPFT_FLOAT},
    {"clipOnly", offsetof(WeaponFullDef, weapDef.bClipOnly), CSPFT_BOOL},
    {"noAmmoPickup", offsetof(WeaponFullDef, weapDef.noAmmoPickup), CSPFT_BOOL},
    {"cookOffHold", offsetof(WeaponFullDef, weapDef.bCookOffHold), CSPFT_BOOL},
    {"adsFire", offsetof(WeaponFullDef, weapDef.adsFireOnly), CSPFT_BOOL},
    {"cancelAutoHolsterWhenEmpty", offsetof(WeaponFullDef, weapDef.cancelAutoHolsterWhenEmpty), CSPFT_BOOL},
    {"disableSwitchToWhenEmpty", offsetof(WeaponFullDef, weapDef.disableSwitchToWhenEmpty), CSPFT_BOOL},
    {"suppressAmmoReserveDisplay", offsetof(WeaponFullDef, weapDef.suppressAmmoReserveDisplay), CSPFT_BOOL},
    {"enhanced", offsetof(WeaponFullDef, weapCompleteDef.enhanced), CSPFT_BOOL},
    {"motionTracker", offsetof(WeaponFullDef, weapCompleteDef.motionTracker), CSPFT_BOOL},
    {"laserSightDuringNightvision", offsetof(WeaponFullDef, weapDef.laserSightDuringNightvision), CSPFT_BOOL},
    {"markableViewmodel", offsetof(WeaponFullDef, weapDef.markableViewmodel), CSPFT_BOOL},
    {"physCollmap", offsetof(WeaponFullDef, weapDef.physCollmap), CSPFT_PHYS_COLLMAP},
    {"noDualWield", offsetof(WeaponFullDef, weapDef.noDualWield), CSPFT_BOOL},
    {"dualWieldViewModelOffset", offsetof(WeaponFullDef, weapDef.dualWieldViewModelOffset), CSPFT_FLOAT},
    {"killIcon", offsetof(WeaponFullDef, weapCompleteDef.killIcon), CSPFT_MATERIAL},
    {"killIconRatio", offsetof(WeaponFullDef, weapDef.killIconRatio), WFT_ICONRATIO_KILL},
    {"flipKillIcon", offsetof(WeaponFullDef, weapDef.flipKillIcon), CSPFT_BOOL},
    {"dpadIcon", offsetof(WeaponFullDef, weapCompleteDef.dpadIcon), CSPFT_MATERIAL},
    {"dpadIconRatio", offsetof(WeaponFullDef, weapCompleteDef.dpadIconRatio), WFT_ICONRATIO_DPAD},
    {"dpadIconShowsAmmo", offsetof(WeaponFullDef, weapCompleteDef.dpadIconShowsAmmo), CSPFT_BOOL},
    {"noPartialReload", offsetof(WeaponFullDef, weapDef.bNoPartialReload), CSPFT_BOOL},
    {"segmentedReload", offsetof(WeaponFullDef, weapDef.bSegmentedReload), CSPFT_BOOL},
    {"reloadAmmoAdd", offsetof(WeaponFullDef, weapDef.iReloadAmmoAdd), CSPFT_INT},
    {"reloadStartAdd", offsetof(WeaponFullDef, weapDef.iReloadStartAdd), CSPFT_INT},
    {"altWeapon", offsetof(WeaponFullDef, weapCompleteDef.szAltWeaponName), CSPFT_STRING},
    {"dropAmmoMin", offsetof(WeaponFullDef, weapDef.ammoDropStockMin), CSPFT_INT},
    {"dropAmmoMax", offsetof(WeaponFullDef, weapCompleteDef.ammoDropStockMax), CSPFT_INT},
    {"ammoDropClipPercentMin", offsetof(WeaponFullDef, weapDef.ammoDropClipPercentMin), CSPFT_INT},
    {"ammoDropClipPercentMax", offsetof(WeaponFullDef, weapDef.ammoDropClipPercentMax), CSPFT_INT},
    {"blocksProne", offsetof(WeaponFullDef, weapDef.blocksProne), CSPFT_BOOL},
    {"silenced", offsetof(WeaponFullDef, weapDef.silenced), CSPFT_BOOL},
    {"isRollingGrenade", offsetof(WeaponFullDef, weapDef.isRollingGrenade), CSPFT_BOOL},
    {"explosionRadius", offsetof(WeaponFullDef, weapDef.iExplosionRadius), CSPFT_INT},
    {"explosionRadiusMin", offsetof(WeaponFullDef, weapDef.iExplosionRadiusMin), CSPFT_INT},
    {"explosionInnerDamage", offsetof(WeaponFullDef, weapDef.iExplosionInnerDamage), CSPFT_INT},
    {"explosionOuterDamage", offsetof(WeaponFullDef, weapDef.iExplosionOuterDamage), CSPFT_INT},
    {"damageConeAngle", offsetof(WeaponFullDef, weapDef.damageConeAngle), CSPFT_FLOAT},
    {"bulletExplDmgMult", offsetof(WeaponFullDef, weapDef.bulletExplDmgMult), CSPFT_FLOAT},
    {"bulletExplRadiusMult", offsetof(WeaponFullDef, weapDef.bulletExplRadiusMult), CSPFT_FLOAT},
    {"projectileSpeed", offsetof(WeaponFullDef, weapDef.iProjectileSpeed), CSPFT_INT},
    {"projectileSpeedUp", offsetof(WeaponFullDef, weapDef.iProjectileSpeedUp), CSPFT_INT},
    {"projectileSpeedForward", offsetof(WeaponFullDef, weapDef.iProjectileSpeedForward), CSPFT_INT},
    {"projectileActivateDist", offsetof(WeaponFullDef, weapDef.iProjectileActivateDist), CSPFT_INT},
    {"projectileLifetime", offsetof(WeaponFullDef, weapDef.projLifetime), CSPFT_FLOAT},
    {"timeToAccelerate", offsetof(WeaponFullDef, weapDef.timeToAccelerate), CSPFT_FLOAT},
    {"projectileCurvature", offsetof(WeaponFullDef, weapDef.projectileCurvature), CSPFT_FLOAT},
    {"projectileModel", offsetof(WeaponFullDef, weapDef.projectileModel), CSPFT_XMODEL},
    {"projExplosionType", offsetof(WeaponFullDef, weapDef.projExplosion), WFT_PROJ_EXPLOSION},
    {"projExplosionEffect", offsetof(WeaponFullDef, weapDef.projExplosionEffect), CSPFT_FX},
    {"projExplosionEffectForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffectForceNormalUp), CSPFT_BOOL},
    {"projExplosionSound", offsetof(WeaponFullDef, weapDef.projExplosionSound), CSPFT_SOUND},
    {"projDudEffect", offsetof(WeaponFullDef, weapDef.projDudEffect), CSPFT_FX},
    {"projDudSound", offsetof(WeaponFullDef, weapDef.projDudSound), CSPFT_SOUND},
    {"projImpactExplode", offsetof(WeaponFullDef, weapDef.bProjImpactExplode), CSPFT_BOOL},
    {"stickiness", offsetof(WeaponFullDef, weapDef.stickiness), WFT_STICKINESS},
    {"stickToPlayers", offsetof(WeaponFullDef, weapDef.stickToPlayers), CSPFT_BOOL},
    {"hasDetonator", offsetof(WeaponFullDef, weapDef.hasDetonator), CSPFT_BOOL},
    {"disableFiring", offsetof(WeaponFullDef, weapDef.disableFiring), CSPFT_BOOL},
    {"timedDetonation", offsetof(WeaponFullDef, weapDef.timedDetonation), CSPFT_BOOL},
    {"rotate", offsetof(WeaponFullDef, weapDef.rotate), CSPFT_BOOL},
    {"holdButtonToThrow", offsetof(WeaponFullDef, weapDef.holdButtonToThrow), CSPFT_BOOL},
    {"freezeMovementWhenFiring", offsetof(WeaponFullDef, weapDef.freezeMovementWhenFiring), CSPFT_BOOL},
    {"lowAmmoWarningThreshold", offsetof(WeaponFullDef, weapDef.lowAmmoWarningThreshold), CSPFT_FLOAT},
    {"ricochetChance", offsetof(WeaponFullDef, weapDef.ricochetChance), CSPFT_FLOAT},
    {"offhandHoldIsCancelable", offsetof(WeaponFullDef, weapDef.offhandHoldIsCancelable), CSPFT_BOOL},
    {"parallelDefaultBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_DEFAULT]), CSPFT_FLOAT},
    {"parallelBarkBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_BARK]), CSPFT_FLOAT},
    {"parallelBrickBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_BRICK]), CSPFT_FLOAT},
    {"parallelCarpetBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CARPET]), CSPFT_FLOAT},
    {"parallelClothBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CLOTH]), CSPFT_FLOAT},
    {"parallelConcreteBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CONCRETE]), CSPFT_FLOAT},
    {"parallelDirtBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_DIRT]), CSPFT_FLOAT},
    {"parallelFleshBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FLESH]), CSPFT_FLOAT},
    {"parallelFoliageBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FOLIAGE]), CSPFT_FLOAT},
    {"parallelGlassBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GLASS]), CSPFT_FLOAT},
    {"parallelGrassBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GRASS]), CSPFT_FLOAT},
    {"parallelGravelBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GRAVEL]), CSPFT_FLOAT},
    {"parallelIceBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ICE]), CSPFT_FLOAT},
    {"parallelMetalBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_METAL]), CSPFT_FLOAT},
    {"parallelMudBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_MUD]), CSPFT_FLOAT},
    {"parallelPaperBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PAPER]), CSPFT_FLOAT},
    {"parallelPlasterBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PLASTER]), CSPFT_FLOAT},
    {"parallelRockBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ROCK]), CSPFT_FLOAT},
    {"parallelSandBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_SAND]), CSPFT_FLOAT},
    {"parallelSnowBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_SNOW]), CSPFT_FLOAT},
    {"parallelWaterBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_WATER]), CSPFT_FLOAT},
    {"parallelWoodBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_WOOD]), CSPFT_FLOAT},
    {"parallelAsphaltBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ASPHALT]), CSPFT_FLOAT},
    {"parallelCeramicBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CERAMIC]), CSPFT_FLOAT},
    {"parallelPlasticBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PLASTIC]), CSPFT_FLOAT},
    {"parallelRubberBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_RUBBER]), CSPFT_FLOAT},
    {"parallelCushionBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CUSHION]), CSPFT_FLOAT},
    {"parallelFruitBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FRUIT]), CSPFT_FLOAT},
    {"parallelPaintedMetalBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PAINTED_METAL]), CSPFT_FLOAT},
    {"parallelRiotShieldBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_RIOT_SHIELD]), CSPFT_FLOAT},
    {"parallelSlushBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_SLUSH]), CSPFT_FLOAT},
    {"perpendicularDefaultBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_DEFAULT]), CSPFT_FLOAT},
    {"perpendicularBarkBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_BARK]), CSPFT_FLOAT},
    {"perpendicularBrickBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_BRICK]), CSPFT_FLOAT},
    {"perpendicularCarpetBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CARPET]), CSPFT_FLOAT},
    {"perpendicularClothBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CLOTH]), CSPFT_FLOAT},
    {"perpendicularConcreteBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CONCRETE]), CSPFT_FLOAT},
    {"perpendicularDirtBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_DIRT]), CSPFT_FLOAT},
    {"perpendicularFleshBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FLESH]), CSPFT_FLOAT},
    {"perpendicularFoliageBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FOLIAGE]), CSPFT_FLOAT},
    {"perpendicularGlassBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GLASS]), CSPFT_FLOAT},
    {"perpendicularGrassBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GRASS]), CSPFT_FLOAT},
    {"perpendicularGravelBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GRAVEL]), CSPFT_FLOAT},
    {"perpendicularIceBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ICE]), CSPFT_FLOAT},
    {"perpendicularMetalBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_METAL]), CSPFT_FLOAT},
    {"perpendicularMudBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_MUD]), CSPFT_FLOAT},
    {"perpendicularPaperBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PAPER]), CSPFT_FLOAT},
    {"perpendicularPlasterBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PLASTER]), CSPFT_FLOAT},
    {"perpendicularRockBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ROCK]), CSPFT_FLOAT},
    {"perpendicularSandBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_SAND]), CSPFT_FLOAT},
    {"perpendicularSnowBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_SNOW]), CSPFT_FLOAT},
    {"perpendicularWaterBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_WATER]), CSPFT_FLOAT},
    {"perpendicularWoodBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_WOOD]), CSPFT_FLOAT},
    {"perpendicularAsphaltBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ASPHALT]), CSPFT_FLOAT},
    {"perpendicularCeramicBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CERAMIC]), CSPFT_FLOAT},
    {"perpendicularPlasticBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PLASTIC]), CSPFT_FLOAT},
    {"perpendicularRubberBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_RUBBER]), CSPFT_FLOAT},
    {"perpendicularCushionBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CUSHION]), CSPFT_FLOAT},
    {"perpendicularFruitBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FRUIT]), CSPFT_FLOAT},
    {"perpendicularPaintedMetalBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PAINTED_METAL]), CSPFT_FLOAT},
    {"perpendicularRiotShieldBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_RIOT_SHIELD]), CSPFT_FLOAT},
    {"perpendicularSlushBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_SLUSH]), CSPFT_FLOAT},
    {"projTrailEffect", offsetof(WeaponFullDef, weapDef.projTrailEffect), CSPFT_FX},
    {"projBeaconEffect", offsetof(WeaponFullDef, weapDef.projBeaconEffect), CSPFT_FX},
    {"projectileRed", offsetof(WeaponFullDef, weapDef.vProjectileColor[0]), CSPFT_FLOAT},
    {"projectileGreen", offsetof(WeaponFullDef, weapDef.vProjectileColor[1]), CSPFT_FLOAT},
    {"projectileBlue", offsetof(WeaponFullDef, weapDef.vProjectileColor[2]), CSPFT_FLOAT},
    {"guidedMissileType", offsetof(WeaponFullDef, weapDef.guidedMissileType), WFT_GUIDED_MISSILE_TYPE},
    {"maxSteeringAccel", offsetof(WeaponFullDef, weapDef.maxSteeringAccel), CSPFT_FLOAT},
    {"projIgnitionDelay", offsetof(WeaponFullDef, weapDef.projIgnitionDelay), CSPFT_INT},
    {"projIgnitionEffect", offsetof(WeaponFullDef, weapDef.projIgnitionEffect), CSPFT_FX},
    {"projIgnitionSound", offsetof(WeaponFullDef, weapDef.projIgnitionSound), CSPFT_SOUND},
    {"adsTransInTime", offsetof(WeaponFullDef, weapCompleteDef.iAdsTransInTime), CSPFT_MILLISECONDS},
    {"adsTransOutTime", offsetof(WeaponFullDef, weapCompleteDef.iAdsTransOutTime), CSPFT_MILLISECONDS},
    {"adsIdleAmount", offsetof(WeaponFullDef, weapDef.fAdsIdleAmount), CSPFT_FLOAT},
    {"adsIdleSpeed", offsetof(WeaponFullDef, weapDef.adsIdleSpeed), CSPFT_FLOAT},
    {"adsZoomFov", offsetof(WeaponFullDef, weapCompleteDef.fAdsZoomFov), CSPFT_FLOAT},
    {"adsZoomInFrac", offsetof(WeaponFullDef, weapDef.fAdsZoomInFrac), CSPFT_FLOAT},
    {"adsZoomOutFrac", offsetof(WeaponFullDef, weapDef.fAdsZoomOutFrac), CSPFT_FLOAT},
    {"adsOverlayShader", offsetof(WeaponFullDef, weapDef.overlayMaterial), CSPFT_MATERIAL},
    {"adsOverlayShaderLowRes", offsetof(WeaponFullDef, weapDef.overlayMaterialLowRes), CSPFT_MATERIAL},
    {"adsOverlayShaderEMP", offsetof(WeaponFullDef, weapDef.overlayMaterialEMP), CSPFT_MATERIAL},
    {"adsOverlayShaderEMPLowRes", offsetof(WeaponFullDef, weapDef.overlayMaterialEMPLowRes), CSPFT_MATERIAL},
    {"adsOverlayReticle", offsetof(WeaponFullDef, weapDef.overlayReticle), WFT_OVERLAYRETICLE},
    {"adsOverlayInterface", offsetof(WeaponFullDef, weapDef.overlayInterface), WFT_OVERLAYINTERFACE},
    {"adsOverlayWidth", offsetof(WeaponFullDef, weapDef.overlayWidth), CSPFT_FLOAT},
    {"adsOverlayHeight", offsetof(WeaponFullDef, weapDef.overlayHeight), CSPFT_FLOAT},
    {"adsOverlayWidthSplitscreen", offsetof(WeaponFullDef, weapDef.overlayWidthSplitscreen), CSPFT_FLOAT},
    {"adsOverlayHeightSplitscreen", offsetof(WeaponFullDef, weapDef.overlayHeightSplitscreen), CSPFT_FLOAT},
    {"adsBobFactor", offsetof(WeaponFullDef, weapDef.fAdsBobFactor), CSPFT_FLOAT},
    {"adsViewBobMult", offsetof(WeaponFullDef, weapDef.fAdsViewBobMult), CSPFT_FLOAT},
    {"adsAimPitch", offsetof(WeaponFullDef, weapDef.fAdsAimPitch), CSPFT_FLOAT},
    {"adsCrosshairInFrac", offsetof(WeaponFullDef, weapDef.fAdsCrosshairInFrac), CSPFT_FLOAT},
    {"adsCrosshairOutFrac", offsetof(WeaponFullDef, weapDef.fAdsCrosshairOutFrac), CSPFT_FLOAT},
    {"adsReloadTransTime", offsetof(WeaponFullDef, weapDef.iPositionReloadTransTime), CSPFT_MILLISECONDS},
    {"adsGunKickReducedKickBullets", offsetof(WeaponFullDef, weapDef.adsGunKickReducedKickBullets), CSPFT_INT},
    {"adsGunKickReducedKickPercent", offsetof(WeaponFullDef, weapDef.adsGunKickReducedKickPercent), CSPFT_FLOAT},
    {"adsGunKickPitchMin", offsetof(WeaponFullDef, weapDef.fAdsGunKickPitchMin), CSPFT_FLOAT},
    {"adsGunKickPitchMax", offsetof(WeaponFullDef, weapDef.fAdsGunKickPitchMax), CSPFT_FLOAT},
    {"adsGunKickYawMin", offsetof(WeaponFullDef, weapDef.fAdsGunKickYawMin), CSPFT_FLOAT},
    {"adsGunKickYawMax", offsetof(WeaponFullDef, weapDef.fAdsGunKickYawMax), CSPFT_FLOAT},
    {"adsGunKickAccel", offsetof(WeaponFullDef, weapDef.fAdsGunKickAccel), CSPFT_FLOAT},
    {"adsGunKickSpeedMax", offsetof(WeaponFullDef, weapDef.fAdsGunKickSpeedMax), CSPFT_FLOAT},
    {"adsGunKickSpeedDecay", offsetof(WeaponFullDef, weapDef.fAdsGunKickSpeedDecay), CSPFT_FLOAT},
    {"adsGunKickStaticDecay", offsetof(WeaponFullDef, weapDef.fAdsGunKickStaticDecay), CSPFT_FLOAT},
    {"adsViewKickPitchMin", offsetof(WeaponFullDef, weapDef.fAdsViewKickPitchMin), CSPFT_FLOAT},
    {"adsViewKickPitchMax", offsetof(WeaponFullDef, weapDef.fAdsViewKickPitchMax), CSPFT_FLOAT},
    {"adsViewKickYawMin", offsetof(WeaponFullDef, weapDef.fAdsViewKickYawMin), CSPFT_FLOAT},
    {"adsViewKickYawMax", offsetof(WeaponFullDef, weapDef.fAdsViewKickYawMax), CSPFT_FLOAT},
    {"adsViewKickCenterSpeed", offsetof(WeaponFullDef, weapCompleteDef.fAdsViewKickCenterSpeed), CSPFT_FLOAT},
    {"adsSpread", offsetof(WeaponFullDef, weapDef.fAdsSpread), CSPFT_FLOAT},
    {"guidedMissileType", offsetof(WeaponFullDef, weapDef.guidedMissileType), WFT_GUIDED_MISSILE_TYPE},
    {"hipSpreadStandMin", offsetof(WeaponFullDef, weapDef.fHipSpreadStandMin), CSPFT_FLOAT},
    {"hipSpreadDuckedMin", offsetof(WeaponFullDef, weapDef.fHipSpreadDuckedMin), CSPFT_FLOAT},
    {"hipSpreadProneMin", offsetof(WeaponFullDef, weapDef.fHipSpreadProneMin), CSPFT_FLOAT},
    {"hipSpreadMax", offsetof(WeaponFullDef, weapDef.hipSpreadStandMax), CSPFT_FLOAT},
    {"hipSpreadDuckedMax", offsetof(WeaponFullDef, weapDef.hipSpreadDuckedMax), CSPFT_FLOAT},
    {"hipSpreadProneMax", offsetof(WeaponFullDef, weapDef.hipSpreadProneMax), CSPFT_FLOAT},
    {"hipSpreadDecayRate", offsetof(WeaponFullDef, weapDef.fHipSpreadDecayRate), CSPFT_FLOAT},
    {"hipSpreadFireAdd", offsetof(WeaponFullDef, weapDef.fHipSpreadFireAdd), CSPFT_FLOAT},
    {"hipSpreadTurnAdd", offsetof(WeaponFullDef, weapDef.fHipSpreadTurnAdd), CSPFT_FLOAT},
    {"hipSpreadMoveAdd", offsetof(WeaponFullDef, weapDef.fHipSpreadMoveAdd), CSPFT_FLOAT},
    {"hipSpreadDuckedDecay", offsetof(WeaponFullDef, weapDef.fHipSpreadDuckedDecay), CSPFT_FLOAT},
    {"hipSpreadProneDecay", offsetof(WeaponFullDef, weapDef.fHipSpreadProneDecay), CSPFT_FLOAT},
    {"hipReticleSidePos", offsetof(WeaponFullDef, weapDef.fHipReticleSidePos), CSPFT_FLOAT},
    {"hipIdleAmount", offsetof(WeaponFullDef, weapDef.fHipIdleAmount), CSPFT_FLOAT},
    {"hipIdleSpeed", offsetof(WeaponFullDef, weapDef.hipIdleSpeed), CSPFT_FLOAT},
    {"hipGunKickReducedKickBullets", offsetof(WeaponFullDef, weapDef.hipGunKickReducedKickBullets), CSPFT_INT},
    {"hipGunKickReducedKickPercent", offsetof(WeaponFullDef, weapDef.hipGunKickReducedKickPercent), CSPFT_FLOAT},
    {"hipGunKickPitchMin", offsetof(WeaponFullDef, weapDef.fHipGunKickPitchMin), CSPFT_FLOAT},
    {"hipGunKickPitchMax", offsetof(WeaponFullDef, weapDef.fHipGunKickPitchMax), CSPFT_FLOAT},
    {"hipGunKickYawMin", offsetof(WeaponFullDef, weapDef.fHipGunKickYawMin), CSPFT_FLOAT},
    {"hipGunKickYawMax", offsetof(WeaponFullDef, weapDef.fHipGunKickYawMax), CSPFT_FLOAT},
    {"hipGunKickAccel", offsetof(WeaponFullDef, weapDef.fHipGunKickAccel), CSPFT_FLOAT},
    {"hipGunKickSpeedMax", offsetof(WeaponFullDef, weapDef.fHipGunKickSpeedMax), CSPFT_FLOAT},
    {"hipGunKickSpeedDecay", offsetof(WeaponFullDef, weapDef.fHipGunKickSpeedDecay), CSPFT_FLOAT},
    {"hipGunKickStaticDecay", offsetof(WeaponFullDef, weapDef.fHipGunKickStaticDecay), CSPFT_FLOAT},
    {"hipViewKickPitchMin", offsetof(WeaponFullDef, weapDef.fHipViewKickPitchMin), CSPFT_FLOAT},
    {"hipViewKickPitchMax", offsetof(WeaponFullDef, weapDef.fHipViewKickPitchMax), CSPFT_FLOAT},
    {"hipViewKickYawMin", offsetof(WeaponFullDef, weapDef.fHipViewKickYawMin), CSPFT_FLOAT},
    {"hipViewKickYawMax", offsetof(WeaponFullDef, weapDef.fHipViewKickYawMax), CSPFT_FLOAT},
    {"hipViewKickCenterSpeed", offsetof(WeaponFullDef, weapCompleteDef.fHipViewKickCenterSpeed), CSPFT_FLOAT},
    {"leftArc", offsetof(WeaponFullDef, weapDef.leftArc), CSPFT_FLOAT},
    {"rightArc", offsetof(WeaponFullDef, weapDef.rightArc), CSPFT_FLOAT},
    {"topArc", offsetof(WeaponFullDef, weapDef.topArc), CSPFT_FLOAT},
    {"bottomArc", offsetof(WeaponFullDef, weapDef.bottomArc), CSPFT_FLOAT},
    {"accuracy", offsetof(WeaponFullDef, weapDef.accuracy), CSPFT_FLOAT},
    {"aiSpread", offsetof(WeaponFullDef, weapDef.aiSpread), CSPFT_FLOAT},
    {"playerSpread", offsetof(WeaponFullDef, weapDef.playerSpread), CSPFT_FLOAT},
    {"maxVertTurnSpeed", offsetof(WeaponFullDef, weapDef.maxTurnSpeed[0]), CSPFT_FLOAT},
    {"maxHorTurnSpeed", offsetof(WeaponFullDef, weapDef.maxTurnSpeed[1]), CSPFT_FLOAT},
    {"minVertTurnSpeed", offsetof(WeaponFullDef, weapDef.minTurnSpeed[0]), CSPFT_FLOAT},
    {"minHorTurnSpeed", offsetof(WeaponFullDef, weapDef.minTurnSpeed[1]), CSPFT_FLOAT},
    {"pitchConvergenceTime", offsetof(WeaponFullDef, weapDef.pitchConvergenceTime), CSPFT_FLOAT},
    {"yawConvergenceTime", offsetof(WeaponFullDef, weapDef.yawConvergenceTime), CSPFT_FLOAT},
    {"suppressionTime", offsetof(WeaponFullDef, weapDef.suppressTime), CSPFT_FLOAT},
    {"maxRange", offsetof(WeaponFullDef, weapDef.maxRange), CSPFT_FLOAT},
    {"animHorRotateInc", offsetof(WeaponFullDef, weapDef.fAnimHorRotateInc), CSPFT_FLOAT},
    {"playerPositionDist", offsetof(WeaponFullDef, weapDef.fPlayerPositionDist), CSPFT_FLOAT},
    {"stance", offsetof(WeaponFullDef, weapDef.stance), WFT_STANCE},
    {"useHintString", offsetof(WeaponFullDef, weapDef.szUseHintString), CSPFT_STRING},
    {"dropHintString", offsetof(WeaponFullDef, weapDef.dropHintString), CSPFT_STRING},
    {"horizViewJitter", offsetof(WeaponFullDef, weapDef.horizViewJitter), CSPFT_FLOAT},
    {"vertViewJitter", offsetof(WeaponFullDef, weapDef.vertViewJitter), CSPFT_FLOAT},
    {"scanSpeed", offsetof(WeaponFullDef, weapDef.scanSpeed), CSPFT_FLOAT},
    {"scanAccel", offsetof(WeaponFullDef, weapDef.scanAccel), CSPFT_FLOAT},
    {"scanPauseTime", offsetof(WeaponFullDef, weapDef.scanPauseTime), CSPFT_MILLISECONDS},
    {"fightDist", offsetof(WeaponFullDef, weapDef.fightDist), CSPFT_FLOAT},
    {"maxDist", offsetof(WeaponFullDef, weapDef.maxDist), CSPFT_FLOAT},
    {"aiVsAiAccuracyGraph", offsetof(WeaponFullDef, weapDef.accuracyGraphName0), CSPFT_STRING},
    {"aiVsPlayerAccuracyGraph", offsetof(WeaponFullDef, weapDef.accuracyGraphName1), CSPFT_STRING},
    {"locNone", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_NONE]), CSPFT_FLOAT},
    {"locHelmet", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_HELMET]), CSPFT_FLOAT},
    {"locHead", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_HEAD]), CSPFT_FLOAT},
    {"locNeck", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_NECK]), CSPFT_FLOAT},
    {"locTorsoUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_TORSO_UPR]), CSPFT_FLOAT},
    {"locTorsoLower", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_TORSO_LWR]), CSPFT_FLOAT},
    {"locRightArmUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_ARM_UPR]), CSPFT_FLOAT},
    {"locRightArmLower", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_ARM_LWR]), CSPFT_FLOAT},
    {"locRightHand", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_HAND]), CSPFT_FLOAT},
    {"locLeftArmUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_ARM_UPR]), CSPFT_FLOAT},
    {"locLeftArmLower", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_ARM_LWR]), CSPFT_FLOAT},
    {"locLeftHand", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_HAND]), CSPFT_FLOAT},
    {"locRightLegUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_LEG_UPR]), CSPFT_FLOAT},
    {"locRightLegLower", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_LEG_LWR]), CSPFT_FLOAT},
    {"locRightFoot", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_R_FOOT]), CSPFT_FLOAT},
    {"locLeftLegUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_LEG_UPR]), CSPFT_FLOAT},
    {"locLeftLegLower", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_LEG_LWR]), CSPFT_FLOAT},
    {"locLeftFoot", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_L_FOOT]), CSPFT_FLOAT},
    {"locGun", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_GUN]), CSPFT_FLOAT},
    {"fireRumble", offsetof(WeaponFullDef, weapDef.fireRumble), CSPFT_STRING},
    {"meleeImpactRumble", offsetof(WeaponFullDef, weapDef.meleeImpactRumble), CSPFT_STRING},
    {"tracerType", offsetof(WeaponFullDef, weapDef.tracerType), CSPFT_TRACER},
    {"adsDofStart", offsetof(WeaponFullDef, weapCompleteDef.adsDofStart), CSPFT_FLOAT},
    {"adsDofEnd", offsetof(WeaponFullDef, weapCompleteDef.adsDofEnd), CSPFT_FLOAT},
    {"turretScopeZoomRate", offsetof(WeaponFullDef, weapDef.turretScopeZoomRate), CSPFT_FLOAT},
    {"turretScopeZoomMin", offsetof(WeaponFullDef, weapDef.turretScopeZoomMin), CSPFT_FLOAT},
    {"turretScopeZoomMax", offsetof(WeaponFullDef, weapDef.turretScopeZoomMax), CSPFT_FLOAT},
    {"thermalScope", offsetof(WeaponFullDef, weapDef.thermalScope), CSPFT_BOOL},
    {"altModeSameWeapon", offsetof(WeaponFullDef, weapDef.altModeSameWeapon), CSPFT_BOOL},
    {"turretOverheatUpRate", offsetof(WeaponFullDef, weapDef.turretOverheatUpRate), CSPFT_FLOAT},
    {"turretOverheatDownRate", offsetof(WeaponFullDef, weapDef.turretOverheatDownRate), CSPFT_FLOAT},
    {"turretOverheatPenalty", offsetof(WeaponFullDef, weapDef.turretOverheatPenalty), CSPFT_FLOAT},
    {"turretOverheatSound", offsetof(WeaponFullDef, weapDef.turretOverheatSound), CSPFT_SOUND},
    {"turretOverheatEffect", offsetof(WeaponFullDef, weapDef.turretOverheatEffect), CSPFT_FX},
    {"turretBarrelSpinEnabled", offsetof(WeaponFullDef, weapDef.turretBarrelSpinEnabled), CSPFT_BOOL},
    {"turretBarrelSpinUpTime", offsetof(WeaponFullDef, weapDef.turretBarrelSpinUpTime), CSPFT_FLOAT},
    {"turretBarrelSpinDownTime", offsetof(WeaponFullDef, weapDef.turretBarrelSpinDownTime), CSPFT_FLOAT},
    {"turretBarrelSpinRumble", offsetof(WeaponFullDef, weapDef.turretBarrelSpinRumble), CSPFT_STRING},
    {"turretBarrelSpinSpeed", offsetof(WeaponFullDef, weapDef.turretBarrelSpinSpeed), CSPFT_FLOAT},
    {"turretBarrelSpinMaxSnd", offsetof(WeaponFullDef, weapDef.turretBarrelSpinMaxSnd), CSPFT_SOUND},
    {"turretBarrelSpinUpSnd1", offsetof(WeaponFullDef, weapDef.turretBarrelSpinUpSnd[0]), CSPFT_SOUND},
    {"turretBarrelSpinUpSnd2", offsetof(WeaponFullDef, weapDef.turretBarrelSpinUpSnd[1]), CSPFT_SOUND},
    {"turretBarrelSpinUpSnd3", offsetof(WeaponFullDef, weapDef.turretBarrelSpinUpSnd[2]), CSPFT_SOUND},
    {"turretBarrelSpinUpSnd4", offsetof(WeaponFullDef, weapDef.turretBarrelSpinUpSnd[3]), CSPFT_SOUND},
    {"turretBarrelSpinDownSnd1", offsetof(WeaponFullDef, weapDef.turretBarrelSpinDownSnd[0]), CSPFT_SOUND},
    {"turretBarrelSpinDownSnd2", offsetof(WeaponFullDef, weapDef.turretBarrelSpinDownSnd[1]), CSPFT_SOUND},
    {"turretBarrelSpinDownSnd3", offsetof(WeaponFullDef, weapDef.turretBarrelSpinDownSnd[2]), CSPFT_SOUND},
    {"turretBarrelSpinDownSnd4", offsetof(WeaponFullDef, weapDef.turretBarrelSpinDownSnd[3]), CSPFT_SOUND},
    {"missileConeSoundEnabled", offsetof(WeaponFullDef, weapDef.missileConeSoundEnabled), CSPFT_BOOL},
    {"missileConeSoundAlias", offsetof(WeaponFullDef, weapDef.missileConeSoundAlias), CSPFT_SOUND},
    {"missileConeSoundAliasAtBase", offsetof(WeaponFullDef, weapDef.missileConeSoundAliasAtBase), CSPFT_SOUND},
    {"missileConeSoundRadiusAtTop", offsetof(WeaponFullDef, weapDef.missileConeSoundRadiusAtTop), CSPFT_FLOAT},
    {"missileConeSoundRadiusAtBase", offsetof(WeaponFullDef, weapDef.missileConeSoundRadiusAtBase), CSPFT_FLOAT},
    {"missileConeSoundHeight", offsetof(WeaponFullDef, weapDef.missileConeSoundHeight), CSPFT_FLOAT},
    {"missileConeSoundOriginOffset", offsetof(WeaponFullDef, weapDef.missileConeSoundOriginOffset), CSPFT_FLOAT},
    {"missileConeSoundVolumescaleAtCore", offsetof(WeaponFullDef, weapDef.missileConeSoundVolumescaleAtCore), CSPFT_FLOAT},
    {"missileConeSoundVolumescaleAtEdge", offsetof(WeaponFullDef, weapDef.missileConeSoundVolumescaleAtEdge), CSPFT_FLOAT},
    {"missileConeSoundVolumescaleCoreSize", offsetof(WeaponFullDef, weapDef.missileConeSoundVolumescaleCoreSize), CSPFT_FLOAT},
    {"missileConeSoundPitchshiftEnabled", offsetof(WeaponFullDef, weapDef.missileConeSoundPitchshiftEnabled), CSPFT_BOOL},
    {"missileConeSoundPitchAtTop", offsetof(WeaponFullDef, weapDef.missileConeSoundPitchAtTop), CSPFT_FLOAT},
    {"missileConeSoundPitchAtBottom", offsetof(WeaponFullDef, weapDef.missileConeSoundPitchAtBottom), CSPFT_FLOAT},
    {"missileConeSoundPitchTopSize", offsetof(WeaponFullDef, weapDef.missileConeSoundPitchTopSize), CSPFT_FLOAT},
    {"missileConeSoundPitchBottomSize", offsetof(WeaponFullDef, weapDef.missileConeSoundPitchBottomSize), CSPFT_FLOAT},
    {"missileConeSoundCrossfadeEnabled", offsetof(WeaponFullDef, weapDef.missileConeSoundCrossfadeEnabled), CSPFT_BOOL},
    {"missileConeSoundCrossfadeTopSize", offsetof(WeaponFullDef, weapDef.missileConeSoundCrossfadeTopSize), CSPFT_FLOAT},
    {"missileConeSoundCrossfadeBottomSize", offsetof(WeaponFullDef, weapDef.missileConeSoundCrossfadeBottomSize), CSPFT_FLOAT},
};

namespace IW4
{
    class InfoStringFromWeaponConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapTypeNames, std::extent<decltype(szWeapTypeNames)>::value);
                break;

            case WFT_WEAPONCLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClassNames, std::extent<decltype(szWeapClassNames)>::value);
                break;

            case WFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames, std::extent<decltype(szWeapOverlayReticleNames)>::value);
                break;

            case WFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent<decltype(penetrateTypeNames)>::value);
                break;

            case WFT_IMPACT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, impactTypeNames, std::extent<decltype(impactTypeNames)>::value);
                break;

            case WFT_STANCE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames, std::extent<decltype(szWeapStanceNames)>::value);
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames, std::extent<decltype(szProjectileExplosionNames)>::value);
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames, std::extent<decltype(offhandClassNames)>::value);
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames, std::extent<decltype(playerAnimTypeNames)>::value);
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames, std::extent<decltype(activeReticleNames)>::value);
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames, std::extent<decltype(guidedMissileNames)>::value);
                break;

            case WFT_BOUNCE_SOUND:
                {
                    const auto* bounceSound = *reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

                    if (bounceSound && bounceSound->name)
                    {
                        const std::string firstBounceSound(bounceSound->name->soundName);
                        const auto endOfBouncePrefix = firstBounceSound.rfind("_default");
                        assert(endOfBouncePrefix != std::string::npos);

                        if (endOfBouncePrefix != std::string::npos)
                        {
                            m_info_string.SetValueForKey(std::string(field.szName),
                                                         firstBounceSound.substr(0, endOfBouncePrefix));
                        }
                        else
                            m_info_string.SetValueForKey(std::string(field.szName), "");
                    }
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");

                    break;
                }

            case WFT_STICKINESS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, stickinessNames, std::extent<decltype(stickinessNames)>::value);
                break;

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames, std::extent<decltype(overlayInterfaceNames)>::value);
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames, std::extent<decltype(szWeapInventoryTypeNames)>::value);
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent<decltype(szWeapFireTypeNames)>::value);
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames, std::extent<decltype(ammoCounterClipNames)>::value);
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_PICKUP:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames, std::extent<decltype(weapIconRatioNames)>::value);
                break;

            case WFT_HIDETAGS:
                {
                    const auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::hideTags)>::value; i++)
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
                    const auto* values = &keys[std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value];
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value; i++)
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
                    const auto* values = &keys[std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value];
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value; i++)
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

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromWeaponConverter(const WeaponFullDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

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
        assert(sizeof(WeaponFullDef::hideTags) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        memcpy(fullDef->hideTags, weapon->hideTags, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        fullDef->weapCompleteDef.hideTags = fullDef->hideTags;
    }

    if (weapon->szXAnims)
    {
        assert(sizeof(WeaponFullDef::szXAnims) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnims, weapon->szXAnims, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapCompleteDef.szXAnims = fullDef->szXAnims;
    }

    if (fullDef->weapDef.gunXModel)
    {
        assert(sizeof(WeaponFullDef::gunXModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        memcpy(fullDef->gunXModel, fullDef->weapDef.gunXModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        fullDef->weapDef.gunXModel = fullDef->gunXModel;
    }

    if (fullDef->weapDef.szXAnimsRightHanded)
    {
        assert(sizeof(WeaponFullDef::szXAnimsRightHanded) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnimsRightHanded, fullDef->weapDef.szXAnimsRightHanded, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapDef.szXAnimsRightHanded = fullDef->szXAnimsRightHanded;
    }

    if (fullDef->weapDef.szXAnimsLeftHanded)
    {
        assert(sizeof(WeaponFullDef::szXAnimsLeftHanded) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnimsLeftHanded, fullDef->weapDef.szXAnimsLeftHanded, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapDef.szXAnimsLeftHanded = fullDef->szXAnimsLeftHanded;
    }

    if (fullDef->weapDef.notetrackSoundMapKeys)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapKeys) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        memcpy(fullDef->notetrackSoundMapKeys, fullDef->weapDef.notetrackSoundMapKeys, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        fullDef->weapDef.notetrackSoundMapKeys = fullDef->notetrackSoundMapKeys;
    }

    if (fullDef->weapDef.notetrackSoundMapValues)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapValues) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        memcpy(fullDef->notetrackSoundMapValues, fullDef->weapDef.notetrackSoundMapValues, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        fullDef->weapDef.notetrackSoundMapValues = fullDef->notetrackSoundMapValues;
    }

    if (fullDef->weapDef.notetrackRumbleMapKeys)
    {
        assert(sizeof(WeaponFullDef::notetrackRumbleMapKeys) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        memcpy(fullDef->notetrackRumbleMapKeys, fullDef->weapDef.notetrackRumbleMapKeys, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        fullDef->weapDef.notetrackRumbleMapKeys = fullDef->notetrackRumbleMapKeys;
    }

    if (fullDef->weapDef.notetrackRumbleMapValues)
    {
        assert(sizeof(WeaponFullDef::notetrackRumbleMapValues) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        memcpy(fullDef->notetrackRumbleMapValues, fullDef->weapDef.notetrackRumbleMapValues, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        fullDef->weapDef.notetrackRumbleMapValues = fullDef->notetrackRumbleMapValues;
    }

    if (fullDef->weapDef.worldModel)
    {
        assert(sizeof(WeaponFullDef::worldModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        memcpy(fullDef->worldModel, fullDef->weapDef.worldModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        fullDef->weapDef.worldModel = fullDef->worldModel;
    }

    if (fullDef->weapDef.parallelBounce)
    {
        assert(sizeof(WeaponFullDef::parallelBounce) >= sizeof(float) * std::extent<decltype(WeaponFullDef::parallelBounce)>::value);
        memcpy(fullDef->parallelBounce, fullDef->weapDef.parallelBounce, sizeof(float) * std::extent<decltype(WeaponFullDef::parallelBounce)>::value);
        fullDef->weapDef.parallelBounce = fullDef->parallelBounce;
    }

    if (fullDef->weapDef.perpendicularBounce)
    {
        assert(sizeof(WeaponFullDef::perpendicularBounce) >= sizeof(float) * std::extent<decltype(WeaponFullDef::perpendicularBounce)>::value);
        memcpy(fullDef->perpendicularBounce, fullDef->weapDef.perpendicularBounce, sizeof(float) * std::extent<decltype(WeaponFullDef::perpendicularBounce)>::value);
        fullDef->weapDef.perpendicularBounce = fullDef->perpendicularBounce;
    }

    if (fullDef->weapDef.locationDamageMultipliers)
    {
        assert(sizeof(WeaponFullDef::locationDamageMultipliers) >= sizeof(float) * std::extent<decltype(WeaponFullDef::locationDamageMultipliers)>::value);
        memcpy(fullDef->locationDamageMultipliers, fullDef->weapDef.locationDamageMultipliers, sizeof(float) * std::extent<decltype(WeaponFullDef::locationDamageMultipliers)>::value);
        fullDef->weapDef.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }
}

InfoString AssetDumperWeapon::CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset)
{
    const auto fullDef = std::make_unique<WeaponFullDef>();
    memset(fullDef.get(), 0, sizeof(WeaponFullDef));
    CopyToFullDef(asset->Asset(), fullDef.get());

    InfoStringFromWeaponConverter converter(fullDef.get(), weapon_fields, std::extent<decltype(weapon_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset->m_zone->m_script_strings.Count());
            if (scrStr >= asset->m_zone->m_script_strings.Count())
                return "";

            return asset->m_zone->m_script_strings[scrStr];
        });

    return converter.Convert();
}

bool AssetDumperWeapon::ShouldDump(XAssetInfo<WeaponCompleteDef>* asset)
{
    return true;
}

bool AssetDumperWeapon::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperWeapon::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperWeapon::GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponCompleteDef>* asset)
{
    return "weapons/" + asset->m_name;
}

GdtEntry AssetDumperWeapon::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperWeapon::DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}
