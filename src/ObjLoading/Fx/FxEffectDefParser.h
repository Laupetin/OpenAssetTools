#pragma once

#include "Fx/FxFlagDef.h"

#include <array>
#include <cstdint>
#include <istream>
#include <span>
#include <string>
#include <vector>

namespace fx
{
    template<typename T> struct FxEditorRange
    {
        T base{};
        T amplitude{};
    };

    struct FxEditorCurve
    {
        size_t dimensionCount{};
        std::vector<float> keys;

        [[nodiscard]] size_t KeyCount() const;
        [[nodiscard]] float Sample(float time, size_t dimension = 0u) const;
    };

    struct FxEditorGraph
    {
        float scale{};
        std::array<FxEditorCurve, 2> curves;
    };

    enum class FxEditorElemType
    {
        NONE,
        BILLBOARD_SPRITE,
        ORIENTED_SPRITE,
        ROTATED_SPRITE,
        TAIL,
        LINE,
        TRAIL,
        CLOUD,
        MODEL,
        LIGHT,
        SPOT_LIGHT,
        SOUND,
        DECAL,
        RUNNER,
    };

    struct FxEditorTrailVertex
    {
        std::array<float, 2> pos{};
        float texCoord{};
    };

    struct FxEditorElem
    {
        std::string name;
        unsigned editorFlags{};
        unsigned flags{};
        FxEditorRange<float> spawnRange;
        FxEditorRange<float> fadeInRange;
        FxEditorRange<float> fadeOutRange;
        float spawnFrustumCullRadius{};
        int spawnLoopingIntervalMsec{};
        int spawnLoopingCount{};
        FxEditorRange<int> spawnOneShotCount;
        FxEditorRange<int> spawnDelayMsec;
        FxEditorRange<int> lifeSpanMsec;
        std::array<FxEditorRange<float>, 3> spawnOrigin;
        FxEditorRange<float> spawnOffsetRadius;
        FxEditorRange<float> spawnOffsetHeight;
        std::array<FxEditorRange<float>, 3> spawnAngles;
        std::array<FxEditorRange<float>, 3> angularVelocity;
        FxEditorRange<float> initialRotation;
        std::array<float, 4> rotationAxis{0.0f, 0.0f, 0.0f, 1.0f};
        FxEditorRange<float> gravity;
        FxEditorRange<float> elasticity;
        unsigned atlasBehavior{};
        int atlasIndex{};
        int atlasFps{};
        int atlasLoopCount{};
        int atlasColIndexBits{};
        int atlasRowIndexBits{};
        int atlasEntryCount{};
        int atlasIndexRange{};
        float windInfluence{};
        std::array<std::array<FxEditorGraph, 3>, 2> velocityGraphs;
        FxEditorGraph rotationGraph;
        std::array<FxEditorGraph, 2> sizeGraphs;
        FxEditorGraph scaleGraph;
        FxEditorGraph colorGraph;
        FxEditorGraph alphaGraph;
        float lightingFrac{};
        std::array<float, 3> collOffset{};
        float collRadius{};
        std::string effectOnImpact;
        std::string effectOnDeath;
        int sortOrder{5};
        std::string emission;
        FxEditorRange<float> emitDist;
        FxEditorRange<float> emitDistVariance;
        std::string attachment;
        int trailSplitDist{};
        float trailScrollTime{};
        int trailRepeatDist{};
        std::vector<FxEditorTrailVertex> trailVertices;
        std::vector<std::uint16_t> trailIndices;
        int alphaFadeTimeMsec{};
        int maxWindMagnitude{};
        int maxWindLife{};
        int maxWindInterval{};
        float billboardTopWidth{1.0f};
        float billboardBottomWidth{1.0f};
        std::string spawnSound;
        FxEditorRange<int> cloudDensity{1024, 0};
        std::array<float, 2> billboardPivot{};
        FxEditorElemType type{FxEditorElemType::NONE};
        std::vector<std::string> visuals;
    };

    struct FxEditorEffect
    {
        int version{};
        unsigned flags{};
        int priority{};
        std::array<float, 3> boundingBoxDim{};
        std::array<float, 3> boundingBoxCentre{};
        std::vector<FxEditorElem> elements;
    };

    struct FxEffectDefParserConfig
    {
        int maxVersion{};
        bool supportsEffectFields{};
        bool supportsWind{};
        bool supportsLine{};
        bool supportsT5Fields{};
        unsigned backCompatVelocityFlag{};
        unsigned runMask{};
        std::span<const FxFlagDef> flagDefs;
    };

    bool ParseEffectDef(std::istream& stream, const std::string& fileName, const FxEffectDefParserConfig& config, FxEditorEffect& effect);
} // namespace fx
