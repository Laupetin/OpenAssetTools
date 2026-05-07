#include "FxEffectDefDumperIW3.h"

#include "Dumping/FxElemDef/FxElemDefDumper.h"
#include "FxEffectDef/FxEffectDefCommon.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>

#define _USE_MATH_DEFINES

#include <Game/IW3/Fx/FxElemDefConstantsIW3.h>
#include <math.h>

using namespace IW3;

namespace
{
    float Rad2Deg(float rad)
    {
        return rad / (float)M_PI * 180.0f;
    }

    void DumpFxElemDef(
        const FxElemDef* subAsset, const FxEffectDef& fxEffectDef, FxElemDefDumper& fxElemDefDumper, unsigned int defIndex, unsigned int elemDefCountLooping)
    {
        fxElemDefDumper.BeginElement();

        fxElemDefDumper.WriteKeyValue("name", std::format("{}", defIndex));

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

        fxElemDefDumper.WriteFxFloatRange("spawnRange", fx_elem_def::FxFloatRange(subAsset->spawnRange.base, subAsset->spawnRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeInRange", fx_elem_def::FxFloatRange(subAsset->fadeInRange.base, subAsset->fadeInRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeOutRange", fx_elem_def::FxFloatRange(subAsset->fadeOutRange.base, subAsset->fadeOutRange.amplitude));

        fxElemDefDumper.WriteFloat("spawnFrustumCullRadius", subAsset->spawnFrustumCullRadius);

        if (defIndex < (unsigned int)fxEffectDef.elemDefCountLooping)
        {
            fxElemDefDumper.WriteFxSpawnDef(fx_elem_def::FxSpawnDef(subAsset->spawn.looping.count, subAsset->spawn.looping.intervalMsec));
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

        fxElemDefDumper.BeginEnum("atlasBehavior");

        FxElemDefAtlasBehavior atlasBehavior;
        atlasBehavior.rawBehavior = subAsset->atlas.behavior;
        if (subAsset->atlas.behavior == 0)
        {
            fxElemDefDumper.WriteEnumEntry("startFixed");
        }
        if (atlasBehavior.bitBehavior.startRandom)
        {
            fxElemDefDumper.WriteEnumEntry("startRandom");
        }
        if (atlasBehavior.bitBehavior.startIndexed)
        {
            fxElemDefDumper.WriteEnumEntry("startIndexed");
        }
        if (atlasBehavior.bitBehavior.playOverLife)
        {
            fxElemDefDumper.WriteEnumEntry("playOverLife");
        }
        if (atlasBehavior.bitBehavior.loopOnlyNTimes)
        {
            fxElemDefDumper.WriteEnumEntry("loopOnlyNTimes");
        }
        fxElemDefDumper.EndEnum();

        fxElemDefDumper.WriteInt("atlasIndex", subAsset->atlas.index);
        fxElemDefDumper.WriteInt("atlasFps", subAsset->atlas.fps);
        fxElemDefDumper.WriteInt("atlasLoopCount", 0);
        fxElemDefDumper.WriteInt("atlasColIndexBits", 0);
        fxElemDefDumper.WriteInt("atlasRowIndexBits", 0);
        fxElemDefDumper.WriteInt("atlasEntryCount", 0);

        fxElemDefDumper.WriteFloat("lightingFrac", static_cast<unsigned char>(subAsset->lightingFrac) / 255.0f, 2);
        fxElemDefDumper.WriteFloatArray("collOffset", subAsset->collMins, 3);
        fxElemDefDumper.WriteKeyValue("collRadius", "");

        std::string impactName = "";
        if (subAsset->effectOnImpact.name)
        {
            impactName = subAsset->effectOnImpact.name;
        }
        fxElemDefDumper.WriteKeyValue("fxOnImpact", impactName);

        std::string deathName = "";
        if (subAsset->effectOnDeath.name)
        {
            impactName = subAsset->effectOnDeath.name;
        }
        fxElemDefDumper.WriteKeyValue("fxOnDeath", deathName);

        fxElemDefDumper.WriteInt("sortOrder", subAsset->sortOrder);
        fxElemDefDumper.WriteKeyValue("emission", "");
        fxElemDefDumper.WriteFxFloatRange("emitDist", fx_elem_def::FxFloatRange(subAsset->emitDist.base, subAsset->emitDist.amplitude));
        fxElemDefDumper.WriteFxFloatRange("emitDistVariance", fx_elem_def::FxFloatRange(subAsset->emitDistVariance.base, subAsset->emitDistVariance.amplitude));

        int splitDist = 0, scrollTimeMsec = 0, repeatDist = 0;
        if (subAsset->trailDef)
        {
            splitDist = subAsset->trailDef->splitDist;
            scrollTimeMsec = subAsset->trailDef->scrollTimeMsec;
            repeatDist = subAsset->trailDef->repeatDist;
        }
        fxElemDefDumper.WriteInt("trailSplitDist", splitDist);
        fxElemDefDumper.WriteInt("trailScrollTime", scrollTimeMsec);
        fxElemDefDumper.WriteInt("trailRepeatDist", repeatDist);

        fxElemDefDumper.BeginVisuals(fx_elem_type_names[subAsset->elemType]);

        if (subAsset->elemType == FX_ELEM_TYPE_DECAL)
        {
            // Load FxElemMarkVisualsArray
        }
        else if (subAsset->visualCount < 1u)
        {
            // Load FxElemVisualsArray
        }
        else if (subAsset->elemType == FX_ELEM_TYPE_MODEL)
        {
            // Load xmodel
        }
        else if (subAsset->elemType == FX_ELEM_TYPE_SOUND)
        {
            // Load sound name
        }
        else if (subAsset->elemType == FX_ELEM_TYPE_RUNNER)
        {
            // Load FxEffectDef + name?
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

        fxElemDefDumper.EndVisuals();

        fxElemDefDumper.EndElement();
    }
} // namespace

namespace fx_effect_def
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFx::Type>& asset)
    {
        const auto* fxEffectDef = asset.Asset();

        const auto fxEffectDefFilePath = GetFileNameForAssetName(fxEffectDef->name);
        const auto assetFile = context.OpenAssetFile(fxEffectDefFilePath);

        if (!assetFile)
            return;

        FxElemDefDumper fxElemDefDumper(*assetFile);
        fxElemDefDumper.Init();

        size_t defCount = fxEffectDef->elemDefCountEmission + fxEffectDef->elemDefCountLooping + fxEffectDef->elemDefCountOneShot;
        for (int i = 0; i < defCount; i++)
        {
            auto* elemDef = &fxEffectDef->elemDefs[i];
            if (!elemDef)
            {
                continue;
            }
            DumpFxElemDef(elemDef, *fxEffectDef, fxElemDefDumper, i, fxEffectDef->elemDefCountLooping);
        }
    }
} // namespace fx_effect_def
