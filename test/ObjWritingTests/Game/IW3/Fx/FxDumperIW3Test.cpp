#include "Game/IW3/Fx/FxEffectDefDumperIW3.h"
#include "Game/IW3/Fx/FxImpactTableDumperIW3.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <limits>
#include <string>

using namespace IW3;

namespace
{
    TEST_CASE("FxEffectDef writer reconstructs an IW3 effect source", "[iw3][fx]")
    {
        Material material{};
        material.info.name = "fx_test_material";

        FxElemVelStateSample velocitySamples[2]{};
        velocitySamples[0].local.velocity.base[0] = 0.001f;
        velocitySamples[0].local.velocity.amplitude[0] = 0.001f;
        velocitySamples[1].local.velocity.base[0] = 0.002f;
        velocitySamples[1].local.velocity.amplitude[0] = 0.001f;
        velocitySamples[0].world.velocity.base[1] = 0.002f;
        velocitySamples[1].world.velocity.base[1] = 0.004f;

        FxElemVisStateSample visualSamples[2]{};
        for (auto& sample : visualSamples)
        {
            sample.base.color[0] = 64;
            sample.base.color[1] = -128;
            sample.base.color[2] = -1;
            sample.base.color[3] = 127;
            sample.amplitude.color[0] = -128;
            sample.amplitude.color[1] = -128;
            sample.amplitude.color[2] = -1;
            sample.amplitude.color[3] = -1;
            sample.base.size[0] = 2.0f;
            sample.amplitude.size[0] = 1.0f;
            sample.base.size[1] = 4.0f;
            sample.amplitude.size[1] = 2.0f;
        }

        FxElemDef elemDefs[2]{};
        auto& elem = elemDefs[0];
        elem.flags = static_cast<int>(FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL | FX_ELEM_HAS_VELOCITY_GRAPH_WORLD | FX_ELEM_SPAWN_RELATIVE_TO_EFFECT
                                      | FX_ELEM_SPAWN_OFFSET_SPHERE | FX_ELEM_RUN_RELATIVE_TO_EFFECT | FX_ELEM_USE_COLLISION | FX_ELEM_NONUNIFORM_SCALE);
        elem.spawn.looping.intervalMsec = 50;
        elem.spawn.looping.count = std::numeric_limits<int>::max();
        elem.spawnAngles[0] = {1.57079632679489661923f, 0.0f};
        elem.angularVelocity[1] = {0.00314159265358979323846f, 0.0f};
        elem.gravity = {1.0f, 0.5f};
        elem.atlas.behavior = static_cast<char>(FX_ATLAS_START_RANDOM | FX_ATLAS_PLAY_OVER_LIFE | FX_ATLAS_LOOP_ONLY_N_TIMES);
        elem.atlas.loopCount = 3;
        elem.atlas.entryCount = 4;
        elem.elemType = FX_ELEM_TYPE_SPRITE_BILLBOARD;
        elem.visualCount = 1;
        elem.velIntervalCount = 1;
        elem.visStateIntervalCount = 1;
        elem.velSamples = velocitySamples;
        elem.visSamples = visualSamples;
        elem.visuals.instance.material = &material;
        elem.collMins[0] = -1.0f;
        elem.collMins[1] = -2.0f;
        elem.collMins[2] = -3.0f;
        elem.collMaxs[0] = 3.0f;
        elem.collMaxs[1] = 2.0f;
        elem.collMaxs[2] = 1.0f;
        elem.effectOnImpact.name = ",impacts/on_touch";
        elem.effectEmitted.name = ",smoke/emitted";
        elem.lightingFrac = -128;
        elem.useItemClip = 1;

        FxEffectDef effect{};
        effect.name = "test/effect";
        effect.elemDefCountLooping = 1;
        effect.elemDefCountEmission = 1;
        effect.elemDefs = elemDefs;

        Zone dumpingZone("DumpingZone", 0, GameId::IW3, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, effect.name, &effect));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::EffectDefDumperIW3 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/test/effect.efx");
        REQUIRE(dumpedFile != nullptr);
        const auto text = dumpedFile->AsString();

        REQUIRE(text.starts_with("iwfx 2\n"));
        REQUIRE(text.find("name \"segment 1\";") != std::string::npos);
        REQUIRE(text.find("segment 2") == std::string::npos);
        REQUIRE(
            text.find(
                "editorFlags looping useRandColor useRandAlpha useRandSize0 useRandSize1 useRandVel0 absVel1 playOnTouch playOnRun boundingSphere useItemClip;")
            != std::string::npos);
        REQUIRE(text.find("flags spawnRelative spawnOffsetSphere runRelToEffect useCollision nonUniformScale;") != std::string::npos);
        REQUIRE(text.find("spawnLooping 50 0;") != std::string::npos);
        REQUIRE(text.find("spawnAnglePitch 90 0;") != std::string::npos);
        REQUIRE(text.find("angleVelYaw 180 0;") != std::string::npos);
        REQUIRE(text.find("gravity 100 50;") != std::string::npos);
        REQUIRE(text.find("atlasBehavior startRandom playOverLife loopOnlyNTimes;") != std::string::npos);
        REQUIRE(text.find("atlasLoopCount 2;") != std::string::npos);
        REQUIRE(text.find("collOffset 1 0 -1;") != std::string::npos);
        REQUIRE(text.find("collRadius 2;") != std::string::npos);
        REQUIRE(text.find("fxOnImpact \"impacts/on_touch\";") != std::string::npos);
        REQUIRE(text.find("emission \"smoke/emitted\";") != std::string::npos);
        REQUIRE(text.find("billboardSprite\n\t{\n\t\t\"fx_test_material\"") != std::string::npos);
    }

    TEST_CASE("FxImpactTable dumper writes resolved entries to the IW3 CSV paths", "[iw3][fx]")
    {
        FxEffectDef defaultEffect{};
        defaultEffect.name = ",impacts/default_hit";
        FxEffectDef fleshEffect{};
        fleshEffect.name = "impacts/flesh_hit";

        FxImpactEntry entries[12]{};
        entries[0].nonflesh[0] = &defaultEffect;
        entries[0].flesh[3] = &fleshEffect;

        SECTION("An unnamed compiled map table uses the zone name")
        {
            FxImpactTable impactTable{.name = "", .table = entries};
            Zone dumpingZone("mp_test", 0, GameId::IW3, GamePlatform::PC);
            dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

            MockSearchPath dumpingObjPath;
            MockOutputPath dumpingOutput;
            AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
            fx::ImpactTableDumperIW3 dumper;
            dumper.Dump(dumpingContext);

            const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/maps/mp_test/iw_impacts.csv");
            REQUIRE(dumpedFile != nullptr);

            const auto text = dumpedFile->AsString();
            REQUIRE(text.find("bullet_small_normal,default,impacts/default_hit\n") != std::string::npos);
            REQUIRE(text.find("bullet_small_normal,flesh_head_fatal,impacts/flesh_hit\n") != std::string::npos);
            REQUIRE(text.find("projectile_dud,paintedmetal,\n") != std::string::npos);
        }

        SECTION("An unnamed compiled common table uses the root CSV path")
        {
            FxImpactTable impactTable{.name = "", .table = entries};
            Zone dumpingZone("common_mp", 0, GameId::IW3, GamePlatform::PC);
            dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

            MockSearchPath dumpingObjPath;
            MockOutputPath dumpingOutput;
            AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
            fx::ImpactTableDumperIW3 dumper;
            dumper.Dump(dumpingContext);

            REQUIRE(dumpingOutput.GetMockedFile("fx/iw_impacts.csv") != nullptr);
        }

        SECTION("The synthetic default table is skipped")
        {
            FxImpactTable impactTable{.name = "default", .table = entries};
            Zone dumpingZone("code_post_gfx_mp", 0, GameId::IW3, GamePlatform::PC);
            dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

            MockSearchPath dumpingObjPath;
            MockOutputPath dumpingOutput;
            AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
            fx::ImpactTableDumperIW3 dumper;
            dumper.Dump(dumpingContext);

            REQUIRE(dumpingOutput.GetMockedFile("fx/maps/default/iw_impacts.csv") == nullptr);
        }
    }
} // namespace
