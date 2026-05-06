#include "FxEffectDefDumperIW3.h"

#include "ObjWriting.h"
#include "FxEffectDef/FxEffectDefCommon.h"
#include "Dumping/FxElemDef/FxElemDefDumper.h"

#include <filesystem>
#include <string>

using namespace IW3;

namespace
{
    void DumpFxElemDef(const FxElemDef* subAsset,
                       const FxEffectDef& fxEffectDef, 
                       FxElemDefDumper& fxElemDefDumper,
                       unsigned int defIndex,
                       unsigned int elemDefCountLooping)
    {
        fxElemDefDumper.BeginElement();

        fxElemDefDumper.WriteKeyValue("name", std::format("{}", defIndex));
        fxElemDefDumper.WriteKeyValue("editorFlags", "");
        fxElemDefDumper.WriteKeyValue("flags", "");

        fxElemDefDumper.WriteFxFloatRange("spawnRange", fx_elem_def::FxFloatRange(subAsset->spawnRange.base, subAsset->spawnRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeInRange", fx_elem_def::FxFloatRange(subAsset->fadeInRange.base, subAsset->fadeInRange.amplitude));
        fxElemDefDumper.WriteFxFloatRange("fadeOutRange", fx_elem_def::FxFloatRange(subAsset->fadeOutRange.base, subAsset->fadeOutRange.amplitude));

        fxElemDefDumper.WriteFloat("spawnFrustumCullRadius", subAsset->spawnFrustumCullRadius);
        fxElemDefDumper.WriteFxSpawnDef(subAsset->spawn, defIndex, fxEffectDef.elemDefCountLooping);
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
        fxElemDefDumper.WriteFxFloatRange("initialRot", fx_elem_def::FxFloatRange(subAsset->initialRotation.base, subAsset->initialRotation.amplitude));
        fxElemDefDumper.WriteFxFloatRange("gravity", fx_elem_def::FxFloatRange(subAsset->gravity.base, subAsset->gravity.amplitude));
        fxElemDefDumper.WriteKeyValue("elasticity", "");

        fxElemDefDumper.WriteKeyValue("atlasBehavior", "startIndexed playOverLife loopOnlyNTimes");
        fxElemDefDumper.WriteInt("atlasIndex", subAsset->atlas.index);
        fxElemDefDumper.WriteInt("atlasFps", subAsset->atlas.fps);
        fxElemDefDumper.WriteInt("atlasLoopCount", subAsset->atlas.loopCount);
        fxElemDefDumper.WriteInt("atlasColIndexBits", subAsset->atlas.colIndexBits);
        fxElemDefDumper.WriteInt("atlasRowIndexBits", subAsset->atlas.rowIndexBits);
        fxElemDefDumper.WriteInt("atlasEntryCount", subAsset->atlas.entryCount);

        
        fxElemDefDumper.WriteInt("lightingFrac", subAsset->lightingFrac);
        fxElemDefDumper.WriteKeyValue("collOffset", "");
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
