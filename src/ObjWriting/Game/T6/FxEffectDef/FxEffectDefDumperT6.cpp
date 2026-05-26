#include "FxEffectDefDumperT6.h"

#include "Dumping/FxElemDef/FxElemDefDumper.h"
#include "FxEffectDef/FxEffectDefCommon.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>

#define _USE_MATH_DEFINES

#include <Game/T6/Fx/FxElemDefConstantsT6.h>
#include <math.h>

using namespace T6;

namespace
{
    float Rad2Deg(float rad)
    {
        return rad / (float)M_PI * 180.0f;
    }

    void DumpFxElemDef(const FxElemDef* subAsset, const FxEffectDef& fxEffectDef, FxElemDefDumper& fxElemDefDumper, unsigned int defIndex)
    {
        fxElemDefDumper.BeginElement();

        fxElemDefDumper.WriteKeyValue("name", std::format("{}", defIndex));

        // fxElemDefDumper.BeginEnum("editorFlags");
        // fxElemDefDumper.WriteEnumEntry("looping");
        // fxElemDefDumper.WriteEnumEntry("useRandSize0");
        // fxElemDefDumper.WriteEnumEntry("useRandSize1");
        // fxElemDefDumper.WriteEnumEntry("useRandVel0");

        fxElemDefDumper.BeginEnum("flags");
        FxElemDefFlags flags;
        flags.rawFlags = subAsset->flags;
        if (flags.bitFlags.spawnRelative)
        {
            fxElemDefDumper.WriteEnumEntry("spawnRelative");
        }
        if (flags.bitFlags.spawnFrustumCull)
        {
            fxElemDefDumper.WriteEnumEntry("spawnFrustumCull");
        }
        if (flags.bitFlags.spawnOffsetNone)
        {
            fxElemDefDumper.WriteEnumEntry("spawnOffsetNone");
        }
        if (flags.bitFlags.runRelToEffect)
        {
            fxElemDefDumper.WriteEnumEntry("runRelToEffect");
        }
        if (flags.bitFlags.runRelToSpawn)
        {
            fxElemDefDumper.WriteEnumEntry("runRelToSpawn");
        }
        if (flags.bitFlags.nonUniformScale)
        {
            fxElemDefDumper.WriteEnumEntry("nonUniformScale");
        }
        fxElemDefDumper.EndEnum();

        // fxElemDefDumper.BeginEnum("extraFlags");

        fxElemDefDumper.WriteFxFloatRange("spawnRange", fx_elem_def::FxFloatRange(subAsset->spawnRange.base, subAsset->spawnRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeInRange", fx_elem_def::FxFloatRange(subAsset->fadeInRange.base, subAsset->fadeInRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeOutRange", fx_elem_def::FxFloatRange(subAsset->fadeOutRange.base, subAsset->fadeOutRange.amplitude));

        fxElemDefDumper.WriteFloat("spawnFrustumCullRadius", subAsset->spawnFrustumCullRadius);

        if (defIndex < (unsigned int)fxEffectDef.elemDefCountLooping)
        {
            fxElemDefDumper.WriteFxSpawnDef(fx_elem_def::FxSpawnDef(subAsset->spawn.looping.count * 100, subAsset->spawn.looping.intervalMsec * 2));
        }
        else
        {
            fxElemDefDumper.WriteFxSpawnDef(fx_elem_def::FxSpawnDef(subAsset->spawn.oneShot.count.base, subAsset->spawn.oneShot.count.amplitude));
        }

        fxElemDefDumper.WriteFxIntRange("spawnDelayMsec", fx_elem_def::FxIntRange(subAsset->spawnDelayMsec.base, subAsset->spawnDelayMsec.amplitude));
        fxElemDefDumper.WriteFxIntRange("lifeSpanMsec", fx_elem_def::FxIntRange(subAsset->lifeSpanMsec.base, subAsset->lifeSpanMsec.amplitude));

        fxElemDefDumper.WriteFxFloatRange("spawnOrgX", fx_elem_def::FxFloatRange(subAsset->spawnOrigin[0].base, subAsset->spawnOrigin[0].amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnOrgY", fx_elem_def::FxFloatRange(subAsset->spawnOrigin[1].base, subAsset->spawnOrigin[1].amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnOrgZ", fx_elem_def::FxFloatRange(subAsset->spawnOrigin[2].base, subAsset->spawnOrigin[2].amplitude));

        fxElemDefDumper.WriteFxFloatRange("spawnOffsetRadius",
                                          fx_elem_def::FxFloatRange(subAsset->spawnOffsetRadius.base, subAsset->spawnOffsetRadius.amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnOffsetHeight",
                                          fx_elem_def::FxFloatRange(subAsset->spawnOffsetHeight.base, subAsset->spawnOffsetHeight.amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnAnglePitch", fx_elem_def::FxFloatRange(subAsset->spawnAngles[0].base, subAsset->spawnAngles[0].amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnAngleYaw", fx_elem_def::FxFloatRange(subAsset->spawnAngles[1].base, subAsset->spawnAngles[1].amplitude));
        fxElemDefDumper.WriteFxFloatRange("spawnAngleRoll", fx_elem_def::FxFloatRange(subAsset->spawnAngles[2].base, subAsset->spawnAngles[2].amplitude));
        fxElemDefDumper.WriteFxFloatRange("angleVelPitch",
                                          fx_elem_def::FxFloatRange(subAsset->angularVelocity[0].base, subAsset->angularVelocity[0].amplitude));
        fxElemDefDumper.WriteFxFloatRange("angleVelYaw", fx_elem_def::FxFloatRange(subAsset->angularVelocity[1].base, subAsset->angularVelocity[1].amplitude));
        fxElemDefDumper.WriteFxFloatRange("angleVelRoll", fx_elem_def::FxFloatRange(subAsset->angularVelocity[2].base, subAsset->angularVelocity[2].amplitude));
        fxElemDefDumper.WriteFxFloatRange("initialRot",
                                          fx_elem_def::FxFloatRange(Rad2Deg(subAsset->initialRotation.base), Rad2Deg(subAsset->initialRotation.amplitude)));

        fxElemDefDumper.WriteFxFloatRange("gravity", fx_elem_def::FxFloatRange(subAsset->gravity.base, subAsset->gravity.amplitude));
        fxElemDefDumper.WriteFxFloatRange("elasticity", fx_elem_def::FxFloatRange(subAsset->reflectionFactor.base, subAsset->reflectionFactor.amplitude));

        fxElemDefDumper.WriteFloat("windinfluence", subAsset->windInfluence);

        fxElemDefDumper.BeginEnum("atlasBehavior");

        FxElemDefAtlasBehavior atlasBehavior;
        atlasBehavior.rawBehavior = subAsset->atlas.behavior;
        if (subAsset->atlas.behavior == 0)
        {
            fxElemDefDumper.WriteEnumEntry("startFixed");
        }
        else if (atlasBehavior.bitBehavior.startRandom)
        {
            fxElemDefDumper.WriteEnumEntry("startRandom");
        }
        else if (atlasBehavior.bitBehavior.startIndexed)
        {
            fxElemDefDumper.WriteEnumEntry("startIndexed");
        }
        if (atlasBehavior.bitBehavior.playOverLife)
        {
            fxElemDefDumper.WriteEnumEntry("playOverLife");
        }
        else if (atlasBehavior.bitBehavior.loopOnlyNTimes)
        {
            fxElemDefDumper.WriteEnumEntry("loopOnlyNTimes");
        }
        fxElemDefDumper.EndEnum();

        fxElemDefDumper.WriteInt("atlasIndex", subAsset->atlas.index);
        fxElemDefDumper.WriteInt("atlasFps", subAsset->atlas.fps);
        fxElemDefDumper.WriteInt("atlasLoopCount", subAsset->atlas.loopCount);
        fxElemDefDumper.WriteInt("atlasColIndexBits", subAsset->atlas.colIndexBits);
        fxElemDefDumper.WriteInt("atlasRowIndexBits", subAsset->atlas.rowIndexBits);
        fxElemDefDumper.WriteInt("atlasEntryCount", subAsset->atlas.entryCountAndIndexRange & 0x1FF);
        fxElemDefDumper.WriteInt("atlasIndexRange", subAsset->atlas.entryCountAndIndexRange >> 9);

        bool shouldStop =
            (strlen(fxEffectDef.name) == strlen("maps/zombie/fx_zombie_tesla_shock")) && 
            (strcmp(fxEffectDef.name, "maps/zombie/fx_zombie_tesla_shock") == 0);

        // TODO: Write velGraph[0-1][X-Y], rotGraph, sizeGraph[0-1], scaleGraph, colorGraph, alphaGraph

        FxElemVelStateSample velSample;
        for (int i = 0; i < subAsset->velIntervalCount; i++)
        {
            velSample = subAsset->velSamples[i];
            fxElemDefDumper.WriteVelGraph(std::format("velGraph{}", i).c_str(),
                                          velSample.local.velocity.base.v,
                                          velSample.local.velocity.amplitude.v,
                                          velSample.local.totalDelta.base.v,
                                          velSample.local.totalDelta.amplitude.v,
                                          velSample.world.velocity.base.v,
                                          velSample.world.velocity.amplitude.v,
                                          velSample.world.totalDelta.base.v,
                                          velSample.world.totalDelta.amplitude.v,
                                          1.0f,
                                          6
            );
        }

        FxElemVisStateSample visSample;
        for (int i = 0; i < subAsset->visStateIntervalCount; i++)
        {
            visSample = subAsset->visSamples[i];

            // Write rotation graph
            float baseRot = visSample.base.rotationDelta;
            float ampRot = visSample.amplitude.rotationDelta;
            float rotScale = fabsf(baseRot) + fabsf(ampRot);
            fxElemDefDumper.WriteRotGraph("rotGraph", baseRot, ampRot, rotScale, 6);

            // Write size graph
            float sizeScale = 16.0f;
            fxElemDefDumper.WriteSizeGraph("sizeGraph0", visSample.base.size, visSample.amplitude.size, sizeScale, 6);
            fxElemDefDumper.WriteSizeGraph("sizeGraph1", visSample.base.size, visSample.amplitude.size, sizeScale, 6);

            // Write scale graph
            float baseScale = visSample.base.scale;
            float ampScale = visSample.amplitude.scale;
            float scaleScale = std::max(fabsf(baseScale) + fabsf(ampScale), 1.0f);
            fxElemDefDumper.WriteScaleGraph("scaleGraph", baseScale, ampScale, scaleScale, 6);

            // Write color graph
            fxElemDefDumper.WriteColorGraph("colorGraph", visSample.base.color, visSample.amplitude.color, 1.0f, 6);

            // Write alpha graph
            fxElemDefDumper.WriteAlphaGraph("alphaGraph", visSample.base.color[3], visSample.amplitude.color[3], 1.0f, 6);
        }

        fxElemDefDumper.WriteFloat("lightingFrac", static_cast<unsigned char>(subAsset->lightingFrac) / 255.0f, 6);
        fxElemDefDumper.WriteFloatArray("collOffset", subAsset->collMins.v, 3, 6);
        fxElemDefDumper.WriteFloat("collRadius", 0, 6);
        fxElemDefDumper.WriteKeyValue("fxOnImpact", subAsset->effectOnImpact.name);
        fxElemDefDumper.WriteKeyValue("fxOnDeath", subAsset->effectOnDeath.name);
        fxElemDefDumper.WriteInt("sortOrder", subAsset->sortOrder);
        fxElemDefDumper.WriteKeyValue("emission", "");
        fxElemDefDumper.WriteFxFloatRange("emitDist", fx_elem_def::FxFloatRange(subAsset->emitDist.base, subAsset->emitDist.amplitude), 6);
        fxElemDefDumper.WriteFxFloatRange("emitDistVariance", fx_elem_def::FxFloatRange(subAsset->emitDistVariance.base, subAsset->emitDistVariance.amplitude), 6);
        fxElemDefDumper.WriteKeyValue("attachment", subAsset->effectAttached.name);

        int a = 0, c = 0;
        float b = 0;
        if (subAsset->elemType == FX_ELEM_TYPE_TRAIL && subAsset->extended.trailDef)
        {
            a = subAsset->extended.trailDef->splitDist;
            b = (float)subAsset->extended.trailDef->scrollTimeMsec / 1000.0f;
            c = subAsset->extended.trailDef->repeatDist;
        }
        else if (subAsset->elemType == FX_ELEM_TYPE_SPOT_LIGHT && subAsset->extended.spotLightDef)
        {
            // TODO: Try to find how these are actually written
            // fxElemDefDumper.WriteFloat("spotlightFovInnerFraction", subAsset->extended.spotLightDef->fovInnerFraction);
            // fxElemDefDumper.WriteFloat("spotlightStartRadius", subAsset->extended.spotLightDef->startRadius);
            // fxElemDefDumper.WriteFloat("spotlightEndRadius", subAsset->extended.spotLightDef->endRadius);
        }
        else if (subAsset->extended.unknownDef)
        {
            // TODO: What gets dumped here?
        }
        fxElemDefDumper.WriteInt("trailSplitDist", a);
        fxElemDefDumper.WriteFloat("trailScrollTime", b, 6);
        fxElemDefDumper.WriteInt("trailRepeatDist", c);

        fxElemDefDumper.WriteInt("alphafadetimemsec", subAsset->alphaFadeTimeMsec);

        fxElemDefDumper.WriteInt("maxwind_mag", subAsset->maxWindStrength);
        fxElemDefDumper.WriteInt("maxwind_life", subAsset->lifespanAtMaxWind);
        fxElemDefDumper.WriteInt("maxwind_interval", subAsset->spawnIntervalAtMaxWind);

        float topWidth = 0, bottomWidth = 0;
        if (subAsset->elemType == FX_ELEM_TYPE_SPRITE_BILLBOARD)
        {
            topWidth = subAsset->u.billboard.topWidth;
            bottomWidth = subAsset->u.billboard.bottomWidth;
        }
        fxElemDefDumper.WriteFloat("billboardTopWidth", topWidth, 6);
        fxElemDefDumper.WriteFloat("billboardBottomWidth", bottomWidth, 6);
        
        fxElemDefDumper.WriteSpawnSound("elemSpawnSound", subAsset->spawnSound.spawnSound);

        int cloudBase = 1024, cloudAmplitude = 0;
        if (subAsset->elemType == FX_ELEM_TYPE_CLOUD)
        {
            cloudBase = subAsset->u.cloudDensityRange.base;
            cloudAmplitude = subAsset->u.cloudDensityRange.amplitude;
        }
        fxElemDefDumper.WriteFxIntRange("cloudDensity", fx_elem_def::FxIntRange(cloudBase, cloudAmplitude));

        fxElemDefDumper.WriteFxFloatRange("billboardPivot", fx_elem_def::FxFloatRange(subAsset->billboardPivot.x, subAsset->billboardPivot.y), 6);

        if (subAsset->elemType < std::extent_v<decltype(fx_elem_type_names)>)
        {
            fxElemDefDumper.BeginVisuals(fx_elem_type_names[subAsset->elemType]);
        }
        else
        {
            fxElemDefDumper.BeginVisuals(std::format("UNKNOWN_ELEM_TYPE {}", (int)subAsset->elemType));
        }

        for (int i = 0; i < subAsset->visualCount; i++)
        {
            FxElemMarkVisuals* markVisuals = subAsset->visuals.markArray;
            if (subAsset->elemType == FX_ELEM_TYPE_DECAL && markVisuals)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (markVisuals[i].materials[j])
                    {
                        fxElemDefDumper.WriteVisualEntry(markVisuals[i].materials[j]->info.name);
                    }
                }
            }
            else if (subAsset->elemType == FX_ELEM_TYPE_MODEL && subAsset->visuals.instance.model && subAsset->visuals.instance.model->name)
            {
                // Load xmodel
                fxElemDefDumper.WriteVisualEntry(subAsset->visuals.instance.model->name);
            }
            else if (subAsset->elemType == FX_ELEM_TYPE_SOUND && subAsset->visuals.instance.soundName)
            {
                // Load sound name
                fxElemDefDumper.WriteVisualEntry(subAsset->visuals.instance.soundName);
            }
            else if (subAsset->elemType == FX_ELEM_TYPE_RUNNER)
            {
                // Load FxEffectDef
                fxElemDefDumper.WriteVisualEntry(subAsset->visuals.instance.effectDef.name);
            }
            else if (subAsset->elemType != FX_ELEM_TYPE_OMNI_LIGHT && subAsset->elemType != FX_ELEM_TYPE_SPOT_LIGHT)
            {
                // Load material
                auto* material = subAsset->visuals.instance.material;
                if (material && material->info.name)
                {
                    if (material->info.name[0] == ',')
                    {
                        fxElemDefDumper.WriteVisualEntry(material->info.name + 1);
                    }
                    else
                    {
                        fxElemDefDumper.WriteVisualEntry(material->info.name);
                    }
                }
            }
        }

        fxElemDefDumper.EndVisuals();

        fxElemDefDumper.EndElement();
    }
} // namespace

namespace fx_effect_def
{
    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFx::Type>& asset)
    {
        const auto* fxEffectDef = asset.Asset();

        const auto fxEffectDefFilePath = GetFileNameForAssetName(fxEffectDef->name);
        const auto assetFile = context.OpenAssetFile(fxEffectDefFilePath);

        if (!assetFile)
            return;

        FxElemDefDumper fxElemDefDumper(*assetFile);
        // TODO: Move version to constant
        fxElemDefDumper.Init(3);

        size_t defCount = fxEffectDef->elemDefCountEmission + fxEffectDef->elemDefCountLooping + fxEffectDef->elemDefCountOneShot;
        for (unsigned int i = 0; i < defCount; i++)
        {
            auto* elemDef = &fxEffectDef->elemDefs[i];
            if (!elemDef)
            {
                continue;
            }
            DumpFxElemDef(elemDef, *fxEffectDef, fxElemDefDumper, i);
        }
    }
} // namespace fx_effect_def
