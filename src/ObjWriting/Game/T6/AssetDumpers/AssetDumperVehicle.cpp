#include "AssetDumperVehicle.h"

#include <cassert>
#include <type_traits>


#include "Game/T6/CommonT6.h"
#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperVehicle::vehicle_fields[]
{
    {"type", offsetof(VehicleDef, type), VFT_TYPE},
    {"remoteControl", offsetof(VehicleDef, remoteControl), CSPFT_QBOOLEAN},
    {"bulletDamage", offsetof(VehicleDef, bulletDamage), CSPFT_QBOOLEAN},
    {"armorPiercingDamage", offsetof(VehicleDef, armorPiercingDamage), CSPFT_QBOOLEAN},
    {"grenadeDamage", offsetof(VehicleDef, grenadeDamage), CSPFT_QBOOLEAN},
    {"projectileDamage", offsetof(VehicleDef, projectileDamage), CSPFT_QBOOLEAN},
    {"projectileSplashDamage", offsetof(VehicleDef, projectileSplashDamage), CSPFT_QBOOLEAN},
    {"heavyExplosiveDamage", offsetof(VehicleDef, heavyExplosiveDamage), CSPFT_QBOOLEAN},
    {"cameraMode", offsetof(VehicleDef, cameraMode), VFT_CAMERAMODE},
    {"autoRecenterOnAccel", offsetof(VehicleDef, autoRecenterOnAccel), CSPFT_QBOOLEAN},
    {"thirdPersonDriver", offsetof(VehicleDef, thirdPersonDriver), CSPFT_QBOOLEAN},
    {"thirdPersonUseVehicleRoll", offsetof(VehicleDef, thirdPersonUseVehicleRoll), CSPFT_QBOOLEAN},
    {"thirdPersonCameraPitchVehicleRelative", offsetof(VehicleDef, thirdPersonCameraPitchVehicleRelative), CSPFT_QBOOLEAN},
    {"thirdPersonCameraHeightWorldRelative", offsetof(VehicleDef, thirdPersonCameraHeightWorldRelative), CSPFT_QBOOLEAN},
    {"thirdPersonCameraHeightMin", offsetof(VehicleDef, thirdPersonCameraHeight[0]), CSPFT_FLOAT},
    {"thirdPersonCameraPitchMin", offsetof(VehicleDef, thirdPersonCameraPitch[0]), CSPFT_FLOAT},
    {"thirdPersonCameraRange", offsetof(VehicleDef, thirdPersonCameraRange), CSPFT_FLOAT},
    {"thirdPersonCameraHeight", offsetof(VehicleDef, thirdPersonCameraHeight[1]), CSPFT_FLOAT},
    {"thirdPersonCameraPitch", offsetof(VehicleDef, thirdPersonCameraPitch[1]), CSPFT_FLOAT},
    {"thirdPersonCameraMinPitchClamp", offsetof(VehicleDef, thirdPersonCameraMinPitchClamp), CSPFT_FLOAT},
    {"thirdPersonCameraMaxPitchClamp", offsetof(VehicleDef, thirdPersonCameraMaxPitchClamp), CSPFT_FLOAT},
    {"cameraAlwaysAutoCenter", offsetof(VehicleDef, cameraAlwaysAutoCenter), CSPFT_QBOOLEAN},
    {"cameraAutoCenterLerpRate", offsetof(VehicleDef, cameraAutoCenterLerpRate), CSPFT_FLOAT},
    {"cameraAutoCenterMaxLerpRate", offsetof(VehicleDef, cameraAutoCenterMaxLerpRate), CSPFT_FLOAT},
    {"thirdPersonCameraSpringDistance", offsetof(VehicleDef, thirdPersonCameraSpringDistance), CSPFT_FLOAT},
    {"thirdPersonCameraSpringTime", offsetof(VehicleDef, thirdPersonCameraSpringTime), CSPFT_FLOAT},
    {"thirdPersonCameraHandbrakeTurnRateInc", offsetof(VehicleDef, thirdPersonCameraHandbrakeTurnRateInc), CSPFT_FLOAT},
    {"cameraRollFraction", offsetof(VehicleDef, cameraRollFraction), CSPFT_FLOAT},
    {"cameraFOV", offsetof(VehicleDef, cameraFOV), CSPFT_FLOAT},
    {"viewInfluence", offsetof(VehicleDef, viewInfluence), CSPFT_FLOAT},
    {"tagPlayerOffsetX", offsetof(VehicleDef, tagPlayerOffset.x), CSPFT_FLOAT},
    {"tagPlayerOffsetY", offsetof(VehicleDef, tagPlayerOffset.y), CSPFT_FLOAT},
    {"tagPlayerOffsetZ", offsetof(VehicleDef, tagPlayerOffset.z), CSPFT_FLOAT},
    {"killcamCollision", offsetof(VehicleDef, killcamCollision), CSPFT_QBOOLEAN},
    {"killcamDist", offsetof(VehicleDef, killcamDist), CSPFT_FLOAT},
    {"killcamZDist", offsetof(VehicleDef, killcamZDist), CSPFT_FLOAT},
    {"killcamMinDist", offsetof(VehicleDef, killcamMinDist), CSPFT_FLOAT},
    {"killcamZTargetOffset", offsetof(VehicleDef, killcamZTargetOffset), CSPFT_FLOAT},
    {"killcamFOV", offsetof(VehicleDef, killcamFOV), CSPFT_FLOAT},
    {"killcamNearBlur", offsetof(VehicleDef, killcamNearBlur), CSPFT_FLOAT},
    {"killcamNearBlurStart", offsetof(VehicleDef, killcamNearBlurStart), CSPFT_FLOAT},
    {"killcamNearBlurEnd", offsetof(VehicleDef, killcamNearBlurEnd), CSPFT_FLOAT},
    {"killcamFarBlur", offsetof(VehicleDef, killcamFarBlur), CSPFT_FLOAT},
    {"killcamFarBlurStart", offsetof(VehicleDef, killcamFarBlurStart), CSPFT_FLOAT},
    {"killcamFarBlurEnd", offsetof(VehicleDef, killcamFarBlurEnd), CSPFT_FLOAT},
    {"isDrivable", offsetof(VehicleDef, isDrivable), CSPFT_QBOOLEAN},
    {"numberOfSeats", offsetof(VehicleDef, numberOfSeats), CSPFT_INT},
    {"numberOfGunners", offsetof(VehicleDef, numberOfGunners), CSPFT_INT},
    {"driverControlledGunPos", offsetof(VehicleDef, driverControlledGunPos), CSPFT_INT},
    {"seatSwitchOrder1", offsetof(VehicleDef, seatSwitchOrder[0]), CSPFT_INT},
    {"seatSwitchOrder2", offsetof(VehicleDef, seatSwitchOrder[1]), CSPFT_INT},
    {"seatSwitchOrder3", offsetof(VehicleDef, seatSwitchOrder[2]), CSPFT_INT},
    {"seatSwitchOrder4", offsetof(VehicleDef, seatSwitchOrder[3]), CSPFT_INT},
    {"seatSwitchOrder5", offsetof(VehicleDef, seatSwitchOrder[4]), CSPFT_INT},
    {"seatSwitchOrder6", offsetof(VehicleDef, seatSwitchOrder[5]), CSPFT_INT},
    {"seatSwitchOrder7", offsetof(VehicleDef, seatSwitchOrder[6]), CSPFT_INT},
    {"seatSwitchOrder8", offsetof(VehicleDef, seatSwitchOrder[7]), CSPFT_INT},
    {"seatSwitchOrder9", offsetof(VehicleDef, seatSwitchOrder[8]), CSPFT_INT},
    {"seatSwitchOrder10", offsetof(VehicleDef, seatSwitchOrder[9]), CSPFT_INT},
    {"seatSwitchOrder11", offsetof(VehicleDef, seatSwitchOrder[10]), CSPFT_INT},
    {"texureScrollScale", offsetof(VehicleDef, texScrollScale), CSPFT_FLOAT},
    {"wheelRotRate", offsetof(VehicleDef, wheelRotRate), CSPFT_FLOAT},
    {"extraWheelRotScale", offsetof(VehicleDef, extraWheelRotScale), CSPFT_FLOAT},
    {"wheelChildTakesSteerYaw", offsetof(VehicleDef, wheelChildTakesSteerYaw), CSPFT_QBOOLEAN},
    {"enterRadiusDriver", offsetof(VehicleDef, entryPointRadius[VEH_POS_DRIVER]), CSPFT_FLOAT},
    {"enterRadiusGunner1", offsetof(VehicleDef, entryPointRadius[VEH_POS_GUNNER_1]), CSPFT_FLOAT},
    {"enterRadiusGunner2", offsetof(VehicleDef, entryPointRadius[VEH_POS_GUNNER_2]), CSPFT_FLOAT},
    {"enterRadiusGunner3", offsetof(VehicleDef, entryPointRadius[VEH_POS_GUNNER_3]), CSPFT_FLOAT},
    {"enterRadiusGunner4", offsetof(VehicleDef, entryPointRadius[VEH_POS_GUNNER_4]), CSPFT_FLOAT},
    {"maxSpeed", offsetof(VehicleDef, maxSpeed), VFT_MPH_TO_INCHES_PER_SECOND},
    {"maxSpeedVertical", offsetof(VehicleDef, maxSpeedVertical), VFT_MPH_TO_INCHES_PER_SECOND},
    {"accel", offsetof(VehicleDef, accel), VFT_MPH_TO_INCHES_PER_SECOND},
    {"accelVertical", offsetof(VehicleDef, accelVertical), VFT_MPH_TO_INCHES_PER_SECOND},
    {"rotRate", offsetof(VehicleDef, rotRate), CSPFT_FLOAT},
    {"rotAccel", offsetof(VehicleDef, rotAccel), CSPFT_FLOAT},
    {"collisionDamage", offsetof(VehicleDef, collisionDamage), CSPFT_FLOAT},
    {"collisionSpeed", offsetof(VehicleDef, collisionSpeed), VFT_MPH_TO_INCHES_PER_SECOND},
    {"suspensionTravel", offsetof(VehicleDef, suspensionTravel), CSPFT_FLOAT},
    {"maxBodyPitch", offsetof(VehicleDef, maxBodyPitch), CSPFT_FLOAT},
    {"maxBodyRoll", offsetof(VehicleDef, maxBodyRoll), CSPFT_FLOAT},
    {"heliCollisionScalar", offsetof(VehicleDef, heliCollisionScalar), CSPFT_FLOAT},
    {"viewPitchOffset", offsetof(VehicleDef, viewPitchOffset), CSPFT_FLOAT},
    {"tiltFromAccelerationPitch", offsetof(VehicleDef, tiltFromAcceleration[0]), CSPFT_FLOAT},
    {"tiltFromAccelerationRoll", offsetof(VehicleDef, tiltFromAcceleration[1]), CSPFT_FLOAT},
    {"tiltFromDecelerationPitch", offsetof(VehicleDef, tiltFromDeceleration[0]), CSPFT_FLOAT},
    {"tiltFromDecelerationRoll", offsetof(VehicleDef, tiltFromDeceleration[1]), CSPFT_FLOAT},
    {"tiltFromVelocityPitch", offsetof(VehicleDef, tiltFromVelocity[0]), CSPFT_FLOAT},
    {"tiltFromVelocityRoll", offsetof(VehicleDef, tiltFromVelocity[1]), CSPFT_FLOAT},
    {"tiltSpeedPitch", offsetof(VehicleDef, tiltSpeed[0]), CSPFT_FLOAT},
    {"tiltSpeedRoll", offsetof(VehicleDef, tiltSpeed[1]), CSPFT_FLOAT},
    {"tracerOffsetForward", offsetof(VehicleDef, tracerOffset[0]), CSPFT_FLOAT},
    {"tracerOffsetUp", offsetof(VehicleDef, tracerOffset[1]), CSPFT_FLOAT},
    {"turretWeapon", offsetof(VehicleDef, turretWeapon), CSPFT_STRING},
    {"turretHorizSpanLeft", offsetof(VehicleDef, turretViewLimits.horizSpanLeft), CSPFT_FLOAT},
    {"turretHorizSpanRight", offsetof(VehicleDef, turretViewLimits.horizSpanRight), CSPFT_FLOAT},
    {"turretVertSpanUp", offsetof(VehicleDef, turretViewLimits.vertSpanUp), CSPFT_FLOAT},
    {"turretVertSpanDown", offsetof(VehicleDef, turretViewLimits.vertSpanDown), CSPFT_FLOAT},
    {"turretHorizResistLeft", offsetof(VehicleDef, turretViewLimits.horizResistLeft), CSPFT_FLOAT},
    {"turretHorizResistRight", offsetof(VehicleDef, turretViewLimits.horizResistRight), CSPFT_FLOAT},
    {"turretVertResistUp", offsetof(VehicleDef, turretViewLimits.vertResistUp), CSPFT_FLOAT},
    {"turretVertResistDown", offsetof(VehicleDef, turretViewLimits.vertResistDown), CSPFT_FLOAT},
    {"turretRotRate", offsetof(VehicleDef, turretRotRate), CSPFT_FLOAT},
    {"turretClampPlayerView", offsetof(VehicleDef, turretClampPlayerView), CSPFT_QBOOLEAN},
    {"turretLockTurretToPlayerView", offsetof(VehicleDef, turretLockTurretToPlayerView), CSPFT_QBOOLEAN},
    {"gunnerWeapon", offsetof(VehicleDef, gunnerWeapon[0]), CSPFT_STRING},
    {"gunnerWeapon1", offsetof(VehicleDef, gunnerWeapon[1]), CSPFT_STRING},
    {"gunnerWeapon2", offsetof(VehicleDef, gunnerWeapon[2]), CSPFT_STRING},
    {"gunnerWeapon3", offsetof(VehicleDef, gunnerWeapon[3]), CSPFT_STRING},
    {"gunnerRotRate", offsetof(VehicleDef, gunnerRotRate), CSPFT_FLOAT},
    {"passenger1HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[0].horizSpanLeft), CSPFT_FLOAT},
    {"passenger1HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[0].horizSpanRight), CSPFT_FLOAT},
    {"passenger1VertSpanUp", offsetof(VehicleDef, passengerViewLimits[0].vertSpanUp), CSPFT_FLOAT},
    {"passenger1VertSpanDown", offsetof(VehicleDef, passengerViewLimits[0].vertSpanDown), CSPFT_FLOAT},
    {"passenger2HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[1].horizSpanLeft), CSPFT_FLOAT},
    {"passenger2HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[1].horizSpanRight), CSPFT_FLOAT},
    {"passenger2VertSpanUp", offsetof(VehicleDef, passengerViewLimits[1].vertSpanUp), CSPFT_FLOAT},
    {"passenger2VertSpanDown", offsetof(VehicleDef, passengerViewLimits[1].vertSpanDown), CSPFT_FLOAT},
    {"passenger3HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[2].horizSpanLeft), CSPFT_FLOAT},
    {"passenger3HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[2].horizSpanRight), CSPFT_FLOAT},
    {"passenger3VertSpanUp", offsetof(VehicleDef, passengerViewLimits[2].vertSpanUp), CSPFT_FLOAT},
    {"passenger3VertSpanDown", offsetof(VehicleDef, passengerViewLimits[2].vertSpanDown), CSPFT_FLOAT},
    {"passenger4HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[3].horizSpanLeft), CSPFT_FLOAT},
    {"passenger4HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[3].horizSpanRight), CSPFT_FLOAT},
    {"passenger4VertSpanUp", offsetof(VehicleDef, passengerViewLimits[3].vertSpanUp), CSPFT_FLOAT},
    {"passenger4VertSpanDown", offsetof(VehicleDef, passengerViewLimits[3].vertSpanDown), CSPFT_FLOAT},
    {"passenger5HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[4].horizSpanLeft), CSPFT_FLOAT},
    {"passenger5HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[4].horizSpanRight), CSPFT_FLOAT},
    {"passenger5VertSpanUp", offsetof(VehicleDef, passengerViewLimits[4].vertSpanUp), CSPFT_FLOAT},
    {"passenger5VertSpanDown", offsetof(VehicleDef, passengerViewLimits[4].vertSpanDown), CSPFT_FLOAT},
    {"passenger6HorizSpanLeft", offsetof(VehicleDef, passengerViewLimits[5].horizSpanLeft), CSPFT_FLOAT},
    {"passenger6HorizSpanRight", offsetof(VehicleDef, passengerViewLimits[5].horizSpanRight), CSPFT_FLOAT},
    {"passenger6VertSpanUp", offsetof(VehicleDef, passengerViewLimits[5].vertSpanUp), CSPFT_FLOAT},
    {"passenger6VertSpanDown", offsetof(VehicleDef, passengerViewLimits[5].vertSpanDown), CSPFT_FLOAT},
    {"turretSpinSnd", offsetof(VehicleDef, sndNames[VEH_TURRET_SPIN_SND]), CSPFT_STRING},
    {"turretStopSnd", offsetof(VehicleDef, sndNames[VEH_TURRET_STOP_SND]), CSPFT_STRING},
    {"wheelRoadNoiseSnd", offsetof(VehicleDef, sndMaterialNames[VEH_WHEEL_ROAD_NOISE]), CSPFT_STRING},
    {"wheelSlidingSnd", offsetof(VehicleDef, sndMaterialNames[VEH_WHEEL_SLIDING]), CSPFT_STRING},
    {"wheelPeelingOutSnd", offsetof(VehicleDef, sndMaterialNames[VEH_WHEEL_PEELING_OUT]), CSPFT_STRING},
    {"futzSnd", offsetof(VehicleDef, futzName), CSPFT_STRING},
    {"futzBlend", offsetof(VehicleDef, futzBlend), CSPFT_FLOAT},
    {"skidSpeedMin", offsetof(VehicleDef, skidSpeedMin), CSPFT_FLOAT},
    {"skidSpeedMax", offsetof(VehicleDef, skidSpeedMax), CSPFT_FLOAT},
    {"animType", offsetof(VehicleDef, animType), CSPFT_INT},
    {"animSet", offsetof(VehicleDef, animSet), CSPFT_STRING},
    {"scriptedAnimationEntry", offsetof(VehicleDef, scriptedAnimationEntry), CSPFT_QBOOLEAN},
    {"mantleAngleFront", offsetof(VehicleDef, mantleAngles[MANTLE_ANGLE_FRONT]), CSPFT_FLOAT},
    {"mantleAngleBack", offsetof(VehicleDef, mantleAngles[MANTLE_ANGLE_BACK]), CSPFT_FLOAT},
    {"mantleAngleLeft", offsetof(VehicleDef, mantleAngles[MANTLE_ANGLE_LEFT]), CSPFT_FLOAT},
    {"mantleAngleRight", offsetof(VehicleDef, mantleAngles[MANTLE_ANGLE_RIGHT]), CSPFT_FLOAT},
    {"driverHideTag1", offsetof(VehicleDef, driverHideTag), CSPFT_SCRIPT_STRING},
    {"extraWheelLeft1", offsetof(VehicleDef, extraWheelTags[FL_WHEEL]), CSPFT_SCRIPT_STRING},
    {"extraWheelRight1", offsetof(VehicleDef, extraWheelTags[FR_WHEEL]), CSPFT_SCRIPT_STRING},
    {"extraWheelLeft2", offsetof(VehicleDef, extraWheelTags[BL_WHEEL]), CSPFT_SCRIPT_STRING},
    {"extraWheelRight2", offsetof(VehicleDef, extraWheelTags[BR_WHEEL]), CSPFT_SCRIPT_STRING},
    {"mod0", offsetof(VehicleDef, attachmentModels[0]), CSPFT_XMODEL},
    {"tag0", offsetof(VehicleDef, attachmentTags[0]), CSPFT_SCRIPT_STRING},
    {"mod1", offsetof(VehicleDef, attachmentModels[1]), CSPFT_XMODEL},
    {"tag1", offsetof(VehicleDef, attachmentTags[1]), CSPFT_SCRIPT_STRING},
    {"mod2", offsetof(VehicleDef, attachmentModels[2]), CSPFT_XMODEL},
    {"tag2", offsetof(VehicleDef, attachmentTags[2]), CSPFT_SCRIPT_STRING},
    {"mod3", offsetof(VehicleDef, attachmentModels[3]), CSPFT_XMODEL},
    {"tag3", offsetof(VehicleDef, attachmentTags[3]), CSPFT_SCRIPT_STRING},
    {"dmod0", offsetof(VehicleDef, deathAttachmentModels[0]), CSPFT_XMODEL},
    {"dtag0", offsetof(VehicleDef, deathAttachmentTags[0]), CSPFT_SCRIPT_STRING},
    {"dmod1", offsetof(VehicleDef, deathAttachmentModels[1]), CSPFT_XMODEL},
    {"dtag1", offsetof(VehicleDef, deathAttachmentTags[1]), CSPFT_SCRIPT_STRING},
    {"dmod2", offsetof(VehicleDef, deathAttachmentModels[2]), CSPFT_XMODEL},
    {"dtag2", offsetof(VehicleDef, deathAttachmentTags[2]), CSPFT_SCRIPT_STRING},
    {"dmod3", offsetof(VehicleDef, deathAttachmentModels[3]), CSPFT_XMODEL},
    {"dtag3", offsetof(VehicleDef, deathAttachmentTags[3]), CSPFT_SCRIPT_STRING},
    {"worldModel", offsetof(VehicleDef, model), CSPFT_XMODEL},
    {"viewModel", offsetof(VehicleDef, viewModel), CSPFT_XMODEL},
    {"deathModel", offsetof(VehicleDef, deathModel), CSPFT_XMODEL},
    {"enemyModel", offsetof(VehicleDef, enemyModel), CSPFT_XMODEL},
    {"modelSwapDelay", offsetof(VehicleDef, modelSwapDelay), CSPFT_FLOAT},
    {"exhaustFx", offsetof(VehicleDef, exhaustFx), CSPFT_FX},
    {"oneExhaust", offsetof(VehicleDef, oneExhaust), CSPFT_QBOOLEAN},
    {"treadFxAsphalt", offsetof(VehicleDef, treadFx[SURF_TYPE_ASPHALT]), CSPFT_FX},
    {"treadFxBark", offsetof(VehicleDef, treadFx[SURF_TYPE_BARK]), CSPFT_FX},
    {"treadFxBrick", offsetof(VehicleDef, treadFx[SURF_TYPE_BRICK]), CSPFT_FX},
    {"treadFxCarpet", offsetof(VehicleDef, treadFx[SURF_TYPE_CARPET]), CSPFT_FX},
    {"treadFxCeramic", offsetof(VehicleDef, treadFx[SURF_TYPE_CERAMIC]), CSPFT_FX},
    {"treadFxCloth", offsetof(VehicleDef, treadFx[SURF_TYPE_CLOTH]), CSPFT_FX},
    {"treadFxConcrete", offsetof(VehicleDef, treadFx[SURF_TYPE_CONCRETE]), CSPFT_FX},
    {"treadFxCushion", offsetof(VehicleDef, treadFx[SURF_TYPE_CUSHION]), CSPFT_FX},
    {"treadFxDefault", offsetof(VehicleDef, treadFx[SURF_TYPE_DEFAULT]), CSPFT_FX},
    {"treadFxDirt", offsetof(VehicleDef, treadFx[SURF_TYPE_DIRT]), CSPFT_FX},
    {"treadFxFlesh", offsetof(VehicleDef, treadFx[SURF_TYPE_FLESH]), CSPFT_FX},
    {"treadFxFoliage", offsetof(VehicleDef, treadFx[SURF_TYPE_FOLIAGE]), CSPFT_FX},
    {"treadFxFruit", offsetof(VehicleDef, treadFx[SURF_TYPE_FRUIT]), CSPFT_FX},
    {"treadFxGlass", offsetof(VehicleDef, treadFx[SURF_TYPE_GLASS]), CSPFT_FX},
    {"treadFxGrass", offsetof(VehicleDef, treadFx[SURF_TYPE_GRASS]), CSPFT_FX},
    {"treadFxGravel", offsetof(VehicleDef, treadFx[SURF_TYPE_GRAVEL]), CSPFT_FX},
    {"treadFxIce", offsetof(VehicleDef, treadFx[SURF_TYPE_ICE]), CSPFT_FX},
    {"treadFxMetal", offsetof(VehicleDef, treadFx[SURF_TYPE_METAL]), CSPFT_FX},
    {"treadFxMud", offsetof(VehicleDef, treadFx[SURF_TYPE_MUD]), CSPFT_FX},
    {"treadFxPaintedMetal", offsetof(VehicleDef, treadFx[SURF_TYPE_PAINTED_METAL]), CSPFT_FX},
    {"treadFxPaper", offsetof(VehicleDef, treadFx[SURF_TYPE_PAPER]), CSPFT_FX},
    {"treadFxPlaster", offsetof(VehicleDef, treadFx[SURF_TYPE_PLASTER]), CSPFT_FX},
    {"treadFxPlastic", offsetof(VehicleDef, treadFx[SURF_TYPE_PLASTIC]), CSPFT_FX},
    {"treadFxRock", offsetof(VehicleDef, treadFx[SURF_TYPE_ROCK]), CSPFT_FX},
    {"treadFxRubber", offsetof(VehicleDef, treadFx[SURF_TYPE_RUBBER]), CSPFT_FX},
    {"treadFxSand", offsetof(VehicleDef, treadFx[SURF_TYPE_SAND]), CSPFT_FX},
    {"treadFxSnow", offsetof(VehicleDef, treadFx[SURF_TYPE_SNOW]), CSPFT_FX},
    {"treadFxWater", offsetof(VehicleDef, treadFx[SURF_TYPE_WATER]), CSPFT_FX},
    {"treadFxWood", offsetof(VehicleDef, treadFx[SURF_TYPE_WOOD]), CSPFT_FX},
    {"deathFxName", offsetof(VehicleDef, deathFx), CSPFT_FX},
    {"deathFxTag", offsetof(VehicleDef, deathFxTag), CSPFT_SCRIPT_STRING},
    {"deathFxSound", offsetof(VehicleDef, deathFxSound), CSPFT_STRING},
    {"lightFxName1", offsetof(VehicleDef, lightFx[0]), CSPFT_FX},
    {"lightFxTag1", offsetof(VehicleDef, lightFxTag[0]), CSPFT_SCRIPT_STRING},
    {"lightFxName2", offsetof(VehicleDef, lightFx[1]), CSPFT_FX},
    {"lightFxTag2", offsetof(VehicleDef, lightFxTag[1]), CSPFT_SCRIPT_STRING},
    {"lightFxName3", offsetof(VehicleDef, lightFx[2]), CSPFT_FX},
    {"lightFxTag3", offsetof(VehicleDef, lightFxTag[2]), CSPFT_SCRIPT_STRING},
    {"lightFxName4", offsetof(VehicleDef, lightFx[3]), CSPFT_FX},
    {"lightFxTag4", offsetof(VehicleDef, lightFxTag[3]), CSPFT_SCRIPT_STRING},
    {"FriendlyLightFxName", offsetof(VehicleDef, friendlyLightFx), CSPFT_FX},
    {"FriendlyLightFxTag", offsetof(VehicleDef, friendlyLightFxTag), CSPFT_SCRIPT_STRING},
    {"EnemyLightFxName", offsetof(VehicleDef, enemyLightFx), CSPFT_FX},
    {"EnemyLightFxTag", offsetof(VehicleDef, enemyLightFxTag), CSPFT_SCRIPT_STRING},
    {"radiusDamageMin", offsetof(VehicleDef, radiusDamageMin), CSPFT_FLOAT},
    {"radiusDamageMax", offsetof(VehicleDef, radiusDamageMax), CSPFT_FLOAT},
    {"radiusDamageRadius", offsetof(VehicleDef, radiusDamageRadius), CSPFT_FLOAT},
    {"shootShock", offsetof(VehicleDef, shootShock), CSPFT_STRING},
    {"shootRumble", offsetof(VehicleDef, shootRumble), CSPFT_STRING},
    {"deathQuakeScale", offsetof(VehicleDef, deathQuakeScale), CSPFT_FLOAT},
    {"deathQuakeDuration", offsetof(VehicleDef, deathQuakeDuration), CSPFT_FLOAT},
    {"deathQuakeRadius", offsetof(VehicleDef, deathQuakeRadius), CSPFT_FLOAT},
    {"rumbleType", offsetof(VehicleDef, rumbleType), CSPFT_STRING},
    {"rumbleScale", offsetof(VehicleDef, rumbleScale), CSPFT_FLOAT},
    {"rumbleDuration", offsetof(VehicleDef, rumbleDuration), CSPFT_FLOAT},
    {"rumbleRadius", offsetof(VehicleDef, rumbleRadius), CSPFT_FLOAT},
    {"rumbleBaseTime", offsetof(VehicleDef, rumbleBaseTime), CSPFT_FLOAT},
    {"rumbleAdditionalTime", offsetof(VehicleDef, rumbleAdditionalTime), CSPFT_FLOAT},
    {"healthDefault", offsetof(VehicleDef, healthDefault), CSPFT_INT},
    {"healthMin", offsetof(VehicleDef, healthMin), CSPFT_INT},
    {"healthMax", offsetof(VehicleDef, healthMax), CSPFT_INT},
    {"team", offsetof(VehicleDef, eTeam), VFT_TEAM},
    {"boostAccelMultiplier", offsetof(VehicleDef, boostAccelMultiplier), CSPFT_INT},
    {"boostDuration", offsetof(VehicleDef, boostDuration), CSPFT_FLOAT},
    {"boostSpeedIncrease", offsetof(VehicleDef, boostSpeedIncrease), CSPFT_FLOAT},
    {"addToCompass", offsetof(VehicleDef, addToCompass), CSPFT_QBOOLEAN},
    {"addToCompassEnemy", offsetof(VehicleDef, addToCompassEnemy), CSPFT_QBOOLEAN},
    {"compassIcon", offsetof(VehicleDef, compassIcon), CSPFT_STRING},
    {"gasButton", offsetof(VehicleDef, gasButtonName), CSPFT_STRING},
    {"reverseBrakeButton", offsetof(VehicleDef, reverseBrakeButtonName), CSPFT_STRING},
    {"handBrakeButton", offsetof(VehicleDef, handBrakeButtonName), CSPFT_STRING},
    {"attackButton", offsetof(VehicleDef, attackButtonName), CSPFT_STRING},
    {"attackSecondaryButton", offsetof(VehicleDef, attackSecondaryButtonName), CSPFT_STRING},
    {"moveUpButton", offsetof(VehicleDef, moveUpButtonName), CSPFT_STRING},
    {"moveDownButton", offsetof(VehicleDef, moveDownButtonName), CSPFT_STRING},
    {"switchSeatButton", offsetof(VehicleDef, switchSeatButtonName), CSPFT_STRING},
    {"boostButton", offsetof(VehicleDef, boostButtonName), CSPFT_STRING},
    {"steerGraph", offsetof(VehicleDef, steerGraphName), CSPFT_STRING},
    {"accelGraph", offsetof(VehicleDef, accelGraphName), CSPFT_STRING},
    {"isNitrous", offsetof(VehicleDef, isNitrous), CSPFT_QBOOLEAN},
    {"isFourWheelSteering", offsetof(VehicleDef, isFourWheelSteering), CSPFT_QBOOLEAN},
    {"useCollmap", offsetof(VehicleDef, useCollmap), CSPFT_QBOOLEAN},
    {"radius", offsetof(VehicleDef, radius), CSPFT_FLOAT},
    {"minHeight", offsetof(VehicleDef, minHeight), CSPFT_FLOAT},
    {"maxHeight", offsetof(VehicleDef, maxHeight), CSPFT_FLOAT},
    {"noDirectionalDamage", offsetof(VehicleDef, noDirectionalDamage), CSPFT_QBOOLEAN},
    {"max_fric_tilt_angle", offsetof(VehicleDef, max_fric_tilt_angle), CSPFT_FLOAT},
    {"max_fric_tilt", offsetof(VehicleDef, max_fric_tilt), CSPFT_FLOAT},
    {"fakeBodyStabilizer", offsetof(VehicleDef, fakeBodyStabilizer), CSPFT_QBOOLEAN},
    {"vehHelicopterBoundsRadius", offsetof(VehicleDef, vehHelicopterBoundsRadius), CSPFT_FLOAT},
    {"vehHelicopterDecelerationFwd", offsetof(VehicleDef, vehHelicopterDecelerationFwd), CSPFT_FLOAT},
    {"vehHelicopterDecelerationSide", offsetof(VehicleDef, vehHelicopterDecelerationSide), CSPFT_FLOAT},
    {"vehHelicopterDecelerationUp", offsetof(VehicleDef, vehHelicopterDecelerationUp), CSPFT_FLOAT},
    {"vehHelicopterTiltFromControllerAxes", offsetof(VehicleDef, vehHelicopterTiltFromControllerAxes), CSPFT_FLOAT},
    {"vehHelicopterTiltFromFwdAndYaw", offsetof(VehicleDef, vehHelicopterTiltFromFwdAndYaw), CSPFT_FLOAT},
    {"vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt", offsetof(VehicleDef, vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt), CSPFT_FLOAT},
    {"vehHelicopterTiltMomentum", offsetof(VehicleDef, vehHelicopterTiltMomentum), CSPFT_FLOAT},
    {"vehHelicopterQuadRotor", offsetof(VehicleDef, vehHelicopterQuadRotor), CSPFT_QBOOLEAN},
    {"vehHelicopterAccelTwardsView", offsetof(VehicleDef, vehHelicopterAccelTwardsView), CSPFT_QBOOLEAN},
    {"maxRotorArmMovementAngle", offsetof(VehicleDef, maxRotorArmMovementAngle), CSPFT_FLOAT},
    {"maxRotorArmRotationAngle", offsetof(VehicleDef, maxRotorArmRotationAngle), CSPFT_FLOAT},
    {"vehHelicopterMaintainHeight", offsetof(VehicleDef, vehHelicopterMaintainHeight), CSPFT_QBOOLEAN},
    {"vehHelicopterMaintainMaxHeight", offsetof(VehicleDef, vehHelicopterMaintainMaxHeight), CSPFT_QBOOLEAN},
    {"vehHelicopterMaintainHeightLimit", offsetof(VehicleDef, vehHelicopterMaintainHeightLimit), CSPFT_FLOAT},
    {"vehHelicopterMaintainHeightAccel", offsetof(VehicleDef, vehHelicopterMaintainHeightAccel), CSPFT_FLOAT},
    {"vehHelicopterMaintainHeightMinimum", offsetof(VehicleDef, vehHelicopterMaintainHeightMinimum), CSPFT_FLOAT},
    {"vehHelicopterMaintainHeightMaximum", offsetof(VehicleDef, vehHelicopterMaintainHeightMaximum), CSPFT_FLOAT},
    {"vehHelicopterMaintainCeilingMinimum", offsetof(VehicleDef, vehHelicopterMaintainCeilingMinimum), CSPFT_FLOAT},
    {"joltVehicle", offsetof(VehicleDef, joltVehicle), CSPFT_QBOOLEAN},
    {"joltVehicleDriver", offsetof(VehicleDef, joltVehicleDriver), CSPFT_QBOOLEAN},
    {"joltMaxTime", offsetof(VehicleDef, joltMaxTime), CSPFT_FLOAT},
    {"joltTime", offsetof(VehicleDef, joltTime), CSPFT_FLOAT},
    {"joltWaves", offsetof(VehicleDef, joltWaves), CSPFT_FLOAT},
    {"joltIntensity", offsetof(VehicleDef, joltIntensity), CSPFT_FLOAT},
    {"nitrous_max_speed", offsetof(VehicleDef, nitrousVehParams.m_speed_max), VFT_MPH_TO_INCHES_PER_SECOND},
    {"nitrous_accel", offsetof(VehicleDef, nitrousVehParams.m_accel_max), VFT_MPH_TO_INCHES_PER_SECOND},
    {"nitrous_auto_handbrake_min_speed", offsetof(VehicleDef, nitrousVehParams.m_auto_hand_brake_min_speed), VFT_MPH_TO_INCHES_PER_SECOND},
    {"nitrous_steer_angle_max", offsetof(VehicleDef, nitrousVehParams.m_steer_angle_max), CSPFT_FLOAT},
    {"nitrous_steer_angle_speed_scale", offsetof(VehicleDef, nitrousVehParams.m_steer_angle_speed_scale), CSPFT_FLOAT},
    {"nitrous_steer_speed", offsetof(VehicleDef, nitrousVehParams.m_steer_speed), CSPFT_FLOAT},
    {"nitrous_body_mass", offsetof(VehicleDef, nitrousVehParams.m_body_mass), VFT_POUNDS_TO_GAME_MASS},
    {"nitrous_wheel_radius", offsetof(VehicleDef, nitrousVehParams.m_wheel_radius), CSPFT_FLOAT},
    {"nitrous_susp_adj", offsetof(VehicleDef, nitrousVehParams.m_susp_adj), CSPFT_FLOAT},
    {"nitrous_susp_spring_k", offsetof(VehicleDef, nitrousVehParams.m_susp_spring_k), CSPFT_FLOAT},
    {"nitrous_susp_damp_k", offsetof(VehicleDef, nitrousVehParams.m_susp_damp_k), CSPFT_FLOAT},
    {"nitrous_susp_hard_limit", offsetof(VehicleDef, nitrousVehParams.m_susp_hard_limit), CSPFT_FLOAT},
    {"nitrous_susp_min_height", offsetof(VehicleDef, nitrousVehParams.m_susp_min_height), CSPFT_FLOAT},
    {"nitrous_tire_damp_hand", offsetof(VehicleDef, nitrousVehParams.m_tire_damp_hand), CSPFT_FLOAT},
    {"nitrous_tire_damp_brake", offsetof(VehicleDef, nitrousVehParams.m_tire_damp_brake), CSPFT_FLOAT},
    {"nitrous_tire_damp_coast", offsetof(VehicleDef, nitrousVehParams.m_tire_damp_coast), CSPFT_FLOAT},
    {"nitrous_tire_fric_brake", offsetof(VehicleDef, nitrousVehParams.m_tire_fric_brake), CSPFT_FLOAT},
    {"nitrous_tire_fric_hand_brake", offsetof(VehicleDef, nitrousVehParams.m_tire_fric_hand_brake), CSPFT_FLOAT},
    {"nitrous_tire_fric_fwd", offsetof(VehicleDef, nitrousVehParams.m_tire_fric_fwd), CSPFT_FLOAT},
    {"nitrous_tire_fric_side", offsetof(VehicleDef, nitrousVehParams.m_tire_fric_side), CSPFT_FLOAT},
    {"nitrous_roll_stability", offsetof(VehicleDef, nitrousVehParams.m_roll_stability), CSPFT_FLOAT},
    {"nitrous_pitch_stability", offsetof(VehicleDef, nitrousVehParams.m_pitch_stability), CSPFT_FLOAT},
    {"nitrous_roll_resistance", offsetof(VehicleDef, nitrousVehParams.m_pitch_roll_resistance), CSPFT_FLOAT},
    {"nitrous_yaw_resistance", offsetof(VehicleDef, nitrousVehParams.m_yaw_resistance), CSPFT_FLOAT},
    {"nitrous_upright_strength", offsetof(VehicleDef, nitrousVehParams.m_upright_strength), CSPFT_FLOAT},
    {"nitrous_tilt_fakey", offsetof(VehicleDef, nitrousVehParams.m_tilt_fakey), CSPFT_FLOAT},
    {"nitrous_traction_type", offsetof(VehicleDef, nitrousVehParams.m_traction_type), VFT_TRACTION_TYPE},
    {"nitrous_peel_out_max_speed", offsetof(VehicleDef, nitrousVehParams.m_peel_out_max_speed), CSPFT_FLOAT},
    {"nitrous_tire_fric_side_max", offsetof(VehicleDef, nitrousVehParams.m_tire_fric_side_max), CSPFT_FLOAT},
    {"nitrous_reverse_scale", offsetof(VehicleDef, nitrousVehParams.m_reverse_scale), CSPFT_FLOAT},
    {"nitrous_water_speed_max", offsetof(VehicleDef, nitrousVehParams.m_water_speed_max), VFT_MPH_TO_INCHES_PER_SECOND},
    {"nitrous_water_accel_max", offsetof(VehicleDef, nitrousVehParams.m_water_accel_max), CSPFT_FLOAT},
    {"nitrous_water_turn_accel", offsetof(VehicleDef, nitrousVehParams.m_water_turn_accel), CSPFT_FLOAT},
    {"nitrous_water_turn_speed_max", offsetof(VehicleDef, nitrousVehParams.m_water_turn_speed_max), CSPFT_FLOAT},
    {"nitrous_boat_ebrake_power", offsetof(VehicleDef, nitrousVehParams.m_water_ebrake_power), CSPFT_FLOAT},
    {"nitrous_bbox_min_x", offsetof(VehicleDef, nitrousVehParams.m_bbox_min.x), CSPFT_FLOAT},
    {"nitrous_bbox_min_y", offsetof(VehicleDef, nitrousVehParams.m_bbox_min.y), CSPFT_FLOAT},
    {"nitrous_bbox_min_z", offsetof(VehicleDef, nitrousVehParams.m_bbox_min.z), CSPFT_FLOAT},
    {"nitrous_bbox_max_x", offsetof(VehicleDef, nitrousVehParams.m_bbox_max.x), CSPFT_FLOAT},
    {"nitrous_bbox_max_y", offsetof(VehicleDef, nitrousVehParams.m_bbox_max.y), CSPFT_FLOAT},
    {"nitrous_bbox_max_z", offsetof(VehicleDef, nitrousVehParams.m_bbox_max.z), CSPFT_FLOAT},
    {"nitrous_mass_center_offset_x", offsetof(VehicleDef, nitrousVehParams.m_mass_center_offset.x), CSPFT_FLOAT},
    {"nitrous_mass_center_offset_y", offsetof(VehicleDef, nitrousVehParams.m_mass_center_offset.y), CSPFT_FLOAT},
    {"nitrous_mass_center_offset_z", offsetof(VehicleDef, nitrousVehParams.m_mass_center_offset.z), CSPFT_FLOAT},
    {"nitrous_buoyancybox_min_x", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_min.x), CSPFT_FLOAT},
    {"nitrous_buoyancybox_min_y", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_min.y), CSPFT_FLOAT},
    {"nitrous_buoyancybox_min_z", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_min.z), CSPFT_FLOAT},
    {"nitrous_buoyancybox_max_x", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_max.x), CSPFT_FLOAT},
    {"nitrous_buoyancybox_max_y", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_max.y), CSPFT_FLOAT},
    {"nitrous_buoyancybox_max_z", offsetof(VehicleDef, nitrousVehParams.m_buoyancybox_max.z), CSPFT_FLOAT},
    {"nitrous_boat_speed_rise", offsetof(VehicleDef, nitrousVehParams.m_boat_speed_rise), CSPFT_FLOAT},
    {"nitrous_boat_speed_tilt", offsetof(VehicleDef, nitrousVehParams.m_boat_speed_tilt), CSPFT_FLOAT},
    {"nitrous_boat_motor_offset_x", offsetof(VehicleDef, nitrousVehParams.m_boat_motor_offset.x), CSPFT_FLOAT},
    {"nitrous_boat_motor_offset_y", offsetof(VehicleDef, nitrousVehParams.m_boat_motor_offset.y), CSPFT_FLOAT},
    {"nitrous_boat_motor_offset_z", offsetof(VehicleDef, nitrousVehParams.m_boat_motor_offset.z), CSPFT_FLOAT},
    {"nitrous_boat_side_fric", offsetof(VehicleDef, nitrousVehParams.m_boat_side_fric_scale), CSPFT_FLOAT},
    {"nitrous_boat_forward_fric", offsetof(VehicleDef, nitrousVehParams.m_boat_forward_fric_scale), CSPFT_FLOAT},
    {"nitrous_boat_vertical_fric", offsetof(VehicleDef, nitrousVehParams.m_boat_vertical_fric_scale), CSPFT_FLOAT},
    {"nitrous_drive_on_walls", offsetof(VehicleDef, nitrousVehParams.m_drive_on_walls), CSPFT_BOOL},
    {"nitrous_linear_drag_scale", offsetof(VehicleDef, nitrousVehParams.m_linear_drag_scale), CSPFT_FLOAT},
    {"nitrous_angular_drag_scale", offsetof(VehicleDef, nitrousVehParams.m_angular_drag_scale), CSPFT_FLOAT},
    {"nitrous_jump_force", offsetof(VehicleDef, nitrousVehParams.m_jump_force), CSPFT_FLOAT},
    {"p_driveby_sound_radius", offsetof(VehicleDef, driveBySoundRadius[0]), CSPFT_FLOAT},
    {"s_driveby_sound_radius", offsetof(VehicleDef, driveBySoundRadius[1]), CSPFT_FLOAT},
    {"p_drive_by_sound_apex0", offsetof(VehicleDef, driveBySounds[0].apex), CSPFT_INT},
    {"p_drive_by_sound_name0", offsetof(VehicleDef, driveBySounds[0].name), CSPFT_STRING},
    {"p_drive_by_sound_apex1", offsetof(VehicleDef, driveBySounds[1].apex), CSPFT_INT},
    {"p_drive_by_sound_name1", offsetof(VehicleDef, driveBySounds[1].name), CSPFT_STRING},
    {"p_drive_by_sound_apex2", offsetof(VehicleDef, driveBySounds[2].apex), CSPFT_INT},
    {"p_drive_by_sound_name2", offsetof(VehicleDef, driveBySounds[2].name), CSPFT_STRING},
    {"p_drive_by_sound_apex3", offsetof(VehicleDef, driveBySounds[3].apex), CSPFT_INT},
    {"p_drive_by_sound_name3", offsetof(VehicleDef, driveBySounds[3].name), CSPFT_STRING},
    {"p_drive_by_sound_apex4", offsetof(VehicleDef, driveBySounds[4].apex), CSPFT_INT},
    {"p_drive_by_sound_name4", offsetof(VehicleDef, driveBySounds[4].name), CSPFT_STRING},
    {"p_drive_by_sound_apex5", offsetof(VehicleDef, driveBySounds[5].apex), CSPFT_INT},
    {"p_drive_by_sound_name5", offsetof(VehicleDef, driveBySounds[5].name), CSPFT_STRING},
    {"p_drive_by_sound_apex6", offsetof(VehicleDef, driveBySounds[6].apex), CSPFT_INT},
    {"p_drive_by_sound_name6", offsetof(VehicleDef, driveBySounds[6].name), CSPFT_STRING},
    {"p_drive_by_sound_apex7", offsetof(VehicleDef, driveBySounds[7].apex), CSPFT_INT},
    {"p_drive_by_sound_name7", offsetof(VehicleDef, driveBySounds[7].name), CSPFT_STRING},
    {"p_drive_by_sound_apex8", offsetof(VehicleDef, driveBySounds[8].apex), CSPFT_INT},
    {"p_drive_by_sound_name8", offsetof(VehicleDef, driveBySounds[8].name), CSPFT_STRING},
    {"p_drive_by_sound_apex9", offsetof(VehicleDef, driveBySounds[9].apex), CSPFT_INT},
    {"p_drive_by_sound_name9", offsetof(VehicleDef, driveBySounds[9].name), CSPFT_STRING},
    {"p_drive_by_sound_apex10", offsetof(VehicleDef, driveBySounds[10].apex), CSPFT_INT},
    {"p_drive_by_sound_name10", offsetof(VehicleDef, driveBySounds[10].name), CSPFT_STRING},
    {"p_drive_by_sound_apex11", offsetof(VehicleDef, driveBySounds[11].apex), CSPFT_INT},
    {"p_drive_by_sound_name11", offsetof(VehicleDef, driveBySounds[11].name), CSPFT_STRING},
    {"p_drive_by_sound_apex12", offsetof(VehicleDef, driveBySounds[12].apex), CSPFT_INT},
    {"p_drive_by_sound_name12", offsetof(VehicleDef, driveBySounds[12].name), CSPFT_STRING},
    {"p_drive_by_sound_apex13", offsetof(VehicleDef, driveBySounds[13].apex), CSPFT_INT},
    {"p_drive_by_sound_name13", offsetof(VehicleDef, driveBySounds[13].name), CSPFT_STRING},
    {"p_drive_by_sound_apex14", offsetof(VehicleDef, driveBySounds[14].apex), CSPFT_INT},
    {"p_drive_by_sound_name14", offsetof(VehicleDef, driveBySounds[14].name), CSPFT_STRING},
    {"p_drive_by_sound_apex15", offsetof(VehicleDef, driveBySounds[15].apex), CSPFT_INT},
    {"p_drive_by_sound_name15", offsetof(VehicleDef, driveBySounds[15].name), CSPFT_STRING},
    {"p_drive_by_sound_apex16", offsetof(VehicleDef, driveBySounds[16].apex), CSPFT_INT},
    {"p_drive_by_sound_name16", offsetof(VehicleDef, driveBySounds[16].name), CSPFT_STRING},
    {"p_drive_by_sound_apex17", offsetof(VehicleDef, driveBySounds[17].apex), CSPFT_INT},
    {"p_drive_by_sound_name17", offsetof(VehicleDef, driveBySounds[17].name), CSPFT_STRING},
    {"p_drive_by_sound_apex18", offsetof(VehicleDef, driveBySounds[18].apex), CSPFT_INT},
    {"p_drive_by_sound_name18", offsetof(VehicleDef, driveBySounds[18].name), CSPFT_STRING},
    {"p_drive_by_sound_apex19", offsetof(VehicleDef, driveBySounds[19].apex), CSPFT_INT},
    {"p_drive_by_sound_name19", offsetof(VehicleDef, driveBySounds[19].name), CSPFT_STRING},
    {"s_drive_by_sound_apex0", offsetof(VehicleDef, driveBySounds[20].apex), CSPFT_INT},
    {"s_drive_by_sound_name0", offsetof(VehicleDef, driveBySounds[20].name), CSPFT_STRING},
    {"s_drive_by_sound_apex1", offsetof(VehicleDef, driveBySounds[21].apex), CSPFT_INT},
    {"s_drive_by_sound_name1", offsetof(VehicleDef, driveBySounds[21].name), CSPFT_STRING},
    {"s_drive_by_sound_apex2", offsetof(VehicleDef, driveBySounds[22].apex), CSPFT_INT},
    {"s_drive_by_sound_name2", offsetof(VehicleDef, driveBySounds[22].name), CSPFT_STRING},
    {"s_drive_by_sound_apex3", offsetof(VehicleDef, driveBySounds[23].apex), CSPFT_INT},
    {"s_drive_by_sound_name3", offsetof(VehicleDef, driveBySounds[23].name), CSPFT_STRING},
    {"s_drive_by_sound_apex4", offsetof(VehicleDef, driveBySounds[24].apex), CSPFT_INT},
    {"s_drive_by_sound_name4", offsetof(VehicleDef, driveBySounds[24].name), CSPFT_STRING},
    {"s_drive_by_sound_apex5", offsetof(VehicleDef, driveBySounds[25].apex), CSPFT_INT},
    {"s_drive_by_sound_name5", offsetof(VehicleDef, driveBySounds[25].name), CSPFT_STRING},
    {"s_drive_by_sound_apex6", offsetof(VehicleDef, driveBySounds[26].apex), CSPFT_INT},
    {"s_drive_by_sound_name6", offsetof(VehicleDef, driveBySounds[26].name), CSPFT_STRING},
    {"s_drive_by_sound_apex7", offsetof(VehicleDef, driveBySounds[27].apex), CSPFT_INT},
    {"s_drive_by_sound_name7", offsetof(VehicleDef, driveBySounds[27].name), CSPFT_STRING},
    {"s_drive_by_sound_apex8", offsetof(VehicleDef, driveBySounds[28].apex), CSPFT_INT},
    {"s_drive_by_sound_name8", offsetof(VehicleDef, driveBySounds[28].name), CSPFT_STRING},
    {"s_drive_by_sound_apex9", offsetof(VehicleDef, driveBySounds[29].apex), CSPFT_INT},
    {"s_drive_by_sound_name9", offsetof(VehicleDef, driveBySounds[29].name), CSPFT_STRING},
    {"s_drive_by_sound_apex10", offsetof(VehicleDef, driveBySounds[30].apex), CSPFT_INT},
    {"s_drive_by_sound_name10", offsetof(VehicleDef, driveBySounds[30].name), CSPFT_STRING},
    {"s_drive_by_sound_apex11", offsetof(VehicleDef, driveBySounds[31].apex), CSPFT_INT},
    {"s_drive_by_sound_name11", offsetof(VehicleDef, driveBySounds[31].name), CSPFT_STRING},
    {"s_drive_by_sound_apex12", offsetof(VehicleDef, driveBySounds[32].apex), CSPFT_INT},
    {"s_drive_by_sound_name12", offsetof(VehicleDef, driveBySounds[32].name), CSPFT_STRING},
    {"s_drive_by_sound_apex13", offsetof(VehicleDef, driveBySounds[33].apex), CSPFT_INT},
    {"s_drive_by_sound_name13", offsetof(VehicleDef, driveBySounds[33].name), CSPFT_STRING},
    {"s_drive_by_sound_apex14", offsetof(VehicleDef, driveBySounds[34].apex), CSPFT_INT},
    {"s_drive_by_sound_name14", offsetof(VehicleDef, driveBySounds[34].name), CSPFT_STRING},
    {"s_drive_by_sound_apex15", offsetof(VehicleDef, driveBySounds[35].apex), CSPFT_INT},
    {"s_drive_by_sound_name15", offsetof(VehicleDef, driveBySounds[35].name), CSPFT_STRING},
    {"s_drive_by_sound_apex16", offsetof(VehicleDef, driveBySounds[36].apex), CSPFT_INT},
    {"s_drive_by_sound_name16", offsetof(VehicleDef, driveBySounds[36].name), CSPFT_STRING},
    {"s_drive_by_sound_apex17", offsetof(VehicleDef, driveBySounds[37].apex), CSPFT_INT},
    {"s_drive_by_sound_name17", offsetof(VehicleDef, driveBySounds[37].name), CSPFT_STRING},
    {"s_drive_by_sound_apex18", offsetof(VehicleDef, driveBySounds[38].apex), CSPFT_INT},
    {"s_drive_by_sound_name18", offsetof(VehicleDef, driveBySounds[38].name), CSPFT_STRING},
    {"s_drive_by_sound_apex19", offsetof(VehicleDef, driveBySounds[39].apex), CSPFT_INT},
    {"s_drive_by_sound_name19", offsetof(VehicleDef, driveBySounds[39].name), CSPFT_STRING},
    {"doFootSteps", offsetof(VehicleDef, doFootSteps), CSPFT_BOOL},
    {"isSentient", offsetof(VehicleDef, isSentient), CSPFT_BOOL},
    {"idleRpms", offsetof(VehicleDef, engine.idleRpms), CSPFT_FLOAT},
    {"maxRpms", offsetof(VehicleDef, engine.maxRpms), CSPFT_FLOAT},
    {"maxTorque", offsetof(VehicleDef, engine.maxTorque), CSPFT_FLOAT},
    {"brakingCoeff", offsetof(VehicleDef, engine.brakingCoeff), CSPFT_FLOAT},
    {"onLoadFadeInStart", offsetof(VehicleDef, engine.loadFadeParams.x), CSPFT_FLOAT},
    {"onLoadFadeInEnd", offsetof(VehicleDef, engine.loadFadeParams.y), CSPFT_FLOAT},
    {"offLoadFadeOutStart", offsetof(VehicleDef, engine.loadFadeParams.z), CSPFT_FLOAT},
    {"offLoadFadeOutEnd", offsetof(VehicleDef, engine.loadFadeParams.w), CSPFT_FLOAT},
    {"loadScale", offsetof(VehicleDef, engine.loadScale), CSPFT_FLOAT},
    {"loadSmooting", offsetof(VehicleDef, engine.loadSmoothing), CSPFT_FLOAT},
    {"throttleLag", offsetof(VehicleDef, engine.throttleLag), CSPFT_FLOAT},
    {"pitchScale", offsetof(VehicleDef, engine.pitchScale), CSPFT_FLOAT},
    {"on_engsnd_name0", offsetof(VehicleDef, engine.onload[0].name), CSPFT_STRING},
    {"on_engsnd_fade_in_start0", offsetof(VehicleDef, engine.onload[0].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_in_end0", offsetof(VehicleDef, engine.onload[0].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_start0", offsetof(VehicleDef, engine.onload[0].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_end0", offsetof(VehicleDef, engine.onload[0].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"on_engsnd_pitch_ref0", offsetof(VehicleDef, engine.onload[0].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"on_engsnd_name1", offsetof(VehicleDef, engine.onload[1].name), CSPFT_STRING},
    {"on_engsnd_fade_in_start1", offsetof(VehicleDef, engine.onload[1].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_in_end1", offsetof(VehicleDef, engine.onload[1].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_start1", offsetof(VehicleDef, engine.onload[1].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_end1", offsetof(VehicleDef, engine.onload[1].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"on_engsnd_pitch_ref1", offsetof(VehicleDef, engine.onload[1].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"on_engsnd_name2", offsetof(VehicleDef, engine.onload[2].name), CSPFT_STRING},
    {"on_engsnd_fade_in_start2", offsetof(VehicleDef, engine.onload[2].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_in_end2", offsetof(VehicleDef, engine.onload[2].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_start2", offsetof(VehicleDef, engine.onload[2].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_end2", offsetof(VehicleDef, engine.onload[2].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"on_engsnd_pitch_ref2", offsetof(VehicleDef, engine.onload[2].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"on_engsnd_name3", offsetof(VehicleDef, engine.onload[3].name), CSPFT_STRING},
    {"on_engsnd_fade_in_start3", offsetof(VehicleDef, engine.onload[3].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_in_end3", offsetof(VehicleDef, engine.onload[3].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_start3", offsetof(VehicleDef, engine.onload[3].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_end3", offsetof(VehicleDef, engine.onload[3].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"on_engsnd_pitch_ref3", offsetof(VehicleDef, engine.onload[3].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"on_engsnd_name4", offsetof(VehicleDef, engine.onload[4].name), CSPFT_STRING},
    {"on_engsnd_fade_in_start4", offsetof(VehicleDef, engine.onload[4].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_in_end4", offsetof(VehicleDef, engine.onload[4].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_start4", offsetof(VehicleDef, engine.onload[4].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"on_engsnd_fade_out_end4", offsetof(VehicleDef, engine.onload[4].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"on_engsnd_pitch_ref4", offsetof(VehicleDef, engine.onload[4].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"off_engsnd_name0", offsetof(VehicleDef, engine.offload[0].name), CSPFT_STRING},
    {"off_engsnd_fade_in_start0", offsetof(VehicleDef, engine.offload[0].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_in_end0", offsetof(VehicleDef, engine.offload[0].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_start0", offsetof(VehicleDef, engine.offload[0].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_end0", offsetof(VehicleDef, engine.offload[0].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"off_engsnd_pitch_ref0", offsetof(VehicleDef, engine.offload[0].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"off_engsnd_name1", offsetof(VehicleDef, engine.offload[1].name), CSPFT_STRING},
    {"off_engsnd_fade_in_start1", offsetof(VehicleDef, engine.offload[1].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_in_end1", offsetof(VehicleDef, engine.offload[1].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_start1", offsetof(VehicleDef, engine.offload[1].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_end1", offsetof(VehicleDef, engine.offload[1].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"off_engsnd_pitch_ref1", offsetof(VehicleDef, engine.offload[1].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"off_engsnd_name2", offsetof(VehicleDef, engine.offload[2].name), CSPFT_STRING},
    {"off_engsnd_fade_in_start2", offsetof(VehicleDef, engine.offload[2].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_in_end2", offsetof(VehicleDef, engine.offload[2].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_start2", offsetof(VehicleDef, engine.offload[2].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_end2", offsetof(VehicleDef, engine.offload[2].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"off_engsnd_pitch_ref2", offsetof(VehicleDef, engine.offload[2].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"off_engsnd_name3", offsetof(VehicleDef, engine.offload[3].name), CSPFT_STRING},
    {"off_engsnd_fade_in_start3", offsetof(VehicleDef, engine.offload[3].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_in_end3", offsetof(VehicleDef, engine.offload[3].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_start3", offsetof(VehicleDef, engine.offload[3].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_end3", offsetof(VehicleDef, engine.offload[3].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"off_engsnd_pitch_ref3", offsetof(VehicleDef, engine.offload[3].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"off_engsnd_name4", offsetof(VehicleDef, engine.offload[4].name), CSPFT_STRING},
    {"off_engsnd_fade_in_start4", offsetof(VehicleDef, engine.offload[4].params[VEH_ENGINESND_FADE_IN_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_in_end4", offsetof(VehicleDef, engine.offload[4].params[VEH_ENGINESND_FADE_IN_END]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_start4", offsetof(VehicleDef, engine.offload[4].params[VEH_ENGINESND_FADE_OUT_START]), CSPFT_FLOAT},
    {"off_engsnd_fade_out_end4", offsetof(VehicleDef, engine.offload[4].params[VEH_ENGINESND_FADE_OUT_END]), CSPFT_FLOAT},
    {"off_engsnd_pitch_ref4", offsetof(VehicleDef, engine.offload[4].params[VEH_ENGINESND_PITCH_REF]), CSPFT_FLOAT},
    {"numGears", offsetof(VehicleDef, engine.numGears), CSPFT_INT},
    {"loopLastGear", offsetof(VehicleDef, engine.loopLastGear), CSPFT_BOOL},
    {"gear_min_rpm0", offsetof(VehicleDef, engine.gears[0].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm0", offsetof(VehicleDef, engine.gears[0].maxRPM), CSPFT_FLOAT},
    {"gear_ratio0", offsetof(VehicleDef, engine.gears[0].ratio), CSPFT_FLOAT},
    {"gear_min_rpm1", offsetof(VehicleDef, engine.gears[1].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm1", offsetof(VehicleDef, engine.gears[1].maxRPM), CSPFT_FLOAT},
    {"gear_ratio1", offsetof(VehicleDef, engine.gears[1].ratio), CSPFT_FLOAT},
    {"gear_min_rpm2", offsetof(VehicleDef, engine.gears[2].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm2", offsetof(VehicleDef, engine.gears[2].maxRPM), CSPFT_FLOAT},
    {"gear_ratio2", offsetof(VehicleDef, engine.gears[2].ratio), CSPFT_FLOAT},
    {"gear_min_rpm3", offsetof(VehicleDef, engine.gears[3].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm3", offsetof(VehicleDef, engine.gears[3].maxRPM), CSPFT_FLOAT},
    {"gear_ratio3", offsetof(VehicleDef, engine.gears[3].ratio), CSPFT_FLOAT},
    {"gear_min_rpm4", offsetof(VehicleDef, engine.gears[4].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm4", offsetof(VehicleDef, engine.gears[4].maxRPM), CSPFT_FLOAT},
    {"gear_ratio4", offsetof(VehicleDef, engine.gears[4].ratio), CSPFT_FLOAT},
    {"gear_min_rpm5", offsetof(VehicleDef, engine.gears[5].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm5", offsetof(VehicleDef, engine.gears[5].maxRPM), CSPFT_FLOAT},
    {"gear_ratio5", offsetof(VehicleDef, engine.gears[5].ratio), CSPFT_FLOAT},
    {"gear_min_rpm6", offsetof(VehicleDef, engine.gears[6].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm6", offsetof(VehicleDef, engine.gears[6].maxRPM), CSPFT_FLOAT},
    {"gear_ratio6", offsetof(VehicleDef, engine.gears[6].ratio), CSPFT_FLOAT},
    {"gear_min_rpm7", offsetof(VehicleDef, engine.gears[7].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm7", offsetof(VehicleDef, engine.gears[7].maxRPM), CSPFT_FLOAT},
    {"gear_ratio7", offsetof(VehicleDef, engine.gears[7].ratio), CSPFT_FLOAT},
    {"gear_min_rpm8", offsetof(VehicleDef, engine.gears[8].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm8", offsetof(VehicleDef, engine.gears[8].maxRPM), CSPFT_FLOAT},
    {"gear_ratio8", offsetof(VehicleDef, engine.gears[8].ratio), CSPFT_FLOAT},
    {"gear_min_rpm9", offsetof(VehicleDef, engine.gears[9].minRPM), CSPFT_FLOAT},
    {"gear_max_rpm9", offsetof(VehicleDef, engine.gears[9].maxRPM), CSPFT_FLOAT},
    {"gear_ratio9", offsetof(VehicleDef, engine.gears[9].ratio), CSPFT_FLOAT},
    {"csvInclude", offsetof(VehicleDef, csvInclude), CSPFT_STRING},
    {"antenna1SpringK", offsetof(VehicleDef, antenna[0].springK), CSPFT_FLOAT},
    {"antenna1Damp", offsetof(VehicleDef, antenna[0].damp), CSPFT_FLOAT},
    {"antenna1Length", offsetof(VehicleDef, antenna[0].length), CSPFT_FLOAT},
    {"antenna1Gravity", offsetof(VehicleDef, antenna[0].gravity), CSPFT_FLOAT},
    {"antenna2SpringK", offsetof(VehicleDef, antenna[1].springK), CSPFT_FLOAT},
    {"antenna2Damp", offsetof(VehicleDef, antenna[1].damp), CSPFT_FLOAT},
    {"antenna2Length", offsetof(VehicleDef, antenna[1].length), CSPFT_FLOAT},
    {"antenna2Gravity", offsetof(VehicleDef, antenna[1].gravity), CSPFT_FLOAT},
    {"customFloat0", offsetof(VehicleDef, customFloat0), CSPFT_FLOAT},
    {"customFloat1", offsetof(VehicleDef, customFloat1), CSPFT_FLOAT},
    {"customFloat2", offsetof(VehicleDef, customFloat2), CSPFT_FLOAT},
    {"customBool0", offsetof(VehicleDef, customBool0), CSPFT_BOOL},
    {"customBool1", offsetof(VehicleDef, customBool1), CSPFT_BOOL},
    {"customBool2", offsetof(VehicleDef, customBool2), CSPFT_BOOL},
};

namespace T6
{
    class InfoStringFromVehicleConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<VehicleFieldType>(field.iFieldType))
            {
            case VFT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleClassNames, std::extent<decltype(s_vehicleClassNames)>::value);
                break;

            case VFT_CAMERAMODE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleCameraModes, std::extent<decltype(s_vehicleCameraModes)>::value);
                break;

            case VFT_TRACTION_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_tractionTypeNames, std::extent<decltype(s_tractionTypeNames)>::value);
                break;

            case VFT_MPH_TO_INCHES_PER_SECOND:
                {
                    const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    m_info_string.SetValueForKey(std::string(field.szName), std::to_string(*num / 17.6f));
                    break;
                }

            case VFT_POUNDS_TO_GAME_MASS:
                {
                    const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    m_info_string.SetValueForKey(std::string(field.szName), std::to_string(*num / 0.001f));
                    break;
                }

            case VFT_TEAM:
                {
                    const auto* num = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    switch (*num)
                    {
                    case TEAM_AXIS:
                        m_info_string.SetValueForKey(std::string(field.szName), "axis");
                        break;

                    case TEAM_ALLIES:
                        m_info_string.SetValueForKey(std::string(field.szName), "allies");
                        break;

                    default:
                        assert(false);
                        m_info_string.SetValueForKey(std::string(field.szName), "");
                        break;
                    }
                    break;
                }

            case VFT_KEY_BINDING:
            case VFT_GRAPH:
            case VFT_WIIUCONTROLOVERRIDE:
            case VFT_NUM:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromVehicleConverter(const VehicleDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperVehicle::CreateInfoString(XAssetInfo<VehicleDef>* asset)
{
    InfoStringFromVehicleConverter converter(asset->Asset(), vehicle_fields, std::extent<decltype(vehicle_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset->m_zone->m_script_strings.size());
            if (scrStr >= asset->m_zone->m_script_strings.size())
                return "";

            return asset->m_zone->m_script_strings[scrStr];
        });

    return converter.Convert();
}

bool AssetDumperVehicle::ShouldDump(XAssetInfo<VehicleDef>* asset)
{
    return true;
}

bool AssetDumperVehicle::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperVehicle::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperVehicle::GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset)
{
    return "vehicles/" + asset->m_name;
}

GdtEntry AssetDumperVehicle::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperVehicle::DumpRaw(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}
