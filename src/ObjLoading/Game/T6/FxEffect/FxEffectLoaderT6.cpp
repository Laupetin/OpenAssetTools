#include "FxEffectLoaderT6.h"

#include "Game/T6/CommonT6.h"
#include "Utils/Logging/Log.h"

#include <charconv>
#include <cmath>
#include <cstring>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace T6;

namespace
{
    constexpr float DEG_TO_RAD = 0.017453292519943f;
    constexpr float DEG_TO_ANGVEL = 0.001f;

    float DegToRad(float deg)
    {
        return deg * DEG_TO_RAD;
    }

    float AngvelToRad(float val)
    {
        return val * DEG_TO_RAD * DEG_TO_ANGVEL;
    }

    std::string_view TrimWhitespace(std::string_view s)
    {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
            s.remove_prefix(1);
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
            s.remove_suffix(1);
        return s;
    }

    std::string_view ReadToken(std::string_view& line)
    {
        line = TrimWhitespace(line);
        if (line.empty())
            return {};

        if (line.front() == '"')
        {
            line.remove_prefix(1);
            auto end = line.find('"');
            if (end == std::string_view::npos)
            {
                auto token = line;
                line = {};
                return token;
            }
            auto token = line.substr(0, end);
            line.remove_prefix(end + 1);
            return token;
        }

        auto end = line.find_first_of(" \t;");
        if (end == std::string_view::npos)
        {
            auto token = line;
            line = {};
            return token;
        }
        auto token = line.substr(0, end);
        line.remove_prefix(end);
        return token;
    }

    void SkipSemicolon(std::string_view& line)
    {
        line = TrimWhitespace(line);
        if (!line.empty() && line.front() == ';')
            line.remove_prefix(1);
    }

    int ParseInt(std::string_view s)
    {
        s = TrimWhitespace(s);
        int value = 0;
        std::from_chars(s.data(), s.data() + s.size(), value);
        return value;
    }

    float ParseFloat(std::string_view s)
    {
        s = TrimWhitespace(s);
        float value = 0.0f;
        std::from_chars(s.data(), s.data() + s.size(), value);
        return value;
    }

    void ParseRangeFloat(std::string_view& line, float& base, float& amp)
    {
        auto tok1 = ReadToken(line);
        auto tok2 = ReadToken(line);
        base = ParseFloat(tok1);
        amp = ParseFloat(tok2);
        SkipSemicolon(line);
    }

    void ParseRangeInt(std::string_view& line, int& base, int& amp)
    {
        auto tok1 = ReadToken(line);
        auto tok2 = ReadToken(line);
        base = ParseInt(tok1);
        amp = ParseInt(tok2);
        SkipSemicolon(line);
    }

    std::string ReadQuotedString(std::string_view& line)
    {
        auto token = ReadToken(line);
        SkipSemicolon(line);
        return std::string(token);
    }

    struct ElementData
    {
        std::string typeName;
        bool isLooping = false;
        bool isEmission = false;
        int index = 0;

        int flags = 0;
        FxSpawnDef spawn{};
        FxFloatRange spawnRange{};
        FxFloatRange fadeInRange{};
        FxFloatRange fadeOutRange{};
        float spawnFrustumCullRadius = 0.0f;
        FxIntRange spawnDelayMsec{};
        FxIntRange lifeSpanMsec{};
        FxFloatRange spawnOrigin[3]{};
        FxFloatRange spawnOffsetRadius{};
        FxFloatRange spawnOffsetHeight{};
        FxFloatRange spawnAngles[3]{};
        FxFloatRange angularVelocity[3]{};
        FxFloatRange initialRotation{};
        FxFloatRange gravity{};
        FxFloatRange reflectionFactor{};
        FxElemAtlas atlas{};
        float windInfluence = 0.0f;
        float lightingFrac = 0.0f;
        float collMins[3]{};
        float collMaxs[3]{};
        std::string effectOnImpact;
        std::string effectOnDeath;
        int sortOrder = 0;
        std::string effectEmitted;
        FxFloatRange emitDist{};
        FxFloatRange emitDistVariance{};
        std::string effectAttached;
        float trailSplitDist = 0.0f;
        float trailScrollTime = 0.0f;
        float trailRepeatDist = 0.0f;
        uint16_t alphaFadeTimeMsec = 0;
        uint16_t maxWindStrength = 0;
        uint16_t lifespanAtMaxWind = 0;
        uint16_t spawnIntervalAtMaxWind = 0;
        float billboardTopWidth = 1.0f;
        float billboardBottomWidth = 1.0f;
        std::string spawnSound;
        float cloudDensityBase = 1024.0f;
        float cloudDensityAmp = 0.0f;
        float billboardPivot[2]{};

        std::vector<FxElemVelStateSample> velSamples;
        uint32_t velIntervalCount = 0;

        struct VisColor
        {
            int base[4]{};
            int amp[4]{};
        };
        struct VisSize
        {
            float base[2]{};
            float baseScale = 1.0f;
            float amp[2]{};
            float ampScale = 0.0f;
        };
        struct VisRotation
        {
            float baseDelta = 0.0f;
            float baseTotal = 1.0f;
            float ampDelta = 0.0f;
            float ampTotal = 0.0f;
        };
        std::vector<VisColor> visColors;
        std::vector<VisSize> visSizes;
        std::vector<VisRotation> visRotations;

        std::vector<std::string> visuals;

        std::vector<float> trailVerts;
        std::vector<uint16_t> trailInds;
    };

    class EfxParser
    {
    public:
        EfxParser(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult ParseAndCreate(const std::string& assetName, AssetCreationContext& context)
        {
            const auto fileName = assetName + ".efx";
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            std::string content(static_cast<size_t>(file.m_length), '\0');
            file.m_stream->read(content.data(), file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            std::istringstream stream(content);
            std::string line;

            // Read header
            if (!std::getline(stream, line))
                return AssetCreationResult::Failure();

            line = TrimWhitespace(line);
            if (line != "iwfx 2")
            {
                con::error("Invalid .efx header: \"{}\"", line);
                return AssetCreationResult::Failure();
            }

            std::vector<ElementData> elements;

            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line.empty())
                    continue;

                // Check for element header
                // Format: looping_typeName index, oneshot_typeName index, emission_typeName index
                std::string elemHeader = line;
                if (elemHeader.find('{') != std::string::npos)
                    elemHeader = elemHeader.substr(0, elemHeader.find('{'));

                elemHeader = TrimWhitespace(elemHeader);

                bool isLooping = false;
                bool isEmission = false;
                bool isOneShot = false;

                if (elemHeader.compare(0, 8, "looping_") == 0)
                    isLooping = true;
                else if (elemHeader.compare(0, 9, "oneshot_") == 0)
                    isOneShot = true;
                else if (elemHeader.compare(0, 9, "emission_") == 0)
                    isEmission = true;

                if (isLooping || isOneShot || isEmission)
                {
                    ElementData elem{};
                    elem.isLooping = isLooping;
                    elem.isEmission = isEmission;

                    auto spacePos = elemHeader.find(' ');
                    if (spacePos != std::string::npos)
                    {
                        auto typeName = elemHeader.substr(isLooping ? 8 : (isOneShot ? 9 : 9), spacePos - (isLooping ? 8 : (isOneShot ? 9 : 9)));
                        elem.typeName = typeName;
                        elem.index = ParseInt(elemHeader.substr(spacePos + 1));
                    }

                    // Read element block
                    if (std::getline(stream, line))
                    {
                        ParseElementBlock(stream, elem);
                    }

                    elements.push_back(std::move(elem));
                    continue;
                }
            }

            // Count elements
            int loopCount = 0;
            int oneShotCount = 0;
            int emissionCount = 0;
            for (const auto& elem : elements)
            {
                if (elem.isLooping)
                    loopCount++;
                else if (elem.isEmission)
                    emissionCount++;
                else
                    oneShotCount++;
            }

            // Allocate FxEffectDef
            auto* fxDef = m_memory.Alloc<FxEffectDef>();
            fxDef->name = m_memory.Dup(assetName.c_str());
            fxDef->flags = 0;
            fxDef->efPriority = 0;
            fxDef->elemDefCountLooping = static_cast<int16_t>(loopCount);
            fxDef->elemDefCountOneShot = static_cast<int16_t>(oneShotCount);
            fxDef->elemDefCountEmission = static_cast<int16_t>(emissionCount);
            fxDef->totalSize = 0;
            fxDef->msecLoopingLife = 0;
            fxDef->msecNonLoopingLife = 0;

            const int totalElements = loopCount + oneShotCount + emissionCount;
            if (totalElements > 0)
            {
                fxDef->elemDefs = m_memory.Alloc<FxElemDef>(static_cast<size_t>(totalElements));
                std::memset(fxDef->elemDefs, 0, sizeof(FxElemDef) * static_cast<size_t>(totalElements));

                int elemIdx = 0;
                for (auto& elemData : elements)
                {
                    auto& elem = fxDef->elemDefs[elemIdx++];

                    elem.flags = elemData.flags;
                    elem.spawn = elemData.spawn;
                    elem.spawnRange = elemData.spawnRange;
                    elem.fadeInRange = elemData.fadeInRange;
                    elem.fadeOutRange = elemData.fadeOutRange;
                    elem.spawnFrustumCullRadius = elemData.spawnFrustumCullRadius;
                    elem.spawnDelayMsec = elemData.spawnDelayMsec;
                    elem.lifeSpanMsec = elemData.lifeSpanMsec;
                    for (int i = 0; i < 3; i++)
                        elem.spawnOrigin[i] = elemData.spawnOrigin[i];
                    elem.spawnOffsetRadius = elemData.spawnOffsetRadius;
                    elem.spawnOffsetHeight = elemData.spawnOffsetHeight;
                    for (int i = 0; i < 3; i++)
                        elem.spawnAngles[i] = elemData.spawnAngles[i];
                    for (int i = 0; i < 3; i++)
                        elem.angularVelocity[i] = elemData.angularVelocity[i];
                    elem.initialRotation = elemData.initialRotation;
                    elem.gravity = elemData.gravity;
                    elem.reflectionFactor = elemData.reflectionFactor;
                    elem.atlas = elemData.atlas;
                    elem.windInfluence = elemData.windInfluence;

                    // Determine element type
                    elem.elemType = GetElementType(elemData.typeName);
                    elem.visualCount = static_cast<char>(elemData.visuals.size());
                    elem.velIntervalCount = static_cast<char>(elemData.velIntervalCount);
                    elem.visStateIntervalCount = static_cast<char>(elemData.visColors.size() > 0 ? elemData.visColors.size() - 1 : 0);

                    // Velocity samples
                    if (!elemData.velSamples.empty())
                    {
                        elem.velSamples = m_memory.Alloc<FxElemVelStateSample>(elemData.velSamples.size());
                        std::memcpy(elem.velSamples, elemData.velSamples.data(), sizeof(FxElemVelStateSample) * elemData.velSamples.size());
                    }

                    // Visual state samples
                    if (!elemData.visColors.empty())
                    {
                        elem.visStateIntervalCount = static_cast<char>(elemData.visColors.size() - 1);
                        elem.visSamples = m_memory.Alloc<FxElemVisStateSample>(elemData.visColors.size());
                        for (size_t v = 0; v < elemData.visColors.size(); v++)
                        {
                            auto& vis = elem.visSamples[v];
                            vis.base.color[0] = static_cast<char>(elemData.visColors[v].base[0]);
                            vis.base.color[1] = static_cast<char>(elemData.visColors[v].base[1]);
                            vis.base.color[2] = static_cast<char>(elemData.visColors[v].base[2]);
                            vis.base.color[3] = static_cast<char>(elemData.visColors[v].base[3]);
                            vis.amplitude.color[0] = static_cast<char>(elemData.visColors[v].amp[0]);
                            vis.amplitude.color[1] = static_cast<char>(elemData.visColors[v].amp[1]);
                            vis.amplitude.color[2] = static_cast<char>(elemData.visColors[v].amp[2]);
                            vis.amplitude.color[3] = static_cast<char>(elemData.visColors[v].amp[3]);

                            if (v < elemData.visSizes.size())
                            {
                                vis.base.size[0] = elemData.visSizes[v].base[0];
                                vis.base.size[1] = elemData.visSizes[v].base[1];
                                vis.base.scale = elemData.visSizes[v].baseScale;
                                vis.amplitude.size[0] = elemData.visSizes[v].amp[0];
                                vis.amplitude.size[1] = elemData.visSizes[v].amp[1];
                                vis.amplitude.scale = elemData.visSizes[v].ampScale;
                            }

                            if (v < elemData.visRotations.size())
                            {
                                vis.base.rotationDelta = elemData.visRotations[v].baseDelta;
                                vis.base.rotationTotal = elemData.visRotations[v].baseTotal;
                                vis.amplitude.rotationDelta = elemData.visRotations[v].ampDelta;
                                vis.amplitude.rotationTotal = elemData.visRotations[v].ampTotal;
                            }
                        }
                    }

                    // Visuals
                    if (!elemData.visuals.empty() && elemData.visuals.size() == 1)
                    {
                        SetSingleVisual(elem, elemData);
                    }
                    else if (elemData.visuals.size() > 1)
                    {
                        auto* array = m_memory.Alloc<FxElemVisuals>(elemData.visuals.size());
                        for (size_t v = 0; v < elemData.visuals.size(); v++)
                        {
                            SetVisual(array[v], elemData, v);
                        }
                        elem.visuals.array = array;
                    }

                    elem.collMins.v[0] = elemData.collMins[0];
                    elem.collMins.v[1] = elemData.collMins[1];
                    elem.collMins.v[2] = elemData.collMins[2];
                    elem.collMaxs.v[0] = elemData.collMaxs[0];
                    elem.collMaxs.v[1] = elemData.collMaxs[1];
                    elem.collMaxs.v[2] = elemData.collMaxs[2];

                    // Sub-effect dependencies
                    if (!elemData.effectOnImpact.empty())
                    {
                        auto* dep = context.LoadDependency<AssetFx>(elemData.effectOnImpact);
                        if (dep)
                            elem.effectOnImpact.handle = dep->Asset();
                        else
                            elem.effectOnImpact.name = m_memory.Dup(elemData.effectOnImpact.c_str());
                    }
                    if (!elemData.effectOnDeath.empty())
                    {
                        auto* dep = context.LoadDependency<AssetFx>(elemData.effectOnDeath);
                        if (dep)
                            elem.effectOnDeath.handle = dep->Asset();
                        else
                            elem.effectOnDeath.name = m_memory.Dup(elemData.effectOnDeath.c_str());
                    }
                    if (!elemData.effectEmitted.empty())
                    {
                        auto* dep = context.LoadDependency<AssetFx>(elemData.effectEmitted);
                        if (dep)
                            elem.effectEmitted.handle = dep->Asset();
                        else
                            elem.effectEmitted.name = m_memory.Dup(elemData.effectEmitted.c_str());
                    }
                    if (!elemData.effectAttached.empty())
                    {
                        auto* dep = context.LoadDependency<AssetFx>(elemData.effectAttached);
                        if (dep)
                            elem.effectAttached.handle = dep->Asset();
                        else
                            elem.effectAttached.name = m_memory.Dup(elemData.effectAttached.c_str());
                    }

                    elem.emitDist = elemData.emitDist;
                    elem.emitDistVariance = elemData.emitDistVariance;

                    // Trail definition
                    if (elemData.trailSplitDist != 0.0f || elemData.trailScrollTime != 0.0f || elemData.trailRepeatDist != 0.0f)
                    {
                        auto* trail = m_memory.Alloc<FxTrailDef>();
                        trail->splitDist = static_cast<int>(elemData.trailSplitDist);
                        trail->scrollTimeMsec = static_cast<int>(elemData.trailScrollTime * 1000.0f);
                        trail->repeatDist = static_cast<int>(elemData.trailRepeatDist);
                        trail->vertCount = 0;
                        trail->verts = nullptr;
                        trail->indCount = 0;
                        trail->inds = nullptr;

                        if (!elemData.trailVerts.empty())
                        {
                            trail->vertCount = static_cast<int>(elemData.trailVerts.size() / 3);
                            trail->verts = m_memory.Alloc<FxTrailVertex>(static_cast<size_t>(trail->vertCount));
                            for (int v = 0; v < trail->vertCount; v++)
                            {
                                trail->verts[v].pos.v[0] = elemData.trailVerts[v * 3 + 0];
                                trail->verts[v].pos.v[1] = elemData.trailVerts[v * 3 + 1];
                                trail->verts[v].texCoord = elemData.trailVerts[v * 3 + 2];
                            }
                        }
                        if (!elemData.trailInds.empty())
                        {
                            trail->indCount = static_cast<int>(elemData.trailInds.size());
                            trail->inds = m_memory.Alloc<uint16_t>(elemData.trailInds.size());
                            std::memcpy(trail->inds, elemData.trailInds.data(), sizeof(uint16_t) * elemData.trailInds.size());
                        }

                        elem.extended.trailDef = trail;
                    }

                    elem.sortOrder = static_cast<char>(elemData.sortOrder);
                    elem.lightingFrac = static_cast<char>(elemData.lightingFrac * 255.0f);
                    elem.alphaFadeTimeMsec = elemData.alphaFadeTimeMsec;
                    elem.maxWindStrength = elemData.maxWindStrength;
                    elem.lifespanAtMaxWind = elemData.lifespanAtMaxWind;
                    elem.spawnIntervalAtMaxWind = elemData.spawnIntervalAtMaxWind;

                    if (elem.elemType == FX_ELEM_TYPE_CLOUD)
                    {
                        elem.u.cloudDensityRange.base = static_cast<int>(elemData.cloudDensityBase);
                        elem.u.cloudDensityRange.amplitude = static_cast<int>(elemData.cloudDensityAmp);
                    }
                    else
                    {
                        elem.u.billboard.topWidth = elemData.billboardTopWidth;
                        elem.u.billboard.bottomWidth = elemData.billboardBottomWidth;
                    }

                    if (!elemData.spawnSound.empty())
                    {
                        auto* snd = m_memory.Alloc<FxElemSpawnSound>();
                        snd->spawnSound = m_memory.Dup(elemData.spawnSound.c_str());
                        elem.spawnSound = *snd;
                    }

                    elem.billboardPivot.v[0] = elemData.billboardPivot[0];
                    elem.billboardPivot.v[1] = elemData.billboardPivot[1];
                }
            }

            return AssetCreationResult::Success(context.AddAsset<AssetFx>(assetName, fxDef));
        }

    private:
        void ParseElementBlock(std::istream& stream, ElementData& elem)
        {
            std::string line;
            int braceDepth = 1;

            while (braceDepth > 0 && std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line.empty())
                    continue;

                if (line == "{")
                {
                    braceDepth++;
                    continue;
                }
                if (line == "}" || line == "};")
                {
                    braceDepth--;
                    continue;
                }

                // Strip trailing semicolons for easier parsing
                if (!line.empty() && line.back() == ';')
                    line.pop_back();

                auto sv = std::string_view(line);
                auto key = ReadToken(sv);
                sv = TrimWhitespace(sv);

                if (key == "editorFlags")
                {
                    // Skip - derived from element flags and sub-effects
                }
                else if (key == "flags")
                {
                    elem.flags = ParseFlagsString(sv);
                }
                else if (key == "spawnRange")
                {
                    ParseRangeFloat(sv, elem.spawnRange.base, elem.spawnRange.amplitude);
                    // Dumper negates, so negate back
                    elem.spawnRange.base = -elem.spawnRange.base;
                    elem.spawnRange.amplitude = -elem.spawnRange.amplitude;
                }
                else if (key == "fadeInRange")
                {
                    ParseRangeFloat(sv, elem.fadeInRange.base, elem.fadeInRange.amplitude);
                    elem.fadeInRange.base = -elem.fadeInRange.base;
                    elem.fadeInRange.amplitude = -elem.fadeInRange.amplitude;
                }
                else if (key == "fadeOutRange")
                {
                    ParseRangeFloat(sv, elem.fadeOutRange.base, elem.fadeOutRange.amplitude);
                    elem.fadeOutRange.base = -elem.fadeOutRange.base;
                    elem.fadeOutRange.amplitude = -elem.fadeOutRange.amplitude;
                }
                else if (key == "spawnFrustumCullRadius")
                {
                    elem.spawnFrustumCullRadius = ParseFloat(ReadToken(sv));
                }
                else if (key == "spawnLooping")
                {
                    auto tok1 = ReadToken(sv);
                    auto tok2 = ReadToken(sv);
                    elem.spawn.looping.intervalMsec = ParseInt(tok1);
                    elem.spawn.looping.count = ParseInt(tok2);
                }
                else if (key == "spawnOneShot")
                {
                    auto tok1 = ReadToken(sv);
                    auto tok2 = ReadToken(sv);
                    elem.spawn.oneShot.count.base = ParseInt(tok1);
                    elem.spawn.oneShot.count.amplitude = ParseInt(tok2);
                }
                else if (key == "spawnDelayMsec")
                {
                    ParseRangeInt(sv, elem.spawnDelayMsec.base, elem.spawnDelayMsec.amplitude);
                    elem.spawnDelayMsec.base = -elem.spawnDelayMsec.base;
                    elem.spawnDelayMsec.amplitude = -elem.spawnDelayMsec.amplitude;
                }
                else if (key == "lifeSpanMsec")
                {
                    ParseRangeInt(sv, elem.lifeSpanMsec.base, elem.lifeSpanMsec.amplitude);
                    elem.lifeSpanMsec.base = -elem.lifeSpanMsec.base;
                    elem.lifeSpanMsec.amplitude = -elem.lifeSpanMsec.amplitude;
                }
                else if (key == "spawnOrgX" || key == "spawnOrgY" || key == "spawnOrgZ")
                {
                    int idx = (key == "spawnOrgX") ? 0 : (key == "spawnOrgY") ? 1 : 2;
                    ParseRangeFloat(sv, elem.spawnOrigin[idx].base, elem.spawnOrigin[idx].amplitude);
                    elem.spawnOrigin[idx].base = -elem.spawnOrigin[idx].base;
                    elem.spawnOrigin[idx].amplitude = -elem.spawnOrigin[idx].amplitude;
                }
                else if (key == "spawnOffsetRadius")
                {
                    ParseRangeFloat(sv, elem.spawnOffsetRadius.base, elem.spawnOffsetRadius.amplitude);
                    elem.spawnOffsetRadius.base = -elem.spawnOffsetRadius.base;
                    elem.spawnOffsetRadius.amplitude = -elem.spawnOffsetRadius.amplitude;
                }
                else if (key == "spawnOffsetHeight")
                {
                    ParseRangeFloat(sv, elem.spawnOffsetHeight.base, elem.spawnOffsetHeight.amplitude);
                    elem.spawnOffsetHeight.base = -elem.spawnOffsetHeight.base;
                    elem.spawnOffsetHeight.amplitude = -elem.spawnOffsetHeight.amplitude;
                }
                else if (key == "spawnAnglePitch" || key == "spawnAngleYaw" || key == "spawnAngleRoll")
                {
                    int idx = (key == "spawnAnglePitch") ? 0 : (key == "spawnAngleYaw") ? 1 : 2;
                    float base, amp;
                    ParseRangeFloat(sv, base, amp);
                    elem.spawnAngles[idx].base = DegToRad(base);
                    elem.spawnAngles[idx].amplitude = DegToRad(amp);
                }
                else if (key == "angleVelPitch" || key == "angleVelYaw" || key == "angleVelRoll")
                {
                    int idx = (key == "angleVelPitch") ? 0 : (key == "angleVelYaw") ? 1 : 2;
                    float base, amp;
                    ParseRangeFloat(sv, base, amp);
                    elem.angularVelocity[idx].base = AngvelToRad(base);
                    elem.angularVelocity[idx].amplitude = AngvelToRad(amp);
                }
                else if (key == "initialRot")
                {
                    float base, amp;
                    ParseRangeFloat(sv, base, amp);
                    elem.initialRotation.base = DegToRad(base);
                    elem.initialRotation.amplitude = DegToRad(amp);
                }
                else if (key == "gravity")
                {
                    float base, amp;
                    ParseRangeFloat(sv, base, amp);
                    elem.gravity.base = base / 100.0f;
                    elem.gravity.amplitude = amp / 100.0f;
                }
                else if (key == "elasticity")
                {
                    ParseRangeFloat(sv, elem.reflectionFactor.base, elem.reflectionFactor.amplitude);
                }
                else if (key == "windinfluence")
                {
                    elem.windInfluence = ParseFloat(ReadToken(sv));
                }
                else if (key == "atlasBehavior")
                {
                    elem.atlas.behavior = ParseAtlasFlagsString(sv);
                }
                else if (key == "atlasIndex")
                {
                    elem.atlas.index = static_cast<char>(ParseInt(ReadToken(sv)));
                }
                else if (key == "atlasFps")
                {
                    elem.atlas.fps = static_cast<char>(ParseInt(ReadToken(sv)));
                }
                else if (key == "atlasLoopCount")
                {
                    elem.atlas.loopCount = static_cast<char>(ParseInt(ReadToken(sv)));
                }
                else if (key == "atlasColIndexBits")
                {
                    elem.atlas.colIndexBits = static_cast<char>(ParseInt(ReadToken(sv)));
                }
                else if (key == "atlasRowIndexBits")
                {
                    elem.atlas.rowIndexBits = static_cast<char>(ParseInt(ReadToken(sv)));
                }
                else if (key == "atlasEntryCount" || key == "atlasIndexRange")
                {
                    // Derived values, skip
                }
                else if (key == "velGraph0X" || key == "velGraph0Y" || key == "velGraph0Z" ||
                         key == "velGraph1X" || key == "velGraph1Y" || key == "velGraph1Z")
                {
                    ParseVelGraph(stream, elem, key);
                }
                else if (key == "color")
                {
                    ParseVisColorBlock(stream, elem);
                }
                else if (key == "size")
                {
                    ParseVisSizeBlock(stream, elem);
                }
                else if (key == "rotation")
                {
                    ParseVisRotationBlock(stream, elem);
                }
                else if (key == "lightingFrac")
                {
                    elem.lightingFrac = ParseFloat(ReadToken(sv));
                }
                else if (key == "collOffset")
                {
                    float ox, oy, oz;
                    auto t1 = ReadToken(sv);
                    auto t2 = ReadToken(sv);
                    auto t3 = ReadToken(sv);
                    ox = ParseFloat(t1);
                    oy = ParseFloat(t2);
                    oz = ParseFloat(t3);
                    // collOffset + collRadius reconstructs collMins/collMaxs
                    // We'll handle this after collRadius
                    elem.collMaxs[0] = ox;
                    elem.collMaxs[1] = oy;
                    elem.collMaxs[2] = oz;
                }
                else if (key == "collRadius")
                {
                    float radix = ParseFloat(ReadToken(sv));
                    elem.collMins[0] = elem.collMaxs[0] - radix;
                    elem.collMins[1] = elem.collMaxs[1] - radix;
                    elem.collMins[2] = elem.collMaxs[2] - radix;
                    elem.collMaxs[0] = elem.collMaxs[0] + radix;
                    elem.collMaxs[1] = elem.collMaxs[1] + radix;
                    elem.collMaxs[2] = elem.collMaxs[2] + radix;
                }
                else if (key == "fxOnImpact")
                {
                    elem.effectOnImpact = ReadQuotedString(sv);
                }
                else if (key == "fxOnDeath")
                {
                    elem.effectOnDeath = ReadQuotedString(sv);
                }
                else if (key == "sortOrder")
                {
                    elem.sortOrder = ParseInt(ReadToken(sv));
                }
                else if (key == "emission")
                {
                    elem.effectEmitted = ReadQuotedString(sv);
                }
                else if (key == "emitDist")
                {
                    ParseRangeFloat(sv, elem.emitDist.base, elem.emitDist.amplitude);
                }
                else if (key == "emitDistVariance")
                {
                    ParseRangeFloat(sv, elem.emitDistVariance.base, elem.emitDistVariance.amplitude);
                }
                else if (key == "attachment")
                {
                    elem.effectAttached = ReadQuotedString(sv);
                }
                else if (key == "trailSplitDist")
                {
                    elem.trailSplitDist = ParseFloat(ReadToken(sv));
                }
                else if (key == "trailScrollTime")
                {
                    elem.trailScrollTime = ParseFloat(ReadToken(sv));
                }
                else if (key == "trailRepeatDist")
                {
                    elem.trailRepeatDist = ParseFloat(ReadToken(sv));
                }
                else if (key == "alphafadetimemsec")
                {
                    elem.alphaFadeTimeMsec = static_cast<uint16_t>(ParseInt(ReadToken(sv)));
                }
                else if (key == "maxwind_mag")
                {
                    elem.maxWindStrength = static_cast<uint16_t>(ParseInt(ReadToken(sv)));
                }
                else if (key == "maxwind_life")
                {
                    elem.lifespanAtMaxWind = static_cast<uint16_t>(ParseInt(ReadToken(sv)));
                }
                else if (key == "maxwind_interval")
                {
                    elem.spawnIntervalAtMaxWind = static_cast<uint16_t>(ParseInt(ReadToken(sv)));
                }
                else if (key == "billboardTopWidth")
                {
                    elem.billboardTopWidth = ParseFloat(ReadToken(sv));
                }
                else if (key == "billboardBottomWidth")
                {
                    elem.billboardBottomWidth = ParseFloat(ReadToken(sv));
                }
                else if (key == "elemSpawnSound")
                {
                    // Block: elemSpawnSound { "soundName" };
                    std::string innerLine;
                    if (std::getline(stream, innerLine))
                    {
                        innerLine = TrimWhitespace(innerLine);
                        if (innerLine == "{")
                        {
                            if (std::getline(stream, innerLine))
                            {
                                innerLine = TrimWhitespace(innerLine);
                                if (!innerLine.empty() && innerLine.front() == '"' && innerLine.back() == '"')
                                    elem.spawnSound = innerLine.substr(1, innerLine.size() - 2);
                            }
                            std::getline(stream, innerLine); // closing }
                        }
                    }
                }
                else if (key == "cloudDensity")
                {
                    auto t1 = ReadToken(sv);
                    auto t2 = ReadToken(sv);
                    elem.cloudDensityBase = ParseFloat(t1);
                    elem.cloudDensityAmp = ParseFloat(t2);
                }
                else if (key == "trailDef")
                {
                    ParseTrailDefBlock(stream, elem);
                }
                else if (key == "billboardPivot")
                {
                    auto t1 = ReadToken(sv);
                    auto t2 = ReadToken(sv);
                    elem.billboardPivot[0] = ParseFloat(t1) * 2.0f;
                    elem.billboardPivot[1] = ParseFloat(t2) * -2.0f;
                }
                else if (key == "billboardSprite" || key == "orientedSprite" || key == "rotatedSprite" ||
                         key == "tail" || key == "line" || key == "trail" || key == "cloud" ||
                         key == "model" || key == "light" || key == "spotLight" ||
                         key == "sound" || key == "decal" || key == "runner")
                {
                    // Visual block: typeName { "visual1" "visual2" ... };
                    ParseVisualBlock(stream, elem, key);
                }
            }
        }

        void ParseVelGraph(std::istream& stream, ElementData& elem, std::string_view key)
        {
            // Format: velGraph0X maxValue { { basePoints } { ampPoints } };
            std::string line;
            std::getline(stream, line); // "maxValue {"
            std::getline(stream, line); // "{"

            // Read base curve
            std::vector<std::pair<float, float>> basePoints;
            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "}")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                auto tok1 = ReadToken(sv);
                auto tok2 = ReadToken(sv);
                float time = ParseFloat(tok1);
                float value = ParseFloat(tok2);
                basePoints.emplace_back(time, value);
            }

            std::getline(stream, line); // "{"

            // Read amplitude curve
            std::vector<std::pair<float, float>> ampPoints;
            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "}")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                auto tok1 = ReadToken(sv);
                auto tok2 = ReadToken(sv);
                float time = ParseFloat(tok1);
                float value = ParseFloat(tok2);
                ampPoints.emplace_back(time, value);
            }

            std::getline(stream, line); // "};"

            // Determine axis from key name
            int axis = 0;
            if (key.back() == 'Y') axis = 1;
            else if (key.back() == 'Z') axis = 2;

            // Ensure we have enough vel samples
            size_t sampleCount = std::max(basePoints.size(), ampPoints.size());
            if (sampleCount < 2) sampleCount = 2;

            if (elem.velSamples.size() < sampleCount)
                elem.velSamples.resize(sampleCount);

            elem.velIntervalCount = static_cast<uint32_t>(sampleCount - 1);

            // Fill in the velocity data for this axis
            for (size_t i = 0; i < basePoints.size() && i < sampleCount; i++)
            {
                elem.velSamples[i].local.velocity.base.v[axis] = basePoints[i].second;
            }
            for (size_t i = 0; i < ampPoints.size() && i < sampleCount; i++)
            {
                elem.velSamples[i].local.velocity.amplitude.v[axis] = ampPoints[i].second;
            }
        }

        void ParseVisColorBlock(std::istream& stream, ElementData& elem)
        {
            std::string line;
            std::getline(stream, line); // "{"

            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "};")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                ReadToken(sv); // time (always 0.0)

                ElementData::VisColor vc{};
                for (int i = 0; i < 4; i++)
                    vc.base[i] = ParseInt(ReadToken(sv));
                for (int i = 0; i < 4; i++)
                    vc.amp[i] = ParseInt(ReadToken(sv));
                ReadToken(sv); // scale (1.0)

                elem.visColors.push_back(vc);
            }
        }

        void ParseVisSizeBlock(std::istream& stream, ElementData& elem)
        {
            std::string line;
            std::getline(stream, line); // "{"

            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "};")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                ReadToken(sv); // time

                ElementData::VisSize vs{};
                vs.base[0] = ParseFloat(ReadToken(sv));
                vs.base[1] = ParseFloat(ReadToken(sv));
                vs.baseScale = ParseFloat(ReadToken(sv));
                vs.amp[0] = ParseFloat(ReadToken(sv));
                vs.amp[1] = ParseFloat(ReadToken(sv));
                vs.ampScale = ParseFloat(ReadToken(sv));

                elem.visSizes.push_back(vs);
            }
        }

        void ParseVisRotationBlock(std::istream& stream, ElementData& elem)
        {
            std::string line;
            std::getline(stream, line); // "{"

            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "};")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                ReadToken(sv); // time

                ElementData::VisRotation vr{};
                vr.baseDelta = ParseFloat(ReadToken(sv));
                vr.baseTotal = ParseFloat(ReadToken(sv));
                vr.ampDelta = ParseFloat(ReadToken(sv));
                vr.ampTotal = ParseFloat(ReadToken(sv));

                elem.visRotations.push_back(vr);
            }
        }

        void ParseTrailDefBlock(std::istream& stream, ElementData& elem)
        {
            // Format: trailDef { verts } { indices };
            std::string line;

            // Read verts block
            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "{")
                    continue;
                if (line == "}" || line == "};")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                auto t1 = ReadToken(sv);
                auto t2 = ReadToken(sv);
                auto t3 = ReadToken(sv);
                elem.trailVerts.push_back(ParseFloat(t1));
                elem.trailVerts.push_back(ParseFloat(t2));
                elem.trailVerts.push_back(ParseFloat(t3));
            }

            // Read indices block
            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "{")
                    continue;
                if (line == "}" || line == "};")
                    break;
                if (line.empty())
                    continue;

                auto sv = std::string_view(line);
                auto tok = ReadToken(sv);
                elem.trailInds.push_back(static_cast<uint16_t>(ParseInt(tok)));
            }
        }

        void ParseVisualBlock(std::istream& stream, ElementData& elem, std::string_view typeName)
        {
            std::string line;
            while (std::getline(stream, line))
            {
                line = TrimWhitespace(line);
                if (line == "{")
                    continue;
                if (line == "};")
                    break;
                if (line.empty())
                    continue;

                // Line should be a quoted string
                if (line.front() == '"' && (line.back() == '"' || line.back() == ';'))
                {
                    auto end = line.find_last_of('"');
                    if (end > 0)
                        elem.visuals.push_back(line.substr(1, end - 1));
                }
            }
        }

        int ParseFlagsString(std::string_view sv)
        {
            int flags = 0;
            while (!sv.empty())
            {
                auto token = ReadToken(sv);
                if (token.empty())
                    break;

                if (token == "spawnRelative") flags |= 0x2;
                else if (token == "spawnFrustumCull") flags |= 0x4;
                else if (token == "spawnOffsetNone") flags |= 0x0;
                else if (token == "spawnOffsetSphere") flags |= 0x10;
                else if (token == "spawnOffsetCylinder") flags |= 0x20;
                else if (token == "runRelToWorld") flags |= 0x0;
                else if (token == "runRelToSpawn") flags |= 0x40;
                else if (token == "runRelToEffect") flags |= 0x80;
                else if (token == "runRelToOffset") flags |= 0xC0;
                else if (token == "useCollision") flags |= 0x100;
                else if (token == "dieOnTouch") flags |= 0x200;
                else if (token == "drawPastFog") flags |= 0x400;
                else if (token == "drawWithViewModel") flags |= 0x800;
                else if (token == "blocksSight") flags |= 0x1000;
                else if (token == "useItemClip") flags |= 0x2000;
                else if (token == "useWorldUp") flags |= 0x80000;
                else if (token == "alignViewpoint") flags |= 0x100000;
                else if (token == "useBillboardPivot") flags |= 0x200000;
                else if (token == "useGaussianCloud") flags |= 0x400000;
                else if (token == "useRotationAxis") flags |= 0x800000;
                else if (token == "modelUsesPhysics") flags |= 0x8000000;
                else if (token == "nonUniformScale") flags |= 0x10000000;
                else if (token == "flamechunk") flags |= 0x20000000;
                else if (token == "hasReflection") flags |= 0x40000000;
                else if (token == "isMatureContent") flags |= 0x80000000;
            }
            return flags;
        }

        char ParseAtlasFlagsString(std::string_view sv)
        {
            char behavior = 0;
            while (!sv.empty())
            {
                auto token = ReadToken(sv);
                if (token.empty())
                    break;

                if (token == "startFixed") behavior |= 0x0;
                else if (token == "startRandom") behavior |= 0x1;
                else if (token == "startIndexed") behavior |= 0x2;
                else if (token == "startFixedRange") behavior |= 0x3;
                else if (token == "playOverLife") behavior |= 0x4;
                else if (token == "loopOnlyNTimes") behavior |= 0x8;
            }
            return behavior;
        }

        FxElemType GetElementType(const std::string& name)
        {
            if (name == "billboardSprite") return FX_ELEM_TYPE_SPRITE_BILLBOARD;
            if (name == "orientedSprite") return FX_ELEM_TYPE_SPRITE_ORIENTED;
            if (name == "rotatedSprite") return FX_ELEM_TYPE_SPRITE_ROTATED;
            if (name == "tail") return FX_ELEM_TYPE_TAIL;
            if (name == "line") return FX_ELEM_TYPE_LINE;
            if (name == "trail") return FX_ELEM_TYPE_TRAIL;
            if (name == "cloud") return FX_ELEM_TYPE_CLOUD;
            if (name == "model") return FX_ELEM_TYPE_MODEL;
            if (name == "light") return FX_ELEM_TYPE_OMNI_LIGHT;
            if (name == "spotLight") return FX_ELEM_TYPE_SPOT_LIGHT;
            if (name == "sound") return FX_ELEM_TYPE_SOUND;
            if (name == "decal") return FX_ELEM_TYPE_DECAL;
            if (name == "runner") return FX_ELEM_TYPE_RUNNER;
            return FX_ELEM_TYPE_SPRITE_BILLBOARD;
        }

        void SetSingleVisual(FxElemDef& elem, const ElementData& elemData)
        {
            const auto& visName = elemData.visuals[0];
            if (visName.empty())
                return;

            switch (elem.elemType)
            {
            case FX_ELEM_TYPE_SPRITE_BILLBOARD:
            case FX_ELEM_TYPE_SPRITE_ORIENTED:
            case FX_ELEM_TYPE_SPRITE_ROTATED:
            case FX_ELEM_TYPE_TRAIL:
            case FX_ELEM_TYPE_LINE:
            case FX_ELEM_TYPE_DECAL:
            {
                auto* mat = m_memory.Alloc<Material>();
                mat->info.name = m_memory.Dup(visName.c_str());
                elem.visuals.instance.material = mat;
                break;
            }
            case FX_ELEM_TYPE_TAIL:
            case FX_ELEM_TYPE_CLOUD:
            case FX_ELEM_TYPE_MODEL:
            case FX_ELEM_TYPE_RUNNER:
                elem.visuals.instance.effectDef.name = m_memory.Dup(visName.c_str());
                break;
            case FX_ELEM_TYPE_SOUND:
                elem.visuals.instance.soundName = m_memory.Dup(visName.c_str());
                break;
            default:
                break;
            }
        }

        void SetVisual(FxElemVisuals& visual, const ElementData& elemData, size_t index)
        {
            const auto& visName = elemData.visuals[index];
            if (visName.empty())
                return;

            switch (GetElementType(elemData.typeName))
            {
            case FX_ELEM_TYPE_SPRITE_BILLBOARD:
            case FX_ELEM_TYPE_SPRITE_ORIENTED:
            case FX_ELEM_TYPE_SPRITE_ROTATED:
            case FX_ELEM_TYPE_TRAIL:
            case FX_ELEM_TYPE_LINE:
            case FX_ELEM_TYPE_DECAL:
            {
                auto* mat = m_memory.Alloc<Material>();
                mat->info.name = m_memory.Dup(visName.c_str());
                visual.material = mat;
                break;
            }
            case FX_ELEM_TYPE_TAIL:
            case FX_ELEM_TYPE_CLOUD:
            case FX_ELEM_TYPE_MODEL:
            case FX_ELEM_TYPE_RUNNER:
                visual.effectDef.name = m_memory.Dup(visName.c_str());
                break;
            case FX_ELEM_TYPE_SOUND:
                visual.soundName = m_memory.Dup(visName.c_str());
                break;
            default:
                break;
            }
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace fx_effect
{
    std::unique_ptr<AssetCreator<T6::AssetFx>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        class Loader final : public AssetCreator<AssetFx>
        {
        public:
            Loader(MemoryManager& memory, ISearchPath& searchPath)
                : m_parser(memory, searchPath)
            {
            }

            AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
            {
                return m_parser.ParseAndCreate(assetName, context);
            }

        private:
            EfxParser m_parser;
        };

        return std::make_unique<Loader>(memory, searchPath);
    }
} // namespace fx_effect