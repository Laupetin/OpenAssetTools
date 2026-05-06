#pragma once

#include "Dumping/AbstractTextDumper.h"

#include <ostream>
#include <array>

namespace fx_elem_def
{
    struct FxIntRange
    {
        int base;
        int amplitude;

        FxIntRange(int base, int amplitude)
            : base(base),
              amplitude(amplitude)
        {
        }
    };

    struct FxFloatRange
    {
        float base;
        float amplitude;

        FxFloatRange(float base, float amplitude)
            : base(base),
              amplitude(amplitude)
        {
        }
    };

    struct FxSpawnDefOneShot
    {
        FxIntRange count;

        FxSpawnDefOneShot(FxIntRange count)
            : count(count)
        {
        }
    };

    struct FxSpawnDefLooping
    {
        int intervalMsec;
        int count;

        FxSpawnDefLooping(int intervalMsec, int count)
            : intervalMsec(intervalMsec),
              count(count)
        {
        }
    };

    union FxSpawnDef
    {
        FxSpawnDefLooping looping;
        FxSpawnDefOneShot oneShot;

        FxSpawnDef(FxSpawnDefLooping looping, FxSpawnDefOneShot oneShot)
        {
        }
    };

    struct FxElemAtlas
    {
        char behavior;
        char index;
        char fps;
        char loopCount;
        char colIndexBits;
        char rowIndexBits;
        int16_t entryCount;

        FxElemAtlas(char behavior, char index, char fps, char loopCount, char colIndexBits, char rowIndexBits, int16_t entryCount)
            : behavior(behavior),
              index(index),
              fps(fps),
              loopCount(loopCount),
              colIndexBits(colIndexBits),
              rowIndexBits(rowIndexBits),
              entryCount(entryCount)
        {
        }
    };

    struct FxElemVec3Range
    {
        std::array<float, 3> base;
        std::array<float, 3> amplitude;

        FxElemVec3Range(const std::array<float, 3>& base, const std::array<float, 3>& amplitude)
            : base(base),
              amplitude(amplitude)
        {
        }
    };

    struct FxElemVelStateInFrame
    {
        FxElemVec3Range velocity;
        FxElemVec3Range totalDelta;

        FxElemVelStateInFrame(FxElemVec3Range velocity, FxElemVec3Range totalDelta)
            : velocity(velocity),
              totalDelta(totalDelta)
        {
        }
    };

    struct FxElemVelStateSample
    {
        FxElemVelStateInFrame local;
        FxElemVelStateInFrame world;

        FxElemVelStateSample(FxElemVelStateInFrame local, FxElemVelStateInFrame world)
            : local(local),
              world(world)
        {
        }
    };

    struct FxElemVisualState
    {
        std::array<char, 4> color;
        float rotationDelta;
        float rotationTotal;
        std::array<float, 2> size;
        float scale;

        FxElemVisualState(const std::array<char, 4>& color, float rotationDelta, float rotationTotal, const std::array<float, 2>& size, float scale)
            : color(color),
              rotationDelta(rotationDelta),
              rotationTotal(rotationTotal),
              size(size),
              scale(scale)
        {
        }
    };

    struct FxElemVisStateSample
    {
        FxElemVisualState base;
        FxElemVisualState amplitude;

        FxElemVisStateSample(FxElemVisualState base, FxElemVisualState amplitude)
            : base(base),
              amplitude(amplitude)
        {
        }
    };
}; // fx_elem_def

class FxElemDefDumper : AbstractTextDumper
{
public:
    explicit FxElemDefDumper(std::ostream& stream);

    void Init() const;
    void WriteComment(const std::string& key) const;

    void BeginElement();
    void EndElement();

    void WriteKeyValue(const std::string& key, const std::string& value) const;
    void WriteFxSpawnDef(const fx_elem_def::FxSpawnDef& fxSpawnDef, unsigned int elemIndex, unsigned int elemDefCountLooping) const;
    void WriteFxFloatRange(const std::string& key, const fx_elem_def::FxFloatRange& fxFloatRange) const;
    void WriteFxIntRange(const std::string& key, const fx_elem_def::FxIntRange& fxIntRange) const;

    void WriteFloat(const std::string& key, const float& value) const;

    void WriteInt(const std::string& key, const int& value) const;

private:
    struct
    {
        bool m_in_element : 1;
    } m_flags;

    unsigned int m_element_index;
};
