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

    struct FxSpawnDef
    {
        int lowerBound;
        int upperBound;

        FxSpawnDef(int lowerBound, int upperBound)
            : lowerBound(lowerBound),
              upperBound(upperBound)
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

    union FxEffectDefRef
    {
        //FxEffectDef* handle;
        const char* name;
    };

    enum FxElemType : char
    {
        FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
        FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
        FX_ELEM_TYPE_TAIL = 0x2,
        FX_ELEM_TYPE_TRAIL = 0x3,
        FX_ELEM_TYPE_CLOUD = 0x4,
        FX_ELEM_TYPE_MODEL = 0x5,
        FX_ELEM_TYPE_OMNI_LIGHT = 0x6,
        FX_ELEM_TYPE_SPOT_LIGHT = 0x7,
        FX_ELEM_TYPE_SOUND = 0x8,
        FX_ELEM_TYPE_DECAL = 0x9,
        FX_ELEM_TYPE_RUNNER = 0xA,
        FX_ELEM_TYPE_COUNT = 0xB,
        FX_ELEM_TYPE_LAST_SPRITE = 0x3,
        FX_ELEM_TYPE_LAST_DRAWN = 0x7,
    };

    union FxElemVisuals
    {
        //const void* anonymous;
        //Material* material;
        //XModel* model;
        FxEffectDefRef effectDef;
        const char* soundName;
    };

    struct FxElemMarkVisuals
    {
        //Material* materials[2];
    };

    union FxElemDefVisuals
    {
        FxElemMarkVisuals* markArray;
        FxElemVisuals* array;
        FxElemVisuals instance;
    };

    struct FxTrailVertex
    {
        std::array<float, 2> pos;
        std::array<float, 2> normal;
        float texCoord;

        FxTrailVertex(const std::array<float, 2>& pos, const std::array<float, 2>& normal, float texCoord)
            : pos(pos),
              normal(normal),
              texCoord(texCoord)
        {
        }
    };

    struct FxTrailDef
    {
        int scrollTimeMsec;
        int repeatDist;
        int splitDist;
        int vertCount;
        FxTrailVertex* verts;
        int indCount;
        uint16_t* inds;

        FxTrailDef(int scrollTimeMsec, int repeatDist, int splitDist, int vertCount, FxTrailVertex* verts, int indCount, uint16_t* inds)
            : scrollTimeMsec(scrollTimeMsec),
              repeatDist(repeatDist),
              splitDist(splitDist),
              vertCount(vertCount),
              verts(verts),
              indCount(indCount),
              inds(inds)
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

    void BeginVisuals(const std::string& key);
    void EndVisuals();
    void WriteVisualEntry(const std::string& entry) const;

    void BeginEnum(const std::string& key);
    void EndEnum();
    void WriteEnumEntry(const std::string& entry) const;


    void WriteKeyValue(const std::string& key, const std::string& value) const;
    void WriteFloatArray(const std::string& key, const float* floatArray, unsigned int floatCount) const;
    void WriteEnum(const std::string& key, unsigned char val, const char* enumStrings[]) const;
    void WriteFxElemDefVisuals(const fx_elem_def::FxElemDefVisuals& fxElemDefVisuals,
                               unsigned char visualCount,
                               fx_elem_def::FxElemType elemType,
                               const char* elemTypeStrings[]);
    void WriteFxSpawnDef(const fx_elem_def::FxSpawnDef& fxSpawnDef) const;
    void WriteFxFloatRange(const std::string& key, const fx_elem_def::FxFloatRange& fxFloatRange) const;
    void WriteFxIntRange(const std::string& key, const fx_elem_def::FxIntRange& fxIntRange) const;

    void WriteFloat(const std::string& key, const float& value, int precision = -1) const;
    void WriteInt(const std::string& key, const int& value) const;
    void WriteBinary(const std::string& key, const int& value) const;

private:
    struct
    {
        bool m_in_element : 1;
        bool m_in_visuals : 1;
        bool m_in_enum: 1;
    } m_flags;

    unsigned int m_element_index;
};
