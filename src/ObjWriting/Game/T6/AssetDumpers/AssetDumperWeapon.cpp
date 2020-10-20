#include "AssetDumperWeapon.h"

#include <cassert>

#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperWeapon::weapon_fields[]
{
    {"displayName", offsetof(WeaponFullDef, weapVariantDef.szDisplayName), CSPFT_STRING},
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
    {"idleAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_IDLE]), CSPFT_STRING},
    {"idleAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_IDLE_LEFT]), CSPFT_STRING},
    {"emptyIdleAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_IDLE]), CSPFT_STRING},
    {"emptyIdleAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_IDLE_LEFT]), CSPFT_STRING},
    {"fireIntroAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRE_INTRO]), CSPFT_STRING},
    {"fireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRE]), CSPFT_STRING},
    {"fireAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRE_LEFT]), CSPFT_STRING},
    {"holdFireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_HOLD_FIRE]), CSPFT_STRING},
    {"lastShotAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LASTSHOT]), CSPFT_STRING},
    {"lastShotAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LASTSHOT_LEFT]), CSPFT_STRING},
    {"flourishAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FINALSHOT]), CSPFT_STRING},
    {"flourishAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FINALSHOT_LEFT]), CSPFT_STRING},
    {"detonateAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DETONATE]), CSPFT_STRING},
    {"rechamberAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RECHAMBER]), CSPFT_STRING},
    {"meleeAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE]), CSPFT_STRING},
    {"meleeAnimEmpty", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE_EMPTY]), CSPFT_STRING},
    {"meleeAnim1", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE1]), CSPFT_STRING},
    {"meleeAnim2", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE2]), CSPFT_STRING},
    {"meleeAnim3", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE3]), CSPFT_STRING},
    {"meleeChargeAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE_CHARGE]), CSPFT_STRING},
    {"meleeChargeAnimEmpty", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MELEE_CHARGE_EMPTY]), CSPFT_STRING},
    {"reloadAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD]), CSPFT_STRING},
    {"reloadAnimRight", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_RIGHT]), CSPFT_STRING},
    {"reloadAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_LEFT]), CSPFT_STRING},
    {"reloadEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_EMPTY]), CSPFT_STRING},
    {"reloadEmptyAnimLeft", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_EMPTY_LEFT]), CSPFT_STRING},
    {"reloadStartAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_START]), CSPFT_STRING},
    {"reloadEndAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_END]), CSPFT_STRING},
    {"reloadQuickAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_QUICK]), CSPFT_STRING},
    {"reloadQuickEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RELOAD_QUICK_EMPTY]), CSPFT_STRING},
    {"raiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_RAISE]), CSPFT_STRING},
    {"dropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DROP]), CSPFT_STRING},
    {"firstRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_FIRST_RAISE]), CSPFT_STRING},
    {"altRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ALT_RAISE]), CSPFT_STRING},
    {"altDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ALT_DROP]), CSPFT_STRING},
    {"quickRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_QUICK_RAISE]), CSPFT_STRING},
    {"quickDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_QUICK_DROP]), CSPFT_STRING},
    {"emptyRaiseAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_RAISE]), CSPFT_STRING},
    {"emptyDropAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_EMPTY_DROP]), CSPFT_STRING},
    {"sprintInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_IN]), CSPFT_STRING},
    {"sprintLoopAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_LOOP]), CSPFT_STRING},
    {"sprintOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_OUT]), CSPFT_STRING},
    {"sprintInEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_EMPTY_IN]), CSPFT_STRING},
    {"sprintLoopEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_EMPTY_LOOP]), CSPFT_STRING},
    {"sprintOutEmptyAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SPRINT_EMPTY_OUT]), CSPFT_STRING},
    {"lowReadyInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LOWREADY_IN]), CSPFT_STRING},
    {"lowReadyLoopAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LOWREADY_LOOP]), CSPFT_STRING},
    {"lowReadyOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_LOWREADY_OUT]), CSPFT_STRING},
    {"contFireInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CONT_FIRE_IN]), CSPFT_STRING},
    {"contFireLoopAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CONT_FIRE_LOOP]), CSPFT_STRING},
    {"contFireOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CONT_FIRE_OUT]), CSPFT_STRING},
    {"crawlInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_IN]), CSPFT_STRING},
    {"crawlForwardAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_FORWARD]), CSPFT_STRING},
    {"crawlBackAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_BACK]), CSPFT_STRING},
    {"crawlRightAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_RIGHT]), CSPFT_STRING},
    {"crawlLeftAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_LEFT]), CSPFT_STRING},
    {"crawlOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_OUT]), CSPFT_STRING},
    {"crawlEmptyInAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_IN]), CSPFT_STRING},
    {"crawlEmptyForwardAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_FORWARD]), CSPFT_STRING},
    {"crawlEmptyBackAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_BACK]), CSPFT_STRING},
    {"crawlEmptyRightAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_RIGHT]), CSPFT_STRING},
    {"crawlEmptyLeftAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_LEFT]), CSPFT_STRING},
    {"crawlEmptyOutAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CRAWL_EMPTY_OUT]), CSPFT_STRING},
    {"deployAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DEPLOY]), CSPFT_STRING},
    {"nightVisionWearAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_NIGHTVISION_WEAR]), CSPFT_STRING},
    {"nightVisionRemoveAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_NIGHTVISION_REMOVE]), CSPFT_STRING},
    {"adsFireAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_FIRE]), CSPFT_STRING},
    {"adsLastShotAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_LASTSHOT]), CSPFT_STRING},
    {"adsRechamberAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_RECHAMBER]), CSPFT_STRING},
    {"adsUpAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_UP]), CSPFT_STRING},
    {"adsDownAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_DOWN]), CSPFT_STRING},
    {"adsUpOtherScopeAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_UP_OTHER_SCOPE]), CSPFT_STRING},
    {"adsFireIntroAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_ADS_FIRE_INTRO]), CSPFT_STRING},
    {"breakdownAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_BREAKDOWN]), CSPFT_STRING},
    {"dtp_in", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_IN]), CSPFT_STRING},
    {"dtp_loop", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_LOOP]), CSPFT_STRING},
    {"dtp_out", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_OUT]), CSPFT_STRING},
    {"dtp_empty_in", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_EMPTY_IN]), CSPFT_STRING},
    {"dtp_empty_loop", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_EMPTY_LOOP]), CSPFT_STRING},
    {"dtp_empty_out", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_DTP_EMPTY_OUT]), CSPFT_STRING},
    {"slide_in", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_SLIDE_IN]), CSPFT_STRING},
    {"mantleAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_MANTLE]), CSPFT_STRING},
    {"sprintCameraAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CAMERA_SPRINT_LOOP]), CSPFT_STRING},
    {"dtpInCameraAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CAMERA_DTP_IN]), CSPFT_STRING},
    {"dtpLoopCameraAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CAMERA_DTP_LOOP]), CSPFT_STRING},
    {"dtpOutCameraAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CAMERA_DTP_OUT]), CSPFT_STRING},
    {"mantleCameraAnim", offsetof(WeaponFullDef, szXAnims[WEAP_ANIM_CAMERA_MANTLE]), CSPFT_STRING},
    {"script", offsetof(WeaponFullDef, weapDef.szScript), CSPFT_STRING},
    {"weaponType", offsetof(WeaponFullDef, weapDef.weapType), WFT_WEAPONTYPE},
    {"weaponClass", offsetof(WeaponFullDef, weapDef.weapClass), WFT_WEAPONCLASS},
    {"penetrateType", offsetof(WeaponFullDef, weapDef.penetrateTWeaponAttachmentype), WFT_PENETRATE_TYPE},
    {"impactType", offsetof(WeaponFullDef, weapDef.impactType), WFT_IMPACT_TYPE},
    {"inventoryType", offsetof(WeaponFullDef, weapDef.inventoryType), WFT_INVENTORYTYPE},
    {"fireType", offsetof(WeaponFullDef, weapDef.fireType), WFT_FIRETYPE},
    {"clipType", offsetof(WeaponFullDef, weapDef.clipType), WFT_CLIPTYPE},
    {"barrelType", offsetof(WeaponFullDef, weapDef.barrelType), WFT_BARRELTYPE},
    {"offhandClass", offsetof(WeaponFullDef, weapDef.offhandClass), WFT_OFFHAND_CLASS},
    {"offhandSlot", offsetof(WeaponFullDef, weapDef.offhandSlot), WFT_OFFHAND_SLOT},
    {"viewFlashEffect", offsetof(WeaponFullDef, weapDef.viewFlashEffect), CSPFT_FX},
    {"worldFlashEffect", offsetof(WeaponFullDef, weapDef.worldFlashEffect), CSPFT_FX},
    {"barrelCooldownEffect", offsetof(WeaponFullDef, weapDef.barrelCooldownEffect), CSPFT_FX},
    {"barrelCooldownMinCount", offsetof(WeaponFullDef, weapDef.barrelCooldownMinCount), CSPFT_INT},
    {"viewFlashOffsetF", offsetof(WeaponFullDef, weapDef.vViewFlashOffset.x), CSPFT_FLOAT},
    {"viewFlashOffsetR", offsetof(WeaponFullDef, weapDef.vViewFlashOffset.y), CSPFT_FLOAT},
    {"viewFlashOffsetU", offsetof(WeaponFullDef, weapDef.vViewFlashOffset.z), CSPFT_FLOAT},
    {"worldFlashOffsetF", offsetof(WeaponFullDef, weapDef.vWorldFlashOffset.x), CSPFT_FLOAT},
    {"worldFlashOffsetR", offsetof(WeaponFullDef, weapDef.vWorldFlashOffset.y), CSPFT_FLOAT},
    {"worldFlashOffsetU", offsetof(WeaponFullDef, weapDef.vWorldFlashOffset.z), CSPFT_FLOAT},
    {"pickupSound", offsetof(WeaponFullDef, weapDef.pickupSound), CSPFT_STRING},
    {"pickupSoundPlayer", offsetof(WeaponFullDef, weapDef.pickupSoundPlayer), CSPFT_STRING},
    {"ammoPickupSound", offsetof(WeaponFullDef, weapDef.ammoPickupSound), CSPFT_STRING},
    {"ammoPickupSoundPlayer", offsetof(WeaponFullDef, weapDef.ammoPickupSoundPlayer), CSPFT_STRING},
    {"projectileSound", offsetof(WeaponFullDef, weapDef.projectileSound), CSPFT_STRING},
    {"pullbackSound", offsetof(WeaponFullDef, weapDef.pullbackSound), CSPFT_STRING},
    {"pullbackSoundPlayer", offsetof(WeaponFullDef, weapDef.pullbackSoundPlayer), CSPFT_STRING},
    {"fireSound", offsetof(WeaponFullDef, weapDef.fireSound), CSPFT_STRING},
    {"crackSound", offsetof(WeaponFullDef, weapDef.crackSound), CSPFT_STRING},
    {"whizbySound", offsetof(WeaponFullDef, weapDef.whizbySound), CSPFT_STRING},
    {"fireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireSoundPlayer), CSPFT_STRING},
    {"loopFireSound", offsetof(WeaponFullDef, weapDef.fireLoopSound), CSPFT_STRING},
    {"loopFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireLoopSoundPlayer), CSPFT_STRING},
    {"loopFireEndSound", offsetof(WeaponFullDef, weapDef.fireLoopEndSound), CSPFT_STRING},
    {"loopFireEndSoundPlayer", offsetof(WeaponFullDef, weapDef.fireLoopEndSoundPlayer), CSPFT_STRING},
    {"startFireSound", offsetof(WeaponFullDef, weapDef.fireStartSound), CSPFT_STRING},
    {"stopFireSound", offsetof(WeaponFullDef, weapDef.fireStopSound), CSPFT_STRING},
    {"killcamStartFireSound", offsetof(WeaponFullDef, weapDef.fireKillcamSound), CSPFT_STRING},
    {"startFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireStartSoundPlayer), CSPFT_STRING},
    {"stopFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireStopSoundPlayer), CSPFT_STRING},
    {"killcamStartFireSoundPlayer", offsetof(WeaponFullDef, weapDef.fireKillcamSoundPlayer), CSPFT_STRING},
    {"lastShotSound", offsetof(WeaponFullDef, weapDef.fireLastSound), CSPFT_STRING},
    {"lastShotSoundPlayer", offsetof(WeaponFullDef, weapDef.fireLastSoundPlayer), CSPFT_STRING},
    {"emptyFireSound", offsetof(WeaponFullDef, weapDef.emptyFireSound), CSPFT_STRING},
    {"emptyFireSoundPlayer", offsetof(WeaponFullDef, weapDef.emptyFireSoundPlayer), CSPFT_STRING},
    {"meleeSwipeSound", offsetof(WeaponFullDef, weapDef.meleeSwipeSound), CSPFT_STRING},
    {"meleeSwipeSoundPlayer", offsetof(WeaponFullDef, weapDef.meleeSwipeSoundPlayer), CSPFT_STRING},
    {"meleeHitSound", offsetof(WeaponFullDef, weapDef.meleeHitSound), CSPFT_STRING},
    {"meleeMissSound", offsetof(WeaponFullDef, weapDef.meleeMissSound), CSPFT_STRING},
    {"rechamberSound", offsetof(WeaponFullDef, weapDef.rechamberSound), CSPFT_STRING},
    {"rechamberSoundPlayer", offsetof(WeaponFullDef, weapDef.rechamberSoundPlayer), CSPFT_STRING},
    {"reloadSound", offsetof(WeaponFullDef, weapDef.reloadSound), CSPFT_STRING},
    {"reloadSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadSoundPlayer), CSPFT_STRING},
    {"reloadEmptySound", offsetof(WeaponFullDef, weapDef.reloadEmptySound), CSPFT_STRING},
    {"reloadEmptySoundPlayer", offsetof(WeaponFullDef, weapDef.reloadEmptySoundPlayer), CSPFT_STRING},
    {"reloadStartSound", offsetof(WeaponFullDef, weapDef.reloadStartSound), CSPFT_STRING},
    {"reloadStartSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadStartSoundPlayer), CSPFT_STRING},
    {"reloadEndSound", offsetof(WeaponFullDef, weapDef.reloadEndSound), CSPFT_STRING},
    {"reloadEndSoundPlayer", offsetof(WeaponFullDef, weapDef.reloadEndSoundPlayer), CSPFT_STRING},
    {"rotateLoopSound", offsetof(WeaponFullDef, weapDef.rotateLoopSound), CSPFT_STRING},
    {"rotateLoopSoundPlayer", offsetof(WeaponFullDef, weapDef.rotateLoopSoundPlayer), CSPFT_STRING},
    {"rotateStopSound", offsetof(WeaponFullDef, weapDef.rotateStopSound), CSPFT_STRING},
    {"rotateStopSoundPlayer", offsetof(WeaponFullDef, weapDef.rotateStopSoundPlayer), CSPFT_STRING},
    {"deploySound", offsetof(WeaponFullDef, weapDef.deploySound), CSPFT_STRING},
    {"deploySoundPlayer", offsetof(WeaponFullDef, weapDef.deploySoundPlayer), CSPFT_STRING},
    {"finishDeploySound", offsetof(WeaponFullDef, weapDef.finishDeploySound), CSPFT_STRING},
    {"finishDeploySoundPlayer", offsetof(WeaponFullDef, weapDef.finishDeploySoundPlayer), CSPFT_STRING},
    {"breakdownSound", offsetof(WeaponFullDef, weapDef.breakdownSound), CSPFT_STRING},
    {"breakdownSoundPlayer", offsetof(WeaponFullDef, weapDef.breakdownSoundPlayer), CSPFT_STRING},
    {"finishBreakdownSound", offsetof(WeaponFullDef, weapDef.finishBreakdownSound), CSPFT_STRING},
    {"finishBreakdownSoundPlayer", offsetof(WeaponFullDef, weapDef.finishBreakdownSoundPlayer), CSPFT_STRING},
    {"detonateSound", offsetof(WeaponFullDef, weapDef.detonateSound), CSPFT_STRING},
    {"detonateSoundPlayer", offsetof(WeaponFullDef, weapDef.detonateSoundPlayer), CSPFT_STRING},
    {"nightVisionWearSound", offsetof(WeaponFullDef, weapDef.nightVisionWearSound), CSPFT_STRING},
    {"nightVisionWearSoundPlayer", offsetof(WeaponFullDef, weapDef.nightVisionWearSoundPlayer), CSPFT_STRING},
    {"nightVisionRemoveSound", offsetof(WeaponFullDef, weapDef.nightVisionRemoveSound), CSPFT_STRING},
    {"nightVisionRemoveSoundPlayer", offsetof(WeaponFullDef, weapDef.nightVisionRemoveSoundPlayer), CSPFT_STRING},
    {"raiseSound", offsetof(WeaponFullDef, weapDef.raiseSound), CSPFT_STRING},
    {"raiseSoundPlayer", offsetof(WeaponFullDef, weapDef.raiseSoundPlayer), CSPFT_STRING},
    {"firstRaiseSound", offsetof(WeaponFullDef, weapDef.firstRaiseSound), CSPFT_STRING},
    {"firstRaiseSoundPlayer", offsetof(WeaponFullDef, weapDef.firstRaiseSoundPlayer), CSPFT_STRING},
    {"altSwitchSound", offsetof(WeaponFullDef, weapDef.altSwitchSound), CSPFT_STRING},
    {"altSwitchSoundPlayer", offsetof(WeaponFullDef, weapDef.altSwitchSoundPlayer), CSPFT_STRING},
    {"adsRaiseSoundPlayer", offsetof(WeaponFullDef, weapDef.adsRaiseSoundPlayer), CSPFT_STRING},
    {"adsLowerSoundPlayer", offsetof(WeaponFullDef, weapDef.adsLowerSoundPlayer), CSPFT_STRING},
    {"putawaySound", offsetof(WeaponFullDef, weapDef.putawaySound), CSPFT_STRING},
    {"putawaySoundPlayer", offsetof(WeaponFullDef, weapDef.putawaySoundPlayer), CSPFT_STRING},
    {"overheatSound", offsetof(WeaponFullDef, weapDef.overheatSound), CSPFT_STRING},
    {"overheatSoundPlayer", offsetof(WeaponFullDef, weapDef.overheatSoundPlayer), CSPFT_STRING},
    {"adsZoomSound", offsetof(WeaponFullDef, weapDef.adsZoomSound), CSPFT_STRING},
    {"shellCasing", offsetof(WeaponFullDef, weapDef.shellCasing), CSPFT_STRING},
    {"shellCasingPlayer", offsetof(WeaponFullDef, weapDef.shellCasingPlayer), CSPFT_STRING},
    {"bounceSound", offsetof(WeaponFullDef, weapDef.bounceSound), WFT_BOUNCE_SOUND},
    {"standMountedWeapdef", offsetof(WeaponFullDef, weapDef.standMountedWeapdef), CSPFT_STRING},
    {"crouchMountedWeapdef", offsetof(WeaponFullDef, weapDef.crouchMountedWeapdef), CSPFT_STRING},
    {"proneMountedWeapdef", offsetof(WeaponFullDef, weapDef.proneMountedWeapdef), CSPFT_STRING},
    {"viewShellEjectEffect", offsetof(WeaponFullDef, weapDef.viewShellEjectEffect), CSPFT_FX},
    {"worldShellEjectEffect", offsetof(WeaponFullDef, weapDef.worldShellEjectEffect), CSPFT_FX},
    {"viewLastShotEjectEffect", offsetof(WeaponFullDef, weapDef.viewLastShotEjectEffect), CSPFT_FX},
    {"worldLastShotEjectEffect", offsetof(WeaponFullDef, weapDef.worldLastShotEjectEffect), CSPFT_FX},
    {"viewShellEjectOffsetF", offsetof(WeaponFullDef, weapDef.vViewShellEjectOffset.x), CSPFT_FLOAT},
    {"viewShellEjectOffsetR", offsetof(WeaponFullDef, weapDef.vViewShellEjectOffset.y), CSPFT_FLOAT},
    {"viewShellEjectOffsetU", offsetof(WeaponFullDef, weapDef.vViewShellEjectOffset.z), CSPFT_FLOAT},
    {"worldShellEjectOffsetF", offsetof(WeaponFullDef, weapDef.vWorldShellEjectOffset.x), CSPFT_FLOAT},
    {"worldShellEjectOffsetR", offsetof(WeaponFullDef, weapDef.vWorldShellEjectOffset.y), CSPFT_FLOAT},
    {"worldShellEjectOffsetU", offsetof(WeaponFullDef, weapDef.vWorldShellEjectOffset.z), CSPFT_FLOAT},
    {"viewShellEjectRotationP", offsetof(WeaponFullDef, weapDef.vViewShellEjectRotation.x), CSPFT_FLOAT},
    {"viewShellEjectRotationY", offsetof(WeaponFullDef, weapDef.vViewShellEjectRotation.y), CSPFT_FLOAT},
    {"viewShellEjectRotationR", offsetof(WeaponFullDef, weapDef.vViewShellEjectRotation.z), CSPFT_FLOAT},
    {"worldShellEjectRotationP", offsetof(WeaponFullDef, weapDef.vWorldShellEjectRotation.x), CSPFT_FLOAT},
    {"worldShellEjectRotationY", offsetof(WeaponFullDef, weapDef.vWorldShellEjectRotation.y), CSPFT_FLOAT},
    {"worldShellEjectRotationR", offsetof(WeaponFullDef, weapDef.vWorldShellEjectRotation.z), CSPFT_FLOAT},
    {"reticleCenter", offsetof(WeaponFullDef, weapDef.reticleCenter), CSPFT_MATERIAL},
    {"reticleSide", offsetof(WeaponFullDef, weapDef.reticleSide), CSPFT_MATERIAL},
    {"reticleCenterSize", offsetof(WeaponFullDef, weapDef.iReticleCenterSize), CSPFT_INT},
    {"reticleSideSize", offsetof(WeaponFullDef, weapDef.iReticleSideSize), CSPFT_INT},
    {"reticleMinOfs", offsetof(WeaponFullDef, weapDef.iReticleMinOfs), CSPFT_INT},
    {"activeReticleType", offsetof(WeaponFullDef, weapDef.activeReticleType), WFT_ACTIVE_RETICLE_TYPE},
    {"standMoveF", offsetof(WeaponFullDef, weapDef.vStandMove.x), CSPFT_FLOAT},
    {"standMoveR", offsetof(WeaponFullDef, weapDef.vStandMove.y), CSPFT_FLOAT},
    {"standMoveU", offsetof(WeaponFullDef, weapDef.vStandMove.z), CSPFT_FLOAT},
    {"standRotP", offsetof(WeaponFullDef, weapDef.vStandRot.x), CSPFT_FLOAT},
    {"standRotY", offsetof(WeaponFullDef, weapDef.vStandRot.y), CSPFT_FLOAT},
    {"standRotR", offsetof(WeaponFullDef, weapDef.vStandRot.z), CSPFT_FLOAT},
    {"duckedOfsF", offsetof(WeaponFullDef, weapDef.vDuckedOfs.x), CSPFT_FLOAT},
    {"duckedOfsR", offsetof(WeaponFullDef, weapDef.vDuckedOfs.y), CSPFT_FLOAT},
    {"duckedOfsU", offsetof(WeaponFullDef, weapDef.vDuckedOfs.z), CSPFT_FLOAT},
    {"duckedMoveF", offsetof(WeaponFullDef, weapDef.vDuckedMove.x), CSPFT_FLOAT},
    {"duckedMoveR", offsetof(WeaponFullDef, weapDef.vDuckedMove.y), CSPFT_FLOAT},
    {"duckedMoveU", offsetof(WeaponFullDef, weapDef.vDuckedMove.z), CSPFT_FLOAT},
    {"duckedSprintOfsF", offsetof(WeaponFullDef, weapDef.vDuckedSprintOfs.x), CSPFT_FLOAT},
    {"duckedSprintOfsR", offsetof(WeaponFullDef, weapDef.vDuckedSprintOfs.y), CSPFT_FLOAT},
    {"duckedSprintOfsU", offsetof(WeaponFullDef, weapDef.vDuckedSprintOfs.z), CSPFT_FLOAT},
    {"duckedSprintRotP", offsetof(WeaponFullDef, weapDef.vDuckedSprintRot.x), CSPFT_FLOAT},
    {"duckedSprintRotY", offsetof(WeaponFullDef, weapDef.vDuckedSprintRot.y), CSPFT_FLOAT},
    {"duckedSprintRotR", offsetof(WeaponFullDef, weapDef.vDuckedSprintRot.z), CSPFT_FLOAT},
    {"duckedSprintBobH", offsetof(WeaponFullDef, weapDef.vDuckedSprintBob.x), CSPFT_FLOAT},
    {"duckedSprintBobV", offsetof(WeaponFullDef, weapDef.vDuckedSprintBob.y), CSPFT_FLOAT},
    {"duckedSprintScale", offsetof(WeaponFullDef, weapDef.fDuckedSprintCycleScale), CSPFT_FLOAT},
    {"sprintOfsF", offsetof(WeaponFullDef, weapDef.vSprintOfs.x), CSPFT_FLOAT},
    {"sprintOfsR", offsetof(WeaponFullDef, weapDef.vSprintOfs.y), CSPFT_FLOAT},
    {"sprintOfsU", offsetof(WeaponFullDef, weapDef.vSprintOfs.z), CSPFT_FLOAT},
    {"sprintRotP", offsetof(WeaponFullDef, weapDef.vSprintRot.x), CSPFT_FLOAT},
    {"sprintRotY", offsetof(WeaponFullDef, weapDef.vSprintRot.y), CSPFT_FLOAT},
    {"sprintRotR", offsetof(WeaponFullDef, weapDef.vSprintRot.z), CSPFT_FLOAT},
    {"sprintBobH", offsetof(WeaponFullDef, weapDef.vSprintBob.x), CSPFT_FLOAT},
    {"sprintBobV", offsetof(WeaponFullDef, weapDef.vSprintBob.y), CSPFT_FLOAT},
    {"sprintScale", offsetof(WeaponFullDef, weapDef.fSprintCycleScale), CSPFT_FLOAT},
    {"lowReadyOfsF", offsetof(WeaponFullDef, weapDef.vLowReadyOfs.x), CSPFT_FLOAT},
    {"lowReadyOfsR", offsetof(WeaponFullDef, weapDef.vLowReadyOfs.y), CSPFT_FLOAT},
    {"lowReadyOfsU", offsetof(WeaponFullDef, weapDef.vLowReadyOfs.z), CSPFT_FLOAT},
    {"lowReadyRotP", offsetof(WeaponFullDef, weapDef.vLowReadyRot.x), CSPFT_FLOAT},
    {"lowReadyRotY", offsetof(WeaponFullDef, weapDef.vLowReadyRot.y), CSPFT_FLOAT},
    {"lowReadyRotR", offsetof(WeaponFullDef, weapDef.vLowReadyRot.z), CSPFT_FLOAT},
    {"rideOfsF", offsetof(WeaponFullDef, weapDef.vRideOfs.x), CSPFT_FLOAT},
    {"rideOfsR", offsetof(WeaponFullDef, weapDef.vRideOfs.y), CSPFT_FLOAT},
    {"rideOfsU", offsetof(WeaponFullDef, weapDef.vRideOfs.z), CSPFT_FLOAT},
    {"rideRotP", offsetof(WeaponFullDef, weapDef.vRideRot.x), CSPFT_FLOAT},
    {"rideRotY", offsetof(WeaponFullDef, weapDef.vRideRot.y), CSPFT_FLOAT},
    {"rideRotR", offsetof(WeaponFullDef, weapDef.vRideRot.z), CSPFT_FLOAT},
    {"dtpOfsF", offsetof(WeaponFullDef, weapDef.vDtpOfs.x), CSPFT_FLOAT},
    {"dtpOfsR", offsetof(WeaponFullDef, weapDef.vDtpOfs.y), CSPFT_FLOAT},
    {"dtpOfsU", offsetof(WeaponFullDef, weapDef.vDtpOfs.z), CSPFT_FLOAT},
    {"dtpRotP", offsetof(WeaponFullDef, weapDef.vDtpRot.x), CSPFT_FLOAT},
    {"dtpRotY", offsetof(WeaponFullDef, weapDef.vDtpRot.y), CSPFT_FLOAT},
    {"dtpRotR", offsetof(WeaponFullDef, weapDef.vDtpRot.z), CSPFT_FLOAT},
    {"dtpBobH", offsetof(WeaponFullDef, weapDef.vDtpBob.x), CSPFT_FLOAT},
    {"dtpBobV", offsetof(WeaponFullDef, weapDef.vDtpBob.y), CSPFT_FLOAT},
    {"dtpScale", offsetof(WeaponFullDef, weapDef.fDtpCycleScale), CSPFT_FLOAT},
    {"mantleOfsF", offsetof(WeaponFullDef, weapDef.vMantleOfs.x), CSPFT_FLOAT},
    {"mantleOfsR", offsetof(WeaponFullDef, weapDef.vMantleOfs.y), CSPFT_FLOAT},
    {"mantleOfsU", offsetof(WeaponFullDef, weapDef.vMantleOfs.z), CSPFT_FLOAT},
    {"mantleRotP", offsetof(WeaponFullDef, weapDef.vMantleRot.x), CSPFT_FLOAT},
    {"mantleRotY", offsetof(WeaponFullDef, weapDef.vMantleRot.y), CSPFT_FLOAT},
    {"mantleRotR", offsetof(WeaponFullDef, weapDef.vMantleRot.z), CSPFT_FLOAT},
    {"slideOfsF", offsetof(WeaponFullDef, weapDef.vSlideOfs.x), CSPFT_FLOAT},
    {"slideOfsR", offsetof(WeaponFullDef, weapDef.vSlideOfs.y), CSPFT_FLOAT},
    {"slideOfsU", offsetof(WeaponFullDef, weapDef.vSlideOfs.z), CSPFT_FLOAT},
    {"slideRotP", offsetof(WeaponFullDef, weapDef.vSlideRot.x), CSPFT_FLOAT},
    {"slideRotY", offsetof(WeaponFullDef, weapDef.vSlideRot.y), CSPFT_FLOAT},
    {"slideRotR", offsetof(WeaponFullDef, weapDef.vSlideRot.z), CSPFT_FLOAT},
    {"duckedRotP", offsetof(WeaponFullDef, weapDef.vDuckedRot.x), CSPFT_FLOAT},
    {"duckedRotY", offsetof(WeaponFullDef, weapDef.vDuckedRot.y), CSPFT_FLOAT},
    {"duckedRotR", offsetof(WeaponFullDef, weapDef.vDuckedRot.z), CSPFT_FLOAT},
    {"proneOfsF", offsetof(WeaponFullDef, weapDef.vProneOfs.x), CSPFT_FLOAT},
    {"proneOfsR", offsetof(WeaponFullDef, weapDef.vProneOfs.y), CSPFT_FLOAT},
    {"proneOfsU", offsetof(WeaponFullDef, weapDef.vProneOfs.z), CSPFT_FLOAT},
    {"proneMoveF", offsetof(WeaponFullDef, weapDef.vProneMove.x), CSPFT_FLOAT},
    {"proneMoveR", offsetof(WeaponFullDef, weapDef.vProneMove.y), CSPFT_FLOAT},
    {"proneMoveU", offsetof(WeaponFullDef, weapDef.vProneMove.z), CSPFT_FLOAT},
    {"proneRotP", offsetof(WeaponFullDef, weapDef.vProneRot.x), CSPFT_FLOAT},
    {"proneRotY", offsetof(WeaponFullDef, weapDef.vProneRot.y), CSPFT_FLOAT},
    {"proneRotR", offsetof(WeaponFullDef, weapDef.vProneRot.z), CSPFT_FLOAT},
    {"strafeMoveF", offsetof(WeaponFullDef, weapDef.vStrafeMove.x), CSPFT_FLOAT},
    {"strafeMoveR", offsetof(WeaponFullDef, weapDef.vStrafeMove.y), CSPFT_FLOAT},
    {"strafeMoveU", offsetof(WeaponFullDef, weapDef.vStrafeMove.z), CSPFT_FLOAT},
    {"strafeRotP", offsetof(WeaponFullDef, weapDef.vStrafeRot.x), CSPFT_FLOAT},
    {"strafeRotY", offsetof(WeaponFullDef, weapDef.vStrafeRot.y), CSPFT_FLOAT},
    {"strafeRotR", offsetof(WeaponFullDef, weapDef.vStrafeRot.z), CSPFT_FLOAT},
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
    {"attachViewModel1", offsetof(WeaponFullDef, attachViewModel[0]), CSPFT_XMODEL},
    {"attachViewModel2", offsetof(WeaponFullDef, attachViewModel[1]), CSPFT_XMODEL},
    {"attachViewModel3", offsetof(WeaponFullDef, attachViewModel[2]), CSPFT_XMODEL},
    {"attachViewModel4", offsetof(WeaponFullDef, attachViewModel[3]), CSPFT_XMODEL},
    {"attachViewModel5", offsetof(WeaponFullDef, attachViewModel[4]), CSPFT_XMODEL},
    {"attachViewModel6", offsetof(WeaponFullDef, attachViewModel[5]), CSPFT_XMODEL},
    {"attachViewModel7", offsetof(WeaponFullDef, attachViewModel[6]), CSPFT_XMODEL},
    {"attachViewModel8", offsetof(WeaponFullDef, attachViewModel[7]), CSPFT_XMODEL},
    {"attachWorldModel1", offsetof(WeaponFullDef, attachWorldModel[0]), CSPFT_XMODEL},
    {"attachWorldModel2", offsetof(WeaponFullDef, attachWorldModel[1]), CSPFT_XMODEL},
    {"attachWorldModel3", offsetof(WeaponFullDef, attachWorldModel[2]), CSPFT_XMODEL},
    {"attachWorldModel4", offsetof(WeaponFullDef, attachWorldModel[3]), CSPFT_XMODEL},
    {"attachWorldModel5", offsetof(WeaponFullDef, attachWorldModel[4]), CSPFT_XMODEL},
    {"attachWorldModel6", offsetof(WeaponFullDef, attachWorldModel[5]), CSPFT_XMODEL},
    {"attachWorldModel7", offsetof(WeaponFullDef, attachWorldModel[6]), CSPFT_XMODEL},
    {"attachWorldModel8", offsetof(WeaponFullDef, attachWorldModel[7]), CSPFT_XMODEL},
    {"attachViewModelTag1", offsetof(WeaponFullDef, attachViewModelTag[0]), CSPFT_STRING},
    {"attachViewModelTag2", offsetof(WeaponFullDef, attachViewModelTag[1]), CSPFT_STRING},
    {"attachViewModelTag3", offsetof(WeaponFullDef, attachViewModelTag[2]), CSPFT_STRING},
    {"attachViewModelTag4", offsetof(WeaponFullDef, attachViewModelTag[3]), CSPFT_STRING},
    {"attachViewModelTag5", offsetof(WeaponFullDef, attachViewModelTag[4]), CSPFT_STRING},
    {"attachViewModelTag6", offsetof(WeaponFullDef, attachViewModelTag[5]), CSPFT_STRING},
    {"attachViewModelTag7", offsetof(WeaponFullDef, attachViewModelTag[6]), CSPFT_STRING},
    {"attachViewModelTag8", offsetof(WeaponFullDef, attachViewModelTag[7]), CSPFT_STRING},
    {"attachWorldModelTag1", offsetof(WeaponFullDef, attachWorldModelTag[0]), CSPFT_STRING},
    {"attachWorldModelTag2", offsetof(WeaponFullDef, attachWorldModelTag[1]), CSPFT_STRING},
    {"attachWorldModelTag3", offsetof(WeaponFullDef, attachWorldModelTag[2]), CSPFT_STRING},
    {"attachWorldModelTag4", offsetof(WeaponFullDef, attachWorldModelTag[3]), CSPFT_STRING},
    {"attachWorldModelTag5", offsetof(WeaponFullDef, attachWorldModelTag[4]), CSPFT_STRING},
    {"attachWorldModelTag6", offsetof(WeaponFullDef, attachWorldModelTag[5]), CSPFT_STRING},
    {"attachWorldModelTag7", offsetof(WeaponFullDef, attachWorldModelTag[6]), CSPFT_STRING},
    {"attachWorldModelTag8", offsetof(WeaponFullDef, attachWorldModelTag[7]), CSPFT_STRING},
    {"attachViewModelOffsetX1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[0]), CSPFT_FLOAT},
    {"attachViewModelOffsetY1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[1]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[2]), CSPFT_FLOAT},
    {"attachViewModelOffsetX2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[3]), CSPFT_FLOAT},
    {"attachViewModelOffsetY2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[4]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[5]), CSPFT_FLOAT},
    {"attachViewModelOffsetX3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[6]), CSPFT_FLOAT},
    {"attachViewModelOffsetY3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[7]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[8]), CSPFT_FLOAT},
    {"attachViewModelOffsetX4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[9]), CSPFT_FLOAT},
    {"attachViewModelOffsetY4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[10]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[11]), CSPFT_FLOAT},
    {"attachViewModelOffsetX5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[12]), CSPFT_FLOAT},
    {"attachViewModelOffsetY5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[13]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[14]), CSPFT_FLOAT},
    {"attachViewModelOffsetX6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[15]), CSPFT_FLOAT},
    {"attachViewModelOffsetY6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[16]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[17]), CSPFT_FLOAT},
    {"attachViewModelOffsetX7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[18]), CSPFT_FLOAT},
    {"attachViewModelOffsetY7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[19]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[20]), CSPFT_FLOAT},
    {"attachViewModelOffsetX8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[21]), CSPFT_FLOAT},
    {"attachViewModelOffsetY8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[22]), CSPFT_FLOAT},
    {"attachViewModelOffsetZ8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelOffsets[23]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[0]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[1]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[2]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[3]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[4]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[5]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[6]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[7]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[8]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[9]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[10]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[11]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[12]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[13]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[14]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[15]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[16]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[17]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[18]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[19]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[20]), CSPFT_FLOAT},
    {"attachWorldModelOffsetX8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[21]), CSPFT_FLOAT},
    {"attachWorldModelOffsetY8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[22]), CSPFT_FLOAT},
    {"attachWorldModelOffsetZ8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelOffsets[23]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[0]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[1]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll1", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[2]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[3]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[4]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll2", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[5]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[6]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[7]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll3", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[8]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[9]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[10]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll4", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[11]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[12]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[13]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll5", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[14]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[15]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[16]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll6", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[17]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[18]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[19]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll7", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[20]), CSPFT_FLOAT},
    {"attachViewModelOffsetPitch8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[21]), CSPFT_FLOAT},
    {"attachViewModelOffsetYaw8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[22]), CSPFT_FLOAT},
    {"attachViewModelOffsetRoll8", offsetof(WeaponFullDef, weapVariantDef.attachViewModelRotations[23]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[0]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[1]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll1", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[2]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[3]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[4]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll2", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[5]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[6]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[7]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll3", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[8]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[9]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[10]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll4", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[11]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[12]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[13]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll5", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[14]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[15]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[16]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll6", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[17]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[18]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[19]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll7", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[20]), CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[21]), CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[22]), CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll8", offsetof(WeaponFullDef, weapVariantDef.attachWorldModelRotations[23]), CSPFT_FLOAT},
    {"ignoreAttachments", offsetof(WeaponFullDef, weapVariantDef.bIgnoreAttachments), CSPFT_BOOL},
    {"stowedModelOffsetsF", offsetof(WeaponFullDef, weapVariantDef.stowedModelOffsets.x), CSPFT_FLOAT},
    {"stowedModelOffsetsR", offsetof(WeaponFullDef, weapVariantDef.stowedModelOffsets.y), CSPFT_FLOAT},
    {"stowedModelOffsetsU", offsetof(WeaponFullDef, weapVariantDef.stowedModelOffsets.z), CSPFT_FLOAT},
    {"stowedModelOffsetsPitch", offsetof(WeaponFullDef, weapVariantDef.stowedModelRotations.x), CSPFT_FLOAT},
    {"stowedModelOffsetsYaw", offsetof(WeaponFullDef, weapVariantDef.stowedModelRotations.y), CSPFT_FLOAT},
    {"stowedModelOffsetsRoll", offsetof(WeaponFullDef, weapVariantDef.stowedModelRotations.z), CSPFT_FLOAT},
    {"worldClipModel", offsetof(WeaponFullDef, weapDef.worldClipModel), CSPFT_XMODEL},
    {"rocketModel", offsetof(WeaponFullDef, weapDef.rocketModel), CSPFT_XMODEL},
    {"mountedModel", offsetof(WeaponFullDef, weapDef.mountedModel), CSPFT_XMODEL},
    {"AdditionalMeleeModel", offsetof(WeaponFullDef, weapDef.additionalMeleeModel), CSPFT_XMODEL},
    {"fireTypeIcon", offsetof(WeaponFullDef, weapDef.fireTypeIcon), CSPFT_MATERIAL},
    {"hudIcon", offsetof(WeaponFullDef, weapDef.hudIcon), CSPFT_MATERIAL},
    {"hudIconRatio", offsetof(WeaponFullDef, weapDef.hudIconRatio), WFT_ICONRATIO_HUD},
    {"indicatorIcon", offsetof(WeaponFullDef, weapDef.indicatorIcon), CSPFT_MATERIAL},
    {"indicatorIconRatio", offsetof(WeaponFullDef, weapDef.indicatorIconRatio), WFT_ICONRATIO_INDICATOR},
    {"ammoCounterIcon", offsetof(WeaponFullDef, weapDef.ammoCounterIcon), CSPFT_MATERIAL},
    {"ammoCounterIconRatio", offsetof(WeaponFullDef, weapDef.ammoCounterIconRatio), WFT_ICONRATIO_AMMOCOUNTER},
    {"ammoCounterClip", offsetof(WeaponFullDef, weapDef.ammoCounterClip), WFT_AMMOCOUNTER_CLIPTYPE},
    {"startAmmo", offsetof(WeaponFullDef, weapDef.iStartAmmo), CSPFT_INT},
    {"ammoDisplayName", offsetof(WeaponFullDef, weapVariantDef.szAmmoDisplayName), CSPFT_STRING},
    {"ammoName", offsetof(WeaponFullDef, weapVariantDef.szAmmoName), CSPFT_STRING},
    {"clipName", offsetof(WeaponFullDef, weapVariantDef.szClipName), CSPFT_STRING},
    {"maxAmmo", offsetof(WeaponFullDef, weapDef.iMaxAmmo), CSPFT_INT},
    {"clipSize", offsetof(WeaponFullDef, weapVariantDef.iClipSize), CSPFT_INT},
    {"shotCount", offsetof(WeaponFullDef, weapDef.shotCount), CSPFT_INT},
    {"sharedAmmoCapName", offsetof(WeaponFullDef, weapDef.szSharedAmmoCapName), CSPFT_STRING},
    {"sharedAmmoCap", offsetof(WeaponFullDef, weapDef.iSharedAmmoCap), CSPFT_INT},
    {"unlimitedAmmo", offsetof(WeaponFullDef, weapDef.unlimitedAmmo), CSPFT_BOOL},
    {"ammoCountClipRelative", offsetof(WeaponFullDef, weapDef.ammoCountClipRelative), CSPFT_BOOL},
    {"sharedAmmo", offsetof(WeaponFullDef, weapDef.sharedAmmo), CSPFT_BOOL},
    {"jamFireTime", offsetof(WeaponFullDef, weapDef.iJamFireTime), CSPFT_MILLISECONDS},
    {"overheatWeapon", offsetof(WeaponFullDef, weapDef.overheatWeapon), CSPFT_INT},
    {"overheatRate", offsetof(WeaponFullDef, weapDef.overheatRate), CSPFT_FLOAT},
    {"cooldownRate", offsetof(WeaponFullDef, weapDef.cooldownRate), CSPFT_FLOAT},
    {"overheatEndVal", offsetof(WeaponFullDef, weapDef.overheatEndVal), CSPFT_FLOAT},
    {"coolWhileFiring", offsetof(WeaponFullDef, weapDef.coolWhileFiring), CSPFT_INT},
    {"fuelTankWeapon", offsetof(WeaponFullDef, weapDef.fuelTankWeapon), CSPFT_INT},
    {"tankLifeTime", offsetof(WeaponFullDef, weapDef.iTankLifeTime), CSPFT_MILLISECONDS},
    {"damage", offsetof(WeaponFullDef, weapDef.damage[0]), CSPFT_INT},
    {"minDamage", offsetof(WeaponFullDef, weapDef.damage[5]), CSPFT_INT},
    {"maxDamageRange", offsetof(WeaponFullDef, weapDef.damageRange[0]), CSPFT_FLOAT},
    {"minDamageRange", offsetof(WeaponFullDef, weapDef.damageRange[5]), CSPFT_FLOAT},
    {"damage2", offsetof(WeaponFullDef, weapDef.damage[1]), CSPFT_INT},
    {"damage3", offsetof(WeaponFullDef, weapDef.damage[2]), CSPFT_INT},
    {"damage4", offsetof(WeaponFullDef, weapDef.damage[3]), CSPFT_INT},
    {"damage5", offsetof(WeaponFullDef, weapDef.damage[4]), CSPFT_INT},
    {"damageRange2", offsetof(WeaponFullDef, weapDef.damageRange[1]), CSPFT_FLOAT},
    {"damageRange3", offsetof(WeaponFullDef, weapDef.damageRange[2]), CSPFT_FLOAT},
    {"damageRange4", offsetof(WeaponFullDef, weapDef.damageRange[3]), CSPFT_FLOAT},
    {"damageRange5", offsetof(WeaponFullDef, weapDef.damageRange[4]), CSPFT_FLOAT},
    {"damageDuration", offsetof(WeaponFullDef, weapDef.damageDuration), CSPFT_FLOAT},
    {"damageInterval", offsetof(WeaponFullDef, weapDef.damageInterval), CSPFT_FLOAT},
    {"playerDamage", offsetof(WeaponFullDef, weapDef.playerDamage), CSPFT_INT},
    {"meleeDamage", offsetof(WeaponFullDef, weapDef.iMeleeDamage), CSPFT_INT},
    {"minPlayerDamage", offsetof(WeaponFullDef, weapDef.minPlayerDamage), CSPFT_INT},
    {"destabilizationRateTime", offsetof(WeaponFullDef, weapDef.destabilizationRateTime), CSPFT_FLOAT},
    {"destabilizationCurvatureMax", offsetof(WeaponFullDef, weapDef.destabilizationCurvatureMax), CSPFT_FLOAT},
    {"destabilizeDistance", offsetof(WeaponFullDef, weapDef.destabilizeDistance), CSPFT_INT},
    {"fireDelay", offsetof(WeaponFullDef, weapDef.iFireDelay), CSPFT_MILLISECONDS},
    {"meleeDelay", offsetof(WeaponFullDef, weapDef.iMeleeDelay), CSPFT_MILLISECONDS},
    {"meleeChargeDelay", offsetof(WeaponFullDef, weapDef.meleeChargeDelay), CSPFT_MILLISECONDS},
    {"spinUpTime", offsetof(WeaponFullDef, weapDef.iSpinUpTime), CSPFT_MILLISECONDS},
    {"spinDownTime", offsetof(WeaponFullDef, weapDef.iSpinDownTime), CSPFT_MILLISECONDS},
    {"spinRate", offsetof(WeaponFullDef, weapDef.spinRate), CSPFT_FLOAT},
    {"spinLoopSound", offsetof(WeaponFullDef, weapDef.spinLoopSound), CSPFT_STRING},
    {"spinLoopSoundPlayer", offsetof(WeaponFullDef, weapDef.spinLoopSoundPlayer), CSPFT_STRING},
    {"startSpinSound", offsetof(WeaponFullDef, weapDef.startSpinSound), CSPFT_STRING},
    {"startSpinSoundPlayer", offsetof(WeaponFullDef, weapDef.startSpinSoundPlayer), CSPFT_STRING},
    {"stopSpinSound", offsetof(WeaponFullDef, weapDef.stopSpinSound), CSPFT_STRING},
    {"stopSpinSoundPlayer", offsetof(WeaponFullDef, weapDef.stopSpinSoundPlayer), CSPFT_STRING},
    {"applySpinPitch", offsetof(WeaponFullDef, weapDef.applySpinPitch), CSPFT_BOOL},
    {"introFireTime", offsetof(WeaponFullDef, weapDef.iIntroFireTime), CSPFT_MILLISECONDS},
    {"introFireLength", offsetof(WeaponFullDef, weapDef.iIntroFireLength), CSPFT_INT},
    {"fireTime", offsetof(WeaponFullDef, weapDef.iFireTime), CSPFT_MILLISECONDS},
    {"flourishTime", offsetof(WeaponFullDef, weapDef.iFlourishTime), CSPFT_MILLISECONDS},
    {"lastFireTime", offsetof(WeaponFullDef, weapDef.iLastFireTime), CSPFT_MILLISECONDS},
    {"rechamberTime", offsetof(WeaponFullDef, weapDef.iRechamberTime), CSPFT_MILLISECONDS},
    {"rechamberBoltTime", offsetof(WeaponFullDef, weapDef.iRechamberBoltTime), CSPFT_MILLISECONDS},
    {"holdFireTime", offsetof(WeaponFullDef, weapDef.iHoldFireTime), CSPFT_MILLISECONDS},
    {"burstFireDelay", offsetof(WeaponFullDef, weapDef.iBurstDelayTime), CSPFT_MILLISECONDS},
    {"detonateTime", offsetof(WeaponFullDef, weapDef.iDetonateTime), CSPFT_MILLISECONDS},
    {"detonateDelay", offsetof(WeaponFullDef, weapDef.iDetonateDelay), CSPFT_MILLISECONDS},
    {"meleeTime", offsetof(WeaponFullDef, weapDef.iMeleeTime), CSPFT_MILLISECONDS},
    {"meleeChargeTime", offsetof(WeaponFullDef, weapDef.meleeChargeTime), CSPFT_MILLISECONDS},
    {"reloadTime", offsetof(WeaponFullDef, weapVariantDef.iReloadTime), CSPFT_MILLISECONDS},
    {"reloadShowRocketTime", offsetof(WeaponFullDef, weapDef.reloadShowRocketTime), CSPFT_MILLISECONDS},
    {"reloadEmptyTime", offsetof(WeaponFullDef, weapVariantDef.iReloadEmptyTime), CSPFT_MILLISECONDS},
    {"reloadAddTime", offsetof(WeaponFullDef, weapDef.iReloadAddTime), CSPFT_MILLISECONDS},
    {"reloadEmptyAddTime", offsetof(WeaponFullDef, weapDef.iReloadEmptyAddTime), CSPFT_MILLISECONDS},
    {"reloadQuickAddTime", offsetof(WeaponFullDef, weapDef.iReloadQuickAddTime), CSPFT_MILLISECONDS},
    {"reloadQuickEmptyAddTime", offsetof(WeaponFullDef, weapDef.iReloadQuickEmptyAddTime), CSPFT_MILLISECONDS},
    {"reloadStartTime", offsetof(WeaponFullDef, weapDef.iReloadStartTime), CSPFT_MILLISECONDS},
    {"reloadStartAddTime", offsetof(WeaponFullDef, weapDef.iReloadStartAddTime), CSPFT_MILLISECONDS},
    {"reloadEndTime", offsetof(WeaponFullDef, weapDef.iReloadEndTime), CSPFT_MILLISECONDS},
    {"reloadQuickTime", offsetof(WeaponFullDef, weapVariantDef.iReloadQuickTime), CSPFT_MILLISECONDS},
    {"reloadQuickEmptyTime", offsetof(WeaponFullDef, weapVariantDef.iReloadQuickEmptyTime), CSPFT_MILLISECONDS},
    {"dropTime", offsetof(WeaponFullDef, weapDef.iDropTime), CSPFT_MILLISECONDS},
    {"raiseTime", offsetof(WeaponFullDef, weapDef.iRaiseTime), CSPFT_MILLISECONDS},
    {"altDropTime", offsetof(WeaponFullDef, weapDef.iAltDropTime), CSPFT_MILLISECONDS},
    {"altRaiseTime", offsetof(WeaponFullDef, weapVariantDef.iAltRaiseTime), CSPFT_MILLISECONDS},
    {"quickDropTime", offsetof(WeaponFullDef, weapDef.quickDropTime), CSPFT_MILLISECONDS},
    {"quickRaiseTime", offsetof(WeaponFullDef, weapDef.quickRaiseTime), CSPFT_MILLISECONDS},
    {"firstRaiseTime", offsetof(WeaponFullDef, weapDef.iFirstRaiseTime), CSPFT_MILLISECONDS},
    {"emptyRaiseTime", offsetof(WeaponFullDef, weapDef.iEmptyRaiseTime), CSPFT_MILLISECONDS},
    {"emptyDropTime", offsetof(WeaponFullDef, weapDef.iEmptyDropTime), CSPFT_MILLISECONDS},
    {"sprintInTime", offsetof(WeaponFullDef, weapDef.sprintInTime), CSPFT_MILLISECONDS},
    {"sprintLoopTime", offsetof(WeaponFullDef, weapDef.sprintLoopTime), CSPFT_MILLISECONDS},
    {"sprintOutTime", offsetof(WeaponFullDef, weapDef.sprintOutTime), CSPFT_MILLISECONDS},
    {"lowReadyInTime", offsetof(WeaponFullDef, weapDef.lowReadyInTime), CSPFT_MILLISECONDS},
    {"lowReadyLoopTime", offsetof(WeaponFullDef, weapDef.lowReadyLoopTime), CSPFT_MILLISECONDS},
    {"lowReadyOutTime", offsetof(WeaponFullDef, weapDef.lowReadyOutTime), CSPFT_MILLISECONDS},
    {"contFireInTime", offsetof(WeaponFullDef, weapDef.contFireInTime), CSPFT_MILLISECONDS},
    {"contFireLoopTime", offsetof(WeaponFullDef, weapDef.contFireLoopTime), CSPFT_MILLISECONDS},
    {"contFireOutTime", offsetof(WeaponFullDef, weapDef.contFireOutTime), CSPFT_MILLISECONDS},
    {"dtpInTime", offsetof(WeaponFullDef, weapDef.dtpInTime), CSPFT_MILLISECONDS},
    {"dtpLoopTime", offsetof(WeaponFullDef, weapDef.dtpLoopTime), CSPFT_MILLISECONDS},
    {"dtpOutTime", offsetof(WeaponFullDef, weapDef.dtpOutTime), CSPFT_MILLISECONDS},
    {"crawlInTime", offsetof(WeaponFullDef, weapDef.crawlInTime), CSPFT_MILLISECONDS},
    {"crawlForwardTime", offsetof(WeaponFullDef, weapDef.crawlForwardTime), CSPFT_MILLISECONDS},
    {"crawlBackTime", offsetof(WeaponFullDef, weapDef.crawlBackTime), CSPFT_MILLISECONDS},
    {"crawlRightTime", offsetof(WeaponFullDef, weapDef.crawlRightTime), CSPFT_MILLISECONDS},
    {"crawlLeftTime", offsetof(WeaponFullDef, weapDef.crawlLeftTime), CSPFT_MILLISECONDS},
    {"crawlOutFireTime", offsetof(WeaponFullDef, weapDef.crawlOutFireTime), CSPFT_MILLISECONDS},
    {"crawlOutTime", offsetof(WeaponFullDef, weapDef.crawlOutTime), CSPFT_MILLISECONDS},
    {"slideInTime", offsetof(WeaponFullDef, weapDef.slideInTime), CSPFT_MILLISECONDS},
    {"deployTime", offsetof(WeaponFullDef, weapDef.deployTime), CSPFT_MILLISECONDS},
    {"breakdownTime", offsetof(WeaponFullDef, weapDef.breakdownTime), CSPFT_MILLISECONDS},
    {"nightVisionWearTime", offsetof(WeaponFullDef, weapDef.nightVisionWearTime), CSPFT_MILLISECONDS},
    {"nightVisionWearTimeFadeOutEnd", offsetof(WeaponFullDef, weapDef.nightVisionWearTimeFadeOutEnd), CSPFT_MILLISECONDS},
    {"nightVisionWearTimePowerUp", offsetof(WeaponFullDef, weapDef.nightVisionWearTimePowerUp), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTime", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTime), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimePowerDown", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTimePowerDown), CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimeFadeInStart", offsetof(WeaponFullDef, weapDef.nightVisionRemoveTimeFadeInStart), CSPFT_MILLISECONDS},
    {"fuseTime", offsetof(WeaponFullDef, weapDef.fuseTime), CSPFT_MILLISECONDS},
    {"aifuseTime", offsetof(WeaponFullDef, weapDef.aiFuseTime), CSPFT_MILLISECONDS},
    {"lockOnRadius", offsetof(WeaponFullDef, weapDef.lockOnRadius), CSPFT_INT},
    {"lockOnSpeed", offsetof(WeaponFullDef, weapDef.lockOnSpeed), CSPFT_INT},
    {"requireLockonToFire", offsetof(WeaponFullDef, weapDef.requireLockonToFire), CSPFT_BOOL},
    {"noAdsWhenMagEmpty", offsetof(WeaponFullDef, weapDef.noAdsWhenMagEmpty), CSPFT_BOOL},
    {"avoidDropCleanup", offsetof(WeaponFullDef, weapDef.avoidDropCleanup), CSPFT_BOOL},
    {"stackFire", offsetof(WeaponFullDef, weapDef.stackFire), CSPFT_INT},
    {"stackFireSpread", offsetof(WeaponFullDef, weapDef.stackFireSpread), CSPFT_FLOAT},
    {"stackFireAccuracyDecay", offsetof(WeaponFullDef, weapDef.stackFireAccuracyDecay), CSPFT_FLOAT},
    {"stackSound", offsetof(WeaponFullDef, weapDef.stackSound), CSPFT_STRING},
    {"autoAimRange", offsetof(WeaponFullDef, weapDef.autoAimRange), CSPFT_FLOAT},
    {"aimAssistRange", offsetof(WeaponFullDef, weapDef.aimAssistRange), CSPFT_FLOAT},
    {"aimAssistRangeAds", offsetof(WeaponFullDef, weapVariantDef.fAimAssistRangeAds), CSPFT_FLOAT},
    {"mountableWeapon", offsetof(WeaponFullDef, weapDef.mountableWeapon), CSPFT_BOOL},
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
    {"adsSwayHorizScale", offsetof(WeaponFullDef, weapVariantDef.fAdsSwayHorizScale), CSPFT_FLOAT},
    {"adsSwayVertScale", offsetof(WeaponFullDef, weapVariantDef.fAdsSwayVertScale), CSPFT_FLOAT},
    {"meleeChargeRange", offsetof(WeaponFullDef, weapDef.meleeChargeRange), CSPFT_FLOAT},
    {"rifleBullet", offsetof(WeaponFullDef, weapDef.bRifleBullet), CSPFT_BOOL},
    {"armorPiercing", offsetof(WeaponFullDef, weapDef.armorPiercing), CSPFT_BOOL},
    {"boltAction", offsetof(WeaponFullDef, weapDef.bBoltAction), CSPFT_BOOL},
    {"shotsBeforeRechamber", offsetof(WeaponFullDef, weapDef.iShotsBeforeRechamber), CSPFT_INT},
    {"useAltTagFlash", offsetof(WeaponFullDef, weapDef.bUseAltTagFlash), CSPFT_BOOL},
    {"useAntiLagRewind", offsetof(WeaponFullDef, weapDef.bUseAntiLagRewind), CSPFT_BOOL},
    {"isCarriedKillstreakWeapon", offsetof(WeaponFullDef, weapDef.bIsCarriedKillstreakWeapon), CSPFT_BOOL},
    {"aimDownSight", offsetof(WeaponFullDef, weapDef.aimDownSight), CSPFT_BOOL},
    {"rechamberWhileAds", offsetof(WeaponFullDef, weapDef.bRechamberWhileAds), CSPFT_BOOL},
    {"reloadWhileAds", offsetof(WeaponFullDef, weapDef.bReloadWhileAds), CSPFT_BOOL},
    {"adsViewErrorMin", offsetof(WeaponFullDef, weapDef.adsViewErrorMin), CSPFT_FLOAT},
    {"adsViewErrorMax", offsetof(WeaponFullDef, weapDef.adsViewErrorMax), CSPFT_FLOAT},
    {"clipOnly", offsetof(WeaponFullDef, weapDef.bClipOnly), CSPFT_BOOL},
    {"canUseInVehicle", offsetof(WeaponFullDef, weapDef.bCanUseInVehicle), CSPFT_BOOL},
    {"noDropsOrRaises", offsetof(WeaponFullDef, weapDef.bNoDropsOrRaises), CSPFT_BOOL},
    {"cookOffHold", offsetof(WeaponFullDef, weapDef.bCookOffHold), CSPFT_BOOL},
    {"adsFire", offsetof(WeaponFullDef, weapDef.adsFireOnly), CSPFT_BOOL},
    {"cancelAutoHolsterWhenEmpty", offsetof(WeaponFullDef, weapDef.cancelAutoHolsterWhenEmpty), CSPFT_BOOL},
    {"suppressAmmoReserveDisplay", offsetof(WeaponFullDef, weapDef.suppressAmmoReserveDisplay), CSPFT_BOOL},
    {"laserSight", offsetof(WeaponFullDef, weapDef.laserSight), CSPFT_BOOL},
    {"laserSightDuringNightvision", offsetof(WeaponFullDef, weapDef.laserSightDuringNightvision), CSPFT_BOOL},
    {"bayonet", offsetof(WeaponFullDef, weapDef.bHasBayonet), CSPFT_BOOL},
    {"dualWield", offsetof(WeaponFullDef, weapDef.bDualWield), CSPFT_BOOL},
    {"hideThirdPerson", offsetof(WeaponFullDef, weapDef.bHideThirdPerson), CSPFT_BOOL},
    {"explodeOnGround", offsetof(WeaponFullDef, weapDef.bExplodeOnGround), CSPFT_BOOL},
    {"throwBack", offsetof(WeaponFullDef, weapDef.bThrowBack), CSPFT_BOOL},
    {"retrievable", offsetof(WeaponFullDef, weapDef.bRetrievable), CSPFT_BOOL},
    {"dieOnRespawn", offsetof(WeaponFullDef, weapDef.bDieOnRespawn), CSPFT_BOOL},
    {"noThirdPersonDropsOrRaises", offsetof(WeaponFullDef, weapDef.bNoThirdPersonDropsOrRaises), CSPFT_BOOL},
    {"continuousFire", offsetof(WeaponFullDef, weapDef.bContinuousFire), CSPFT_BOOL},
    {"useAsMelee", offsetof(WeaponFullDef, weapDef.bUseAsMelee), CSPFT_BOOL},
    {"antiQuickScope", offsetof(WeaponFullDef, weapVariantDef.bAntiQuickScope), CSPFT_BOOL},
    {"noPing", offsetof(WeaponFullDef, weapDef.bNoPing), CSPFT_BOOL},
    {"forceBounce", offsetof(WeaponFullDef, weapDef.bForceBounce), CSPFT_BOOL},
    {"useDroppedModelAsStowed", offsetof(WeaponFullDef, weapDef.bUseDroppedModelAsStowed), CSPFT_BOOL},
    {"noQuickDropWhenEmpty", offsetof(WeaponFullDef, weapDef.bNoQuickDropWhenEmpty), CSPFT_BOOL},
    {"keepCrosshairWhenADS", offsetof(WeaponFullDef, weapDef.bKeepCrosshairWhenADS), CSPFT_BOOL},
    {"useOnlyAltWeaoponHideTagsInAltMode", offsetof(WeaponFullDef, weapDef.bUseOnlyAltWeaoponHideTagsInAltMode), CSPFT_BOOL},
    {"altWeaponAdsOnly", offsetof(WeaponFullDef, weapDef.bAltWeaponAdsOnly), CSPFT_BOOL},
    {"altWeaponDisableSwitching", offsetof(WeaponFullDef, weapDef.bAltWeaponDisableSwitching), CSPFT_BOOL},
    {"killIcon", offsetof(WeaponFullDef, weapDef.killIcon), CSPFT_MATERIAL},
    {"killIconRatio", offsetof(WeaponFullDef, weapDef.killIconRatio), WFT_ICONRATIO_KILL},
    {"flipKillIcon", offsetof(WeaponFullDef, weapDef.flipKillIcon), CSPFT_BOOL},
    {"dpadIcon", offsetof(WeaponFullDef, weapVariantDef.dpadIcon), CSPFT_MATERIAL},
    {"dpadIconRatio", offsetof(WeaponFullDef, weapVariantDef.dpadIconRatio), WFT_ICONRATIO_DPAD},
    {"noAmmoOnDpadIcon", offsetof(WeaponFullDef, weapVariantDef.noAmmoOnDpadIcon), CSPFT_BOOL},
    {"noPartialReload", offsetof(WeaponFullDef, weapDef.bNoPartialReload), CSPFT_BOOL},
    {"segmentedReload", offsetof(WeaponFullDef, weapDef.bSegmentedReload), CSPFT_BOOL},
    {"noADSAutoReload", offsetof(WeaponFullDef, weapDef.bNoADSAutoReload), CSPFT_BOOL},
    {"reloadAmmoAdd", offsetof(WeaponFullDef, weapDef.iReloadAmmoAdd), CSPFT_INT},
    {"reloadStartAdd", offsetof(WeaponFullDef, weapDef.iReloadStartAdd), CSPFT_INT},
    {"attachmentUnique", offsetof(WeaponFullDef, weapVariantDef.szAttachmentUnique), CSPFT_STRING},
    {"altWeapon", offsetof(WeaponFullDef, weapVariantDef.szAltWeaponName), CSPFT_STRING},
    {"DualWieldWeapon", offsetof(WeaponFullDef, weapDef.szDualWieldWeaponName), CSPFT_STRING},
    {"grenadeWeapon", offsetof(WeaponFullDef, weapDef.szSpawnedGrenadeWeaponName), CSPFT_STRING},
    {"dropAmmoMin", offsetof(WeaponFullDef, weapDef.iDropAmmoMin), CSPFT_INT},
    {"dropAmmoMax", offsetof(WeaponFullDef, weapDef.iDropAmmoMax), CSPFT_INT},
    {"dropClipAmmoMin", offsetof(WeaponFullDef, weapDef.iDropClipAmmoMin), CSPFT_INT},
    {"dropClipAmmoMax", offsetof(WeaponFullDef, weapDef.iDropClipAmmoMax), CSPFT_INT},
    {"blocksProne", offsetof(WeaponFullDef, weapDef.blocksProne), CSPFT_BOOL},
    {"silenced", offsetof(WeaponFullDef, weapVariantDef.bSilenced), CSPFT_BOOL},
    {"dualMag", offsetof(WeaponFullDef, weapVariantDef.bDualMag), CSPFT_BOOL},
    {"infrared", offsetof(WeaponFullDef, weapVariantDef.bInfraRed), CSPFT_BOOL},
    {"tvguided", offsetof(WeaponFullDef, weapVariantDef.bTVGuided), CSPFT_BOOL},
    {"airburstWeapon", offsetof(WeaponFullDef, weapDef.bAirburstWeapon), CSPFT_BOOL},
    {"perks1", offsetof(WeaponFullDef, weapVariantDef.perks[0]), CSPFT_UINT},
    {"perks0", offsetof(WeaponFullDef, weapVariantDef.perks[1]), CSPFT_UINT},
    {"isRollingGrenade", offsetof(WeaponFullDef, weapDef.isRollingGrenade), CSPFT_QBOOLEAN},
    {"useBallisticPrediction", offsetof(WeaponFullDef, weapDef.useBallisticPrediction), CSPFT_QBOOLEAN},
    {"isValuable", offsetof(WeaponFullDef, weapDef.isValuable), CSPFT_QBOOLEAN},
    {"isTacticalInsertion", offsetof(WeaponFullDef, weapDef.isTacticalInsertion), CSPFT_QBOOLEAN},
    {"isReviveWeapon", offsetof(WeaponFullDef, weapDef.isReviveWeapon), CSPFT_BOOL},
    {"bUseRigidBodyOnVehicle", offsetof(WeaponFullDef, weapDef.bUseRigidBodyOnVehicle), CSPFT_BOOL},
    {"showIndicator", offsetof(WeaponFullDef, weapDef.bShowIndicator), CSPFT_BOOL},
    {"explosionRadius", offsetof(WeaponFullDef, weapDef.iExplosionRadius), CSPFT_INT},
    {"explosionRadiusMin", offsetof(WeaponFullDef, weapDef.iExplosionRadiusMin), CSPFT_INT},
    {"indicatorRadius", offsetof(WeaponFullDef, weapDef.iIndicatorRadius), CSPFT_INT},
    {"explosionInnerDamage", offsetof(WeaponFullDef, weapDef.iExplosionInnerDamage), CSPFT_INT},
    {"explosionOuterDamage", offsetof(WeaponFullDef, weapDef.iExplosionOuterDamage), CSPFT_INT},
    {"damageConeAngle", offsetof(WeaponFullDef, weapDef.damageConeAngle), CSPFT_FLOAT},
    {"projectileSpeed", offsetof(WeaponFullDef, weapDef.iProjectileSpeed), CSPFT_INT},
    {"projectileSpeedRelativeUp", offsetof(WeaponFullDef, weapDef.iProjectileSpeedRelativeUp), CSPFT_INT},
    {"projectileSpeedUp", offsetof(WeaponFullDef, weapDef.iProjectileSpeedUp), CSPFT_INT},
    {"projectileSpeedForward", offsetof(WeaponFullDef, weapDef.iProjectileSpeedForward), CSPFT_INT},
    {"projectileTakeParentVel", offsetof(WeaponFullDef, weapDef.fProjectileTakeParentVelocity), CSPFT_FLOAT},
    {"projectileActivateDist", offsetof(WeaponFullDef, weapDef.iProjectileActivateDist), CSPFT_INT},
    {"projectileLifetime", offsetof(WeaponFullDef, weapDef.projLifetime), CSPFT_FLOAT},
    {"timeToAccelerate", offsetof(WeaponFullDef, weapDef.timeToAccelerate), CSPFT_FLOAT},
    {"projectileCurvature", offsetof(WeaponFullDef, weapDef.projectileCurvature), CSPFT_FLOAT},
    {"projectileModel", offsetof(WeaponFullDef, weapDef.projectileModel), CSPFT_XMODEL},
    {"projExplosionType", offsetof(WeaponFullDef, weapDef.projExplosion), WFT_PROJ_EXPLOSION},
    {"projExplosionEffect", offsetof(WeaponFullDef, weapDef.projExplosionEffect), CSPFT_FX},
    {"projExplosionEffectForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffectForceNormalUp), CSPFT_BOOL},
    {"projExplosionEffect2", offsetof(WeaponFullDef, weapDef.projExplosionEffect2), CSPFT_FX},
    {"projExplosionEffect2ForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffect2ForceNormalUp), CSPFT_BOOL},
    {"projExplosionEffect3", offsetof(WeaponFullDef, weapDef.projExplosionEffect3), CSPFT_FX},
    {"projExplosionEffect3ForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffect3ForceNormalUp), CSPFT_BOOL},
    {"projExplosionEffect4", offsetof(WeaponFullDef, weapDef.projExplosionEffect4), CSPFT_FX},
    {"projExplosionEffect4ForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffect4ForceNormalUp), CSPFT_BOOL},
    {"projExplosionEffect5", offsetof(WeaponFullDef, weapDef.projExplosionEffect5), CSPFT_FX},
    {"projExplosionEffect5ForceNormalUp", offsetof(WeaponFullDef, weapDef.projExplosionEffect5ForceNormalUp), CSPFT_BOOL},
    {"projExplosionSound", offsetof(WeaponFullDef, weapDef.projExplosionSound), CSPFT_STRING},
    {"projDudEffect", offsetof(WeaponFullDef, weapDef.projDudEffect), CSPFT_FX},
    {"projDudSound", offsetof(WeaponFullDef, weapDef.projDudSound), CSPFT_STRING},
    {"projImpactExplode", offsetof(WeaponFullDef, weapDef.bProjImpactExplode), CSPFT_BOOL},
    {"sentientImpactExplode", offsetof(WeaponFullDef, weapDef.bProjSentientImpactExplode), CSPFT_BOOL},
    {"explodeWhenStationary", offsetof(WeaponFullDef, weapDef.bProjExplodeWhenStationary), CSPFT_BOOL},
    {"bulletImpactExplode", offsetof(WeaponFullDef, weapDef.bBulletImpactExplode), CSPFT_BOOL},
    {"mortarShellSound", offsetof(WeaponFullDef, weapDef.mortarShellSound), CSPFT_STRING},
    {"tankShellSound", offsetof(WeaponFullDef, weapDef.tankShellSound), CSPFT_STRING},
    {"stickiness", offsetof(WeaponFullDef, weapDef.stickiness), WFT_STICKINESS},
    {"rotateType", offsetof(WeaponFullDef, weapDef.rotateType), WFT_ROTATETYPE},
    {"hasDetonator", offsetof(WeaponFullDef, weapDef.hasDetonator), CSPFT_BOOL},
    {"plantable", offsetof(WeaponFullDef, weapDef.plantable), CSPFT_BOOL},
    {"timedDetonation", offsetof(WeaponFullDef, weapDef.timedDetonation), CSPFT_BOOL},
    {"noCrumpleMissile", offsetof(WeaponFullDef, weapDef.bNoCrumpleMissile), CSPFT_BOOL},
    {"rotate", offsetof(WeaponFullDef, weapDef.rotate), CSPFT_BOOL},
    {"keepRolling", offsetof(WeaponFullDef, weapDef.bKeepRolling), CSPFT_BOOL},
    {"holdButtonToThrow", offsetof(WeaponFullDef, weapDef.holdButtonToThrow), CSPFT_BOOL},
    {"offhandHoldIsCancelable", offsetof(WeaponFullDef, weapDef.offhandHoldIsCancelable), CSPFT_BOOL},
    {"freezeMovementWhenFiring", offsetof(WeaponFullDef, weapDef.freezeMovementWhenFiring), CSPFT_BOOL},
    {"lowAmmoWarningThreshold", offsetof(WeaponFullDef, weapDef.lowAmmoWarningThreshold), CSPFT_FLOAT},
    {"explosionTag", offsetof(WeaponFullDef, weapDef.explosionTag), WFT_EXPLOSION_TAG},
    {"bDisallowAtMatchStart", offsetof(WeaponFullDef, weapDef.bDisallowAtMatchStart), CSPFT_BOOL},
    {"isCameraSensor", offsetof(WeaponFullDef, weapDef.isCameraSensor), CSPFT_BOOL},
    {"isAcousticSensor", offsetof(WeaponFullDef, weapDef.isAcousticSensor), CSPFT_BOOL},
    {"isLaserSensor", offsetof(WeaponFullDef, weapDef.isLaserSensor), CSPFT_BOOL},
    {"isHoldUseGrenade", offsetof(WeaponFullDef, weapDef.isHoldUseGrenade), CSPFT_BOOL},
    {"parallelDefaultBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_DEFAULT]), CSPFT_FLOAT},
    {"parallelAsphaltBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ASPHALT]), CSPFT_FLOAT},
    {"parallelBarkBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_BARK]), CSPFT_FLOAT},
    {"parallelBrickBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_BRICK]), CSPFT_FLOAT},
    {"parallelCarpetBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CARPET]), CSPFT_FLOAT},
    {"parallelCeramicBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CERAMIC]), CSPFT_FLOAT},
    {"parallelClothBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CLOTH]), CSPFT_FLOAT},
    {"parallelConcreteBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CONCRETE]), CSPFT_FLOAT},
    {"parallelCushionBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_CUSHION]), CSPFT_FLOAT},
    {"parallelDirtBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_DIRT]), CSPFT_FLOAT},
    {"parallelFleshBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FLESH]), CSPFT_FLOAT},
    {"parallelFoliageBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FOLIAGE]), CSPFT_FLOAT},
    {"parallelFruitBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_FRUIT]), CSPFT_FLOAT},
    {"parallelGlassBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GLASS]), CSPFT_FLOAT},
    {"parallelGrassBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GRASS]), CSPFT_FLOAT},
    {"parallelGravelBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_GRAVEL]), CSPFT_FLOAT},
    {"parallelIceBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ICE]), CSPFT_FLOAT},
    {"parallelMetalBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_METAL]), CSPFT_FLOAT},
    {"parallelMudBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_MUD]), CSPFT_FLOAT},
    {"parallelPaintedMetalBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PAINTED_METAL]), CSPFT_FLOAT},
    {"parallelPaperBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PAPER]), CSPFT_FLOAT},
    {"parallelPlasterBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PLASTER]), CSPFT_FLOAT},
    {"parallelPlasticBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_PLASTIC]), CSPFT_FLOAT},
    {"parallelRockBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_ROCK]), CSPFT_FLOAT},
    {"parallelRubberBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_RUBBER]), CSPFT_FLOAT},
    {"parallelSandBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_SAND]), CSPFT_FLOAT},
    {"parallelSnowBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_SNOW]), CSPFT_FLOAT},
    {"parallelWaterBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_WATER]), CSPFT_FLOAT},
    {"parallelWoodBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_WOOD]), CSPFT_FLOAT},
    {"parallelRiotShieldBounce", offsetof(WeaponFullDef, parallelBounce[SURF_TYPE_RIOT_SHIELD]), CSPFT_FLOAT},
    {"perpendicularDefaultBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_DEFAULT]), CSPFT_FLOAT},
    {"perpendicularAsphaltBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ASPHALT]), CSPFT_FLOAT},
    {"perpendicularBarkBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_BARK]), CSPFT_FLOAT},
    {"perpendicularBrickBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_BRICK]), CSPFT_FLOAT},
    {"perpendicularCarpetBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CARPET]), CSPFT_FLOAT},
    {"perpendicularCeramicBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CERAMIC]), CSPFT_FLOAT},
    {"perpendicularClothBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CLOTH]), CSPFT_FLOAT},
    {"perpendicularConcreteBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CONCRETE]), CSPFT_FLOAT},
    {"perpendicularCushionBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_CUSHION]), CSPFT_FLOAT},
    {"perpendicularDirtBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_DIRT]), CSPFT_FLOAT},
    {"perpendicularFleshBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FLESH]), CSPFT_FLOAT},
    {"perpendicularFoliageBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FOLIAGE]), CSPFT_FLOAT},
    {"perpendicularFruitBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_FRUIT]), CSPFT_FLOAT},
    {"perpendicularGlassBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GLASS]), CSPFT_FLOAT},
    {"perpendicularGrassBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GRASS]), CSPFT_FLOAT},
    {"perpendicularGravelBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_GRAVEL]), CSPFT_FLOAT},
    {"perpendicularIceBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ICE]), CSPFT_FLOAT},
    {"perpendicularMetalBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_METAL]), CSPFT_FLOAT},
    {"perpendicularMudBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_MUD]), CSPFT_FLOAT},
    {"perpendicularPaintedMetalBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PAINTED_METAL]), CSPFT_FLOAT},
    {"perpendicularPaperBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PAPER]), CSPFT_FLOAT},
    {"perpendicularPlasterBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PLASTER]), CSPFT_FLOAT},
    {"perpendicularPlasticBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_PLASTIC]), CSPFT_FLOAT},
    {"perpendicularRockBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_ROCK]), CSPFT_FLOAT},
    {"perpendicularRubberBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_RUBBER]), CSPFT_FLOAT},
    {"perpendicularSandBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_SAND]), CSPFT_FLOAT},
    {"perpendicularSnowBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_SNOW]), CSPFT_FLOAT},
    {"perpendicularWaterBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_WATER]), CSPFT_FLOAT},
    {"perpendicularWoodBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_WOOD]), CSPFT_FLOAT},
    {"perpendicularRiotShieldBounce", offsetof(WeaponFullDef, perpendicularBounce[SURF_TYPE_RIOT_SHIELD]), CSPFT_FLOAT},
    {"projTrailEffect", offsetof(WeaponFullDef, weapDef.projTrailEffect), CSPFT_FX},
    {"projectileRed", offsetof(WeaponFullDef, weapDef.vProjectileColor.x), CSPFT_FLOAT},
    {"projectileGreen", offsetof(WeaponFullDef, weapDef.vProjectileColor.y), CSPFT_FLOAT},
    {"projectileBlue", offsetof(WeaponFullDef, weapDef.vProjectileColor.z), CSPFT_FLOAT},
    {"guidedMissileType", offsetof(WeaponFullDef, weapDef.guidedMissileType), WFT_GUIDED_MISSILE_TYPE},
    {"maxSteeringAccel", offsetof(WeaponFullDef, weapDef.maxSteeringAccel), CSPFT_FLOAT},
    {"projIgnitionDelay", offsetof(WeaponFullDef, weapDef.projIgnitionDelay), CSPFT_INT},
    {"projIgnitionEffect", offsetof(WeaponFullDef, weapDef.projIgnitionEffect), CSPFT_FX},
    {"projIgnitionSound", offsetof(WeaponFullDef, weapDef.projIgnitionSound), CSPFT_STRING},
    {"tagFx_preparationEffect", offsetof(WeaponFullDef, weapDef.tagFx_preparationEffect), CSPFT_FX},
    {"tagFlash_preparationEffect", offsetof(WeaponFullDef, weapDef.tagFlash_preparationEffect), CSPFT_FX},
    {"adsTransInTime", offsetof(WeaponFullDef, weapVariantDef.iAdsTransInTime), CSPFT_MILLISECONDS},
    {"adsTransOutTime", offsetof(WeaponFullDef, weapVariantDef.iAdsTransOutTime), CSPFT_MILLISECONDS},
    {"adsIdleAmount", offsetof(WeaponFullDef, weapDef.fAdsIdleAmount), CSPFT_FLOAT},
    {"adsIdleSpeed", offsetof(WeaponFullDef, weapDef.adsIdleSpeed), CSPFT_FLOAT},
    {"adsZoomFov1", offsetof(WeaponFullDef, weapVariantDef.fAdsZoomFov1), CSPFT_FLOAT},
    {"adsZoomFov2", offsetof(WeaponFullDef, weapVariantDef.fAdsZoomFov2), CSPFT_FLOAT},
    {"adsZoomFov3", offsetof(WeaponFullDef, weapVariantDef.fAdsZoomFov3), CSPFT_FLOAT},
    {"adsZoomInFrac", offsetof(WeaponFullDef, weapVariantDef.fAdsZoomInFrac), CSPFT_FLOAT},
    {"adsZoomOutFrac", offsetof(WeaponFullDef, weapVariantDef.fAdsZoomOutFrac), CSPFT_FLOAT},
    {"adsOverlayShader", offsetof(WeaponFullDef, weapVariantDef.overlayMaterial), CSPFT_MATERIAL_STREAM},
    {"adsOverlayShaderLowRes", offsetof(WeaponFullDef, weapVariantDef.overlayMaterialLowRes), CSPFT_MATERIAL_STREAM},
    {"adsOverlayReticle", offsetof(WeaponFullDef, weapDef.overlayReticle), WFT_OVERLAYRETICLE},
    {"adsOverlayInterface", offsetof(WeaponFullDef, weapDef.overlayInterface), WFT_OVERLAYINTERFACE},
    {"adsOverlayWidth", offsetof(WeaponFullDef, weapDef.overlayWidth), CSPFT_FLOAT},
    {"adsOverlayHeight", offsetof(WeaponFullDef, weapDef.overlayHeight), CSPFT_FLOAT},
    {"adsOverlayAlphaScale", offsetof(WeaponFullDef, weapVariantDef.fOverlayAlphaScale), CSPFT_FLOAT},
    {"adsBobFactor", offsetof(WeaponFullDef, weapDef.fAdsBobFactor), CSPFT_FLOAT},
    {"adsViewBobMult", offsetof(WeaponFullDef, weapDef.fAdsViewBobMult), CSPFT_FLOAT},
    {"holdBreathToSteady", offsetof(WeaponFullDef, weapDef.bHoldBreathToSteady), CSPFT_BOOL},
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
    {"adsViewKickMinMagnitude", offsetof(WeaponFullDef, weapDef.fAdsViewKickMinMagnitude), CSPFT_FLOAT},
    {"adsViewKickYawMin", offsetof(WeaponFullDef, weapDef.fAdsViewKickYawMin), CSPFT_FLOAT},
    {"adsViewKickYawMax", offsetof(WeaponFullDef, weapDef.fAdsViewKickYawMax), CSPFT_FLOAT},
    {"adsRecoilReductionRate", offsetof(WeaponFullDef, weapDef.fAdsRecoilReductionRate), CSPFT_FLOAT},
    {"adsRecoilReductionLimit", offsetof(WeaponFullDef, weapDef.fAdsRecoilReductionLimit), CSPFT_FLOAT},
    {"adsRecoilReturnRate", offsetof(WeaponFullDef, weapDef.fAdsRecoilReturnRate), CSPFT_FLOAT},
    {"adsViewKickCenterSpeed", offsetof(WeaponFullDef, weapVariantDef.fAdsViewKickCenterSpeed), CSPFT_FLOAT},
    {"adsViewKickCenterDuckedScale", offsetof(WeaponFullDef, weapDef.fAdsViewKickCenterDuckedScale), CSPFT_FLOAT},
    {"adsViewKickCenterProneScale", offsetof(WeaponFullDef, weapDef.fAdsViewKickCenterProneScale), CSPFT_FLOAT},
    {"adsSpread", offsetof(WeaponFullDef, weapDef.fAdsSpread), CSPFT_FLOAT},
    {"guidedMissileType", offsetof(WeaponFullDef, weapDef.guidedMissileType), WFT_GUIDED_MISSILE_TYPE},
    {"antiQuickScopeTime", offsetof(WeaponFullDef, weapDef.fAntiQuickScopeTime), CSPFT_FLOAT},
    {"antiQuickScopeScale", offsetof(WeaponFullDef, weapDef.fAntiQuickScopeScale), CSPFT_FLOAT},
    {"antiQuickScopeSpreadMultiplier", offsetof(WeaponFullDef, weapDef.fAntiQuickScopeSpreadMultiplier), CSPFT_FLOAT},
    {"antiQuickScopeSpreadMax", offsetof(WeaponFullDef, weapDef.fAntiQuickScopeSpreadMax), CSPFT_FLOAT},
    {"antiQuickScopeSwayFactor", offsetof(WeaponFullDef, weapDef.fAntiQuickScopeSwayFactor), CSPFT_FLOAT},
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
    {"hipViewKickMinMagnitude", offsetof(WeaponFullDef, weapDef.fHipViewKickMinMagnitude), CSPFT_FLOAT},
    {"hipViewKickYawMin", offsetof(WeaponFullDef, weapDef.fHipViewKickYawMin), CSPFT_FLOAT},
    {"hipViewKickYawMax", offsetof(WeaponFullDef, weapDef.fHipViewKickYawMax), CSPFT_FLOAT},
    {"hipViewKickCenterSpeed", offsetof(WeaponFullDef, weapVariantDef.fHipViewKickCenterSpeed), CSPFT_FLOAT},
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
    {"cameraShakeScale", offsetof(WeaponFullDef, weapDef.cameraShakeScale), CSPFT_FLOAT},
    {"cameraShakeDuration", offsetof(WeaponFullDef, weapDef.cameraShakeDuration), CSPFT_INT},
    {"cameraShakeRadius", offsetof(WeaponFullDef, weapDef.cameraShakeRadius), CSPFT_INT},
    {"explosionCameraShakeScale", offsetof(WeaponFullDef, weapDef.explosionCameraShakeScale), CSPFT_FLOAT},
    {"explosionCameraShakeDuration", offsetof(WeaponFullDef, weapDef.explosionCameraShakeDuration), CSPFT_INT},
    {"explosionCameraShakeRadius", offsetof(WeaponFullDef, weapDef.explosionCameraShakeRadius), CSPFT_INT},
    {"fightDist", offsetof(WeaponFullDef, weapDef.fightDist), CSPFT_FLOAT},
    {"maxDist", offsetof(WeaponFullDef, weapDef.maxDist), CSPFT_FLOAT},
    {"aiVsAiAccuracyGraph", offsetof(WeaponFullDef, weapDef.accuracyGraphName0), CSPFT_STRING},
    {"aiVsPlayerAccuracyGraph", offsetof(WeaponFullDef, weapDef.accuracyGraphName1), CSPFT_STRING},
    {"locNone", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_NONE]), CSPFT_FLOAT},
    {"locHelmet", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_HELMET]), CSPFT_FLOAT},
    {"locHead", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_HEAD]), CSPFT_FLOAT},
    {"locNeck", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_NECK]), CSPFT_FLOAT},
    {"locTorsoUpper", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_TORSO_UPR]), CSPFT_FLOAT},
    {"locTorsoMid", offsetof(WeaponFullDef, locationDamageMultipliers[HITLOC_TORSO_MID]), CSPFT_FLOAT},
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
    {"reloadRumble", offsetof(WeaponFullDef, weapDef.reloadRumble), CSPFT_STRING},
    {"explosionRumble", offsetof(WeaponFullDef, weapDef.explosionRumble), CSPFT_STRING},
    {"tracerType", offsetof(WeaponFullDef, weapDef.tracerType), CSPFT_TRACER},
    {"enemyTracerType", offsetof(WeaponFullDef, weapDef.enemyTracerType), CSPFT_TRACER},
    {"adsDofStart", offsetof(WeaponFullDef, weapDef.adsDofStart), CSPFT_FLOAT},
    {"adsDofEnd", offsetof(WeaponFullDef, weapDef.adsDofEnd), CSPFT_FLOAT},
    {"scanSpeed", offsetof(WeaponFullDef, weapDef.scanSpeed), CSPFT_FLOAT},
    {"scanAccel", offsetof(WeaponFullDef, weapDef.scanAccel), CSPFT_FLOAT},
    {"scanPauseTime", offsetof(WeaponFullDef, weapDef.scanPauseTime), CSPFT_MILLISECONDS},
    {"flameTableFirstPerson", offsetof(WeaponFullDef, weapDef.flameTableFirstPerson), CSPFT_STRING},
    {"flameTableThirdPerson", offsetof(WeaponFullDef, weapDef.flameTableThirdPerson), CSPFT_STRING},
    {"mmsWeapon", offsetof(WeaponFullDef, weapVariantDef.mmsWeapon), CSPFT_BOOL},
    {"mmsInScope", offsetof(WeaponFullDef, weapVariantDef.mmsInScope), CSPFT_BOOL},
    {"mmsFOV", offsetof(WeaponFullDef, weapVariantDef.mmsFOV), CSPFT_FLOAT},
    {"mmsAspect", offsetof(WeaponFullDef, weapVariantDef.mmsAspect), CSPFT_FLOAT},
    {"mmsMaxDist", offsetof(WeaponFullDef, weapVariantDef.mmsMaxDist), CSPFT_FLOAT},
    {"ikLeftHandIdlePosF", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandIdlePos.x), CSPFT_FLOAT},
    {"ikLeftHandIdlePosR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandIdlePos.y), CSPFT_FLOAT},
    {"ikLeftHandIdlePosU", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandIdlePos.z), CSPFT_FLOAT},
    {"ikLeftHandOffsetF", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandOffset.x), CSPFT_FLOAT},
    {"ikLeftHandOffsetR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandOffset.y), CSPFT_FLOAT},
    {"ikLeftHandOffsetU", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandOffset.z), CSPFT_FLOAT},
    {"ikLeftHandRotationP", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandRotation.x), CSPFT_FLOAT},
    {"ikLeftHandRotationY", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandRotation.y), CSPFT_FLOAT},
    {"ikLeftHandRotationR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandRotation.z), CSPFT_FLOAT},
    {"usingLeftHandProneIK", offsetof(WeaponFullDef, weapVariantDef.bUsingLeftHandProneIK), CSPFT_BOOL},
    {"ikLeftHandProneOffsetF", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneOffset.x), CSPFT_FLOAT},
    {"ikLeftHandProneOffsetR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneOffset.y), CSPFT_FLOAT},
    {"ikLeftHandProneOffsetU", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneOffset.z), CSPFT_FLOAT},
    {"ikLeftHandProneRotationP", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneRotation.x), CSPFT_FLOAT},
    {"ikLeftHandProneRotationY", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneRotation.y), CSPFT_FLOAT},
    {"ikLeftHandProneRotationR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandProneRotation.z), CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetF", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerOffset.x), CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerOffset.y), CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetU", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerOffset.z), CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationP", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerRotation.x), CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationY", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerRotation.y), CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationR", offsetof(WeaponFullDef, weapVariantDef.ikLeftHandUiViewerRotation.z), CSPFT_FLOAT},
    {"parentWeaponName", offsetof(WeaponFullDef, weapDef.parentWeaponName), CSPFT_STRING},
    {"doGibbing", offsetof(WeaponFullDef, weapDef.doGibbing), CSPFT_BOOL},
    {"maxGibDistance", offsetof(WeaponFullDef, weapDef.maxGibDistance), CSPFT_FLOAT},
    {"altScopeADSTransInTime", offsetof(WeaponFullDef, weapDef.altScopeADSTransInTime), CSPFT_FLOAT},
    {"altScopeADSTransOutTime", offsetof(WeaponFullDef, weapDef.altScopeADSTransOutTime), CSPFT_FLOAT},
    {"meleeSwipeEffect", offsetof(WeaponFullDef, weapDef.meleeSwipeEffect), CSPFT_FX},
    {"meleeImpactEffect", offsetof(WeaponFullDef, weapDef.meleeImpactEffect), CSPFT_FX},
    {"meleeImpactNoBloodEffect", offsetof(WeaponFullDef, weapDef.meleeImpactNoBloodEffect), CSPFT_FX},
    {"throwBackType", offsetof(WeaponFullDef, weapDef.throwBackType), CSPFT_STRING},
    {"camo", offsetof(WeaponFullDef, weapDef.weaponCamo), WFT_WEAPON_CAMO},
    {"customFloat0", offsetof(WeaponFullDef, weapDef.customFloat0), CSPFT_FLOAT},
    {"customFloat1", offsetof(WeaponFullDef, weapDef.customFloat1), CSPFT_FLOAT},
    {"customFloat2", offsetof(WeaponFullDef, weapDef.customFloat2), CSPFT_FLOAT},
    {"customBool0", offsetof(WeaponFullDef, weapDef.customBool0), CSPFT_BOOL},
    {"customBool1", offsetof(WeaponFullDef, weapDef.customBool1), CSPFT_BOOL},
    {"customBool2", offsetof(WeaponFullDef, weapDef.customBool2), CSPFT_BOOL},
};

namespace T6
{
    const char* szWeapTypeNames[]
    {
        "bullet",
        "grenade",
        "projectile",
        "binoculars",
        "gas",
        "bomb",
        "mine",
        "melee",
        "riotshield"
    };

    const char* szWeapClassNames[]
    {
        "rifle",
        "mg",
        "smg",
        "spread",
        "pistol",
        "grenade",
        "rocketlauncher",
        "turret",
        "non-player",
        "gas",
        "item",
        "melee",
        "Killstreak Alt Stored Weapon",
        "pistol spread"
    };

    const char* szWeapOverlayReticleNames[]
    {
        "none",
        "crosshair"
    };

    const char* szWeapInventoryTypeNames[]
    {
        "primary",
        "offhand",
        "item",
        "altmode",
        "melee",
        "dwlefthand"
    };

    const char* szWeapFireTypeNames[]
    {
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
        "5-Round Burst",
        "Stacked Fire",
        "Minigun",
        "Charge Shot",
        "Jetgun"
    };

    const char* szWeapClipTypeNames[]
    {
        "bottom",
        "top",
        "left",
        "dp28",
        "ptrs",
        "lmg"
    };

    const char* barrelTypeNames[]
    {
        "Single",
        "Dual Barrel",
        "Dual Barrel Alternate",
        "Quad Barrel",
        "Quad Barrel Alternate",
        "Quad Barrel Double Alternate"
    };

    const char* penetrateTypeNames[]
    {
        "none",
        "small",
        "medium",
        "large"
    };

    const char* impactTypeNames[]
    {
        "none",
        "bullet_small",
        "bullet_large",
        "bullet_ap",
        "bullet_xtreme",
        "shotgun",
        "grenade_bounce",
        "grenade_explode",
        "rifle_grenade",
        "rocket_explode",
        "rocket_explode_xtreme",
        "projectile_dud",
        "mortar_shell",
        "tank_shell",
        "bolt",
        "blade"
    };

    const char* szWeapStanceNames[]
    {
        "stand",
        "duck",
        "prone"
    };

    const char* szProjectileExplosionNames[]
    {
        "grenade",
        "rocket",
        "flashbang",
        "none",
        "dud",
        "smoke",
        "heavy explosive",
        "fire",
        "napalmblob",
        "bolt",
        "shrapnel span"
    };

    const char* offhandClassNames[]
    {
        "None",
        "Frag Grenade",
        "Smoke Grenade",
        "Flash Grenade",
        "Gear",
        "Supply Drop Marker"
    };

    const char* offhandSlotNames[]
    {
        "None",
        "Lethal grenade",
        "Tactical grenade",
        "Equipment",
        "Specific use"
    };

    const char* playerAnimTypeNames[]
    {
        "none",
        "default",
        "other",
        "sniper",
        "m203",
        "hold",
        "briefcase",
        "reviver",
        "radio",
        "dualwield",
        "remotecontrol",
        "crossbow",
        "minigun",
        "beltfed",
        "g11",
        "rearclip",
        "handleclip",
        "rearclipsniper",
        "ballisticknife",
        "singleknife",
        "nopump",
        "hatchet",
        "grimreaper",
        "zipline",
        "riotshield",
        "tablet",
        "turned",
        "screecher"
    };

    const char* activeReticleNames[]
    {
        "None",
        "Pip-On-A-Stick",
        "Bouncing Diamond",
        "Missile Lock"
    };

    const char* guidedMissileNames[]
    {
        "None",
        "Sidewinder",
        "Hellfire",
        "Javelin",
        "Ballistic",
        "WireGuided",
        "TVGuided",
        "Drone",
        "HeatSeeking"
    };

    const char* stickinessNames[]
    {
        "Don't stick",
        "Stick to all",
        "Stick to all, except ai and clients",
        "Stick to ground",
        "Stick to ground, maintain yaw",
        "Stick to flesh"
    };

    const char* rotateTypeNames[]
    {
        "Rotate both axis, grenade style",
        "Rotate one axis, blade style",
        "Rotate like a cylinder"
    };

    const char* overlayInterfaceNames[]
    {
        "None",
        "Javelin",
        "Turret Scope"
    };

    const char* ammoCounterClipNames[]
    {
        "None",
        "Magazine",
        "ShortMagazine",
        "Shotgun",
        "Rocket",
        "Beltfed",
        "AltWeapon"
    };

    const char* weapIconRatioNames[]
    {
        "1:1",
        "2:1",
        "4:1"
    };

    class InfoStringFromWeaponConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapTypeNames, _countof(szWeapTypeNames));
                break;

            case WFT_WEAPONCLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClassNames, _countof(szWeapClassNames));
                break;
            case WFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames,
                                _countof(szWeapOverlayReticleNames));
                break;

            case WFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames,
                                _countof(penetrateTypeNames));
                break;

            case WFT_IMPACT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, impactTypeNames, _countof(impactTypeNames));
                break;

            case WFT_STANCE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames,
                                _countof(szWeapStanceNames));
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames,
                                _countof(szProjectileExplosionNames));
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames,
                                _countof(offhandClassNames));
                break;

            case WFT_OFFHAND_SLOT:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandSlotNames, _countof(offhandSlotNames));
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames,
                                _countof(playerAnimTypeNames));
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames,
                                _countof(activeReticleNames));
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames,
                                _countof(guidedMissileNames));
                break;

            case WFT_BOUNCE_SOUND:
                {
                    const auto* bounceSound = *reinterpret_cast<const char***>(reinterpret_cast<uintptr_t>(m_structure)
                        + field.iOffset);

                    if (bounceSound && bounceSound[0])
                    {
                        const std::string firstBounceSound(bounceSound[0]);
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
                FillFromEnumInt(std::string(field.szName), field.iOffset, stickinessNames, _countof(stickinessNames));
                break;

            case WFT_ROTATETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, rotateTypeNames, _countof(rotateTypeNames));
                break;

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames,
                                _countof(overlayInterfaceNames));
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames,
                                _countof(szWeapInventoryTypeNames));
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames,
                                _countof(szWeapFireTypeNames));
                break;

            case WFT_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClipTypeNames,
                                _countof(szWeapClipTypeNames));
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames,
                                _countof(ammoCounterClipNames));
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
            case WFT_ICONRATIO_INDICATOR:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames,
                                _countof(weapIconRatioNames));
                break;

            case WFT_BARRELTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, barrelTypeNames, _countof(barrelTypeNames));
                break;

            case WFT_HIDETAGS:
                {
                    break;
                }

            case WFT_EXPLOSION_TAG: break;

            case WFT_NOTETRACKSOUNDMAP: break;

            case WFT_WEAPON_CAMO:
                {
                    const auto* camo = *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field
                        .iOffset);

                    if (camo)
                        m_info_string.SetValueForKey(std::string(field.szName), std::string(camo->name));
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");
                    break;
                }

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromWeaponConverter(const WeaponFullDef* structure, const cspField_t* fields, const size_t fieldCount)
            : InfoStringFromStructConverter(structure, fields, fieldCount)
        {
        }
    };
}

void AssetDumperWeapon::CopyToFullDef(const WeaponVariantDef* weapon, WeaponFullDef* fullDef) const
{
    fullDef->weapVariantDef = *weapon;

    if (weapon->weapDef)
    {
        fullDef->weapDef = *weapon->weapDef;
        fullDef->weapVariantDef.weapDef = &fullDef->weapDef;
    }

    if (weapon->attachments)
    {
        memcpy_s(fullDef->attachments, sizeof WeaponFullDef::attachments, weapon->attachments, sizeof(void*) * 63);
        fullDef->weapVariantDef.attachments = fullDef->attachments;
    }

    if (weapon->attachmentUniques)
    {
        memcpy_s(fullDef->attachmentUniques, sizeof WeaponFullDef::attachmentUniques, weapon->attachmentUniques,
                 sizeof(void*) * 95);
        fullDef->weapVariantDef.attachmentUniques = fullDef->attachmentUniques;
    }

    if (fullDef->weapDef.gunXModel)
    {
        memcpy_s(fullDef->gunXModel, sizeof WeaponFullDef::gunXModel, fullDef->weapDef.gunXModel, sizeof(void*) * 16);
        fullDef->weapDef.gunXModel = fullDef->gunXModel;
    }

    if (weapon->szXAnims)
    {
        memcpy_s(fullDef->szXAnims, sizeof WeaponFullDef::szXAnims, weapon->szXAnims, sizeof(void*) * 88);
        fullDef->weapVariantDef.szXAnims = fullDef->szXAnims;
    }

    if (weapon->hideTags)
    {
        memcpy_s(fullDef->hideTags, sizeof WeaponFullDef::hideTags, weapon->hideTags, sizeof(scr_string_t) * 32);
        fullDef->weapVariantDef.hideTags = fullDef->hideTags;
    }

    if (fullDef->weapDef.notetrackSoundMapKeys)
    {
        memcpy_s(fullDef->notetrackSoundMapKeys, sizeof WeaponFullDef::notetrackSoundMapKeys,
                 fullDef->weapDef.notetrackSoundMapKeys, sizeof(scr_string_t) * 20);
        fullDef->weapDef.notetrackSoundMapKeys = fullDef->notetrackSoundMapKeys;
    }

    if (fullDef->weapDef.notetrackSoundMapValues)
    {
        memcpy_s(fullDef->notetrackSoundMapValues, sizeof WeaponFullDef::notetrackSoundMapValues,
                 fullDef->weapDef.notetrackSoundMapValues, sizeof(scr_string_t) * 20);
        fullDef->weapDef.notetrackSoundMapValues = fullDef->notetrackSoundMapValues;
    }

    if (fullDef->weapDef.worldModel)
    {
        memcpy_s(fullDef->worldModel, sizeof WeaponFullDef::worldModel, fullDef->weapDef.worldModel,
                 sizeof(void*) * 16);
        fullDef->weapDef.worldModel = fullDef->worldModel;
    }

    if (weapon->attachViewModel)
    {
        memcpy_s(fullDef->attachViewModel, sizeof WeaponFullDef::attachViewModel, weapon->attachViewModel,
                 sizeof(void*) * 8);
        fullDef->weapVariantDef.attachViewModel = fullDef->attachViewModel;
    }

    if (weapon->attachWorldModel)
    {
        memcpy_s(fullDef->attachWorldModel, sizeof WeaponFullDef::attachWorldModel, weapon->attachWorldModel,
                 sizeof(void*) * 8);
        fullDef->weapVariantDef.attachWorldModel = fullDef->attachWorldModel;
    }

    if (weapon->attachViewModelTag)
    {
        memcpy_s(fullDef->attachViewModelTag, sizeof WeaponFullDef::attachViewModelTag, weapon->attachViewModelTag,
                 sizeof(void*) * 8);
        fullDef->weapVariantDef.attachViewModelTag = fullDef->attachViewModelTag;
    }

    if (weapon->attachWorldModelTag)
    {
        memcpy_s(fullDef->attachWorldModelTag, sizeof WeaponFullDef::attachWorldModelTag, weapon->attachWorldModelTag,
                 sizeof(void*) * 8);
        fullDef->weapVariantDef.attachWorldModelTag = fullDef->attachWorldModelTag;
    }

    if (fullDef->weapDef.parallelBounce)
    {
        memcpy_s(fullDef->parallelBounce, sizeof WeaponFullDef::parallelBounce, fullDef->weapDef.parallelBounce,
                 sizeof(float) * 32);
        fullDef->weapDef.parallelBounce = fullDef->parallelBounce;
    }

    if (fullDef->weapDef.perpendicularBounce)
    {
        memcpy_s(fullDef->perpendicularBounce, sizeof WeaponFullDef::perpendicularBounce,
                 fullDef->weapDef.perpendicularBounce, sizeof(float) * 32);
        fullDef->weapDef.perpendicularBounce = fullDef->perpendicularBounce;
    }

    if (fullDef->weapDef.locationDamageMultipliers)
    {
        memcpy_s(fullDef->locationDamageMultipliers, sizeof WeaponFullDef::locationDamageMultipliers,
                 fullDef->weapDef.locationDamageMultipliers, sizeof(float) * 21);
        fullDef->weapDef.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }

    if (fullDef->weapDef.weaponCamo && fullDef->weapDef.weaponCamo->name)
    {
        strcpy_s(fullDef->weaponCamo, fullDef->weapDef.weaponCamo->name);
    }
}

bool AssetDumperWeapon::ShouldDump(XAssetInfo<WeaponVariantDef>* asset)
{
    return true;
}

std::string AssetDumperWeapon::GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset)
{
    return "weapons/" + asset->m_name;
}

void AssetDumperWeapon::DumpAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset, FileAPI::File* out)
{
    auto* fullDef = new WeaponFullDef;
    memset(fullDef, 0, sizeof WeaponFullDef);
    CopyToFullDef(asset->Asset(), fullDef);

    InfoStringFromWeaponConverter converter(fullDef, weapon_fields, _countof(weapon_fields));
    const auto infoString = converter.Convert();
    const auto stringValue = infoString.ToString("WEAPONFILE");
    out->Write(stringValue.c_str(), 1, stringValue.length());
}
