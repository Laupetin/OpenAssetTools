#include "Game/IW5/Vehicle/VehicleDumperIW5.h"

#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace IW5;

namespace
{
    constexpr auto VEHICLE_NAME = "test_vehicle_iw5";
    constexpr auto PHYS_PRESET_NAME = "test_phys_preset_iw5";
    constexpr auto WEAPON_NAME = "test_turret_weapon_iw5";
    constexpr auto EXPLODE_FX_NAME = "test_trophy_explode_fx";
    constexpr auto FLASH_FX_NAME = "test_trophy_flash_fx";
    constexpr auto MATERIAL_NAME = "test_compass_alt_material";
    constexpr auto ALT_IDLE_SOUND_NAME = "test_alt_idle_sound";
    constexpr auto SURFACE_SOUND_PREFIX = "test_iw5_surface";

    struct TestVehicle
    {
        std::unique_ptr<PhysPreset> physPreset;
        std::unique_ptr<WeaponCompleteDef> weapon;
        std::unique_ptr<FxEffectDef> explodeFx;
        std::unique_ptr<FxEffectDef> flashFx;
        std::unique_ptr<Material> material;
        std::unique_ptr<snd_alias_list_name> altIdleSound;
        std::unique_ptr<VehicleDef> vehicle;
    };

    TestVehicle CreateTestVehicle()
    {
        TestVehicle result{
            .physPreset = std::make_unique<PhysPreset>(),
            .weapon = std::make_unique<WeaponCompleteDef>(),
            .explodeFx = std::make_unique<FxEffectDef>(),
            .flashFx = std::make_unique<FxEffectDef>(),
            .material = std::make_unique<Material>(),
            .altIdleSound = std::make_unique<snd_alias_list_name>(),
            .vehicle = std::make_unique<VehicleDef>(),
        };

        result.physPreset->name = PHYS_PRESET_NAME;
        result.weapon->szInternalName = WEAPON_NAME;
        result.explodeFx->name = EXPLODE_FX_NAME;
        result.flashFx->name = FLASH_FX_NAME;
        result.material->info.name = MATERIAL_NAME;
        result.altIdleSound->soundName = ALT_IDLE_SOUND_NAME;

        result.vehicle->name = VEHICLE_NAME;
        result.vehicle->type = VEH_UGV;
        result.vehicle->health = 900;
        result.vehicle->topSpeed = 176.0f;
        result.vehicle->suspensionTravel = 5.5f;
        result.vehicle->fakeBodyStabilizer = true;
        result.vehicle->vehPhysDef.physicsEnabled = true;
        result.vehicle->vehPhysDef.physPresetName = PHYS_PRESET_NAME;
        result.vehicle->vehPhysDef.physPreset = result.physPreset.get();
        result.vehicle->vehPhysDef.steeringAxle = VEH_AXLE_ALL;
        result.vehicle->vehHelicopterBoundsRadius = 123.25f;
        result.vehicle->camRelativeControl = true;
        result.vehicle->vehCam_zOffsetMode = VEHCAM_ZMODE_VIEW;
        result.vehicle->vehCam_zOffsetMode3P = VEHCAM_ZMODE_VEHICLE;
        result.vehicle->turretWeaponName = WEAPON_NAME;
        result.vehicle->turretWeapon = result.weapon.get();
        result.vehicle->turretHorizResistLeft = 27.5f;
        result.vehicle->trophyExplodeFx = result.explodeFx.get();
        result.vehicle->trophyFlashFx = result.flashFx.get();
        result.vehicle->compassFriendlyAltIcon = result.material.get();
        result.vehicle->idleLowSndAlt.name = result.altIdleSound.get();
        result.vehicle->surfaceSndPrefix = SURFACE_SOUND_PREFIX;

        return result;
    }

    std::string DumpVehicle(VehicleDef& vehicle)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::IW5, GamePlatform::PC);
        vehicle.trophyTags[0] = dumpingZone.m_script_strings.AddOrGetScriptString("tag_trophy_iw5");
        vehicle.audioOriginTag = dumpingZone.m_script_strings.AddOrGetScriptString("tag_audio_origin");
        vehicle.audioOriginTagAlt = dumpingZone.m_script_strings.AddOrGetScriptString("tag_audio_origin_alt");
        dumpingZone.m_pools.AddAsset(ASSET_TYPE_VEHICLE, vehicle.name, &vehicle, {}, {}, {});

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        vehicle::DumperIW5 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("vehicles/test_vehicle_iw5");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("Vehicle dumper serializes raw fields (IW5)", "[iw5][vehicle][system]")
    {
        auto testVehicle = CreateTestVehicle();
        std::istringstream dumpedStream(DumpVehicle(*testVehicle.vehicle));

        InfoString infoString;
        REQUIRE(infoString.FromStream("VEHICLEFILE", dumpedStream));
        REQUIRE(infoString.GetValueForKey("type") == "ugv");
        REQUIRE(infoString.GetValueForKey("topSpeed") == "10.000000");
        REQUIRE(infoString.GetValueForKey("fakeBodyStabilizer") == "1");
        REQUIRE(infoString.GetValueForKey("physicsPreset") == PHYS_PRESET_NAME);
        REQUIRE(infoString.GetValueForKey("vehHelicopterBoundsRadius") == "123.25");
        REQUIRE(infoString.GetValueForKey("camRelativeControl") == "1");
        REQUIRE(infoString.GetValueForKey("vehCam_zOffsetMode") == "view");
        REQUIRE(infoString.GetValueForKey("vehCam_zOffsetMode3P") == "vehicle");
        REQUIRE(infoString.GetValueForKey("turretHorizResistLeft") == "27.5");
        REQUIRE(infoString.GetValueForKey("trophyExplodeFx") == EXPLODE_FX_NAME);
        REQUIRE(infoString.GetValueForKey("trophyFlashFx") == FLASH_FX_NAME);
        REQUIRE(infoString.GetValueForKey("compassFriendlyAltIcon") == MATERIAL_NAME);
        REQUIRE(infoString.GetValueForKey("audioOriginTag") == "tag_audio_origin");
        REQUIRE(infoString.GetValueForKey("audioOriginTagAlt") == "tag_audio_origin_alt");
        REQUIRE(infoString.GetValueForKey("lowIdleSndAlt") == ALT_IDLE_SOUND_NAME);
    }
} // namespace
