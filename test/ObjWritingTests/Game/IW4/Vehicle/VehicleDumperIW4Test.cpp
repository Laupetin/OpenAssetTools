#include "Game/IW4/Vehicle/VehicleDumperIW4.h"

#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>

using namespace IW4;

namespace
{
    constexpr auto VEHICLE_NAME = "test_vehicle";
    constexpr auto PHYS_PRESET_NAME = "test_phys_preset";
    constexpr auto WEAPON_NAME = "test_turret_weapon";
    constexpr auto MATERIAL_NAME = "test_compass_material";
    constexpr auto IDLE_SOUND_NAME = "test_idle_sound";
    constexpr auto SURFACE_SOUND_PREFIX = "test_surface";

    struct TestVehicle
    {
        std::unique_ptr<PhysPreset> physPreset;
        std::unique_ptr<WeaponCompleteDef> weapon;
        std::unique_ptr<Material> material;
        std::unique_ptr<snd_alias_list_name> idleSound;
        std::unique_ptr<VehicleDef> vehicle;
    };

    TestVehicle CreateTestVehicle()
    {
        TestVehicle result{
            .physPreset = std::make_unique<PhysPreset>(),
            .weapon = std::make_unique<WeaponCompleteDef>(),
            .material = std::make_unique<Material>(),
            .idleSound = std::make_unique<snd_alias_list_name>(),
            .vehicle = std::make_unique<VehicleDef>(),
        };

        result.physPreset->name = PHYS_PRESET_NAME;
        result.weapon->szInternalName = WEAPON_NAME;
        result.material->info.name = MATERIAL_NAME;
        result.idleSound->soundName = IDLE_SOUND_NAME;

        result.vehicle->name = VEHICLE_NAME;
        result.vehicle->type = VEH_TANK;
        result.vehicle->health = 750;
        result.vehicle->topSpeed = 35.2f;
        result.vehicle->suspensionTravel = 3.5f;
        result.vehicle->vehPhysDef.physicsEnabled = true;
        result.vehicle->vehPhysDef.physPresetName = PHYS_PRESET_NAME;
        result.vehicle->vehPhysDef.physPreset = result.physPreset.get();
        result.vehicle->vehPhysDef.steeringAxle = VEH_AXLE_FRONT;
        result.vehicle->vehPhysDef.powerAxle = VEH_AXLE_REAR;
        result.vehicle->vehPhysDef.brakingAxle = VEH_AXLE_ALL;
        result.vehicle->turretWeaponName = WEAPON_NAME;
        result.vehicle->turretWeapon = result.weapon.get();
        result.vehicle->compassFriendlyIcon = result.material.get();
        result.vehicle->idleLowSnd.name = result.idleSound.get();
        result.vehicle->engineStartUpLength = 1500;
        result.vehicle->surfaceSndPrefix = SURFACE_SOUND_PREFIX;

        return result;
    }

    std::string DumpVehicle(VehicleDef& vehicle)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::IW4, GamePlatform::PC);
        vehicle.trophyTags[0] = dumpingZone.m_script_strings.AddOrGetScriptString("tag_trophy_left");
        vehicle.trophyTags[1] = dumpingZone.m_script_strings.AddOrGetScriptString("tag_trophy_right");
        dumpingZone.m_pools.AddAsset(ASSET_TYPE_VEHICLE, vehicle.name, &vehicle, {}, {}, {});

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        vehicle::DumperIW4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("vehicles/test_vehicle");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("Vehicle dumper serializes raw fields (IW4)", "[iw4][vehicle][system]")
    {
        auto testVehicle = CreateTestVehicle();
        std::istringstream dumpedStream(DumpVehicle(*testVehicle.vehicle));

        InfoString infoString;
        REQUIRE(infoString.FromStream("VEHICLEFILE", dumpedStream));
        REQUIRE(infoString.GetValueForKey("type") == "tank");
        REQUIRE(infoString.GetValueForKey("health") == "750");
        REQUIRE(infoString.GetValueForKey("topSpeed") == "2.000000");
        REQUIRE(infoString.GetValueForKey("physicsPreset") == PHYS_PRESET_NAME);
        REQUIRE(infoString.GetValueForKey("steeringAxle") == "front");
        REQUIRE(infoString.GetValueForKey("powerAxle") == "rear");
        REQUIRE(infoString.GetValueForKey("brakingAxle") == "all");
        REQUIRE(infoString.GetValueForKey("turretWeaponName") == WEAPON_NAME);
        REQUIRE(infoString.GetValueForKey("trophyTags") == "tag_trophy_left\ntag_trophy_right");
        REQUIRE(infoString.GetValueForKey("compassFriendlyIcon") == MATERIAL_NAME);
        REQUIRE(infoString.GetValueForKey("lowIdleSnd") == IDLE_SOUND_NAME);
        REQUIRE(infoString.GetValueForKey("engineStartUpLength") == "1.5");
        REQUIRE(infoString.GetValueForKey("surfaceSndPrefix") == SURFACE_SOUND_PREFIX);
    }
} // namespace
