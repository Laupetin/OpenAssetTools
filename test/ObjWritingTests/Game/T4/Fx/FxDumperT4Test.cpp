#include "Game/T4/Fx/FxEffectDefDumperT4.h"
#include "Game/T4/Fx/FxImpactTableDumperT4.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T4;

namespace
{
    TEST_CASE("FxEffectDef writer reconstructs T4-only effect fields", "[t4][fx]")
    {
        Material material{};
        material.info.name = "fx_test_line";

        FxElemVisStateSample visualSamples[2]{};
        for (auto& sample : visualSamples)
        {
            sample.base.size[1] = 2.0f;
            sample.amplitude.size[1] = 1.0f;
            sample.base.scale = 1.0f;
            sample.amplitude.scale = 0.5f;
        }

        FxElemDef elem{};
        elem.flags = static_cast<int>(FX_ELEM_FLAME_CHUNK | FX_ELEM_HAS_REFLECTION | FX_ELEM_IS_MATURE_CONTENT);
        elem.windInfluence = 0.5f;
        elem.elemType = FX_ELEM_TYPE_LINE;
        elem.visualCount = 1;
        elem.visStateIntervalCount = 1;
        elem.visSamples = visualSamples;
        elem.visuals.instance.material = &material;

        FxEffectDef effect{};
        effect.name = "test/line";
        effect.elemDefCountOneShot = 1;
        effect.efPriority = 115;
        effect.elemDefs = &elem;

        Zone dumpingZone("DumpingZone", 0, GameId::T4, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, effect.name, &effect));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::EffectDefDumperT4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/test/line.efx");
        REQUIRE(dumpedFile != nullptr);
        const auto text = dumpedFile->AsString();

        REQUIRE(text.starts_with("iwfx 2\n"));
        REQUIRE(text.find("editorFlags useRandSize1;") != std::string::npos);
        REQUIRE(text.find("flags isMatureContent spawnOffsetNone runRelToWorld flamechunk hasReflection;") != std::string::npos);
        REQUIRE(text.find("windinfluence 0.5;") != std::string::npos);
        REQUIRE(text.find("efPriority 115;") != std::string::npos);
        REQUIRE(text.find("line\n\t{\n\t\t\"fx_test_line\"") != std::string::npos);
    }

    TEST_CASE("FxImpactTable dumper uses T4 impact and surface ordering", "[t4][fx]")
    {
        FxEffectDef underwaterEffect{};
        underwaterEffect.name = ",impacts/underwater_tallgrass";
        FxEffectDef physicsEffect{};
        physicsEffect.name = "impacts/physics_player";
        FxEffectDef shotgunEffect{};
        shotgunEffect.name = "impacts/shotgun";
        FxEffectDef armorPiercingEffect{};
        armorPiercingEffect.name = "impacts/armor_piercing";

        FxImpactEntry entries[16]{};
        entries[2].nonflesh[SURF_TYPE_TALL_GRASS] = &underwaterEffect;
        entries[5].nonflesh[SURF_TYPE_DEFAULT] = &shotgunEffect;
        entries[7].nonflesh[SURF_TYPE_DEFAULT] = &armorPiercingEffect;
        entries[15].nonflesh[SURF_TYPE_PLAYER] = &physicsEffect;

        FxImpactTable impactTable{.name = "", .table = entries};
        Zone dumpingZone("mp_test", 0, GameId::T4, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::ImpactTableDumperT4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/maps/mp_test/iw_impacts.csv");
        REQUIRE(dumpedFile != nullptr);
        const auto text = dumpedFile->AsString();

        REQUIRE(text.find("bullet_small_underwater,tallgrass,impacts/underwater_tallgrass\n") != std::string::npos);
        REQUIRE(text.find("bullet_ap_normal,default,impacts/armor_piercing\n") != std::string::npos);
        REQUIRE(text.find("shotgun_normal,default,impacts/shotgun\n") != std::string::npos);
        REQUIRE(text.find("physics_impact,player,impacts/physics_player\n") != std::string::npos);

        const auto armorPiercingPosition = text.find("bullet_ap_normal,default,");
        const auto shotgunPosition = text.find("shotgun_normal,default,");
        const auto physicsPosition = text.find("physics_impact,default,");
        const auto mortarPosition = text.find("mortar_shell,default,");
        REQUIRE(armorPiercingPosition < shotgunPosition);
        REQUIRE(physicsPosition < mortarPosition);

        const auto paintedMetalPosition = text.find("bullet_small_normal,paintedmetal,");
        const auto fleshPosition = text.find("bullet_small_normal,flesh_body_nonfatal,");
        const auto playerPosition = text.find("bullet_small_normal,player,");
        REQUIRE(paintedMetalPosition < fleshPosition);
        REQUIRE(fleshPosition < playerPosition);
        REQUIRE(text.find("grenade_bounce,flesh_body_nonfatal,") == std::string::npos);
    }
} // namespace
