#include "FxEffectDumperT5.h"

#include "Game/T5/CommonT5.h"

#include <cmath>
#include <format>
#include <vector>

using namespace T5;

namespace
{
    constexpr float RAD_TO_DEG = 57.29577951308232f;
    constexpr float DEG_TO_RAD = 0.017453292519943f;

    float RadToDeg(float rad)
    {
        return rad * RAD_TO_DEG;
    }

    float AngularVelToDeg(float val)
    {
        return val * RAD_TO_DEG * 1000.0f;
    }

    void WriteRangeFloat(std::ostream& s, const char* name, float base, float amp)
    {
        s << std::format("\t{} {} {};\n", name, base, amp);
    }

    void WriteRangeInt(std::ostream& s, const char* name, int base, int amp)
    {
        s << std::format("\t{} {} {};\n", name, base, amp);
    }

    void WriteRangeFloatInverse(std::ostream& s, const char* name, float base, float amp)
    {
        s << std::format("\t{} {} {};\n", name, -base, -amp);
    }

    void WriteRangeIntInverse(std::ostream& s, const char* name, int base, int amp)
    {
        s << std::format("\t{} {} {};\n", name, -base, -amp);
    }

    std::string BuildEditorFlags(bool looping, const FxElemDef& elem)
    {
        std::string flags;
        if (looping)
            flags += " looping";
        if (elem.effectOnImpact.handle)
            flags += " playOnTouch";
        if (elem.effectOnDeath.handle)
            flags += " playOnDeath";
        if (elem.effectEmitted.handle)
            flags += " playOnRun";
        if (elem.effectAttached.handle)
            flags += " playAttached";
        return flags;
    }

    std::string BuildElementFlags(int flags)
    {
        std::string result;

        if (flags & 0x2) result += " spawnRelative";
        if (flags & 0x4) result += " spawnFrustumCull";

        switch (flags & 0x30)
        {
        case 0x0: result += " spawnOffsetNone"; break;
        case 0x10: result += " spawnOffsetSphere"; break;
        case 0x20: result += " spawnOffsetCylinder"; break;
        }

        switch (flags & 0xC0)
        {
        case 0x0: result += " runRelToWorld"; break;
        case 0x40: result += " runRelToSpawn"; break;
        case 0x80: result += " runRelToEffect"; break;
        case 0xC0: result += " runRelToOffset"; break;
        }

        if (flags & 0x100) result += " useCollision";
        if (flags & 0x200) result += " dieOnTouch";
        if (flags & 0x400) result += " drawPastFog";
        if (flags & 0x800) result += " drawWithViewModel";
        if (flags & 0x1000) result += " blocksSight";
        if (flags & 0x2000) result += " useItemClip";
        if (flags & 0x80000) result += " useWorldUp";
        if (flags & 0x100000) result += " alignViewpoint";
        if (flags & 0x200000) result += " useBillboardPivot";
        if (flags & 0x400000) result += " useGaussianCloud";
        if (flags & 0x800000) result += " useRotationAxis";
        if (flags & 0x8000000) result += " modelUsesPhysics";
        if (flags & 0x10000000) result += " nonUniformScale";
        if (flags & 0x20000000) result += " flamechunk";
        if (flags & 0x40000000) result += " hasReflection";
        if (flags & 0x80000000) result += " isMatureContent";

        return result;
    }

    std::string BuildAtlasFlags(char behavior)
    {
        std::string result;

        switch (behavior & 0x3)
        {
        case 0x0: result += " startFixed"; break;
        case 0x1: result += " startRandom"; break;
        case 0x2: result += " startIndexed"; break;
        case 0x3: result += " startFixedRange"; break;
        }

        if (behavior & 0x4) result += " playOverLife";
        if (behavior & 0x8) result += " loopOnlyNTimes";

        return result;
    }

    void WriteVelGraph(std::ostream& s, const char* name, uint32_t axis,
                       const FxElemVelStateSample* samples, uint32_t count)
    {
        float maxValue = 0;
        for (uint32_t i = 0; i < count; i++)
        {
            float shifted = std::abs(samples[i].local.velocity.base[axis] * static_cast<float>((count - 1) * 1000));
            if (maxValue < shifted) maxValue = shifted;

            shifted = std::abs((samples[i].local.velocity.base[axis] + samples[i].local.velocity.amplitude[axis]) * static_cast<float>((count - 1) * 1000));
            if (maxValue < shifted) maxValue = shifted;
        }

        s << std::format("\t{} {}\n\t{{\n", name, maxValue * 2);

        s << "\t\t{\n";
        for (uint32_t i = 0; i < count; i++)
        {
            float timeFrame = (i > 0) ? (static_cast<float>(i) / static_cast<float>(count - 1)) : 0.0f;
            float curvePoint = (maxValue > 0) ? (samples[i].local.velocity.base[axis] * static_cast<float>((count - 1) * 1000) / maxValue / 2.0f) : 0.0f;
            s << std::format("\t\t\t{} {} 0 0\n", timeFrame, curvePoint);
        }
        s << "\t\t}\n";

        s << "\t\t{\n";
        for (uint32_t i = 0; i < count; i++)
        {
            float timeFrame = (i > 0) ? (static_cast<float>(i) / static_cast<float>(count - 1)) : 0.0f;
            float curvePoint = (maxValue > 0) ? (samples[i].local.velocity.amplitude[axis] * static_cast<float>((count - 1) * 1000) / maxValue / 2.0f) : 0.0f;
            s << std::format("\t\t\t{} {} 0 0\n", timeFrame, curvePoint);
        }
        s << "\t\t}\n";

        s << "\t};\n";
    }

    void WriteVisSamples(std::ostream& s, const FxElemVisStateSample* samples, uint32_t count)
    {
        std::vector<FxElemVisStateSample> visSamples(samples, samples + count);
        if (visSamples.size() == 1)
        {
            FxElemVisStateSample defaultSample{};
            defaultSample.base.rotationTotal = 1.0f;
            defaultSample.base.size[0] = 0.5f;
            defaultSample.base.size[1] = 0.5f;
            defaultSample.base.scale = 1.0f;
            defaultSample.base.color[0] = -1;
            defaultSample.base.color[1] = -1;
            defaultSample.base.color[2] = -1;
            defaultSample.base.color[3] = -1;
            defaultSample.amplitude.color[0] = -1;
            defaultSample.amplitude.color[1] = -1;
            defaultSample.amplitude.color[2] = -1;
            defaultSample.amplitude.color[3] = -1;
            visSamples.insert(visSamples.begin(), defaultSample);
        }

        s << "\tcolor\n\t{\n";
        for (auto& sample : visSamples)
        {
            s << std::format("\t\t{} {} {} {} {} {} {} {} {} {}\n",
                0.0f,
                static_cast<int>(static_cast<uint8_t>(sample.base.color[0])),
                static_cast<int>(static_cast<uint8_t>(sample.base.color[1])),
                static_cast<int>(static_cast<uint8_t>(sample.base.color[2])),
                static_cast<int>(static_cast<uint8_t>(sample.base.color[3])),
                static_cast<int>(static_cast<uint8_t>(sample.amplitude.color[0])),
                static_cast<int>(static_cast<uint8_t>(sample.amplitude.color[1])),
                static_cast<int>(static_cast<uint8_t>(sample.amplitude.color[2])),
                static_cast<int>(static_cast<uint8_t>(sample.amplitude.color[3])),
                1.0f);
        }
        s << "\t};\n";

        s << "\tsize\n\t{\n";
        for (auto& sample : visSamples)
        {
            s << std::format("\t\t{} {} {} {} {} {} {}\n",
                0.0f,
                sample.base.size[0], sample.base.size[1],
                sample.base.scale,
                sample.amplitude.size[0], sample.amplitude.size[1],
                sample.amplitude.scale);
        }
        s << "\t};\n";

        s << "\trotation\n\t{\n";
        for (auto& sample : visSamples)
        {
            s << std::format("\t\t{} {} {} {} {}\n",
                0.0f,
                sample.base.rotationDelta, sample.base.rotationTotal,
                sample.amplitude.rotationDelta, sample.amplitude.rotationTotal);
        }
        s << "\t};\n";
    }

    const char* GetElementTypeName(uint8_t type)
    {
        switch (type)
        {
        case 0x0: return "billboardSprite";
        case 0x1: return "orientedSprite";
        case 0x2: return "rotatedSprite";
        case 0x3: return "tail";
        case 0x4: return "line";
        case 0x5: return "trail";
        case 0x6: return "cloud";
        case 0x7: return "model";
        case 0x8: return "light";
        case 0x9: return "spotLight";
        case 0xA: return "sound";
        case 0xB: return "decal";
        case 0xC: return "runner";
        default: return "unknown";
        }
    }

    void DumpElement(std::ostream& s, const FxElemDef& elem, uint32_t elemIndex, uint32_t elemType, bool looping)
    {
        if (looping)
            s << std::format("looping_{} {}\n{{\n", GetElementTypeName(elem.elemType), elemIndex);
        else if (elemType == 1)
            s << std::format("oneshot_{} {}\n{{\n", GetElementTypeName(elem.elemType), elemIndex);
        else
            s << std::format("emission_{} {}\n{{\n", GetElementTypeName(elem.elemType), elemIndex);

        s << std::format("\teditorFlags{};\n", BuildEditorFlags(looping, elem));
        s << std::format("\tflags{};\n", BuildElementFlags(elem.flags));

        WriteRangeFloatInverse(s, "\tspawnRange", elem.spawnRange.base, elem.spawnRange.amplitude);
        WriteRangeFloatInverse(s, "\tfadeInRange", elem.fadeInRange.base, elem.fadeInRange.amplitude);
        WriteRangeFloatInverse(s, "\tfadeOutRange", elem.fadeOutRange.base, elem.fadeOutRange.amplitude);

        s << std::format("\tspawnFrustumCullRadius {};\n", elem.spawnFrustumCullRadius);

        s << std::format("\tspawnLooping {} {};\n", elem.spawn.looping.intervalMsec, elem.spawn.looping.count);
        s << std::format("\tspawnOneShot {} {};\n", elem.spawn.oneShot.count.base,
            (elem.spawn.oneShot.count.amplitude != 0x7FFFFFFF) ? elem.spawn.oneShot.count.amplitude : 0);

        WriteRangeIntInverse(s, "\tspawnDelayMsec", elem.spawnDelayMsec.base, elem.spawnDelayMsec.amplitude);
        WriteRangeIntInverse(s, "\tlifeSpanMsec", elem.lifeSpanMsec.base, elem.lifeSpanMsec.amplitude);

        for (int i = 0; i < 3; i++)
        {
            const char* names[] = { "\tspawnOrgX", "\tspawnOrgY", "\tspawnOrgZ" };
            WriteRangeFloatInverse(s, names[i], elem.spawnOrigin[i].base, elem.spawnOrigin[i].amplitude);
        }

        WriteRangeFloatInverse(s, "\tspawnOffsetRadius", elem.spawnOffsetRadius.base, elem.spawnOffsetRadius.amplitude);
        WriteRangeFloatInverse(s, "\tspawnOffsetHeight", elem.spawnOffsetHeight.base, elem.spawnOffsetHeight.amplitude);

        for (int i = 0; i < 3; i++)
        {
            const char* names[] = { "\tspawnAnglePitch", "\tspawnAngleYaw", "\tspawnAngleRoll" };
            WriteRangeFloat(s, names[i], RadToDeg(elem.spawnAngles[i].base), RadToDeg(elem.spawnAngles[i].amplitude));
        }

        for (int i = 0; i < 3; i++)
        {
            const char* names[] = { "\tangleVelPitch", "\tangleVelYaw", "\tangleVelRoll" };
            WriteRangeFloat(s, names[i], AngularVelToDeg(elem.angularVelocity[i].base), AngularVelToDeg(elem.angularVelocity[i].amplitude));
        }

        WriteRangeFloat(s, "\tinitialRot", RadToDeg(elem.initialRotation.base), RadToDeg(elem.initialRotation.amplitude));

        WriteRangeFloat(s, "\tgravity", elem.gravity.base * 100.0f, elem.gravity.amplitude * 100.0f);
        WriteRangeFloat(s, "\telasticity", elem.reflectionFactor.base, elem.reflectionFactor.amplitude);
        s << std::format("\twindinfluence {};\n", elem.windInfluence);

        s << std::format("\tatlasBehavior{};\n", BuildAtlasFlags(elem.atlas.behavior));
        s << std::format("\tatlasIndex {};\n", static_cast<int>(elem.atlas.index));
        s << std::format("\tatlasFps {};\n", static_cast<int>(elem.atlas.fps));
        s << std::format("\tatlasLoopCount {};\n", static_cast<int>(elem.atlas.loopCount));
        s << std::format("\tatlasColIndexBits {};\n", static_cast<int>(elem.atlas.colIndexBits));
        s << std::format("\tatlasRowIndexBits {};\n", static_cast<int>(elem.atlas.rowIndexBits));
        s << std::format("\tatlasEntryCount {};\n", 1 << (elem.atlas.colIndexBits + elem.atlas.rowIndexBits));
        s << std::format("\tatlasIndexRange {};\n", (elem.atlas.entryCountAndIndexRange >> 9) & 0x7F);

        if (elem.velSamples && elem.velIntervalCount > 0)
        {
            uint32_t velCount = static_cast<uint32_t>(elem.velIntervalCount + 1);
            WriteVelGraph(s, "velGraph0X", 0, elem.velSamples, velCount);
            WriteVelGraph(s, "velGraph0Y", 1, elem.velSamples, velCount);
            WriteVelGraph(s, "velGraph0Z", 2, elem.velSamples, velCount);
        }

        {
            FxElemVelStateSample zero{};
            WriteVelGraph(s, "velGraph1X", 0, &zero, 2);
            WriteVelGraph(s, "velGraph1Y", 1, &zero, 2);
            WriteVelGraph(s, "velGraph1Z", 2, &zero, 2);
        }

        if (elem.visSamples && elem.visStateIntervalCount > 0)
        {
            uint32_t visCount = static_cast<uint32_t>(elem.visStateIntervalCount + 1);
            WriteVisSamples(s, elem.visSamples, visCount);
        }

        s << std::format("\tlightingFrac {};\n", static_cast<float>(elem.lightingFrac) / 255.0f);

        float colRadix = (std::abs(elem.collMins[0]) + std::abs(elem.collMaxs[1])) / 2.0f;
        s << std::format("\tcollOffset {} {} {};\n",
            elem.collMaxs[0] - colRadix, elem.collMaxs[1] - colRadix, elem.collMaxs[2] - colRadix);
        s << std::format("\tcollRadius {};\n", colRadix);

        const char* onImpact = elem.effectOnImpact.name ? elem.effectOnImpact.name : "";
        const char* onDeath = elem.effectOnDeath.name ? elem.effectOnDeath.name : "";
        s << std::format("\tfxOnImpact \"{}\";\n\tfxOnDeath \"{}\";\n", onImpact, onDeath);

        s << std::format("\tsortOrder {};\n", static_cast<int>(elem.sortOrder));

        const char* emitted = elem.effectEmitted.name ? elem.effectEmitted.name : "";
        s << std::format("\temission \"{}\";\n", emitted);
        WriteRangeFloat(s, "\temitDist", elem.emitDist.base, elem.emitDist.amplitude);
        WriteRangeFloat(s, "\temitDistVariance", elem.emitDistVariance.base, elem.emitDistVariance.amplitude);

        const char* attached = elem.effectAttached.name ? elem.effectAttached.name : "";
        s << std::format("\tattachment \"{}\";\n", attached);

        // T5: trailDef is a direct pointer, not behind a union
        const auto* trailDef = elem.trailDef;
        if (trailDef)
        {
            s << std::format("\ttrailSplitDist {};\n", trailDef->splitDist);
            s << std::format("\ttrailScrollTime {};\n", static_cast<float>(trailDef->scrollTimeMsec) / 1000.0f);
            s << std::format("\ttrailRepeatDist {};\n", trailDef->repeatDist);
        }

        s << std::format("\talphafadetimemsec {};\n", elem.alphaFadeTimeMsec);
        s << std::format("\tmaxwind_mag {};\n", elem.maxWindStrength);
        s << std::format("\tmaxwind_life {};\n", elem.lifespanAtMaxWind);
        s << std::format("\tmaxwind_interval {};\n", elem.spawnIntervalAtMaxWind);

        if (elem.elemType != FX_ELEM_TYPE_CLOUD)
        {
            s << std::format("\tbillboardTopWidth {};\n", elem.u.billboard.topWidth);
            s << std::format("\tbillboardBottomWidth {};\n", elem.u.billboard.bottomWidth);
        }
        else
        {
            s << "\tbillboardTopWidth 1;\n";
            s << "\tbillboardBottomWidth 1;\n";
        }

        s << "\telemSpawnSound\n\t{\n";
        if (elem.spawnSound.spawnSound && elem.spawnSound.spawnSound[0])
            s << std::format("\t\t\"{}\"\n", elem.spawnSound.spawnSound);
        s << "\t};\n";

        if (elem.elemType == FX_ELEM_TYPE_CLOUD)
        {
            s << std::format("\tcloudDensity {} {};\n", elem.u.cloudDensityRange.base, elem.u.cloudDensityRange.amplitude);
        }
        else
        {
            s << "\tcloudDensity 1024 0;\n";
        }

        if (trailDef)
        {
            if (trailDef->vertCount > 0 && trailDef->verts)
            {
                s << "\ttrailDef\n\t{\n";
                for (int v = 0; v < trailDef->vertCount; v++)
                {
                    s << std::format("\t\t {} {} {}\n",
                        trailDef->verts[v].pos[0], trailDef->verts[v].pos[1], trailDef->verts[v].texCoord);
                }
                s << "\t} {\n";
            }

            if (trailDef->indCount > 0 && trailDef->inds)
            {
                for (int v = 0; v < trailDef->indCount; v++)
                {
                    s << std::format("\t\t {}\n", trailDef->inds[v]);
                }
            }

            if (trailDef->vertCount > 0 || trailDef->indCount > 0)
            {
                s << "\t};\n";
            }
        }

        s << std::format("\tbillboardPivot {} {};\n", elem.billboardPivot[0] / 2.0f, elem.billboardPivot[1] / -2.0f);

        // T5 FxElemVisuals has no lightDef, so all types except omniLight/spotLight have visuals
        bool hasVisuals = (elem.elemType != FX_ELEM_TYPE_OMNI_LIGHT && elem.elemType != FX_ELEM_TYPE_SPOT_LIGHT);
        if (hasVisuals)
            s << std::format("\t{}\n\t{{\n", GetElementTypeName(elem.elemType));

        if (elem.visualCount == 1)
        {
            switch (elem.elemType)
            {
            case FX_ELEM_TYPE_SPRITE_BILLBOARD:
            case FX_ELEM_TYPE_SPRITE_ORIENTED:
            case FX_ELEM_TYPE_SPRITE_ROTATED:
            case FX_ELEM_TYPE_TRAIL:
            case FX_ELEM_TYPE_LINE:
                if (elem.visuals.instance.material)
                    s << std::format("\t\t\"{}\"\n", elem.visuals.instance.material->info.name);
                break;
            case FX_ELEM_TYPE_TAIL:
            case FX_ELEM_TYPE_CLOUD:
            case FX_ELEM_TYPE_MODEL:
            case FX_ELEM_TYPE_RUNNER:
                if (elem.visuals.instance.effectDef.name)
                    s << std::format("\t\t\"{}\"\n", elem.visuals.instance.effectDef.name);
                break;
            case FX_ELEM_TYPE_DECAL:
                if (elem.visuals.instance.material)
                    s << std::format("\t\t\"{}\"\n", elem.visuals.instance.material->info.name);
                break;
            case FX_ELEM_TYPE_SOUND:
                if (elem.visuals.instance.soundName)
                    s << std::format("\t\t\"{}\"\n", elem.visuals.instance.soundName);
                break;
            }
        }
        else if (elem.visualCount > 1 && elem.visuals.array)
        {
            for (uint32_t v = 0; v < static_cast<uint32_t>(elem.visualCount); v++)
            {
                const auto& visual = elem.visuals.array[v];
                switch (elem.elemType)
                {
                case FX_ELEM_TYPE_SPRITE_BILLBOARD:
                case FX_ELEM_TYPE_SPRITE_ORIENTED:
                case FX_ELEM_TYPE_SPRITE_ROTATED:
                case FX_ELEM_TYPE_TRAIL:
                case FX_ELEM_TYPE_LINE:
                    if (visual.material)
                        s << std::format("\t\t\"{}\"\n", visual.material->info.name);
                    break;
                case FX_ELEM_TYPE_TAIL:
                case FX_ELEM_TYPE_CLOUD:
                case FX_ELEM_TYPE_MODEL:
                case FX_ELEM_TYPE_RUNNER:
                    if (visual.effectDef.name)
                        s << std::format("\t\t\"{}\"\n", visual.effectDef.name);
                    break;
                case FX_ELEM_TYPE_DECAL:
                    if (visual.material)
                        s << std::format("\t\t\"{}\"\n", visual.material->info.name);
                    break;
                case FX_ELEM_TYPE_SOUND:
                    if (visual.soundName)
                        s << std::format("\t\t\"{}\"\n", visual.soundName);
                    break;
                }
            }
        }

        if (hasVisuals)
            s << "\t};\n";

        s << "}\n\n";
    }
} // namespace

namespace fx_effect
{
    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFx::Type>& asset)
    {
        const auto* fxDef = asset.Asset();
        if (!fxDef || !fxDef->name)
        {
            con::error("FxEffectDef asset data was invalid!");
            return;
        }

        const auto assetFile = context.OpenAssetFile(std::format("{}.efx", asset.m_name));
        if (!assetFile)
        {
            con::error("Could not open FX file for dumping!");
            return;
        }

        auto& s = *assetFile;

        s << "iwfx 2\n\n";

        if (!fxDef->elemDefs)
            return;

        const auto* elem = fxDef->elemDefs;

        for (int i = 0; i < fxDef->elemDefCountLooping; i++)
        {
            DumpElement(s, elem[i], static_cast<uint32_t>(i), 0, true);
        }

        for (int i = 0; i < fxDef->elemDefCountOneShot; i++)
        {
            DumpElement(s, elem[fxDef->elemDefCountLooping + i], static_cast<uint32_t>(i), 1, false);
        }

        for (int i = 0; i < fxDef->elemDefCountEmission; i++)
        {
            DumpElement(s, elem[fxDef->elemDefCountLooping + fxDef->elemDefCountOneShot + i], static_cast<uint32_t>(i), 2, false);
        }
    }
} // namespace fx_effect