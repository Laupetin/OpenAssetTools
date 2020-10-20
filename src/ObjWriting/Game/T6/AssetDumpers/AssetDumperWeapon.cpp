#include "AssetDumperWeapon.h"

#include <cassert>

#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperWeapon::weapon_fields[]
{
    {"displayName", 0xc, CSPFT_STRING},
    {"AIOverlayDescription", 0x2cc, CSPFT_STRING},
    {"modeName", 0x2d8, CSPFT_STRING},
    {"playerAnimType", 0x2e4, WFT_ANIMTYPE},
    {"gunModel", 0xed4, CSPFT_XMODEL},
    {"gunModel2", 0xed8, CSPFT_XMODEL},
    {"gunModel3", 0xedc, CSPFT_XMODEL},
    {"gunModel4", 0xee0, CSPFT_XMODEL},
    {"gunModel5", 0xee4, CSPFT_XMODEL},
    {"gunModel6", 0xee8, CSPFT_XMODEL},
    {"gunModel7", 0xeec, CSPFT_XMODEL},
    {"gunModel8", 0xef0, CSPFT_XMODEL},
    {"gunModel9", 0xef4, CSPFT_XMODEL},
    {"gunModel10", 0xef8, CSPFT_XMODEL},
    {"gunModel11", 0xefc, CSPFT_XMODEL},
    {"gunModel12", 0xf00, CSPFT_XMODEL},
    {"gunModel13", 0xf04, CSPFT_XMODEL},
    {"gunModel14", 0xf08, CSPFT_XMODEL},
    {"gunModel15", 0xf0c, CSPFT_XMODEL},
    {"gunModel16", 0xf10, CSPFT_XMODEL},
    {"handModel", 0x2d4, CSPFT_XMODEL},
    {"hideTags", 0x1074, WFT_HIDETAGS},
    {"notetrackSoundMap", 0x10b4, WFT_NOTETRACKSOUNDMAP},
    {"idleAnim", 0xf18, CSPFT_STRING},
    {"idleAnimLeft", 0x1058, CSPFT_STRING},
    {"emptyIdleAnim", 0xf1c, CSPFT_STRING},
    {"emptyIdleAnimLeft", 0x105c, CSPFT_STRING},
    {"fireIntroAnim", 0xf20, CSPFT_STRING},
    {"fireAnim", 0xf24, CSPFT_STRING},
    {"fireAnimLeft", 0x104c, CSPFT_STRING},
    {"holdFireAnim", 0xf28, CSPFT_STRING},
    {"lastShotAnim", 0xf2c, CSPFT_STRING},
    {"lastShotAnimLeft", 0x1050, CSPFT_STRING},
    {"flourishAnim", 0xf30, CSPFT_STRING},
    {"flourishAnimLeft", 0x1054, CSPFT_STRING},
    {"detonateAnim", 0xffc, CSPFT_STRING},
    {"rechamberAnim", 0xf34, CSPFT_STRING},
    {"meleeAnim", 0xf38, CSPFT_STRING},
    {"meleeAnimEmpty", 0xf48, CSPFT_STRING},
    {"meleeAnim1", 0xf3c, CSPFT_STRING},
    {"meleeAnim2", 0xf40, CSPFT_STRING},
    {"meleeAnim3", 0xf44, CSPFT_STRING},
    {"meleeChargeAnim", 0xf4c, CSPFT_STRING},
    {"meleeChargeAnimEmpty", 0xf50, CSPFT_STRING},
    {"reloadAnim", 0xf54, CSPFT_STRING},
    {"reloadAnimRight", 0xf58, CSPFT_STRING},
    {"reloadAnimLeft", 0x1064, CSPFT_STRING},
    {"reloadEmptyAnim", 0xf5c, CSPFT_STRING},
    {"reloadEmptyAnimLeft", 0x1060, CSPFT_STRING},
    {"reloadStartAnim", 0xf60, CSPFT_STRING},
    {"reloadEndAnim", 0xf64, CSPFT_STRING},
    {"reloadQuickAnim", 0xf68, CSPFT_STRING},
    {"reloadQuickEmptyAnim", 0xf6c, CSPFT_STRING},
    {"raiseAnim", 0xf70, CSPFT_STRING},
    {"dropAnim", 0xf78, CSPFT_STRING},
    {"firstRaiseAnim", 0xf74, CSPFT_STRING},
    {"altRaiseAnim", 0xf7c, CSPFT_STRING},
    {"altDropAnim", 0xf80, CSPFT_STRING},
    {"quickRaiseAnim", 0xf84, CSPFT_STRING},
    {"quickDropAnim", 0xf88, CSPFT_STRING},
    {"emptyRaiseAnim", 0xf8c, CSPFT_STRING},
    {"emptyDropAnim", 0xf90, CSPFT_STRING},
    {"sprintInAnim", 0xf94, CSPFT_STRING},
    {"sprintLoopAnim", 0xf98, CSPFT_STRING},
    {"sprintOutAnim", 0xf9c, CSPFT_STRING},
    {"sprintInEmptyAnim", 0xfa0, CSPFT_STRING},
    {"sprintLoopEmptyAnim", 0xfa4, CSPFT_STRING},
    {"sprintOutEmptyAnim", 0xfa8, CSPFT_STRING},
    {"lowReadyInAnim", 0xfac, CSPFT_STRING},
    {"lowReadyLoopAnim", 0xfb0, CSPFT_STRING},
    {"lowReadyOutAnim", 0xfb4, CSPFT_STRING},
    {"contFireInAnim", 0xfb8, CSPFT_STRING},
    {"contFireLoopAnim", 0xfbc, CSPFT_STRING},
    {"contFireOutAnim", 0xfc0, CSPFT_STRING},
    {"crawlInAnim", 0xfc4, CSPFT_STRING},
    {"crawlForwardAnim", 0xfc8, CSPFT_STRING},
    {"crawlBackAnim", 0xfcc, CSPFT_STRING},
    {"crawlRightAnim", 0xfd0, CSPFT_STRING},
    {"crawlLeftAnim", 0xfd4, CSPFT_STRING},
    {"crawlOutAnim", 0xfd8, CSPFT_STRING},
    {"crawlEmptyInAnim", 0xfdc, CSPFT_STRING},
    {"crawlEmptyForwardAnim", 0xfe0, CSPFT_STRING},
    {"crawlEmptyBackAnim", 0xfe4, CSPFT_STRING},
    {"crawlEmptyRightAnim", 0xfe8, CSPFT_STRING},
    {"crawlEmptyLeftAnim", 0xfec, CSPFT_STRING},
    {"crawlEmptyOutAnim", 0xff0, CSPFT_STRING},
    {"deployAnim", 0xff4, CSPFT_STRING},
    {"nightVisionWearAnim", 0x1000, CSPFT_STRING},
    {"nightVisionRemoveAnim", 0x1004, CSPFT_STRING},
    {"adsFireAnim", 0x1008, CSPFT_STRING},
    {"adsLastShotAnim", 0x100c, CSPFT_STRING},
    {"adsRechamberAnim", 0x1014, CSPFT_STRING},
    {"adsUpAnim", 0x1068, CSPFT_STRING},
    {"adsDownAnim", 0x106c, CSPFT_STRING},
    {"adsUpOtherScopeAnim", 0x1070, CSPFT_STRING},
    {"adsFireIntroAnim", 0x1010, CSPFT_STRING},
    {"breakdownAnim", 0xff8, CSPFT_STRING},
    {"dtp_in", 0x1018, CSPFT_STRING},
    {"dtp_loop", 0x101c, CSPFT_STRING},
    {"dtp_out", 0x1020, CSPFT_STRING},
    {"dtp_empty_in", 0x1024, CSPFT_STRING},
    {"dtp_empty_loop", 0x1028, CSPFT_STRING},
    {"dtp_empty_out", 0x102c, CSPFT_STRING},
    {"slide_in", 0x1030, CSPFT_STRING},
    {"mantleAnim", 0x1034, CSPFT_STRING},
    {"sprintCameraAnim", 0x1038, CSPFT_STRING},
    {"dtpInCameraAnim", 0x103c, CSPFT_STRING},
    {"dtpLoopCameraAnim", 0x1040, CSPFT_STRING},
    {"dtpOutCameraAnim", 0x1044, CSPFT_STRING},
    {"mantleCameraAnim", 0x1048, CSPFT_STRING},
    {"script", 0xbb8, CSPFT_STRING},
    {"weaponType", 0x2e8, WFT_WEAPONTYPE},
    {"weaponClass", 0x2ec, WFT_WEAPONCLASS},
    {"penetrateType", 0x2f0, WFT_PENETRATE_TYPE},
    {"impactType", 0x2f4, WFT_IMPACT_TYPE},
    {"inventoryType", 0x2f8, WFT_INVENTORYTYPE},
    {"fireType", 0x2fc, WFT_FIRETYPE},
    {"clipType", 0x300, WFT_CLIPTYPE},
    {"barrelType", 0x304, WFT_BARRELTYPE},
    {"offhandClass", 0x32c, WFT_OFFHAND_CLASS},
    {"offhandSlot", 0x330, WFT_OFFHAND_SLOT},
    {"viewFlashEffect", 0x338, CSPFT_FX},
    {"worldFlashEffect", 0x33c, CSPFT_FX},
    {"barrelCooldownEffect", 0x340, CSPFT_FX},
    {"barrelCooldownMinCount", 0x344, CSPFT_INT},
    {"viewFlashOffsetF", 0x348, CSPFT_FLOAT},
    {"viewFlashOffsetR", 0x34c, CSPFT_FLOAT},
    {"viewFlashOffsetU", 0x350, CSPFT_FLOAT},
    {"worldFlashOffsetF", 0x354, CSPFT_FLOAT},
    {"worldFlashOffsetR", 0x358, CSPFT_FLOAT},
    {"worldFlashOffsetU", 0x35c, CSPFT_FLOAT},
    {"pickupSound", 0x360, CSPFT_STRING},
    {"pickupSoundPlayer", 0x364, CSPFT_STRING},
    {"ammoPickupSound", 0x368, CSPFT_STRING},
    {"ammoPickupSoundPlayer", 0x36c, CSPFT_STRING},
    {"projectileSound", 0x370, CSPFT_STRING},
    {"pullbackSound", 0x374, CSPFT_STRING},
    {"pullbackSoundPlayer", 0x378, CSPFT_STRING},
    {"fireSound", 0x37c, CSPFT_STRING},
    {"crackSound", 0x3bc, CSPFT_STRING},
    {"whizbySound", 0x3c0, CSPFT_STRING},
    {"fireSoundPlayer", 0x380, CSPFT_STRING},
    {"loopFireSound", 0x384, CSPFT_STRING},
    {"loopFireSoundPlayer", 0x388, CSPFT_STRING},
    {"loopFireEndSound", 0x38c, CSPFT_STRING},
    {"loopFireEndSoundPlayer", 0x390, CSPFT_STRING},
    {"startFireSound", 0x394, CSPFT_STRING},
    {"stopFireSound", 0x398, CSPFT_STRING},
    {"killcamStartFireSound", 0x39c, CSPFT_STRING},
    {"startFireSoundPlayer", 0x3a0, CSPFT_STRING},
    {"stopFireSoundPlayer", 0x3a4, CSPFT_STRING},
    {"killcamStartFireSoundPlayer", 0x3a8, CSPFT_STRING},
    {"lastShotSound", 0x3ac, CSPFT_STRING},
    {"lastShotSoundPlayer", 0x3b0, CSPFT_STRING},
    {"emptyFireSound", 0x3b4, CSPFT_STRING},
    {"emptyFireSoundPlayer", 0x3b8, CSPFT_STRING},
    {"meleeSwipeSound", 0x3c4, CSPFT_STRING},
    {"meleeSwipeSoundPlayer", 0x3c8, CSPFT_STRING},
    {"meleeHitSound", 0x3cc, CSPFT_STRING},
    {"meleeMissSound", 0x3d0, CSPFT_STRING},
    {"rechamberSound", 0x3d4, CSPFT_STRING},
    {"rechamberSoundPlayer", 0x3d8, CSPFT_STRING},
    {"reloadSound", 0x3dc, CSPFT_STRING},
    {"reloadSoundPlayer", 0x3e0, CSPFT_STRING},
    {"reloadEmptySound", 0x3e4, CSPFT_STRING},
    {"reloadEmptySoundPlayer", 0x3e8, CSPFT_STRING},
    {"reloadStartSound", 0x3ec, CSPFT_STRING},
    {"reloadStartSoundPlayer", 0x3f0, CSPFT_STRING},
    {"reloadEndSound", 0x3f4, CSPFT_STRING},
    {"reloadEndSoundPlayer", 0x3f8, CSPFT_STRING},
    {"rotateLoopSound", 0x3fc, CSPFT_STRING},
    {"rotateLoopSoundPlayer", 0x400, CSPFT_STRING},
    {"rotateStopSound", 0x404, CSPFT_STRING},
    {"rotateStopSoundPlayer", 0x408, CSPFT_STRING},
    {"deploySound", 0x40c, CSPFT_STRING},
    {"deploySoundPlayer", 0x410, CSPFT_STRING},
    {"finishDeploySound", 0x414, CSPFT_STRING},
    {"finishDeploySoundPlayer", 0x418, CSPFT_STRING},
    {"breakdownSound", 0x41c, CSPFT_STRING},
    {"breakdownSoundPlayer", 0x420, CSPFT_STRING},
    {"finishBreakdownSound", 0x424, CSPFT_STRING},
    {"finishBreakdownSoundPlayer", 0x428, CSPFT_STRING},
    {"detonateSound", 0x42c, CSPFT_STRING},
    {"detonateSoundPlayer", 0x430, CSPFT_STRING},
    {"nightVisionWearSound", 0x434, CSPFT_STRING},
    {"nightVisionWearSoundPlayer", 0x438, CSPFT_STRING},
    {"nightVisionRemoveSound", 0x43c, CSPFT_STRING},
    {"nightVisionRemoveSoundPlayer", 0x440, CSPFT_STRING},
    {"raiseSound", 0x44c, CSPFT_STRING},
    {"raiseSoundPlayer", 0x450, CSPFT_STRING},
    {"firstRaiseSound", 0x454, CSPFT_STRING},
    {"firstRaiseSoundPlayer", 0x458, CSPFT_STRING},
    {"altSwitchSound", 0x444, CSPFT_STRING},
    {"altSwitchSoundPlayer", 0x448, CSPFT_STRING},
    {"adsRaiseSoundPlayer", 0x45c, CSPFT_STRING},
    {"adsLowerSoundPlayer", 0x460, CSPFT_STRING},
    {"putawaySound", 0x464, CSPFT_STRING},
    {"putawaySoundPlayer", 0x468, CSPFT_STRING},
    {"overheatSound", 0x46c, CSPFT_STRING},
    {"overheatSoundPlayer", 0x470, CSPFT_STRING},
    {"adsZoomSound", 0x474, CSPFT_STRING},
    {"shellCasing", 0x478, CSPFT_STRING},
    {"shellCasingPlayer", 0x47c, CSPFT_STRING},
    {"bounceSound", 0x480, WFT_BOUNCE_SOUND},
    {"standMountedWeapdef", 0x484, CSPFT_STRING},
    {"crouchMountedWeapdef", 0x488, CSPFT_STRING},
    {"proneMountedWeapdef", 0x48c, CSPFT_STRING},
    {"viewShellEjectEffect", 0x49c, CSPFT_FX},
    {"worldShellEjectEffect", 0x4a0, CSPFT_FX},
    {"viewLastShotEjectEffect", 0x4a4, CSPFT_FX},
    {"worldLastShotEjectEffect", 0x4a8, CSPFT_FX},
    {"viewShellEjectOffsetF", 0x4ac, CSPFT_FLOAT},
    {"viewShellEjectOffsetR", 0x4b0, CSPFT_FLOAT},
    {"viewShellEjectOffsetU", 0x4b4, CSPFT_FLOAT},
    {"worldShellEjectOffsetF", 0x4b8, CSPFT_FLOAT},
    {"worldShellEjectOffsetR", 0x4bc, CSPFT_FLOAT},
    {"worldShellEjectOffsetU", 0x4c0, CSPFT_FLOAT},
    {"viewShellEjectRotationP", 0x4c4, CSPFT_FLOAT},
    {"viewShellEjectRotationY", 0x4c8, CSPFT_FLOAT},
    {"viewShellEjectRotationR", 0x4cc, CSPFT_FLOAT},
    {"worldShellEjectRotationP", 0x4d0, CSPFT_FLOAT},
    {"worldShellEjectRotationY", 0x4d4, CSPFT_FLOAT},
    {"worldShellEjectRotationR", 0x4d8, CSPFT_FLOAT},
    {"reticleCenter", 0x4dc, CSPFT_MATERIAL},
    {"reticleSide", 0x4e0, CSPFT_MATERIAL},
    {"reticleCenterSize", 0x4e4, CSPFT_INT},
    {"reticleSideSize", 0x4e8, CSPFT_INT},
    {"reticleMinOfs", 0x4ec, CSPFT_INT},
    {"activeReticleType", 0x4f0, WFT_ACTIVE_RETICLE_TYPE},
    {"standMoveF", 0x4f4, CSPFT_FLOAT},
    {"standMoveR", 0x4f8, CSPFT_FLOAT},
    {"standMoveU", 0x4fc, CSPFT_FLOAT},
    {"standRotP", 0x500, CSPFT_FLOAT},
    {"standRotY", 0x504, CSPFT_FLOAT},
    {"standRotR", 0x508, CSPFT_FLOAT},
    {"duckedOfsF", 0x50c, CSPFT_FLOAT},
    {"duckedOfsR", 0x510, CSPFT_FLOAT},
    {"duckedOfsU", 0x514, CSPFT_FLOAT},
    {"duckedMoveF", 0x518, CSPFT_FLOAT},
    {"duckedMoveR", 0x51c, CSPFT_FLOAT},
    {"duckedMoveU", 0x520, CSPFT_FLOAT},
    {"duckedSprintOfsF", 0x524, CSPFT_FLOAT},
    {"duckedSprintOfsR", 0x528, CSPFT_FLOAT},
    {"duckedSprintOfsU", 0x52c, CSPFT_FLOAT},
    {"duckedSprintRotP", 0x530, CSPFT_FLOAT},
    {"duckedSprintRotY", 0x534, CSPFT_FLOAT},
    {"duckedSprintRotR", 0x538, CSPFT_FLOAT},
    {"duckedSprintBobH", 0x53c, CSPFT_FLOAT},
    {"duckedSprintBobV", 0x540, CSPFT_FLOAT},
    {"duckedSprintScale", 0x544, CSPFT_FLOAT},
    {"sprintOfsF", 0x548, CSPFT_FLOAT},
    {"sprintOfsR", 0x54c, CSPFT_FLOAT},
    {"sprintOfsU", 0x550, CSPFT_FLOAT},
    {"sprintRotP", 0x554, CSPFT_FLOAT},
    {"sprintRotY", 0x558, CSPFT_FLOAT},
    {"sprintRotR", 0x55c, CSPFT_FLOAT},
    {"sprintBobH", 0x560, CSPFT_FLOAT},
    {"sprintBobV", 0x564, CSPFT_FLOAT},
    {"sprintScale", 0x568, CSPFT_FLOAT},
    {"lowReadyOfsF", 0x56c, CSPFT_FLOAT},
    {"lowReadyOfsR", 0x570, CSPFT_FLOAT},
    {"lowReadyOfsU", 0x574, CSPFT_FLOAT},
    {"lowReadyRotP", 0x578, CSPFT_FLOAT},
    {"lowReadyRotY", 0x57c, CSPFT_FLOAT},
    {"lowReadyRotR", 0x580, CSPFT_FLOAT},
    {"rideOfsF", 0x584, CSPFT_FLOAT},
    {"rideOfsR", 0x588, CSPFT_FLOAT},
    {"rideOfsU", 0x58c, CSPFT_FLOAT},
    {"rideRotP", 0x590, CSPFT_FLOAT},
    {"rideRotY", 0x594, CSPFT_FLOAT},
    {"rideRotR", 0x598, CSPFT_FLOAT},
    {"dtpOfsF", 0x59c, CSPFT_FLOAT},
    {"dtpOfsR", 0x5a0, CSPFT_FLOAT},
    {"dtpOfsU", 0x5a4, CSPFT_FLOAT},
    {"dtpRotP", 0x5a8, CSPFT_FLOAT},
    {"dtpRotY", 0x5ac, CSPFT_FLOAT},
    {"dtpRotR", 0x5b0, CSPFT_FLOAT},
    {"dtpBobH", 0x5b4, CSPFT_FLOAT},
    {"dtpBobV", 0x5b8, CSPFT_FLOAT},
    {"dtpScale", 0x5bc, CSPFT_FLOAT},
    {"mantleOfsF", 0x5c0, CSPFT_FLOAT},
    {"mantleOfsR", 0x5c4, CSPFT_FLOAT},
    {"mantleOfsU", 0x5c8, CSPFT_FLOAT},
    {"mantleRotP", 0x5cc, CSPFT_FLOAT},
    {"mantleRotY", 0x5d0, CSPFT_FLOAT},
    {"mantleRotR", 0x5d4, CSPFT_FLOAT},
    {"slideOfsF", 0x5d8, CSPFT_FLOAT},
    {"slideOfsR", 0x5dc, CSPFT_FLOAT},
    {"slideOfsU", 0x5e0, CSPFT_FLOAT},
    {"slideRotP", 0x5e4, CSPFT_FLOAT},
    {"slideRotY", 0x5e8, CSPFT_FLOAT},
    {"slideRotR", 0x5ec, CSPFT_FLOAT},
    {"duckedRotP", 0x5f0, CSPFT_FLOAT},
    {"duckedRotY", 0x5f4, CSPFT_FLOAT},
    {"duckedRotR", 0x5f8, CSPFT_FLOAT},
    {"proneOfsF", 0x5fc, CSPFT_FLOAT},
    {"proneOfsR", 0x600, CSPFT_FLOAT},
    {"proneOfsU", 0x604, CSPFT_FLOAT},
    {"proneMoveF", 0x608, CSPFT_FLOAT},
    {"proneMoveR", 0x60c, CSPFT_FLOAT},
    {"proneMoveU", 0x610, CSPFT_FLOAT},
    {"proneRotP", 0x614, CSPFT_FLOAT},
    {"proneRotY", 0x618, CSPFT_FLOAT},
    {"proneRotR", 0x61c, CSPFT_FLOAT},
    {"strafeMoveF", 0x620, CSPFT_FLOAT},
    {"strafeMoveR", 0x624, CSPFT_FLOAT},
    {"strafeMoveU", 0x628, CSPFT_FLOAT},
    {"strafeRotP", 0x62c, CSPFT_FLOAT},
    {"strafeRotY", 0x630, CSPFT_FLOAT},
    {"strafeRotR", 0x634, CSPFT_FLOAT},
    {"posMoveRate", 0x638, CSPFT_FLOAT},
    {"posProneMoveRate", 0x63c, CSPFT_FLOAT},
    {"standMoveMinSpeed", 0x640, CSPFT_FLOAT},
    {"duckedMoveMinSpeed", 0x644, CSPFT_FLOAT},
    {"proneMoveMinSpeed", 0x648, CSPFT_FLOAT},
    {"posRotRate", 0x64c, CSPFT_FLOAT},
    {"posProneRotRate", 0x650, CSPFT_FLOAT},
    {"standRotMinSpeed", 0x654, CSPFT_FLOAT},
    {"duckedRotMinSpeed", 0x658, CSPFT_FLOAT},
    {"proneRotMinSpeed", 0x65c, CSPFT_FLOAT},
    {"worldModel", 0x1104, CSPFT_XMODEL},
    {"worldModel2", 0x1108, CSPFT_XMODEL},
    {"worldModel3", 0x110c, CSPFT_XMODEL},
    {"worldModel4", 0x1110, CSPFT_XMODEL},
    {"worldModel5", 0x1114, CSPFT_XMODEL},
    {"worldModel6", 0x1118, CSPFT_XMODEL},
    {"worldModel7", 0x111c, CSPFT_XMODEL},
    {"worldModel8", 0x1120, CSPFT_XMODEL},
    {"worldModel9", 0x1124, CSPFT_XMODEL},
    {"worldModel10", 0x1128, CSPFT_XMODEL},
    {"worldModel11", 0x112c, CSPFT_XMODEL},
    {"worldModel12", 0x1130, CSPFT_XMODEL},
    {"worldModel13", 0x1134, CSPFT_XMODEL},
    {"worldModel14", 0x1138, CSPFT_XMODEL},
    {"worldModel15", 0x113c, CSPFT_XMODEL},
    {"worldModel16", 0x1140, CSPFT_XMODEL},
    {"attachViewModel1", 0x1144, CSPFT_XMODEL},
    {"attachViewModel2", 0x1148, CSPFT_XMODEL},
    {"attachViewModel3", 0x114c, CSPFT_XMODEL},
    {"attachViewModel4", 0x1150, CSPFT_XMODEL},
    {"attachViewModel5", 0x1154, CSPFT_XMODEL},
    {"attachViewModel6", 0x1158, CSPFT_XMODEL},
    {"attachViewModel7", 0x115c, CSPFT_XMODEL},
    {"attachViewModel8", 0x1160, CSPFT_XMODEL},
    {"attachWorldModel1", 0x1164, CSPFT_XMODEL},
    {"attachWorldModel2", 0x1168, CSPFT_XMODEL},
    {"attachWorldModel3", 0x116c, CSPFT_XMODEL},
    {"attachWorldModel4", 0x1170, CSPFT_XMODEL},
    {"attachWorldModel5", 0x1174, CSPFT_XMODEL},
    {"attachWorldModel6", 0x1178, CSPFT_XMODEL},
    {"attachWorldModel7", 0x117c, CSPFT_XMODEL},
    {"attachWorldModel8", 0x1180, CSPFT_XMODEL},
    {"attachViewModelTag1", 0x1184, CSPFT_STRING},
    {"attachViewModelTag2", 0x1188, CSPFT_STRING},
    {"attachViewModelTag3", 0x118c, CSPFT_STRING},
    {"attachViewModelTag4", 0x1190, CSPFT_STRING},
    {"attachViewModelTag5", 0x1194, CSPFT_STRING},
    {"attachViewModelTag6", 0x1198, CSPFT_STRING},
    {"attachViewModelTag7", 0x119c, CSPFT_STRING},
    {"attachViewModelTag8", 0x11a0, CSPFT_STRING},
    {"attachWorldModelTag1", 0x11a4, CSPFT_STRING},
    {"attachWorldModelTag2", 0x11a8, CSPFT_STRING},
    {"attachWorldModelTag3", 0x11ac, CSPFT_STRING},
    {"attachWorldModelTag4", 0x11b0, CSPFT_STRING},
    {"attachWorldModelTag5", 0x11b4, CSPFT_STRING},
    {"attachWorldModelTag6", 0x11b8, CSPFT_STRING},
    {"attachWorldModelTag7", 0x11bc, CSPFT_STRING},
    {"attachWorldModelTag8", 0x11c0, CSPFT_STRING},
    {"attachViewModelOffsetX1", 0x38, CSPFT_FLOAT},
    {"attachViewModelOffsetY1", 0x3c, CSPFT_FLOAT},
    {"attachViewModelOffsetZ1", 0x40, CSPFT_FLOAT},
    {"attachViewModelOffsetX2", 0x44, CSPFT_FLOAT},
    {"attachViewModelOffsetY2", 0x48, CSPFT_FLOAT},
    {"attachViewModelOffsetZ2", 0x4c, CSPFT_FLOAT},
    {"attachViewModelOffsetX3", 0x50, CSPFT_FLOAT},
    {"attachViewModelOffsetY3", 0x54, CSPFT_FLOAT},
    {"attachViewModelOffsetZ3", 0x58, CSPFT_FLOAT},
    {"attachViewModelOffsetX4", 0x5c, CSPFT_FLOAT},
    {"attachViewModelOffsetY4", 0x60, CSPFT_FLOAT},
    {"attachViewModelOffsetZ4", 0x64, CSPFT_FLOAT},
    {"attachViewModelOffsetX5", 0x68, CSPFT_FLOAT},
    {"attachViewModelOffsetY5", 0x6c, CSPFT_FLOAT},
    {"attachViewModelOffsetZ5", 0x70, CSPFT_FLOAT},
    {"attachViewModelOffsetX6", 0x74, CSPFT_FLOAT},
    {"attachViewModelOffsetY6", 0x78, CSPFT_FLOAT},
    {"attachViewModelOffsetZ6", 0x7c, CSPFT_FLOAT},
    {"attachViewModelOffsetX7", 0x80, CSPFT_FLOAT},
    {"attachViewModelOffsetY7", 0x84, CSPFT_FLOAT},
    {"attachViewModelOffsetZ7", 0x88, CSPFT_FLOAT},
    {"attachViewModelOffsetX8", 0x8c, CSPFT_FLOAT},
    {"attachViewModelOffsetY8", 0x90, CSPFT_FLOAT},
    {"attachViewModelOffsetZ8", 0x94, CSPFT_FLOAT},
    {"attachWorldModelOffsetX1", 0x98, CSPFT_FLOAT},
    {"attachWorldModelOffsetY1", 0x9c, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ1", 0xa0, CSPFT_FLOAT},
    {"attachWorldModelOffsetX2", 0xa4, CSPFT_FLOAT},
    {"attachWorldModelOffsetY2", 0xa8, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ2", 0xac, CSPFT_FLOAT},
    {"attachWorldModelOffsetX3", 0xb0, CSPFT_FLOAT},
    {"attachWorldModelOffsetY3", 0xb4, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ3", 0xb8, CSPFT_FLOAT},
    {"attachWorldModelOffsetX4", 0xbc, CSPFT_FLOAT},
    {"attachWorldModelOffsetY4", 0xc0, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ4", 0xc4, CSPFT_FLOAT},
    {"attachWorldModelOffsetX5", 0xc8, CSPFT_FLOAT},
    {"attachWorldModelOffsetY5", 0xcc, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ5", 0xd0, CSPFT_FLOAT},
    {"attachWorldModelOffsetX6", 0xd4, CSPFT_FLOAT},
    {"attachWorldModelOffsetY6", 0xd8, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ6", 0xdc, CSPFT_FLOAT},
    {"attachWorldModelOffsetX7", 0xe0, CSPFT_FLOAT},
    {"attachWorldModelOffsetY7", 0xe4, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ7", 0xe8, CSPFT_FLOAT},
    {"attachWorldModelOffsetX8", 0xec, CSPFT_FLOAT},
    {"attachWorldModelOffsetY8", 0xf0, CSPFT_FLOAT},
    {"attachWorldModelOffsetZ8", 0xf4, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch1", 0xf8, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw1", 0xfc, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll1", 0x100, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch2", 0x104, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw2", 0x108, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll2", 0x10c, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch3", 0x110, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw3", 0x114, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll3", 0x118, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch4", 0x11c, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw4", 0x120, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll4", 0x124, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch5", 0x128, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw5", 0x12c, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll5", 0x130, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch6", 0x134, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw6", 0x138, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll6", 0x13c, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch7", 0x140, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw7", 0x144, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll7", 0x148, CSPFT_FLOAT},
    {"attachViewModelOffsetPitch8", 0x14c, CSPFT_FLOAT},
    {"attachViewModelOffsetYaw8", 0x150, CSPFT_FLOAT},
    {"attachViewModelOffsetRoll8", 0x154, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch1", 0x158, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw1", 0x15c, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll1", 0x160, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch2", 0x164, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw2", 0x168, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll2", 0x16c, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch3", 0x170, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw3", 0x174, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll3", 0x178, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch4", 0x17c, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw4", 0x180, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll4", 0x184, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch5", 0x188, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw5", 0x18c, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll5", 0x190, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch6", 0x194, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw6", 0x198, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll6", 0x19c, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch7", 0x1a0, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw7", 0x1a4, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll7", 0x1a8, CSPFT_FLOAT},
    {"attachWorldModelOffsetPitch8", 0x1ac, CSPFT_FLOAT},
    {"attachWorldModelOffsetYaw8", 0x1b0, CSPFT_FLOAT},
    {"attachWorldModelOffsetRoll8", 0x1b4, CSPFT_FLOAT},
    {"ignoreAttachments", 0x1d8, CSPFT_BOOL},
    {"stowedModelOffsetsF", 0x1b8, CSPFT_FLOAT},
    {"stowedModelOffsetsR", 0x1bc, CSPFT_FLOAT},
    {"stowedModelOffsetsU", 0x1c0, CSPFT_FLOAT},
    {"stowedModelOffsetsPitch", 0x1c4, CSPFT_FLOAT},
    {"stowedModelOffsetsYaw", 0x1c8, CSPFT_FLOAT},
    {"stowedModelOffsetsRoll", 0x1cc, CSPFT_FLOAT},
    {"worldClipModel", 0x664, CSPFT_XMODEL},
    {"rocketModel", 0x668, CSPFT_XMODEL},
    {"mountedModel", 0x66c, CSPFT_XMODEL},
    {"AdditionalMeleeModel", 0x670, CSPFT_XMODEL},
    {"fireTypeIcon", 0x674, CSPFT_MATERIAL},
    {"hudIcon", 0x678, CSPFT_MATERIAL},
    {"hudIconRatio", 0x67c, WFT_ICONRATIO_HUD},
    {"indicatorIcon", 0x680, CSPFT_MATERIAL},
    {"indicatorIconRatio", 0x684, WFT_ICONRATIO_INDICATOR},
    {"ammoCounterIcon", 0x688, CSPFT_MATERIAL},
    {"ammoCounterIconRatio", 0x68c, WFT_ICONRATIO_AMMOCOUNTER},
    {"ammoCounterClip", 0x690, WFT_AMMOCOUNTER_CLIPTYPE},
    {"startAmmo", 0x694, CSPFT_INT},
    {"ammoDisplayName", 0x1fc, CSPFT_STRING},
    {"ammoName", 0x200, CSPFT_STRING},
    {"clipName", 0x208, CSPFT_STRING},
    {"maxAmmo", 0x698, CSPFT_INT},
    {"clipSize", 0x1dc, CSPFT_INT},
    {"shotCount", 0x69c, CSPFT_INT},
    {"sharedAmmoCapName", 0x6a0, CSPFT_STRING},
    {"sharedAmmoCap", 0x6a8, CSPFT_INT},
    {"unlimitedAmmo", 0x6ac, CSPFT_BOOL},
    {"ammoCountClipRelative", 0x6ad, CSPFT_BOOL},
    {"sharedAmmo", 0x8fc, CSPFT_BOOL},
    {"jamFireTime", 0x310, CSPFT_MILLISECONDS},
    {"overheatWeapon", 0x314, CSPFT_INT},
    {"overheatRate", 0x318, CSPFT_FLOAT},
    {"cooldownRate", 0x31c, CSPFT_FLOAT},
    {"overheatEndVal", 0x320, CSPFT_FLOAT},
    {"coolWhileFiring", 0x324, CSPFT_INT},
    {"fuelTankWeapon", 0x325, CSPFT_INT},
    {"tankLifeTime", 0x328, CSPFT_MILLISECONDS},
    {"damage", 0x6b0, CSPFT_INT},
    {"minDamage", 0x6c4, CSPFT_INT},
    {"maxDamageRange", 0x6c8, CSPFT_FLOAT},
    {"minDamageRange", 0x6dc, CSPFT_FLOAT},
    {"damage2", 0x6b4, CSPFT_INT},
    {"damage3", 0x6b8, CSPFT_INT},
    {"damage4", 0x6bc, CSPFT_INT},
    {"damage5", 0x6c0, CSPFT_INT},
    {"damageRange2", 0x6cc, CSPFT_FLOAT},
    {"damageRange3", 0x6d0, CSPFT_FLOAT},
    {"damageRange4", 0x6d4, CSPFT_FLOAT},
    {"damageRange5", 0x6d8, CSPFT_FLOAT},
    {"damageDuration", 0x6e4, CSPFT_FLOAT},
    {"damageInterval", 0x6e8, CSPFT_FLOAT},
    {"playerDamage", 0x6ec, CSPFT_INT},
    {"meleeDamage", 0x6f0, CSPFT_INT},
    {"minPlayerDamage", 0x6e0, CSPFT_INT},
    {"destabilizationRateTime", 0xbbc, CSPFT_FLOAT},
    {"destabilizationCurvatureMax", 0xbc0, CSPFT_FLOAT},
    {"destabilizeDistance", 0xbc4, CSPFT_INT},
    {"fireDelay", 0x6fc, CSPFT_MILLISECONDS},
    {"meleeDelay", 0x700, CSPFT_MILLISECONDS},
    {"meleeChargeDelay", 0x704, CSPFT_MILLISECONDS},
    {"spinUpTime", 0x70c, CSPFT_MILLISECONDS},
    {"spinDownTime", 0x710, CSPFT_MILLISECONDS},
    {"spinRate", 0x714, CSPFT_FLOAT},
    {"spinLoopSound", 0x718, CSPFT_STRING},
    {"spinLoopSoundPlayer", 0x71c, CSPFT_STRING},
    {"startSpinSound", 0x720, CSPFT_STRING},
    {"startSpinSoundPlayer", 0x724, CSPFT_STRING},
    {"stopSpinSound", 0x728, CSPFT_STRING},
    {"stopSpinSoundPlayer", 0x72c, CSPFT_STRING},
    {"applySpinPitch", 0x730, CSPFT_BOOL},
    {"introFireTime", 0xc28, CSPFT_MILLISECONDS},
    {"introFireLength", 0xc2c, CSPFT_INT},
    {"fireTime", 0x734, CSPFT_MILLISECONDS},
    {"flourishTime", 0x7fc, CSPFT_MILLISECONDS},
    {"lastFireTime", 0x738, CSPFT_MILLISECONDS},
    {"rechamberTime", 0x73c, CSPFT_MILLISECONDS},
    {"rechamberBoltTime", 0x740, CSPFT_MILLISECONDS},
    {"holdFireTime", 0x744, CSPFT_MILLISECONDS},
    {"burstFireDelay", 0x750, CSPFT_MILLISECONDS},
    {"detonateTime", 0x748, CSPFT_MILLISECONDS},
    {"detonateDelay", 0x708, CSPFT_MILLISECONDS},
    {"meleeTime", 0x74c, CSPFT_MILLISECONDS},
    {"meleeChargeTime", 0x754, CSPFT_MILLISECONDS},
    {"reloadTime", 0x1e0, CSPFT_MILLISECONDS},
    {"reloadShowRocketTime", 0x760, CSPFT_MILLISECONDS},
    {"reloadEmptyTime", 0x1e4, CSPFT_MILLISECONDS},
    {"reloadAddTime", 0x768, CSPFT_MILLISECONDS},
    {"reloadEmptyAddTime", 0x76c, CSPFT_MILLISECONDS},
    {"reloadQuickAddTime", 0x770, CSPFT_MILLISECONDS},
    {"reloadQuickEmptyAddTime", 0x774, CSPFT_MILLISECONDS},
    {"reloadStartTime", 0x778, CSPFT_MILLISECONDS},
    {"reloadStartAddTime", 0x77c, CSPFT_MILLISECONDS},
    {"reloadEndTime", 0x780, CSPFT_MILLISECONDS},
    {"reloadQuickTime", 0x1e8, CSPFT_MILLISECONDS},
    {"reloadQuickEmptyTime", 0x1ec, CSPFT_MILLISECONDS},
    {"dropTime", 0x784, CSPFT_MILLISECONDS},
    {"raiseTime", 0x788, CSPFT_MILLISECONDS},
    {"altDropTime", 0x78c, CSPFT_MILLISECONDS},
    {"altRaiseTime", 0x1f8, CSPFT_MILLISECONDS},
    {"quickDropTime", 0x790, CSPFT_MILLISECONDS},
    {"quickRaiseTime", 0x794, CSPFT_MILLISECONDS},
    {"firstRaiseTime", 0x798, CSPFT_MILLISECONDS},
    {"emptyRaiseTime", 0x79c, CSPFT_MILLISECONDS},
    {"emptyDropTime", 0x7a0, CSPFT_MILLISECONDS},
    {"sprintInTime", 0x7a4, CSPFT_MILLISECONDS},
    {"sprintLoopTime", 0x7a8, CSPFT_MILLISECONDS},
    {"sprintOutTime", 0x7ac, CSPFT_MILLISECONDS},
    {"lowReadyInTime", 0x7b0, CSPFT_MILLISECONDS},
    {"lowReadyLoopTime", 0x7b4, CSPFT_MILLISECONDS},
    {"lowReadyOutTime", 0x7b8, CSPFT_MILLISECONDS},
    {"contFireInTime", 0x7bc, CSPFT_MILLISECONDS},
    {"contFireLoopTime", 0x7c0, CSPFT_MILLISECONDS},
    {"contFireOutTime", 0x7c4, CSPFT_MILLISECONDS},
    {"dtpInTime", 0x7c8, CSPFT_MILLISECONDS},
    {"dtpLoopTime", 0x7cc, CSPFT_MILLISECONDS},
    {"dtpOutTime", 0x7d0, CSPFT_MILLISECONDS},
    {"crawlInTime", 0x7d4, CSPFT_MILLISECONDS},
    {"crawlForwardTime", 0x7d8, CSPFT_MILLISECONDS},
    {"crawlBackTime", 0x7dc, CSPFT_MILLISECONDS},
    {"crawlRightTime", 0x7e0, CSPFT_MILLISECONDS},
    {"crawlLeftTime", 0x7e4, CSPFT_MILLISECONDS},
    {"crawlOutFireTime", 0x7e8, CSPFT_MILLISECONDS},
    {"crawlOutTime", 0x7ec, CSPFT_MILLISECONDS},
    {"slideInTime", 0x7f0, CSPFT_MILLISECONDS},
    {"deployTime", 0x7f4, CSPFT_MILLISECONDS},
    {"breakdownTime", 0x7f8, CSPFT_MILLISECONDS},
    {"nightVisionWearTime", 0x800, CSPFT_MILLISECONDS},
    {"nightVisionWearTimeFadeOutEnd", 0x804, CSPFT_MILLISECONDS},
    {"nightVisionWearTimePowerUp", 0x808, CSPFT_MILLISECONDS},
    {"nightVisionRemoveTime", 0x80c, CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimePowerDown", 0x810, CSPFT_MILLISECONDS},
    {"nightVisionRemoveTimeFadeInStart", 0x814, CSPFT_MILLISECONDS},
    {"fuseTime", 0x818, CSPFT_MILLISECONDS},
    {"aifuseTime", 0x81c, CSPFT_MILLISECONDS},
    {"lockOnRadius", 0x820, CSPFT_INT},
    {"lockOnSpeed", 0x824, CSPFT_INT},
    {"requireLockonToFire", 0x828, CSPFT_BOOL},
    {"noAdsWhenMagEmpty", 0x829, CSPFT_BOOL},
    {"avoidDropCleanup", 0x82a, CSPFT_BOOL},
    {"stackFire", 0x82c, CSPFT_INT},
    {"stackFireSpread", 0x830, CSPFT_FLOAT},
    {"stackFireAccuracyDecay", 0x834, CSPFT_FLOAT},
    {"stackSound", 0x838, CSPFT_STRING},
    {"autoAimRange", 0x83c, CSPFT_FLOAT},
    {"aimAssistRange", 0x840, CSPFT_FLOAT},
    {"aimAssistRangeAds", 0x210, CSPFT_FLOAT},
    {"mountableWeapon", 0x844, CSPFT_BOOL},
    {"aimPadding", 0x848, CSPFT_FLOAT},
    {"enemyCrosshairRange", 0x84c, CSPFT_FLOAT},
    {"crosshairColorChange", 0x850, CSPFT_BOOL},
    {"moveSpeedScale", 0x854, CSPFT_FLOAT},
    {"adsMoveSpeedScale", 0x858, CSPFT_FLOAT},
    {"sprintDurationScale", 0x85c, CSPFT_FLOAT},
    {"idleCrouchFactor", 0x8c0, CSPFT_FLOAT},
    {"idleProneFactor", 0x8c4, CSPFT_FLOAT},
    {"gunMaxPitch", 0x8c8, CSPFT_FLOAT},
    {"gunMaxYaw", 0x8cc, CSPFT_FLOAT},
    {"swayMaxAngle", 0x8d0, CSPFT_FLOAT},
    {"swayLerpSpeed", 0x8d4, CSPFT_FLOAT},
    {"swayPitchScale", 0x8d8, CSPFT_FLOAT},
    {"swayYawScale", 0x8dc, CSPFT_FLOAT},
    {"swayHorizScale", 0x8e0, CSPFT_FLOAT},
    {"swayVertScale", 0x8e4, CSPFT_FLOAT},
    {"swayShellShockScale", 0x8e8, CSPFT_FLOAT},
    {"adsSwayMaxAngle", 0x8ec, CSPFT_FLOAT},
    {"adsSwayLerpSpeed", 0x8f0, CSPFT_FLOAT},
    {"adsSwayPitchScale", 0x8f4, CSPFT_FLOAT},
    {"adsSwayYawScale", 0x8f8, CSPFT_FLOAT},
    {"adsSwayHorizScale", 0x214, CSPFT_FLOAT},
    {"adsSwayVertScale", 0x218, CSPFT_FLOAT},
    {"meleeChargeRange", 0xa18, CSPFT_FLOAT},
    {"rifleBullet", 0x8fd, CSPFT_BOOL},
    {"armorPiercing", 0x8fe, CSPFT_BOOL},
    {"boltAction", 0x900, CSPFT_BOOL},
    {"shotsBeforeRechamber", 0x95c, CSPFT_INT},
    {"useAltTagFlash", 0x901, CSPFT_BOOL},
    {"useAntiLagRewind", 0x902, CSPFT_BOOL},
    {"isCarriedKillstreakWeapon", 0x903, CSPFT_BOOL},
    {"aimDownSight", 0x904, CSPFT_BOOL},
    {"rechamberWhileAds", 0x905, CSPFT_BOOL},
    {"reloadWhileAds", 0x906, CSPFT_BOOL},
    {"adsViewErrorMin", 0x908, CSPFT_FLOAT},
    {"adsViewErrorMax", 0x90c, CSPFT_FLOAT},
    {"clipOnly", 0x911, CSPFT_BOOL},
    {"canUseInVehicle", 0x912, CSPFT_BOOL},
    {"noDropsOrRaises", 0x913, CSPFT_BOOL},
    {"cookOffHold", 0x910, CSPFT_BOOL},
    {"adsFire", 0x914, CSPFT_BOOL},
    {"cancelAutoHolsterWhenEmpty", 0x915, CSPFT_BOOL},
    {"suppressAmmoReserveDisplay", 0x916, CSPFT_BOOL},
    {"laserSight", 0x917, CSPFT_BOOL},
    {"laserSightDuringNightvision", 0x918, CSPFT_BOOL},
    {"bayonet", 0x91a, CSPFT_BOOL},
    {"dualWield", 0x91b, CSPFT_BOOL},
    {"hideThirdPerson", 0x919, CSPFT_BOOL},
    {"explodeOnGround", 0x91c, CSPFT_BOOL},
    {"throwBack", 0x91d, CSPFT_BOOL},
    {"retrievable", 0x91e, CSPFT_BOOL},
    {"dieOnRespawn", 0x91f, CSPFT_BOOL},
    {"noThirdPersonDropsOrRaises", 0x920, CSPFT_BOOL},
    {"continuousFire", 0x921, CSPFT_BOOL},
    {"useAsMelee", 0xa1c, CSPFT_BOOL},
    {"antiQuickScope", 0x250, CSPFT_BOOL},
    {"noPing", 0x922, CSPFT_BOOL},
    {"forceBounce", 0x923, CSPFT_BOOL},
    {"useDroppedModelAsStowed", 0x924, CSPFT_BOOL},
    {"noQuickDropWhenEmpty", 0x925, CSPFT_BOOL},
    {"keepCrosshairWhenADS", 0x926, CSPFT_BOOL},
    {"useOnlyAltWeaoponHideTagsInAltMode", 0x927, CSPFT_BOOL},
    {"altWeaponAdsOnly", 0x928, CSPFT_BOOL},
    {"altWeaponDisableSwitching", 0x929, CSPFT_BOOL},
    {"killIcon", 0x92c, CSPFT_MATERIAL},
    {"killIconRatio", 0x930, WFT_ICONRATIO_KILL},
    {"flipKillIcon", 0x934, CSPFT_BOOL},
    {"dpadIcon", 0x25c, CSPFT_MATERIAL},
    {"dpadIconRatio", 0x260, WFT_ICONRATIO_DPAD},
    {"noAmmoOnDpadIcon", 0x264, CSPFT_BOOL},
    {"noPartialReload", 0x935, CSPFT_BOOL},
    {"segmentedReload", 0x936, CSPFT_BOOL},
    {"noADSAutoReload", 0x937, CSPFT_BOOL},
    {"reloadAmmoAdd", 0x938, CSPFT_INT},
    {"reloadStartAdd", 0x93c, CSPFT_INT},
    {"attachmentUnique", 0x14, CSPFT_STRING},
    {"altWeapon", 0x10, CSPFT_STRING},
    {"DualWieldWeapon", 0x944, CSPFT_STRING},
    {"grenadeWeapon", 0x940, CSPFT_STRING},
    {"dropAmmoMin", 0x94c, CSPFT_INT},
    {"dropAmmoMax", 0x950, CSPFT_INT},
    {"dropClipAmmoMin", 0x954, CSPFT_INT},
    {"dropClipAmmoMax", 0x958, CSPFT_INT},
    {"blocksProne", 0x960, CSPFT_BOOL},
    {"silenced", 0x244, CSPFT_BOOL},
    {"dualMag", 0x245, CSPFT_BOOL},
    {"infrared", 0x246, CSPFT_BOOL},
    {"tvguided", 0x247, CSPFT_BOOL},
    {"airburstWeapon", 0x8ff, CSPFT_BOOL},
    {"perks1", 0x248, CSPFT_UINT},
    {"perks0", 0x24c, CSPFT_UINT},
    {"isRollingGrenade", 0x964, CSPFT_QBOOLEAN},
    {"useBallisticPrediction", 0x968, CSPFT_QBOOLEAN},
    {"isValuable", 0x96c, CSPFT_QBOOLEAN},
    {"isTacticalInsertion", 0x970, CSPFT_QBOOLEAN},
    {"isReviveWeapon", 0x974, CSPFT_BOOL},
    {"bUseRigidBodyOnVehicle", 0x975, CSPFT_BOOL},
    {"showIndicator", 0x961, CSPFT_BOOL},
    {"explosionRadius", 0x978, CSPFT_INT},
    {"explosionRadiusMin", 0x97c, CSPFT_INT},
    {"indicatorRadius", 0x980, CSPFT_INT},
    {"explosionInnerDamage", 0x984, CSPFT_INT},
    {"explosionOuterDamage", 0x988, CSPFT_INT},
    {"damageConeAngle", 0x98c, CSPFT_FLOAT},
    {"projectileSpeed", 0x990, CSPFT_INT},
    {"projectileSpeedRelativeUp", 0x998, CSPFT_INT},
    {"projectileSpeedUp", 0x994, CSPFT_INT},
    {"projectileSpeedForward", 0x99c, CSPFT_INT},
    {"projectileTakeParentVel", 0x9a0, CSPFT_FLOAT},
    {"projectileActivateDist", 0x9a4, CSPFT_INT},
    {"projectileLifetime", 0x9a8, CSPFT_FLOAT},
    {"timeToAccelerate", 0x9ac, CSPFT_FLOAT},
    {"projectileCurvature", 0x9b0, CSPFT_FLOAT},
    {"projectileModel", 0x9b4, CSPFT_XMODEL},
    {"projExplosionType", 0x9b8, WFT_PROJ_EXPLOSION},
    {"projExplosionEffect", 0x9bc, CSPFT_FX},
    {"projExplosionEffectForceNormalUp", 0x9c0, CSPFT_BOOL},
    {"projExplosionEffect2", 0x9c4, CSPFT_FX},
    {"projExplosionEffect2ForceNormalUp", 0x9c8, CSPFT_BOOL},
    {"projExplosionEffect3", 0x9cc, CSPFT_FX},
    {"projExplosionEffect3ForceNormalUp", 0x9d0, CSPFT_BOOL},
    {"projExplosionEffect4", 0x9d4, CSPFT_FX},
    {"projExplosionEffect4ForceNormalUp", 0x9d8, CSPFT_BOOL},
    {"projExplosionEffect5", 0x9dc, CSPFT_FX},
    {"projExplosionEffect5ForceNormalUp", 0x9e0, CSPFT_BOOL},
    {"projExplosionSound", 0x9e8, CSPFT_STRING},
    {"projDudEffect", 0x9e4, CSPFT_FX},
    {"projDudSound", 0x9ec, CSPFT_STRING},
    {"projImpactExplode", 0x9f8, CSPFT_BOOL},
    {"sentientImpactExplode", 0x9f9, CSPFT_BOOL},
    {"explodeWhenStationary", 0x9fa, CSPFT_BOOL},
    {"bulletImpactExplode", 0x9fb, CSPFT_BOOL},
    {"mortarShellSound", 0x9f0, CSPFT_STRING},
    {"tankShellSound", 0x9f4, CSPFT_STRING},
    {"stickiness", 0x9fc, WFT_STICKINESS},
    {"rotateType", 0xa00, WFT_ROTATETYPE},
    {"hasDetonator", 0xa05, CSPFT_BOOL},
    {"plantable", 0xa04, CSPFT_BOOL},
    {"timedDetonation", 0xa06, CSPFT_BOOL},
    {"noCrumpleMissile", 0xa07, CSPFT_BOOL},
    {"rotate", 0xa08, CSPFT_BOOL},
    {"keepRolling", 0xa09, CSPFT_BOOL},
    {"holdButtonToThrow", 0xa0a, CSPFT_BOOL},
    {"offhandHoldIsCancelable", 0xa0b, CSPFT_BOOL},
    {"freezeMovementWhenFiring", 0xa0c, CSPFT_BOOL},
    {"lowAmmoWarningThreshold", 0xa10, CSPFT_FLOAT},
    {"explosionTag", 0x6f8, WFT_EXPLOSION_TAG},
    {"bDisallowAtMatchStart", 0xa14, CSPFT_BOOL},
    {"isCameraSensor", 0xa1d, CSPFT_BOOL},
    {"isAcousticSensor", 0xa1e, CSPFT_BOOL},
    {"isLaserSensor", 0xa1f, CSPFT_BOOL},
    {"isHoldUseGrenade", 0xa20, CSPFT_BOOL},
    {"parallelDefaultBounce", 0x11c4, CSPFT_FLOAT},
    {"parallelAsphaltBounce", 0x121c, CSPFT_FLOAT},
    {"parallelBarkBounce", 0x11c8, CSPFT_FLOAT},
    {"parallelBrickBounce", 0x11cc, CSPFT_FLOAT},
    {"parallelCarpetBounce", 0x11d0, CSPFT_FLOAT},
    {"parallelCeramicBounce", 0x1220, CSPFT_FLOAT},
    {"parallelClothBounce", 0x11d4, CSPFT_FLOAT},
    {"parallelConcreteBounce", 0x11d8, CSPFT_FLOAT},
    {"parallelCushionBounce", 0x122c, CSPFT_FLOAT},
    {"parallelDirtBounce", 0x11dc, CSPFT_FLOAT},
    {"parallelFleshBounce", 0x11e0, CSPFT_FLOAT},
    {"parallelFoliageBounce", 0x11e4, CSPFT_FLOAT},
    {"parallelFruitBounce", 0x1230, CSPFT_FLOAT},
    {"parallelGlassBounce", 0x11e8, CSPFT_FLOAT},
    {"parallelGrassBounce", 0x11ec, CSPFT_FLOAT},
    {"parallelGravelBounce", 0x11f0, CSPFT_FLOAT},
    {"parallelIceBounce", 0x11f4, CSPFT_FLOAT},
    {"parallelMetalBounce", 0x11f8, CSPFT_FLOAT},
    {"parallelMudBounce", 0x11fc, CSPFT_FLOAT},
    {"parallelPaintedMetalBounce", 0x1234, CSPFT_FLOAT},
    {"parallelPaperBounce", 0x1200, CSPFT_FLOAT},
    {"parallelPlasterBounce", 0x1204, CSPFT_FLOAT},
    {"parallelPlasticBounce", 0x1224, CSPFT_FLOAT},
    {"parallelRockBounce", 0x1208, CSPFT_FLOAT},
    {"parallelRubberBounce", 0x1228, CSPFT_FLOAT},
    {"parallelSandBounce", 0x120c, CSPFT_FLOAT},
    {"parallelSnowBounce", 0x1210, CSPFT_FLOAT},
    {"parallelWaterBounce", 0x1214, CSPFT_FLOAT},
    {"parallelWoodBounce", 0x1218, CSPFT_FLOAT},
    {"parallelRiotShieldBounce", 0x1240, CSPFT_FLOAT},
    {"perpendicularDefaultBounce", 0x1244, CSPFT_FLOAT},
    {"perpendicularAsphaltBounce", 0x129c, CSPFT_FLOAT},
    {"perpendicularBarkBounce", 0x1248, CSPFT_FLOAT},
    {"perpendicularBrickBounce", 0x124c, CSPFT_FLOAT},
    {"perpendicularCarpetBounce", 0x1250, CSPFT_FLOAT},
    {"perpendicularCeramicBounce", 0x12a0, CSPFT_FLOAT},
    {"perpendicularClothBounce", 0x1254, CSPFT_FLOAT},
    {"perpendicularConcreteBounce", 0x1258, CSPFT_FLOAT},
    {"perpendicularCushionBounce", 0x12ac, CSPFT_FLOAT},
    {"perpendicularDirtBounce", 0x125c, CSPFT_FLOAT},
    {"perpendicularFleshBounce", 0x1260, CSPFT_FLOAT},
    {"perpendicularFoliageBounce", 0x1264, CSPFT_FLOAT},
    {"perpendicularFruitBounce", 0x12b0, CSPFT_FLOAT},
    {"perpendicularGlassBounce", 0x1268, CSPFT_FLOAT},
    {"perpendicularGrassBounce", 0x126c, CSPFT_FLOAT},
    {"perpendicularGravelBounce", 0x1270, CSPFT_FLOAT},
    {"perpendicularIceBounce", 0x1274, CSPFT_FLOAT},
    {"perpendicularMetalBounce", 0x1278, CSPFT_FLOAT},
    {"perpendicularMudBounce", 0x127c, CSPFT_FLOAT},
    {"perpendicularPaintedMetalBounce", 0x12b4, CSPFT_FLOAT},
    {"perpendicularPaperBounce", 0x1280, CSPFT_FLOAT},
    {"perpendicularPlasterBounce", 0x1284, CSPFT_FLOAT},
    {"perpendicularPlasticBounce", 0x12a4, CSPFT_FLOAT},
    {"perpendicularRockBounce", 0x1288, CSPFT_FLOAT},
    {"perpendicularRubberBounce", 0x12a8, CSPFT_FLOAT},
    {"perpendicularSandBounce", 0x128c, CSPFT_FLOAT},
    {"perpendicularSnowBounce", 0x1290, CSPFT_FLOAT},
    {"perpendicularWaterBounce", 0x1294, CSPFT_FLOAT},
    {"perpendicularWoodBounce", 0x1298, CSPFT_FLOAT},
    {"perpendicularRiotShieldBounce", 0x12c0, CSPFT_FLOAT},
    {"projTrailEffect", 0xa2c, CSPFT_FX},
    {"projectileRed", 0xa30, CSPFT_FLOAT},
    {"projectileGreen", 0xa34, CSPFT_FLOAT},
    {"projectileBlue", 0xa38, CSPFT_FLOAT},
    {"guidedMissileType", 0xa3c, WFT_GUIDED_MISSILE_TYPE},
    {"maxSteeringAccel", 0xa40, CSPFT_FLOAT},
    {"projIgnitionDelay", 0xa44, CSPFT_INT},
    {"projIgnitionEffect", 0xa48, CSPFT_FX},
    {"projIgnitionSound", 0xa4c, CSPFT_STRING},
    {"tagFx_preparationEffect", 0xc10, CSPFT_FX},
    {"tagFlash_preparationEffect", 0xc14, CSPFT_FX},
    {"adsTransInTime", 0x1f0, CSPFT_MILLISECONDS},
    {"adsTransOutTime", 0x1f4, CSPFT_MILLISECONDS},
    {"adsIdleAmount", 0x8b0, CSPFT_FLOAT},
    {"adsIdleSpeed", 0x8b8, CSPFT_FLOAT},
    {"adsZoomFov1", 0x224, CSPFT_FLOAT},
    {"adsZoomFov2", 0x228, CSPFT_FLOAT},
    {"adsZoomFov3", 0x22c, CSPFT_FLOAT},
    {"adsZoomInFrac", 0x230, CSPFT_FLOAT},
    {"adsZoomOutFrac", 0x234, CSPFT_FLOAT},
    {"adsOverlayShader", 0x254, CSPFT_MATERIAL_STREAM},
    {"adsOverlayShaderLowRes", 0x258, CSPFT_MATERIAL_STREAM},
    {"adsOverlayReticle", 0x860, WFT_OVERLAYRETICLE},
    {"adsOverlayInterface", 0x864, WFT_OVERLAYINTERFACE},
    {"adsOverlayWidth", 0x868, CSPFT_FLOAT},
    {"adsOverlayHeight", 0x86c, CSPFT_FLOAT},
    {"adsOverlayAlphaScale", 0x238, CSPFT_FLOAT},
    {"adsBobFactor", 0x870, CSPFT_FLOAT},
    {"adsViewBobMult", 0x874, CSPFT_FLOAT},
    {"holdBreathToSteady", 0x878, CSPFT_BOOL},
    {"adsAimPitch", 0xa50, CSPFT_FLOAT},
    {"adsCrosshairInFrac", 0xa54, CSPFT_FLOAT},
    {"adsCrosshairOutFrac", 0xa58, CSPFT_FLOAT},
    {"adsReloadTransTime", 0xb40, CSPFT_MILLISECONDS},
    {"adsGunKickReducedKickBullets", 0xa5c, CSPFT_INT},
    {"adsGunKickReducedKickPercent", 0xa60, CSPFT_FLOAT},
    {"adsGunKickPitchMin", 0xa64, CSPFT_FLOAT},
    {"adsGunKickPitchMax", 0xa68, CSPFT_FLOAT},
    {"adsGunKickYawMin", 0xa6c, CSPFT_FLOAT},
    {"adsGunKickYawMax", 0xa70, CSPFT_FLOAT},
    {"adsGunKickAccel", 0xa74, CSPFT_FLOAT},
    {"adsGunKickSpeedMax", 0xa78, CSPFT_FLOAT},
    {"adsGunKickSpeedDecay", 0xa7c, CSPFT_FLOAT},
    {"adsGunKickStaticDecay", 0xa80, CSPFT_FLOAT},
    {"adsViewKickPitchMin", 0xa84, CSPFT_FLOAT},
    {"adsViewKickPitchMax", 0xa88, CSPFT_FLOAT},
    {"adsViewKickMinMagnitude", 0xa8c, CSPFT_FLOAT},
    {"adsViewKickYawMin", 0xa90, CSPFT_FLOAT},
    {"adsViewKickYawMax", 0xa94, CSPFT_FLOAT},
    {"adsRecoilReductionRate", 0xa98, CSPFT_FLOAT},
    {"adsRecoilReductionLimit", 0xa9c, CSPFT_FLOAT},
    {"adsRecoilReturnRate", 0xaa0, CSPFT_FLOAT},
    {"adsViewKickCenterSpeed", 0x21c, CSPFT_FLOAT},
    {"adsViewKickCenterDuckedScale", 0xaf4, CSPFT_FLOAT},
    {"adsViewKickCenterProneScale", 0xaf8, CSPFT_FLOAT},
    {"adsSpread", 0xaac, CSPFT_FLOAT},
    {"guidedMissileType", 0xa3c, WFT_GUIDED_MISSILE_TYPE},
    {"antiQuickScopeTime", 0xafc, CSPFT_FLOAT},
    {"antiQuickScopeScale", 0xb00, CSPFT_FLOAT},
    {"antiQuickScopeSpreadMultiplier", 0xb04, CSPFT_FLOAT},
    {"antiQuickScopeSpreadMax", 0xb08, CSPFT_FLOAT},
    {"antiQuickScopeSwayFactor", 0xb0c, CSPFT_FLOAT},
    {"hipSpreadStandMin", 0x87c, CSPFT_FLOAT},
    {"hipSpreadDuckedMin", 0x880, CSPFT_FLOAT},
    {"hipSpreadProneMin", 0x884, CSPFT_FLOAT},
    {"hipSpreadMax", 0x888, CSPFT_FLOAT},
    {"hipSpreadDuckedMax", 0x88c, CSPFT_FLOAT},
    {"hipSpreadProneMax", 0x890, CSPFT_FLOAT},
    {"hipSpreadDecayRate", 0x894, CSPFT_FLOAT},
    {"hipSpreadFireAdd", 0x898, CSPFT_FLOAT},
    {"hipSpreadTurnAdd", 0x89c, CSPFT_FLOAT},
    {"hipSpreadMoveAdd", 0x8a0, CSPFT_FLOAT},
    {"hipSpreadDuckedDecay", 0x8a4, CSPFT_FLOAT},
    {"hipSpreadProneDecay", 0x8a8, CSPFT_FLOAT},
    {"hipReticleSidePos", 0x8ac, CSPFT_FLOAT},
    {"hipIdleAmount", 0x8b4, CSPFT_FLOAT},
    {"hipIdleSpeed", 0x8bc, CSPFT_FLOAT},
    {"hipGunKickReducedKickBullets", 0xab0, CSPFT_INT},
    {"hipGunKickReducedKickPercent", 0xab4, CSPFT_FLOAT},
    {"hipGunKickPitchMin", 0xab8, CSPFT_FLOAT},
    {"hipGunKickPitchMax", 0xabc, CSPFT_FLOAT},
    {"hipGunKickYawMin", 0xac0, CSPFT_FLOAT},
    {"hipGunKickYawMax", 0xac4, CSPFT_FLOAT},
    {"hipGunKickAccel", 0xac8, CSPFT_FLOAT},
    {"hipGunKickSpeedMax", 0xacc, CSPFT_FLOAT},
    {"hipGunKickSpeedDecay", 0xad0, CSPFT_FLOAT},
    {"hipGunKickStaticDecay", 0xad4, CSPFT_FLOAT},
    {"hipViewKickPitchMin", 0xad8, CSPFT_FLOAT},
    {"hipViewKickPitchMax", 0xadc, CSPFT_FLOAT},
    {"hipViewKickMinMagnitude", 0xae0, CSPFT_FLOAT},
    {"hipViewKickYawMin", 0xae4, CSPFT_FLOAT},
    {"hipViewKickYawMax", 0xae8, CSPFT_FLOAT},
    {"hipViewKickCenterSpeed", 0x220, CSPFT_FLOAT},
    {"leftArc", 0xb44, CSPFT_FLOAT},
    {"rightArc", 0xb48, CSPFT_FLOAT},
    {"topArc", 0xb4c, CSPFT_FLOAT},
    {"bottomArc", 0xb50, CSPFT_FLOAT},
    {"accuracy", 0xb54, CSPFT_FLOAT},
    {"aiSpread", 0xb58, CSPFT_FLOAT},
    {"playerSpread", 0xb5c, CSPFT_FLOAT},
    {"maxVertTurnSpeed", 0xb68, CSPFT_FLOAT},
    {"maxHorTurnSpeed", 0xb6c, CSPFT_FLOAT},
    {"minVertTurnSpeed", 0xb60, CSPFT_FLOAT},
    {"minHorTurnSpeed", 0xb64, CSPFT_FLOAT},
    {"pitchConvergenceTime", 0xb70, CSPFT_FLOAT},
    {"yawConvergenceTime", 0xb74, CSPFT_FLOAT},
    {"suppressionTime", 0xb78, CSPFT_FLOAT},
    {"maxRange", 0xb7c, CSPFT_FLOAT},
    {"animHorRotateInc", 0xb80, CSPFT_FLOAT},
    {"playerPositionDist", 0xb84, CSPFT_FLOAT},
    {"stance", 0x334, WFT_STANCE},
    {"useHintString", 0xb88, CSPFT_STRING},
    {"dropHintString", 0xb8c, CSPFT_STRING},
    {"horizViewJitter", 0xb98, CSPFT_FLOAT},
    {"vertViewJitter", 0xb9c, CSPFT_FLOAT},
    {"cameraShakeScale", 0xba0, CSPFT_FLOAT},
    {"cameraShakeDuration", 0xba4, CSPFT_INT},
    {"cameraShakeRadius", 0xba8, CSPFT_INT},
    {"explosionCameraShakeScale", 0xbac, CSPFT_FLOAT},
    {"explosionCameraShakeDuration", 0xbb0, CSPFT_INT},
    {"explosionCameraShakeRadius", 0xbb4, CSPFT_INT},
    {"fightDist", 0xb10, CSPFT_FLOAT},
    {"maxDist", 0xb14, CSPFT_FLOAT},
    {"aiVsAiAccuracyGraph", 0xb18, CSPFT_STRING},
    {"aiVsPlayerAccuracyGraph", 0xb1c, CSPFT_STRING},
    {"locNone", 0x12c4, CSPFT_FLOAT},
    {"locHelmet", 0x12c8, CSPFT_FLOAT},
    {"locHead", 0x12cc, CSPFT_FLOAT},
    {"locNeck", 0x12d0, CSPFT_FLOAT},
    {"locTorsoUpper", 0x12d4, CSPFT_FLOAT},
    {"locTorsoMid", 0x12d8, CSPFT_FLOAT},
    {"locTorsoLower", 0x12dc, CSPFT_FLOAT},
    {"locRightArmUpper", 0x12e0, CSPFT_FLOAT},
    {"locRightArmLower", 0x12e8, CSPFT_FLOAT},
    {"locRightHand", 0x12f0, CSPFT_FLOAT},
    {"locLeftArmUpper", 0x12e4, CSPFT_FLOAT},
    {"locLeftArmLower", 0x12ec, CSPFT_FLOAT},
    {"locLeftHand", 0x12f4, CSPFT_FLOAT},
    {"locRightLegUpper", 0x12f8, CSPFT_FLOAT},
    {"locRightLegLower", 0x1300, CSPFT_FLOAT},
    {"locRightFoot", 0x1308, CSPFT_FLOAT},
    {"locLeftLegUpper", 0x12fc, CSPFT_FLOAT},
    {"locLeftLegLower", 0x1304, CSPFT_FLOAT},
    {"locLeftFoot", 0x130c, CSPFT_FLOAT},
    {"locGun", 0x1310, CSPFT_FLOAT},
    {"fireRumble", 0xbcc, CSPFT_STRING},
    {"meleeImpactRumble", 0xbd0, CSPFT_STRING},
    {"reloadRumble", 0xbd4, CSPFT_STRING},
    {"explosionRumble", 0xbd8, CSPFT_STRING},
    {"tracerType", 0xbdc, CSPFT_TRACER},
    {"enemyTracerType", 0xbe0, CSPFT_TRACER},
    {"adsDofStart", 0xbe4, CSPFT_FLOAT},
    {"adsDofEnd", 0xbe8, CSPFT_FLOAT},
    {"scanSpeed", 0xbf4, CSPFT_FLOAT},
    {"scanAccel", 0xbf8, CSPFT_FLOAT},
    {"scanPauseTime", 0xbfc, CSPFT_MILLISECONDS},
    {"flameTableFirstPerson", 0xc00, CSPFT_STRING},
    {"flameTableThirdPerson", 0xc04, CSPFT_STRING},
    {"mmsWeapon", 0x265, CSPFT_BOOL},
    {"mmsInScope", 0x266, CSPFT_BOOL},
    {"mmsFOV", 0x268, CSPFT_FLOAT},
    {"mmsAspect", 0x26c, CSPFT_FLOAT},
    {"mmsMaxDist", 0x270, CSPFT_FLOAT},
    {"ikLeftHandIdlePosF", 0x274, CSPFT_FLOAT},
    {"ikLeftHandIdlePosR", 0x278, CSPFT_FLOAT},
    {"ikLeftHandIdlePosU", 0x27c, CSPFT_FLOAT},
    {"ikLeftHandOffsetF", 0x280, CSPFT_FLOAT},
    {"ikLeftHandOffsetR", 0x284, CSPFT_FLOAT},
    {"ikLeftHandOffsetU", 0x288, CSPFT_FLOAT},
    {"ikLeftHandRotationP", 0x28c, CSPFT_FLOAT},
    {"ikLeftHandRotationY", 0x290, CSPFT_FLOAT},
    {"ikLeftHandRotationR", 0x294, CSPFT_FLOAT},
    {"usingLeftHandProneIK", 0x298, CSPFT_BOOL},
    {"ikLeftHandProneOffsetF", 0x29c, CSPFT_FLOAT},
    {"ikLeftHandProneOffsetR", 0x2a0, CSPFT_FLOAT},
    {"ikLeftHandProneOffsetU", 0x2a4, CSPFT_FLOAT},
    {"ikLeftHandProneRotationP", 0x2a8, CSPFT_FLOAT},
    {"ikLeftHandProneRotationY", 0x2ac, CSPFT_FLOAT},
    {"ikLeftHandProneRotationR", 0x2b0, CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetF", 0x2b4, CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetR", 0x2b8, CSPFT_FLOAT},
    {"ikLeftHandUiViewerOffsetU", 0x2bc, CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationP", 0x2c0, CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationY", 0x2c4, CSPFT_FLOAT},
    {"ikLeftHandUiViewerRotationR", 0x2c8, CSPFT_FLOAT},
    {"parentWeaponName", 0x30c, CSPFT_STRING},
    {"doGibbing", 0xc18, CSPFT_BOOL},
    {"maxGibDistance", 0xc1c, CSPFT_FLOAT},
    {"altScopeADSTransInTime", 0xc20, CSPFT_FLOAT},
    {"altScopeADSTransOutTime", 0xc24, CSPFT_FLOAT},
    {"meleeSwipeEffect", 0xc30, CSPFT_FX},
    {"meleeImpactEffect", 0xc34, CSPFT_FX},
    {"meleeImpactNoBloodEffect", 0xc38, CSPFT_FX},
    {"throwBackType", 0xc3c, CSPFT_STRING},
    {"camo", 0xc40, WFT_WEAPON_CAMO},
    {"customFloat0", 0xc44, CSPFT_FLOAT},
    {"customFloat1", 0xc48, CSPFT_FLOAT},
    {"customFloat2", 0xc4c, CSPFT_FLOAT},
    {"customBool0", 0xc50, CSPFT_BOOL},
    {"customBool1", 0xc54, CSPFT_BOOL},
    {"customBool2", 0xc58, CSPFT_BOOL},
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
