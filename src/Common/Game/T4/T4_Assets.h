#pragma once

#ifndef __T4_ASSETS_H
#define __T4_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace T4
{
#endif
    enum XFileBlock
    {
        XFILE_BLOCK_TEMP,
        XFILE_BLOCK_RUNTIME,
        XFILE_BLOCK_LARGE_RUNTIME,
        XFILE_BLOCK_PHYSICAL_RUNTIME,
        XFILE_BLOCK_VIRTUAL,
        XFILE_BLOCK_LARGE,
        XFILE_BLOCK_PHYSICAL,

        MAX_XFILE_COUNT
    };

    struct XModelPieces;
    struct PhysPreset;
    struct PhysConstraints;
    struct DestructibleDef;
    struct XAnimParts;
    struct XModel;
    struct Material;
    struct MaterialPixelShader;
    struct MaterialVertexShader;
    struct MaterialTechniqueSet;
    struct GfxImage;
    struct snd_alias_list_t;
    struct LoadedSound;
    struct clipMap_t;
    struct ComWorld;
    struct GameWorldSp;
    struct GameWorldMp;
    struct MapEnts;
    struct GfxWorld;
    struct GfxLightDef;
    struct Font_s;
    struct MenuList;
    struct menuDef_t;
    struct LocalizeEntry;
    struct WeaponDef;
    struct SndDriverGlobals;
    struct FxEffectDef;
    struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    struct PackIndex;

    typedef unsigned short ScriptString;
    typedef tdef_align32(16) char raw_byte16;

    union XAssetHeader
    {
        // XModelPieces *xmodelPieces; // NOT AN ASSET
        PhysPreset* physPreset;
        PhysConstraints* physConstraints;
        DestructibleDef* destructibleDef;
        XAnimParts* parts;
        XModel* model;
        Material* material;
        MaterialPixelShader* pixelShader;
        MaterialVertexShader* vertexShader;
        MaterialTechniqueSet* techniqueSet;
        GfxImage* image;
        snd_alias_list_t* sound;
        LoadedSound* loadSnd;
        clipMap_t* clipMap;
        ComWorld* comWorld;
        GameWorldSp* gameWorldSp;
        GameWorldMp* gameWorldMp;
        MapEnts* mapEnts;
        GfxWorld* gfxWorld;
        GfxLightDef* lightDef;
        Font_s* font;
        MenuList* menuList;
        menuDef_t* menu;
        LocalizeEntry* localize;
        WeaponDef* weapon;
        SndDriverGlobals* sndDriverGlobals;
        FxEffectDef* fx;
        FxImpactTable* impactFx;
        RawFile* rawfile;
        StringTable* stringTable;
        PackIndex* packIndex;
        void* data;
    };

    typedef char cbrushedge_t;
    typedef tdef_align32(128) unsigned int raw_uint128;

    union vec2_t
    {
        float v[2];

        struct
        {
            float x;
            float y;
        };
    };

    union vec3_t
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        float v[3];
    };

    union vec4_t
    {
        float v[4];

        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
    };

    enum ConstraintType : int
    {
        CONSTRAINT_NONE = 0x0,
        CONSTRAINT_POINT = 0x1,
        CONSTRAINT_DISTANCE = 0x2,
        CONSTRAINT_HINGE = 0x3,
        CONSTRAINT_JOINT = 0x4,
        CONSTRAINT_ACTUATOR = 0x5,
        CONSTRAINT_FAKE_SHAKE = 0x6,
        CONSTRAINT_LAUNCH = 0x7,
        CONSTRAINT_ROPE = 0x8,
        NUM_CONSTRAINT_TYPES = 0x9,
    };

    enum AttachPointType : int
    {
        ATTACH_POINT_WORLD = 0x0,
        ATTACH_POINT_DYNENT = 0x1,
        ATTACH_POINT_ENT = 0x2,
        ATTACH_POINT_BONE = 0x3,
    };

    struct XModelPiece
    {
        XModel* model;
        float offset[3];
    };

    struct XModelPieces
    {
        const char* name;
        int numpieces;
        XModelPiece* pieces;
    };

    struct PhysPreset
    {
        const char* name;
        int type;
        float mass;
        float bounce;
        float friction;
        float bulletForceScale;
        float explosiveForceScale;
        const char* sndAliasPrefix;
        float piecesSpreadFraction;
        float piecesUpwardVelocity;
        int canFloat;
        float gravityScale;
    };

    struct PhysPresetInfo
    {
        float mass;
        float bounce;
        float friction;
        int isFrictionInfinity;
        float bulletForceScale;
        float explosiveForceScale;
        const char* sndAliasPrefix;
        float piecesSpreadFraction;
        float piecesUpwardVelocity;
        int canFloat;
        float gravityScale;
    };

    union XAnimIndices
    {
        unsigned char* _1;
        uint16_t* _2;
        void* data;
    };

    struct XAnimNotifyInfo
    {
        ScriptString name;
        float time;
    };

    typedef unsigned char ByteVec[3];
    typedef tdef_align32(4) unsigned short UShortVec[3];

    union XAnimDynamicIndicesTrans
    {
        unsigned char _1[1];
        uint16_t _2[1];
    };

    union XAnimDynamicFrames
    {
        ByteVec* _1;
        UShortVec* _2;
    };

    struct XAnimPartTransFrames
    {
        vec3_t mins;
        vec3_t size;
        XAnimDynamicFrames frames;
        XAnimDynamicIndicesTrans indices;
    };

    union XAnimPartTransData
    {
        XAnimPartTransFrames frames;
        vec3_t frame0;
    };

    struct XAnimPartTrans
    {
        uint16_t size;
        unsigned char smallTrans;
        XAnimPartTransData u;
    };

    struct type_align(4) XQuat
    {
        int16_t value[4];
    };

    struct type_align(4) XQuat2
    {
        int16_t value[2];
    };

    union XAnimDynamicIndicesQuat
    {
        unsigned char _1[1];
        uint16_t _2[1];
    };

    struct XAnimDeltaPartQuatDataFrames
    {
        XQuat2* frames;
        XAnimDynamicIndicesQuat indices;
    };

    union XAnimDeltaPartQuatData
    {
        XAnimDeltaPartQuatDataFrames frames;
        XQuat2 frame0;
    };

    struct XAnimDeltaPartQuat
    {
        uint16_t size;
        XAnimDeltaPartQuatData u;
    };

    struct XAnimDeltaPart
    {
        XAnimPartTrans* trans;
        XAnimDeltaPartQuat* quat;
    };

    enum XAnimPartType
    {
        PART_TYPE_NO_QUAT = 0x0,
        PART_TYPE_HALF_QUAT = 0x1,
        PART_TYPE_FULL_QUAT = 0x2,
        PART_TYPE_HALF_QUAT_NO_SIZE = 0x3,
        PART_TYPE_FULL_QUAT_NO_SIZE = 0x4,
        PART_TYPE_SMALL_TRANS = 0x5,
        PART_TYPE_TRANS = 0x6,
        PART_TYPE_TRANS_NO_SIZE = 0x7,
        PART_TYPE_NO_TRANS = 0x8,
        PART_TYPE_ALL = 0x9,

        PART_TYPE_COUNT
    };

    struct XAnimParts
    {
        const char* name;
        uint16_t dataByteCount;
        uint16_t dataShortCount;
        uint16_t dataIntCount;
        uint16_t randomDataByteCount;
        uint16_t randomDataIntCount;
        uint16_t numframes;
        bool bLoop;
        bool bDelta;
        unsigned char boneCount[PART_TYPE_COUNT];
        unsigned char notifyCount;
        unsigned char assetType;
        bool isDefault;
        unsigned int randomDataShortCount;
        unsigned int indexCount;
        float framerate;
        float frequency;
        ScriptString* names;
        unsigned char* dataByte;
        int16_t* dataShort;
        int* dataInt;
        int16_t* randomDataShort;
        unsigned char* randomDataByte;
        int* randomDataInt;
        XAnimIndices indices;
        XAnimNotifyInfo* notify;
        XAnimDeltaPart* deltaPart;
    };

    struct DObjSkelMat
    {
        float axis[3][4];
        float origin[4];
    };

    struct DObjAnimMat
    {
        vec4_t quat;
        vec3_t trans;
        float transWeight;
    };

    struct XSurfaceCollisionAabb
    {
        uint16_t mins[3];
        uint16_t maxs[3];
    };

    struct type_align(16) XSurfaceCollisionNode
    {
        XSurfaceCollisionAabb aabb;
        uint16_t childBeginIndex;
        uint16_t childCount;
    };

    struct XSurfaceCollisionLeaf
    {
        uint16_t triangleBeginIndex;
    };

    struct XSurfaceCollisionTree
    {
        float trans[3];
        float scale[3];
        unsigned int nodeCount;
        XSurfaceCollisionNode* nodes;
        unsigned int leafCount;
        XSurfaceCollisionLeaf* leafs;
    };

    struct XRigidVertList
    {
        uint16_t boneOffset;
        uint16_t vertCount;
        uint16_t triOffset;
        uint16_t triCount;
        XSurfaceCollisionTree* collisionTree;
    };

    union GfxColor
    {
        unsigned int packed;
        unsigned char array[4];
    };

    union PackedTexCoords
    {
        unsigned int packed;
    };

    union PackedUnitVec
    {
        unsigned int packed;
        unsigned char array[4];
    };

    struct type_align(16) GfxPackedVertex
    {
        vec3_t xyz;
        float binormalSign;
        GfxColor color;
        PackedTexCoords texCoord;
        PackedUnitVec normal;
        PackedUnitVec tangent;
    };

    struct XSurfaceVertexInfo
    {
        int16_t vertCount[4];
        uint16_t* vertsBlend;
    };

    struct XSurfaceTri
    {
        uint16_t i[3];
    };

    typedef tdef_align32(16) XSurfaceTri XSurfaceTri16;

    struct XSurface
    {
        char tileMode;
        bool deformed;
        uint16_t vertCount;
        uint16_t triCount;
        char zoneHandle;
        uint16_t baseTriIndex;
        uint16_t baseVertIndex;
        XSurfaceTri16* triIndices;
        XSurfaceVertexInfo vertInfo;
        GfxPackedVertex* verts0;
        void /*IDirect3DVertexBuffer9*/* vb0;
        unsigned int vertListCount;
        XRigidVertList* vertList;
        int partBits[4];
        void /*IDirect3DIndexBuffer9*/* indexBuffer;
    };

    struct XModelLodInfo
    {
        float dist;
        uint16_t numsurfs;
        uint16_t surfIndex;
        int partBits[4];
        char lod;
        char smcIndexPlusOne;
        char smcAllocBits;
        char unused;
    };

    struct XModelCollTri_s
    {
        float plane[4];
        float svec[4];
        float tvec[4];
    };

    struct XModelCollSurf_s
    {
        XModelCollTri_s* collTris;
        int numCollTris;
        float mins[3];
        float maxs[3];
        int boneIdx;
        int contents;
        int surfFlags;
    };

    struct XBoneInfo
    {
        vec3_t bounds[2];
        vec3_t offset;
        float radiusSquared;
    };

    struct cplane_s
    {
        float normal[3];
        float dist;
        char type;
        char signbits;
        char pad[2];
    };

    struct cbrushside_t
    {
        cplane_s* plane;
        unsigned int materialNum;
        int16_t firstAdjacentSideOffset;
        char edgeCount;
    };

    struct type_align32(16) BrushWrapper
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int16_t axialMaterialNum[2][3];
        cbrushedge_t* baseAdjacentSide;
        int16_t firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
        unsigned int numverts;
        vec3_t* verts;
        int totalEdgeCount;
        cplane_s* planes;
    };

    struct PhysGeomInfo
    {
        BrushWrapper* brush;
        int type;
        float orientation[3][3];
        float offset[3];
        float halfLengths[3];
    };

    struct PhysMass
    {
        float centerOfMass[3];
    };

    struct PhysGeomList
    {
        unsigned int count;
        PhysGeomInfo* geoms;
        PhysMass mass;
    };

    struct PhysConstraint
    {
        ScriptString targetname;
        ConstraintType type;
        AttachPointType attach_point_type1;
        int target_index1;
        ScriptString target_ent1;
        const char* target_bone1;
        AttachPointType attach_point_type2;
        int target_index2;
        ScriptString target_ent2;
        const char* target_bone2;
        float offset[3];
        float pos[3];
        float pos2[3];
        float dir[3];
        int flags;
        int timeout;
        int min_health;
        int max_health;
        float distance;
        float damp;
        float power;
        float scale[3];
        float spin_scale;
        float minAngle;
        float maxAngle;
        Material* material;
        int constraintHandle;
        int rope_index;
    };

    struct PhysConstraints
    {
        const char* name;
        unsigned int count;
        PhysConstraint data[16];
    };

    enum XModelLodRampType : unsigned char
    {
        XMODEL_LOD_RAMP_RIGID = 0x0,
        XMODEL_LOD_RAMP_SKINNED = 0x1,

        XMODEL_LOD_RAMP_COUNT
    };

    struct XModelHighMipBounds
    {
        float mins[3];
        float maxs[3];
    };

    struct XModelStreamInfo
    {
        char pad;
    };

    struct XModelQuat
    {
        int16_t v[4];
    };

    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        XModelLodRampType lodRampType;
        ScriptString* boneNames;
        unsigned char* parentList;
        XModelQuat* quats;
        float* trans;
        unsigned char* partClassification;
        DObjAnimMat* baseMat;
        XSurface* surfs;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        XModelCollSurf_s* collSurfs;
        int numCollSurfs;
        int contents;
        XBoneInfo* boneInfo;
        float radius;
        vec3_t mins;
        vec3_t maxs;
        uint16_t numLods;
        int16_t collLod;
        XModelStreamInfo streamInfo;
        int memUsage;
        unsigned char flags;
        bool bad;
        PhysPreset* physPreset;
        PhysGeomList* physGeoms;
        PhysGeomList* collmap;
        PhysConstraints* physConstraints;
    };

    enum GfxBlend
    {
        GFXS_BLEND_DISABLED = 0x0,
        GFXS_BLEND_ZERO = 0x1,
        GFXS_BLEND_ONE = 0x2,
        GFXS_BLEND_SRCCOLOR = 0x3,
        GFXS_BLEND_INVSRCCOLOR = 0x4,
        GFXS_BLEND_SRCALPHA = 0x5,
        GFXS_BLEND_INVSRCALPHA = 0x6,
        GFXS_BLEND_DESTALPHA = 0x7,
        GFXS_BLEND_INVDESTALPHA = 0x8,
        GFXS_BLEND_DESTCOLOR = 0x9,
        GFXS_BLEND_INVDESTCOLOR = 0xA,

        GFXS_BLEND_COUNT
    };

    enum GfxBlendOp
    {
        GFXS_BLENDOP_DISABLED = 0x0,
        GFXS_BLENDOP_ADD = 0x1,
        GFXS_BLENDOP_SUBTRACT = 0x2,
        GFXS_BLENDOP_REVSUBTRACT = 0x3,
        GFXS_BLENDOP_MIN = 0x4,
        GFXS_BLENDOP_MAX = 0x5,

        GFXS_BLENDOP_COUNT
    };

    enum GfxAlphaTest_e
    {
        GFXS_ALPHA_TEST_GT_0 = 1,
        GFXS_ALPHA_TEST_LT_128 = 2,
        GFXS_ALPHA_TEST_GE_128 = 3,

        GFXS_ALPHA_TEST_COUNT
    };

    enum GfxCullFace_e
    {
        GFXS_CULL_NONE = 1,
        GFXS_CULL_BACK = 2,
        GFXS_CULL_FRONT = 3,
    };

    enum GfxDepthTest_e
    {
        GFXS_DEPTHTEST_ALWAYS = 0,
        GFXS_DEPTHTEST_LESS = 1,
        GFXS_DEPTHTEST_EQUAL = 2,
        GFXS_DEPTHTEST_LESSEQUAL = 3
    };

    enum GfxPolygonOffset_e
    {
        GFXS_POLYGON_OFFSET_0 = 0,
        GFXS_POLYGON_OFFSET_1 = 1,
        GFXS_POLYGON_OFFSET_2 = 2,
        GFXS_POLYGON_OFFSET_SHADOWMAP = 3
    };

    enum GfxStencilOp
    {
        GFXS_STENCILOP_KEEP = 0x0,
        GFXS_STENCILOP_ZERO = 0x1,
        GFXS_STENCILOP_REPLACE = 0x2,
        GFXS_STENCILOP_INCRSAT = 0x3,
        GFXS_STENCILOP_DECRSAT = 0x4,
        GFXS_STENCILOP_INVERT = 0x5,
        GFXS_STENCILOP_INCR = 0x6,
        GFXS_STENCILOP_DECR = 0x7
    };

    enum GfxStencilFunc
    {
        GFXS_STENCILFUNC_NEVER = 0x0,
        GFXS_STENCILFUNC_LESS = 0x1,
        GFXS_STENCILFUNC_EQUAL = 0x2,
        GFXS_STENCILFUNC_LESSEQUAL = 0x3,
        GFXS_STENCILFUNC_GREATER = 0x4,
        GFXS_STENCILFUNC_NOTEQUAL = 0x5,
        GFXS_STENCILFUNC_GREATEREQUAL = 0x6,
        GFXS_STENCILFUNC_ALWAYS = 0x7
    };

    enum GfxStateBitsEnum : unsigned int
    {
        GFXS0_SRCBLEND_RGB_SHIFT = 0x0,
        GFXS0_SRCBLEND_RGB_MASK = 0xF,

        GFXS0_DSTBLEND_RGB_SHIFT = 0x4,
        GFXS0_DSTBLEND_RGB_MASK = 0xF0,

        GFXS0_BLENDOP_RGB_SHIFT = 0x8,
        GFXS0_BLENDOP_RGB_MASK = 0x700,
        GFXS0_BLEND_RGB_MASK = 0x7FF,

        GFXS0_ATEST_DISABLE = 0x800,
        GFXS0_ATEST_GT_0 = 0x1000,
        GFXS0_ATEST_LT_128 = 0x2000,
        GFXS0_ATEST_GE_128 = 0x3000,
        GFXS0_ATEST_MASK = 0x3000,

        GFXS0_CULL_NONE = 0x4000,
        GFXS0_CULL_BACK = 0x8000,
        GFXS0_CULL_FRONT = 0xC000,
        GFXS0_CULL_SHIFT = 0xE,
        GFXS0_CULL_MASK = 0xC000,

        GFXS0_SRCBLEND_ALPHA_SHIFT = 0x10,
        GFXS0_SRCBLEND_ALPHA_MASK = 0xF0000,

        GFXS0_DSTBLEND_ALPHA_SHIFT = 0x14,
        GFXS0_DSTBLEND_ALPHA_MASK = 0xF00000,

        GFXS0_BLENDOP_ALPHA_SHIFT = 0x18,
        GFXS0_BLENDOP_ALPHA_MASK = 0x7000000,
        GFXS0_BLEND_ALPHA_MASK = 0x7FF0000,

        GFXS0_COLORWRITE_RGB = 0x8000000,
        GFXS0_COLORWRITE_ALPHA = 0x10000000,
        GFXS0_COLORWRITE_MASK = 0x18000000,

        GFXS0_POLYMODE_LINE = 0x80000000,

        GFXS1_DEPTHWRITE = 0x1,

        GFXS1_DEPTHTEST_DISABLE = 0x2,
        GFXS1_DEPTHTEST_ALWAYS = 0x0,
        GFXS1_DEPTHTEST_LESS = 0x4,
        GFXS1_DEPTHTEST_EQUAL = 0x8,
        GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
        GFXS1_DEPTHTEST_SHIFT = 0x2,
        GFXS1_DEPTHTEST_MASK = 0xC,

        GFXS1_POLYGON_OFFSET_0 = 0x0,
        GFXS1_POLYGON_OFFSET_1 = 0x10,
        GFXS1_POLYGON_OFFSET_2 = 0x20,
        GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
        GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
        GFXS1_POLYGON_OFFSET_MASK = 0x30,

        GFXS1_STENCIL_FRONT_ENABLE = 0x40,
        GFXS1_STENCIL_BACK_ENABLE = 0x80,
        GFXS1_STENCIL_MASK = 0xC0,

        GFXS1_STENCIL_FRONT_PASS_SHIFT = 0x8,
        GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
        GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
        GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
        GFXS1_STENCIL_FRONT_MASK = 0xFFF00,

        GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
        GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
        GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
        GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
        GFXS1_STENCIL_BACK_MASK = 0xFFF00000,

        GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
        GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
    };

    struct GfxStateBitsLoadBitsStructured
    {
        // Byte 0
        unsigned int srcBlendRgb : 4;       // 0-3
        unsigned int dstBlendRgb : 4;       // 4-7
        unsigned int blendOpRgb : 3;        // 8-10
        unsigned int alphaTestDisabled : 1; // 11
        unsigned int alphaTest : 2;         // 12-13
        unsigned int cullFace : 2;          // 14-15
        unsigned int srcBlendAlpha : 4;     // 16-19
        unsigned int dstBlendAlpha : 4;     // 20-23
        unsigned int blendOpAlpha : 3;      // 24-26
        unsigned int colorWriteRgb : 1;     // 27
        unsigned int colorWriteAlpha : 1;   // 28
        unsigned int unused1 : 2;           // 29-30
        unsigned int polymodeLine : 1;      // 31

        // Byte 1
        unsigned int depthWrite : 1;          // 0
        unsigned int depthTestDisabled : 1;   // 1
        unsigned int depthTest : 2;           // 2-3
        unsigned int polygonOffset : 2;       // 4-5
        unsigned int stencilFrontEnabled : 1; // 6
        unsigned int stencilBackEnabled : 1;  // 7
        unsigned int stencilFrontPass : 3;    // 8-10
        unsigned int stencilFrontFail : 3;    // 11-13
        unsigned int stencilFrontZFail : 3;   // 14-16
        unsigned int stencilFrontFunc : 3;    // 17-19
        unsigned int stencilBackPass : 3;     // 20-22
        unsigned int stencilBackFail : 3;     // 23-25
        unsigned int stencilBackZFail : 3;    // 26-28
        unsigned int stencilBackFunc : 3;     // 29-31
    };

    union GfxStateBitsLoadBits
    {
        unsigned int raw[2];
        GfxStateBitsLoadBitsStructured structured;
    };

#ifndef __zonecodegenerator
    static_assert(sizeof(GfxStateBitsLoadBits) == 8);
    static_assert(sizeof(GfxStateBitsLoadBitsStructured) == 8);
#endif

    struct GfxStateBits
    {
        GfxStateBitsLoadBits loadBits;
    };

    struct type_align(16) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        vec4_t literal;
    };

    struct complex_s
    {
        float real;
        float imag;
    };

    struct WaterWritable
    {
        float floatTime;
    };

    struct water_t
    {
        WaterWritable writable;
        complex_s* H0;
        float* wTerm;
        int M;
        int N;
        float Lx;
        float Lz;
        float gravity;
        float windvel;
        float winddir[2];
        float amplitude;
        float codeConstant[4];
        GfxImage* image;
    };

    union MaterialTextureDefInfo
    {
        GfxImage* image;
        water_t* water;
    };

    enum TextureFilter
    {
        TEXTURE_FILTER_DISABLED = 0x0,
        TEXTURE_FILTER_NEAREST = 0x1,
        TEXTURE_FILTER_LINEAR = 0x2,
        TEXTURE_FILTER_ANISO2X = 0x3,
        TEXTURE_FILTER_ANISO4X = 0x4,

        TEXTURE_FILTER_COUNT
    };

    enum SamplerStateBitsMipMap_e
    {
        SAMPLER_MIPMAP_ENUM_DISABLED,
        SAMPLER_MIPMAP_ENUM_NEAREST,
        SAMPLER_MIPMAP_ENUM_LINEAR,

        SAMPLER_MIPMAP_ENUM_COUNT
    };

    enum SamplerStateBits_e
    {
        SAMPLER_FILTER_SHIFT = 0x0,
        SAMPLER_FILTER_NEAREST = 0x1,
        SAMPLER_FILTER_LINEAR = 0x2,
        SAMPLER_FILTER_ANISO2X = 0x3,
        SAMPLER_FILTER_ANISO4X = 0x4,
        SAMPLER_FILTER_MASK = 0x7,

        SAMPLER_MIPMAP_SHIFT = 0x3,
        SAMPLER_MIPMAP_DISABLED = 0x0,
        SAMPLER_MIPMAP_NEAREST = 0x8,
        SAMPLER_MIPMAP_LINEAR = 0x10,
        SAMPLER_MIPMAP_COUNT = 0x3,
        SAMPLER_MIPMAP_MASK = 0x18,

        SAMPLER_CLAMP_U_SHIFT = 0x5,
        SAMPLER_CLAMP_V_SHIFT = 0x6,
        SAMPLER_CLAMP_W_SHIFT = 0x7,
        SAMPLER_CLAMP_U = 0x20,
        SAMPLER_CLAMP_V = 0x40,
        SAMPLER_CLAMP_W = 0x80,
        SAMPLER_CLAMP_MASK = 0xE0,
    };

    struct MaterialTextureDefSamplerState
    {
        unsigned char filter : 3;
        unsigned char mipMap : 2;
        unsigned char clampU : 1;
        unsigned char clampV : 1;
        unsigned char clampW : 1;
    };

#ifndef __zonecodegenerator
    static_assert(sizeof(MaterialTextureDefSamplerState) == 1u);
#endif

    struct MaterialTextureDef
    {
        unsigned int nameHash;
        char nameStart;
        char nameEnd;
        MaterialTextureDefSamplerState samplerState; // SamplerStateBits_e
        unsigned char semantic;                      // TextureSemantic
        char isMatureContent;
        MaterialTextureDefInfo u;
    };

    struct gcc_align32(8) GfxDrawSurfFields
    {
        uint64_t objectId : 16;
        uint64_t reflectionProbeIndex : 8;
        uint64_t customIndex : 5;
        uint64_t materialSortedIndex : 11;
        uint64_t prepass : 2;
        uint64_t primaryLightIndex : 8;
        uint64_t surfType : 4;
        uint64_t primarySortKey : 6;
        uint64_t unused : 4;
    };

    union GfxDrawSurf
    {
        gcc_align32(8) GfxDrawSurfFields fields;
        gcc_align32(8) uint64_t packed;
    };

    enum materialSurfType_t
    {
        SURF_TYPE_DEFAULT,
        SURF_TYPE_BARK,
        SURF_TYPE_BRICK,
        SURF_TYPE_CARPET,
        SURF_TYPE_CLOTH,
        SURF_TYPE_CONCRETE,
        SURF_TYPE_DIRT,
        SURF_TYPE_FLESH,
        SURF_TYPE_FOLIAGE,
        SURF_TYPE_GLASS,
        SURF_TYPE_GRASS,
        SURF_TYPE_GRAVEL,
        SURF_TYPE_ICE,
        SURF_TYPE_METAL,
        SURF_TYPE_MUD,
        SURF_TYPE_PAPER,
        SURF_TYPE_PLASTER,
        SURF_TYPE_ROCK,
        SURF_TYPE_SAND,
        SURF_TYPE_SNOW,
        SURF_TYPE_WATER,
        SURF_TYPE_WOOD,
        SURF_TYPE_ASPHALT,
        SURF_TYPE_CERAMIC,
        SURF_TYPE_PLASTIC,
        SURF_TYPE_RUBBER,
        SURF_TYPE_CUSHION,
        SURF_TYPE_FRUIT,
        SURF_TYPE_PAINTED_METAL,
        SURF_TYPE_PLAYER,
        SURF_TYPE_TALL_GRASS,

        SURF_TYPE_NUM
    };

    // Unknown if this is the right amount of flags
    enum MaterialGameFlags
    {
        MTL_GAMEFLAG_1 = 0x1,
        MTL_GAMEFLAG_2 = 0x2,
        MTL_GAMEFLAG_4 = 0x4,
        MTL_GAMEFLAG_8 = 0x8,
        MTL_GAMEFLAG_10 = 0x10,
        MTL_GAMEFLAG_20 = 0x20,
        MTL_GAMEFLAG_CASTS_SHADOW = 0x40,
        MTL_GAMEFLAG_80 = 0x80,
        MTL_GAMEFLAG_100 = 0x100,
        MTL_GAMEFLAG_200 = 0x200,
        MTL_GAMEFLAG_400 = 0x400,
        MTL_GAMEFLAG_800 = 0x800,
        MTL_GAMEFLAG_1000 = 0x1000,
    };

    struct MaterialInfo
    {
        const char* name;
        unsigned char gameFlags;
        unsigned char sortKey;
        unsigned char textureAtlasRowCount;
        unsigned char textureAtlasColumnCount;
        GfxDrawSurf drawSurf;
        unsigned int surfaceTypeBits;
        uint16_t hashIndex;
    };

    enum GfxCameraRegionType
    {
        CAMERA_REGION_LIT = 0x0,
        CAMERA_REGION_DECAL = 0x1,
        CAMERA_REGION_EMISSIVE = 0x2,

        CAMERA_REGION_COUNT,
        CAMERA_REGION_NONE = CAMERA_REGION_COUNT,
    };

    enum MaterialStateFlags
    {
        STATE_FLAG_CULL_BACK = 0x1,
        STATE_FLAG_CULL_FRONT = 0x2,
        STATE_FLAG_DECAL = 0x4,
        STATE_FLAG_WRITES_DEPTH = 0x8,
        STATE_FLAG_USES_DEPTH_BUFFER = 0x10,
        STATE_FLAG_USES_STENCIL_BUFFER = 0x20,
        STATE_FLAG_CULL_BACK_SHADOW = 0x40,
        STATE_FLAG_CULL_FRONT_SHADOW = 0x80,
    };

    struct Material
    {
        MaterialInfo info;
        char stateBitsEntry[67];
        unsigned char textureCount;
        unsigned char constantCount;
        unsigned char stateBitsCount;
        unsigned char stateFlags;
        unsigned char cameraRegion;
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
    };

    struct MaterialArgumentCodeConst
    {
        uint16_t index;
        unsigned char firstRow;
        unsigned char rowCount;
    };

    union MaterialArgumentDef
    {
        const float (*literalConst)[4];
        MaterialArgumentCodeConst codeConst;
        unsigned int codeSampler;
        unsigned int nameHash;
    };

    enum MaterialShaderArgumentType
    {
        MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
        MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
        MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x2,

        MTL_ARG_CODE_PRIM_BEGIN = 0x3,

        MTL_ARG_CODE_VERTEX_CONST = 0x3,
        MTL_ARG_CODE_PIXEL_SAMPLER = 0x4,
        MTL_ARG_CODE_PIXEL_CONST = 0x5,

        MTL_ARG_CODE_PRIM_END = 0x6,

        MTL_ARG_MATERIAL_PIXEL_CONST = 0x6,
        MTL_ARG_LITERAL_PIXEL_CONST = 0x7,

        MTL_ARG_COUNT
    };

    enum MaterialType
    {
        MTL_TYPE_DEFAULT = 0x0,
        MTL_TYPE_MODEL = 0x1,         // m_
        MTL_TYPE_MODEL_VERTCOL = 0x2, // mc_
        MTL_TYPE_WORLD = 0x3,         // w_
        MTL_TYPE_WORLD_VERTCOL = 0x4, // wc_

        MTL_TYPE_COUNT,
    };

    struct MaterialTypeInfo
    {
        const char* materialPrefix;
        const char* techniqueSetPrefix;
    };

    enum ShaderCodeConstants
    {
        CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,

        CONST_SRC_CODE_LIGHT_POSITION = 0x0,
        CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
        CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
        CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
        CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
        CONST_SRC_CODE_NEARPLANE_ORG = 0x5,
        CONST_SRC_CODE_NEARPLANE_DX = 0x6,
        CONST_SRC_CODE_NEARPLANE_DY = 0x7,
        CONST_SRC_CODE_SHADOW_PARMS = 0x8,
        CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x9,
        CONST_SRC_CODE_RENDER_TARGET_SIZE = 0xA,
        CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0xB,
        CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0xC,
        CONST_SRC_CODE_DOF_EQUATION_SCENE = 0xD,
        CONST_SRC_CODE_DOF_LERP_SCALE = 0xE,
        CONST_SRC_CODE_DOF_LERP_BIAS = 0xF,
        CONST_SRC_CODE_DOF_ROW_DELTA = 0x10,
        CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x11,
        CONST_SRC_CODE_GAMETIME = 0x12,

        CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x13,
        CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x13,

        CONST_SRC_CODE_PIXEL_COST_FRACS = 0x13,
        CONST_SRC_CODE_PIXEL_COST_DECODE = 0x14,
        CONST_SRC_CODE_FILTER_TAP_0 = 0x15,
        CONST_SRC_CODE_FILTER_TAP_1 = 0x16,
        CONST_SRC_CODE_FILTER_TAP_2 = 0x17,
        CONST_SRC_CODE_FILTER_TAP_3 = 0x18,
        CONST_SRC_CODE_FILTER_TAP_4 = 0x19,
        CONST_SRC_CODE_FILTER_TAP_5 = 0x1A,
        CONST_SRC_CODE_FILTER_TAP_6 = 0x1B,
        CONST_SRC_CODE_FILTER_TAP_7 = 0x1C,
        CONST_SRC_CODE_COLOR_MATRIX_R = 0x1D,
        CONST_SRC_CODE_COLOR_MATRIX_G = 0x1E,
        CONST_SRC_CODE_COLOR_MATRIX_B = 0x1F,

        CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x20,
        CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x20,

        CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x20,
        CONST_SRC_CODE_SHADOWMAP_SCALE = 0x21,
        CONST_SRC_CODE_ZNEAR = 0x22,
        CONST_SRC_CODE_SUN_POSITION = 0x23,
        CONST_SRC_CODE_SUN_DIFFUSE = 0x24,
        CONST_SRC_CODE_SUN_SPECULAR = 0x25,
        CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x26,
        CONST_SRC_CODE_DEBUG_BUMPMAP = 0x27,
        CONST_SRC_CODE_MATERIAL_COLOR = 0x28,
        CONST_SRC_CODE_FOG = 0x29,
        CONST_SRC_CODE_FOG_COLOR = 0x2A,
        CONST_SRC_CODE_GLOW_SETUP = 0x2B,
        CONST_SRC_CODE_GLOW_APPLY = 0x2C,
        CONST_SRC_CODE_COLOR_BIAS = 0x2D,
        CONST_SRC_CODE_COLOR_TINT_BASE = 0x2E,
        CONST_SRC_CODE_COLOR_TINT_DELTA = 0x2F,
        CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x30,
        CONST_SRC_CODE_ENVMAP_PARMS = 0x31,
        CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x32,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x33,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x34,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x35,
        CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x36,
        CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x37,
        CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x38,

        CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x38,

        CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x39,

        CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0x3A,
        CONST_SRC_CODE_COUNT_FLOAT4 = 0x3A,
        CONST_SRC_FIRST_CODE_MATRIX = 0x3A,

        CONST_SRC_CODE_WORLD_MATRIX = 0x3A,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0x3B,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0x3C,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0x3D,
        CONST_SRC_CODE_VIEW_MATRIX = 0x3E,
        CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x3F,
        CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x40,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x41,
        CONST_SRC_CODE_PROJECTION_MATRIX = 0x42,
        CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x43,
        CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x44,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x45,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0x46,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0x47,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x48,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x49,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x4A,
        CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x4B,
        CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4C,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4D,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0x4E,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x4F,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x50,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x51,
        CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x52,
        CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x53,
        CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x54,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x55,
        CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x56,
        CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x57,
        CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x58,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x59,

        CONST_SRC_TOTAL_COUNT,
        CONST_SRC_NONE
    };

    enum MaterialTextureSource
    {
        TEXTURE_SRC_CODE_BLACK = 0x0,
        TEXTURE_SRC_CODE_WHITE = 0x1,
        TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
        TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
        TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
        TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
        TEXTURE_SRC_CODE_SHADOWCOOKIE = 0x6,
        TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x7,
        TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x8,
        TEXTURE_SRC_CODE_FEEDBACK = 0x9,
        TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0xA,
        TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xB,
        TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xC,
        TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xD,
        TEXTURE_SRC_CODE_SKY = 0xE,
        TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0xF,
        TEXTURE_SRC_CODE_DYNAMIC_SHADOWS = 0x10,
        TEXTURE_SRC_CODE_OUTDOOR = 0x11,
        TEXTURE_SRC_CODE_FLOATZ = 0x12,
        TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x13,
        TEXTURE_SRC_CODE_RAW_FLOATZ = 0x14,
        TEXTURE_SRC_CODE_CASE_TEXTURE = 0x15,
        TEXTURE_SRC_CODE_CINEMATIC_Y = 0x16,
        TEXTURE_SRC_CODE_CINEMATIC_CR = 0x17,
        TEXTURE_SRC_CODE_CINEMATIC_CB = 0x18,
        TEXTURE_SRC_CODE_CINEMATIC_A = 0x19,
        TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1A,

        TEXTURE_SRC_CODE_COUNT
    };

    enum CustomSamplers
    {
        CUSTOM_SAMPLER_REFLECTION_PROBE = 0x0,
        CUSTOM_SAMPLER_LIGHTMAP_PRIMARY = 0x1,
        CUSTOM_SAMPLER_LIGHTMAP_SECONDARY = 0x2,

        CUSTOM_SAMPLER_COUNT
    };

    enum MaterialUpdateFrequency
    {
        MTL_UPDATE_PER_PRIM = 0x0,
        MTL_UPDATE_PER_OBJECT = 0x1,
        MTL_UPDATE_RARELY = 0x2,
        MTL_UPDATE_CUSTOM = 0x3,
    };

    struct MaterialShaderArgument
    {
        uint16_t type;
        uint16_t dest;
        MaterialArgumentDef u;
    };

    enum MaterialStreamStreamSource_e
    {
        STREAM_SRC_POSITION = 0x0,
        STREAM_SRC_COLOR = 0x1,
        STREAM_SRC_TEXCOORD_0 = 0x2,
        STREAM_SRC_NORMAL = 0x3,

        STREAM_SRC_PRE_OPTIONAL_BEGIN = 0x4,

        STREAM_SRC_TANGENT = 0x4,

        STREAM_SRC_OPTIONAL_BEGIN = 0x5,

        STREAM_SRC_TEXCOORD_1 = 0x5,
        STREAM_SRC_TEXCOORD_2 = 0x6,
        STREAM_SRC_NORMAL_TRANSFORM_0 = 0x7,
        STREAM_SRC_NORMAL_TRANSFORM_1 = 0x8,

        STREAM_SRC_COUNT
    };

    enum MaterialStreamDestination_e
    {
        STREAM_DST_POSITION = 0x0,
        STREAM_DST_NORMAL = 0x1,
        STREAM_DST_COLOR_0 = 0x2,
        STREAM_DST_COLOR_1 = 0x3,
        STREAM_DST_TEXCOORD_0 = 0x4,
        STREAM_DST_TEXCOORD_1 = 0x5,
        STREAM_DST_TEXCOORD_2 = 0x6,
        STREAM_DST_TEXCOORD_3 = 0x7,
        STREAM_DST_TEXCOORD_4 = 0x8,
        STREAM_DST_TEXCOORD_5 = 0x9,
        STREAM_DST_TEXCOORD_6 = 0xA,
        STREAM_DST_TEXCOORD_7 = 0xB,
        STREAM_DST_COUNT
    };

    struct MaterialStreamRouting
    {
        unsigned char source;
        unsigned char dest;
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[16];
        void /*IDirect3DVertexDeclaration9*/* decl[17];
    };

    struct MaterialVertexDeclaration
    {
        unsigned char streamCount;
        bool hasOptionalSource;
        bool isLoaded;
        MaterialVertexStreamRouting routing;
    };

    struct GfxVertexShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    struct MaterialVertexShaderProgram
    {
        void /*IDirect3DVertexShader9*/* vs;
        GfxVertexShaderLoadDef loadDef;
    };

    struct MaterialVertexShader
    {
        const char* name;
        MaterialVertexShaderProgram prog;
    };

    struct GfxPixelShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    struct MaterialPixelShaderProgram
    {
        void /*IDirect3DPixelShader9*/* ps;
        GfxPixelShaderLoadDef loadDef;
    };

    struct MaterialPixelShader
    {
        const char* name;
        MaterialPixelShaderProgram prog;
    };

    struct MaterialPass
    {
        MaterialVertexDeclaration* vertexDecl;
        MaterialVertexShader* vertexShader;
        MaterialPixelShader* pixelShader;
        unsigned char perPrimArgCount;
        unsigned char perObjArgCount;
        unsigned char stableArgCount;
        unsigned char customSamplerFlags;
        MaterialShaderArgument* args;
    };

    enum TechniqueFlags
    {
        MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN = 0x1,
        MTL_TECHFLAG_NEEDS_RESOLVED_SCENE = 0x2,
        MTL_TECHFLAG_ZPREPASS = 0x4,

        MTL_TECHFLAG_DECL_HAS_OPTIONAL_SOURCE = 0x8,

        MTL_TECHFLAG_USES_LIGHT_SPOT_FACTORS = 0x10,
        MTL_TECHFLAG_USES_FLOATZ = 0x20,
    };

    struct MaterialTechnique
    {
        const char* name;
        uint16_t flags;
        uint16_t passCount;
        MaterialPass passArray[1];
    };

    enum MaterialTechniqueType
    {
        TECHNIQUE_DEPTH_PREPASS = 0x0,
        TECHNIQUE_BUILD_FLOAT_Z = 0x1,
        TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
        TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
        TECHNIQUE_UNLIT = 0x4,
        TECHNIQUE_EMISSIVE = 0x5,
        TECHNIQUE_EMISSIVE_SHADOW = 0x6,
        TECHNIQUE_EMISSIVE_REFLECTED = 0x7,

        TECHNIQUE_LIT_BEGIN = 0x8,

        TECHNIQUE_LIT = 0x8,
        TECHNIQUE_LIT_FADE = 0x9,
        TECHNIQUE_LIT_SUN = 0xA,
        TECHNIQUE_LIT_SUN_FADE = 0xB,
        TECHNIQUE_LIT_SUN_SHADOW = 0xC,
        TECHNIQUE_LIT_SUN_SHADOW_FADE = 0xD,
        TECHNIQUE_LIT_SPOT = 0xE,
        TECHNIQUE_LIT_SPOT_FADE = 0xF,
        TECHNIQUE_LIT_SPOT_SHADOW = 0x10,
        TECHNIQUE_LIT_SPOT_SHADOW_FADE = 0x11,
        TECHNIQUE_LIT_OMNI = 0x12,
        TECHNIQUE_LIT_OMNI_FADE = 0x13,
        TECHNIQUE_LIT_OMNI_SHADOW = 0x14,
        TECHNIQUE_LIT_OMNI_SHADOW_FADE = 0x15,
        TECHNIQUE_LIT_CHARRED = 0x16,
        TECHNIQUE_LIT_FADE_CHARRED = 0x17,
        TECHNIQUE_LIT_SUN_CHARRED = 0x18,
        TECHNIQUE_LIT_SUN_FADE_CHARRED = 0x19,
        TECHNIQUE_LIT_SUN_SHADOW_CHARRED = 0x1A,
        TECHNIQUE_LIT_SUN_SHADOW_FADE_CHARRED = 0x1B,
        TECHNIQUE_LIT_SPOT_CHARRED = 0x1C,
        TECHNIQUE_LIT_SPOT_FADE_CHARRED = 0x1D,
        TECHNIQUE_LIT_SPOT_SHADOW_CHARRED = 0x1E,
        TECHNIQUE_LIT_SPOT_SHADOW_FADE_CHARRED = 0x1F,
        TECHNIQUE_LIT_OMNI_CHARRED = 0x20,
        TECHNIQUE_LIT_OMNI_FADE_CHARRED = 0x21,
        TECHNIQUE_LIT_OMNI_SHADOW_CHARRED = 0x22,
        TECHNIQUE_LIT_OMNI_SHADOW_FADE_CHARRED = 0x23,

        TECHNIQUE_LIT_END = 0x24,

        TECHNIQUE_LIGHT_SPOT = 0x24,
        TECHNIQUE_LIGHT_OMNI = 0x25,
        TECHNIQUE_LIGHT_SPOT_SHADOW = 0x26,
        TECHNIQUE_LIGHT_SPOT_CHARRED = 0x27,
        TECHNIQUE_LIGHT_OMNI_CHARRED = 0x28,
        TECHNIQUE_LIGHT_SPOT_SHADOW_CHARRED = 0x29,
        TECHNIQUE_FAKELIGHT_NORMAL = 0x2A,
        TECHNIQUE_FAKELIGHT_VIEW = 0x2B,
        TECHNIQUE_SUNLIGHT_PREVIEW = 0x2C,
        TECHNIQUE_CASE_TEXTURE = 0x2D,
        TECHNIQUE_WIREFRAME_SOLID = 0x2E,
        TECHNIQUE_WIREFRAME_SHADED = 0x2F,
        TECHNIQUE_SHADOWCOOKIE_CASTER = 0x30,
        TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x31,
        TECHNIQUE_DEBUG_BUMPMAP = 0x32,
        TECHNIQUE_COUNT = 0x33,
        TECHNIQUE_TOTAL_COUNT = 0x34,
        TECHNIQUE_NONE = 0x35,
    };

    enum MaterialWorldVertexFormat : unsigned char
    {
        MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
        MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
        MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
        MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
        MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
        MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
        MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
        MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
        MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
        MTL_WORLDVERT_TEX_5_NRM_1 = 0x9,
        MTL_WORLDVERT_TEX_5_NRM_2 = 0xA,
        MTL_WORLDVERT_TEX_5_NRM_3 = 0xB,
    };

    struct MaterialTechniqueSet
    {
        const char* name;
        MaterialWorldVertexFormat worldVertFormat;
        bool hasBeenUploaded;
        char unused[1];
        MaterialTechniqueSet* remappedTechniqueSet;
        MaterialTechnique* techniques[59];
    };

    struct CardMemory
    {
        int platform[2];
    };

    struct Picmip
    {
        char platform[2];
    };

    struct GfxImageLoadDef
    {
        char levelCount;
        char flags;
        uint16_t dimensions[3];
        int format;
        unsigned int resourceSize;
        char data[1];
    };

    union GfxTexture
    {
        // void/*IDirect3DBaseTexture9*/* basemap;
        // void/*IDirect3DTexture9*/* map;
        // void/*IDirect3DVolumeTexture9*/* volmap;
        // void/*IDirect3DCubeTexture9*/* cubemap;
        GfxImageLoadDef* loadDef;
    };

    enum MapType
    {
        MAPTYPE_NONE = 0x0,
        MAPTYPE_INVALID1 = 0x1,
        MAPTYPE_INVALID2 = 0x2,
        MAPTYPE_2D = 0x3,
        MAPTYPE_3D = 0x4,
        MAPTYPE_CUBE = 0x5,

        MAPTYPE_COUNT
    };

    enum TextureSemantic
    {
        TS_2D = 0x0,
        TS_FUNCTION = 0x1,
        TS_COLOR_MAP = 0x2,
        TS_UNUSED_1 = 0x3,
        TS_UNUSED_2 = 0x4,
        TS_NORMAL_MAP = 0x5,
        TS_UNUSED_3 = 0x6,
        TS_UNUSED_4 = 0x7,
        TS_SPECULAR_MAP = 0x8,
        TS_UNUSED_5 = 0x9,
        TS_UNUSED_6 = 0xA,
        TS_WATER_MAP = 0xB,
    };

    enum ImageCategory
    {
        IMG_CATEGORY_UNKNOWN = 0x0,
        IMG_CATEGORY_AUTO_GENERATED = 0x1,
        IMG_CATEGORY_LIGHTMAP = 0x2,
        IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
        IMG_CATEGORY_RAW = 0x4,
        IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
        IMG_CATEGORY_WATER = 0x5,
        IMG_CATEGORY_RENDERTARGET = 0x6,
        IMG_CATEGORY_TEMP = 0x7,
    };

    struct GfxImage
    {
        MapType mapType;
        GfxTexture texture;
        Picmip picmip;
        bool noPicmip;
        char semantic;
        char track;
        CardMemory cardMemory;
        uint16_t width;
        uint16_t height;
        uint16_t depth;
        char category;
        bool delayLoadPixels;
        const char* name;
    };

    struct MSSSpeakerLevels
    {
        int speaker;
        int numLevels;
        float levels[2];
    };

    struct MSSChannelMap
    {
        int speakerCount;
        MSSSpeakerLevels speakers[6];
    };

    struct SpeakerMap
    {
        bool isDefault;
        const char* name;
        MSSChannelMap channelMaps[2][2];
    };

    enum snd_limit_type_t
    {
        SND_LIMIT_NONE = 0x0,
        SND_LIMIT_OLDEST = 0x1,
        SND_LIMIT_REJECT = 0x2,
        SND_LIMIT_PRIORITY = 0x3,
        SND_LIMIT_SOFTEST = 0x4,
        SND_LIMIT_COUNT = 0x5,
    };

    enum snd_randomize_type_t
    {
        SND_RANDOMIZE_INSTANCE = 0x0,
        SND_RANDOMIZE_ENTITY_VOLUME = 0x1,
        SND_RANDOMIZE_ENTITY_PITCH = 0x2,
        SND_RANDOMIZE_ENTITY_VARIANT = 0x4,
    };

    enum snd_category_t
    {
        SND_CATEGORY_SFX = 0x0,
        SND_CATEGORY_MUSIC = 0x1,
        SND_CATEGORY_VOICE = 0x2,
    };

    struct snd_asset
    {
        char* data;
        int data_size;
    };

    struct LoadedSound
    {
        const char* name;
        snd_asset sound;
    };

    struct StreamFileName
    {
        unsigned int hash;
        const char* dir;
        const char* name;
    };

    struct PrimedSound
    {
        const char* name;
        unsigned char* buffer;
        unsigned int size;
    };

    struct StreamedSound
    {
        StreamFileName filename;
        PrimedSound* primeSnd;
    };

    union SoundFileRef
    {
        LoadedSound* loadSnd;
        StreamedSound streamSnd;
    };

    enum snd_alias_type_t
    {
        SAT_UNKNOWN = 0x0,
        SAT_LOADED = 0x1,
        SAT_STREAMED = 0x2,
        SAT_PRIMED = 0x3,
        SAT_COUNT = 0x4,
    };

    struct SoundFile
    {
        unsigned char type;
        unsigned char exists;
        SoundFileRef u;
    };

    struct snd_alias_t
    {
        const char* aliasName;
        int aliasNameHash;
        const char* subtitle;
        const char* secondaryAliasName;
        const char* chainAliasName;
        SoundFile* soundFile;
        unsigned int sequence;
        float volMin;
        float volMax;
        float pitchMin;
        float pitchMax;
        float distMin;
        float distMax;
        float distReverbMax;
        float slavePercentage;
        float probability;
        float lfePercentage;
        float centerPercentage;
        float envelopMin;
        float envelopMax;
        float envelopPercentage;
        float minPriority;
        float maxPriority;
        float minPriorityThreshold;
        float maxPriorityThreshold;
        float reverbSend;
        int gap2;
        int gap3;
        float occlusionLevel;
        float occlusionWetDry;
        unsigned int moveTime;
        unsigned int startDelay;
        unsigned int speakerMap;
        int flags;
        unsigned int volumeFalloffCurve;
        unsigned int reverbFalloffCurve;
        unsigned int volumeMinFalloffCurve;
        unsigned int reverbMinFalloffCurve;
        snd_limit_type_t limitType;
        int gap5;
        unsigned int entityLimitCount;
        int gap6;
        snd_randomize_type_t randomizeType;
        int gap7;
        int gap8;
        int gap9;
    };

    struct snd_alias_list_t
    {
        const char* aliasName;
        snd_alias_t* head;
        int count;
    };

    struct cStaticModelWritable
    {
        uint16_t nextModelInWorldSector;
    };

    struct cStaticModel_s
    {
        cStaticModelWritable writable;
        XModel* xmodel;
        float origin[3];
        float invScaledAxis[3][3];
        float absmin[3];
        float absmax[3];
    };

    struct dmaterial_t
    {
        char material[64];
        int surfaceFlags;
        int contentFlags;
    };

    struct cNode_t
    {
        cplane_s* plane;
        int16_t children[2];
    };

    struct cLeaf_t
    {
        uint16_t firstCollAabbIndex;
        uint16_t collAabbCount;
        int brushContents;
        int terrainContents;
        float mins[3];
        float maxs[3];
        int leafBrushNode;
        int16_t cluster;
    };

    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        uint16_t childOffset[2];
    };

    struct cLeafBrushNodeLeaf_t
    {
        uint16_t* brushes;
    };

    union cLeafBrushNodeData_t
    {
        cLeafBrushNodeLeaf_t leaf;
        cLeafBrushNodeChildren_t children;
    };

    struct cLeafBrushNode_s
    {
        char axis;
        int16_t leafBrushCount;
        int contents;
        cLeafBrushNodeData_t data;
    };

    struct CollisionBorder
    {
        float distEq[3];
        float zBase;
        float zSlope;
        float start;
        float length;
    };

    struct CollisionPartition
    {
        unsigned char triCount;
        unsigned char borderCount;
        int firstTri;
        int nuinds;
        int fuind;
        CollisionBorder* borders;
    };

    union CollisionAabbTreeIndex
    {
        int firstChildIndex;
        int partitionIndex;
    };

    struct CollisionAabbTree
    {
        float origin[3];
        uint16_t materialIndex;
        uint16_t childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    struct cmodel_t
    {
        float mins[3];
        float maxs[3];
        float radius;
        cLeaf_t leaf;
    };

    struct type_align32(16) cbrush_t
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int16_t axialMaterialNum[2][3];
        cbrushedge_t* baseAdjacentSide;
        int16_t firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
        unsigned int numverts;
        vec3_t* verts;
    };

    struct GfxPlacement
    {
        float quat[4];
        float origin[3];
    };

    enum DynEntityType
    {
        DYNENT_TYPE_INVALID = 0x0,
        DYNENT_TYPE_CLUTTER = 0x1,
        DYNENT_TYPE_DESTRUCT = 0x2,
        DYNENT_TYPE_COUNT = 0x3,
    };

    struct DynEntityDef
    {
        DynEntityType type;
        GfxPlacement pose;
        XModel* xModel;
        uint16_t brushModel;
        uint16_t physicsBrushModel;
        FxEffectDef* destroyFx;
        XModelPieces* destroyPieces;
        PhysPreset* physPreset;
        unsigned char physConstraints[4];
        int health;
        PhysMass mass;
        int flags;
        int contents;
        ScriptString targetname;
    };

    struct DynEntityPose
    {
        GfxPlacement pose;
        float radius;
    };

    struct DynEntityClient
    {
        int physObjId;
        uint16_t flags;
        uint16_t lightingHandle;
        int health;
        uint16_t burnTime;
        uint16_t fadeTime;
    };

    struct DynEntityServer
    {
        uint16_t flags;
        int health;
    };

    struct DynEntityColl
    {
        uint16_t sector;
        uint16_t nextEntInSector;
        float linkMins[3];
        float linkMaxs[3];
        int contents;
    };

    typedef unsigned short LeafBrush;

    typedef tdef_align32(16) cbrush_t cbrush_array_t;

    struct clipMap_t
    {
        const char* name;
        int isInUse;
        int planeCount;
        cplane_s* planes;
        unsigned int numStaticModels;
        cStaticModel_s* staticModelList;
        unsigned int numMaterials;
        dmaterial_t* materials;
        unsigned int numBrushSides;
        cbrushside_t* brushsides;
        unsigned int numBrushEdges;
        cbrushedge_t* brushEdges;
        unsigned int numNodes;
        cNode_t* nodes;
        unsigned int numLeafs;
        cLeaf_t* leafs;
        unsigned int leafbrushNodesCount;
        cLeafBrushNode_s* leafbrushNodes;
        unsigned int numLeafBrushes;
        LeafBrush* leafbrushes;
        unsigned int numLeafSurfaces;
        unsigned int* leafsurfaces;
        unsigned int vertCount;
        vec3_t* verts;
        unsigned int numBrushVerts;
        vec3_t* brushVerts;
        unsigned int nuinds;
        uint16_t* uinds;
        int triCount;
        uint16_t* triIndices;
        char* triEdgeIsWalkable;
        int borderCount;
        CollisionBorder* borders;
        int partitionCount;
        CollisionPartition* partitions;
        int aabbTreeCount;
        CollisionAabbTree* aabbTrees;
        unsigned int numSubModels;
        cmodel_t* cmodels;
        uint16_t numBrushes;
        cbrush_t* brushes;
        int numClusters;
        int clusterBytes;
        char* visibility;
        int vised;
        MapEnts* mapEnts;
        cbrush_t* box_brush;
        cmodel_t box_model;
        uint16_t originalDynEntCount;
        uint16_t dynEntCount[4];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityServer* dynEntServerList[2];
        DynEntityColl* dynEntCollList[4];
        int num_constraints;
        PhysConstraint* constraints;
        unsigned int checksum;
    };

    struct ComPrimaryLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        unsigned char exponent;
        char priority;
        int16_t cullDist;
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        float cosHalfFovExpanded;
        float rotationLimit;
        float translationLimit;
        const char* defName;
    };

    struct ComWaterHeader
    {
        int minx;
        int miny;
        int maxx;
        int maxy;
    };

    struct ComWaterCell
    {
        int16_t waterheight;
        char flooroffset;
        unsigned char shoredist;
        unsigned char color[4];
    };

    struct ComBurnableHeader
    {
        int minx;
        int miny;
        int maxx;
        int maxy;
    };

    struct ComBurnableSample
    {
        unsigned char state;
    };

    struct ComBurnableCell
    {
        int x;
        int y;
        ComBurnableSample* data;
    };

    struct ComWorld
    {
        const char* name;
        int isInUse;
        unsigned int primaryLightCount;
        ComPrimaryLight* primaryLights;
        ComWaterHeader waterHeader;
        unsigned int numWaterCells;
        ComWaterCell* waterCells;
        ComBurnableHeader burnableHeader;
        unsigned int numBurnableCells;
        ComBurnableCell* burnableCells;
    };

    struct pathnode_tree_nodes_t
    {
        int nodeCount;
        uint16_t* nodes;
    };

    struct pathnode_t;
    struct pathnode_tree_t;

    union pathnode_tree_info_t
    {
        pathnode_tree_t* child[2];
        pathnode_tree_nodes_t s;
    };

    struct pathnode_tree_t
    {
        int axis;
        float dist;
        pathnode_tree_info_t u;
    };

    struct type_align(16) pathbasenode_t
    {
        float vOrigin[3];
        unsigned int type;
    };

    struct pathnode_transient_t
    {
        int iSearchFrame;
        pathnode_t* pNextOpen;
        pathnode_t* pPrevOpen;
        pathnode_t* pParent;
        float fCost;
        float fHeuristic;
        float costFactor;
    };

    struct pathnode_dynamic_t
    {
        void* pOwner;
        int iFreeTime;
        int iValidTime[3];
        int inPlayerLOSTime;
        int16_t wLinkCount;
        int16_t wOverlapCount;
        int16_t turretEntNumber;
        int16_t userCount;
    };

    struct pathlink_s
    {
        float fDist;
        uint16_t nodeNum;
        char disconnectCount;
        char negotiationLink;
        char ubBadPlaceCount[4];
    };

    enum nodeType
    {
        NODE_BADNODE = 0x0,
        NODE_PATHNODE = 0x1,
        NODE_COVER_STAND = 0x2,
        NODE_COVER_CROUCH = 0x3,
        NODE_COVER_CROUCH_WINDOW = 0x4,
        NODE_COVER_PRONE = 0x5,
        NODE_COVER_RIGHT = 0x6,
        NODE_COVER_LEFT = 0x7,
        NODE_COVER_WIDE_RIGHT = 0x8,
        NODE_COVER_WIDE_LEFT = 0x9,
        NODE_CONCEALMENT_STAND = 0xA,
        NODE_CONCEALMENT_CROUCH = 0xB,
        NODE_CONCEALMENT_PRONE = 0xC,
        NODE_REACQUIRE = 0xD,
        NODE_BALCONY = 0xE,
        NODE_SCRIPTED = 0xF,
        NODE_NEGOTIATION_BEGIN = 0x10,
        NODE_NEGOTIATION_END = 0x11,
        NODE_TURRET = 0x12,
        NODE_GUARD = 0x13,
        NODE_NUMTYPES = 0x14,
        NODE_DONTLINK = 0x14,
    };

    struct pathnode_constant_t
    {
        nodeType type;
        uint16_t spawnflags;
        uint16_t targetname;
        uint16_t script_linkName;
        uint16_t script_noteworthy;
        uint16_t target;
        uint16_t animscript;
        int animscriptfunc;
        float vOrigin[3];
        float fAngle;
        float forward[2];
        float fRadius;
        float minUseDistSq;
        int16_t wOverlapNode[2];
        int16_t wChainId;
        int16_t wChainDepth;
        int16_t wChainParent;
        uint16_t totalLinkCount;
        pathlink_s* Links;
    };

    struct pathnode_t
    {
        pathnode_constant_t constant;
        pathnode_dynamic_t dynamic;
        pathnode_transient_t transient;
    };

    struct PathData
    {
        unsigned int nodeCount;
        pathnode_t* nodes;
        pathbasenode_t* basenodes;
        unsigned int chainNodeCount;
        uint16_t* chainNodeForNode;
        uint16_t* nodeForChainNode;
        int visBytes;
        char* pathVis;
        int nodeTreeCount;
        pathnode_tree_t* nodeTree;
    };

    struct GameWorldSp
    {
        const char* name;
        PathData path;
    };

    struct GameWorldMp
    {
        const char* name;
        PathData path;
    };

    struct MapEnts
    {
        const char* name;
        const char* entityString;
        int numEntityChars;
    };

    struct GfxWorldVertex
    {
        float xyz[3];
        float binormalSign;
        GfxColor color;
        float texCoord[2];
        float lmapCoord[2];
        PackedUnitVec normal;
        PackedUnitVec tangent;
    };

    struct GfxWorldVertexData
    {
        GfxWorldVertex* vertices;
        void /*IDirect3DVertexBuffer9*/* worldVb;
    };

    struct GfxWorldVertexLayerData
    {
        char* data;
        void /*IDirect3DVertexBuffer9*/* layerVb;
    };

    struct SunLightParseParams
    {
        char name[64];
        float ambientScale;
        float ambientColor[3];
        float diffuseFraction;
        float sunLight;
        float sunColor[3];
        float diffuseColor[3];
        bool diffuseColorHasBeenSet;
        float angles[3];
        float treeScatterIntensity;
        float treeScatterAmount;
    };

    struct GfxLightImage
    {
        GfxImage* image;
        char samplerState;
    };

    struct GfxLightDef
    {
        const char* name;
        GfxLightImage attenuation;
        int lmapLookupStart;
    };

    struct GfxLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        int16_t cullDist;
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
        unsigned int spotShadowIndex;
        GfxLightDef* def;
    };

    struct GfxReflectionProbe
    {
        float origin[3];
        GfxImage* reflectionImage;
    };

    struct GfxLightCorona
    {
        float origin[3];
        float radius;
        float color[3];
        float intensity;
    };

    struct GfxWorldDpvsPlanes
    {
        int cellCount;
        cplane_s* planes;
        uint16_t* nodes;
        unsigned int* sceneEntCellBits;
    };

    struct DpvsPlane
    {
        float coeffs[4];
        char side[3];
        char pad;
    };

    struct GfxCell;
    struct GfxPortal;

    struct GfxPortalWritable
    {
        bool isQueued;
        bool isAncestor;
        char recursionDepth;
        char hullPointCount;
        float (*hullPoints)[2];
        GfxPortal* queuedParent;
    };

    struct GfxPortal
    {
        GfxPortalWritable writable;
        DpvsPlane plane;
        GfxCell* cell;
        vec3_t* vertices;
        char vertexCount;
        float hullAxis[2][3];
    };

    struct GfxAabbTree
    {
        float mins[3];
        float maxs[3];
        uint16_t childCount;
        uint16_t surfaceCount;
        uint16_t startSurfIndex;
        uint16_t smodelIndexCount;
        uint16_t* smodelIndexes;
        int childrenOffset;
    };

    struct GfxCell
    {
        float mins[3];
        float maxs[3];
        int aabbTreeCount;
        GfxAabbTree* aabbTree;
        int portalCount;
        GfxPortal* portals;
        int cullGroupCount;
        int* cullGroups;
        char reflectionProbeCount;
        char* reflectionProbes;
    };

    struct GfxLightmapArray
    {
        GfxImage* primary;
        GfxImage* secondary;
    };

    struct type_align(4) GfxLightGridEntry
    {
        uint16_t colorsIndex;
        char primaryLightIndex;
        char needsTrace;
    };

    struct type_align(4) GfxLightGridColors
    {
        char rgb[56][3];
    };

    struct GfxLightGrid
    {
        bool hasLightRegions;
        unsigned int sunPrimaryLightIndex;
        uint16_t mins[3];
        uint16_t maxs[3];
        unsigned int rowAxis;
        unsigned int colAxis;
        uint16_t* rowDataStart;
        unsigned int rawRowDataSize;
        char* rawRowData;
        unsigned int entryCount;
        GfxLightGridEntry* entries;
        unsigned int colorCount;
        GfxLightGridColors* colors;
    };

    struct GfxBrushModelWritable
    {
        float mins[3];
        float maxs[3];
    };

    struct GfxBrushModel
    {
        GfxBrushModelWritable writable;
        float bounds[2][3];
        unsigned int surfaceCount;
        unsigned int startSurfIndex;
    };

    struct MaterialMemory
    {
        Material* material;
        int memory;
    };

    struct sunflare_t
    {
        bool hasValidData;
        Material* spriteMaterial;
        Material* flareMaterial;
        float spriteSize;
        float flareMinSize;
        float flareMinDot;
        float flareMaxSize;
        float flareMaxDot;
        float flareMaxAlpha;
        int flareFadeInTime;
        int flareFadeOutTime;
        float blindMinDot;
        float blindMaxDot;
        float blindMaxDarken;
        int blindFadeInTime;
        int blindFadeOutTime;
        float glareMinDot;
        float glareMaxDot;
        float glareMaxLighten;
        int glareFadeInTime;
        int glareFadeOutTime;
        float sunFxPosition[3];
    };

    struct XModelDrawInfo
    {
        uint16_t lod;
        uint16_t surfId;
    };

    struct GfxSceneDynModel
    {
        XModelDrawInfo info;
        uint16_t dynEntId;
    };

    struct BModelDrawInfo
    {
        uint16_t surfId;
    };

    struct type_align(4) GfxSceneDynBrush
    {
        BModelDrawInfo info;
        uint16_t dynEntId;
    };

    struct GfxShadowGeometry
    {
        uint16_t surfaceCount;
        uint16_t smodelCount;
        uint16_t* sortedSurfIndex;
        uint16_t* smodelIndex;
    };

    struct GfxLightRegionAxis
    {
        float dir[3];
        float midPoint;
        float halfSize;
    };

    struct GfxLightRegionHull
    {
        float kdopMidPoint[9];
        float kdopHalfSize[9];
        unsigned int axisCount;
        GfxLightRegionAxis* axis;
    };

    struct GfxLightRegion
    {
        unsigned int hullCount;
        GfxLightRegionHull* hulls;
    };

    struct GfxPackedPlacement
    {
        float origin[3];
        float axis[3][3];
        float scale;
    };

    struct ModelLodFade
    {
        unsigned int value;
    };

    struct GfxStaticModelDrawInst
    {
        float cullDist;
        GfxPackedPlacement placement;
        XModel* model;
        ModelLodFade lodFade;
        int cachedLightSettingIndex;
        char field_44;
        char field_45;
        unsigned char reflectionProbeIndex;
        unsigned char primaryLightIndex;
        int flags;
        unsigned int smodelCacheIndex[4];
    };

    struct GfxStaticModelInst
    {
        float mins[3];
        float maxs[3];
        GfxColor groundLighting;
    };

    struct srfTriangles_t
    {
        int vertexLayerData;
        int firstVertex;
        uint16_t vertexCount;
        uint16_t triCount;
        int baseIndex;
    };

    struct GfxSurface
    {
        srfTriangles_t tris;
        int pad;
        Material* material;
        char lightmapIndex;
        char reflectionProbeIndex;
        char primaryLightIndex;
        char flags;
        float bounds[2][3];
    };

    struct GfxCullGroup
    {
        float mins[3];
        float maxs[3];
        int surfaceCount;
        int startSurfIndex;
    };

    struct GfxWorldDpvsStatic
    {
        unsigned int smodelCount;
        unsigned int staticSurfaceCount;
        unsigned int litSurfsBegin;
        unsigned int litSurfsEnd;
        unsigned int decalSurfsBegin;
        unsigned int decalSurfsEnd;
        unsigned int emissiveSurfsBegin;
        unsigned int emissiveSurfsEnd;
        unsigned int smodelVisDataCount;
        unsigned int surfaceVisDataCount;
        char* smodelVisData[3];
        char* surfaceVisData[3];
        raw_uint128* lodData;
        uint16_t* sortedSurfIndex;
        GfxStaticModelInst* smodelInsts;
        GfxSurface* surfaces;
        GfxCullGroup* cullGroups;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxDrawSurf* surfaceMaterials;
        raw_uint128* surfaceCastsSunShadow;
        volatile int usageCount;
    };

    struct GfxWorldDpvsDynamic
    {
        unsigned int dynEntClientWordCount[2];
        unsigned int dynEntClientCount[2];
        unsigned int* dynEntCellBits[2];
        raw_byte16* dynEntVisData[2][3];
    };

    struct GfxWorldLodChain
    {
        float origin[3];
        float lastDist;
        unsigned int firstLodInfo;
        uint16_t lodInfoCount;
    };

    struct GfxWorldLodInfo
    {
        float dist;
        unsigned int firstSurf;
        uint16_t surfCount;
    };

    struct GfxWaterBuffer
    {
        unsigned int bufferSize;
        vec4_t* buffer;
    };

    typedef tdef_align32(4) GfxSceneDynModel GfxSceneDynModel4;

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        int indexCount;
        uint16_t* indices;
        int surfaceCount;
        int streamInfo;
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        char skySamplerState;
        const char* skyBoxModel;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        unsigned int vertexStream2DataSize;
        SunLightParseParams sunParse;
        GfxLight* sunLight;
        float sunColorFromBsp[3];
        unsigned int sunPrimaryLightIndex;
        unsigned int primaryLightCount;
        int cullGroupCount;
        unsigned int reflectionProbeCount;
        GfxReflectionProbe* reflectionProbes;
        GfxTexture* reflectionProbeTextures;
        unsigned int coronaCount;
        GfxLightCorona* coronas;
        GfxWorldDpvsPlanes dpvsPlanes;
        int cellBitsCount;
        GfxCell* cells;
        int lightmapCount;
        GfxLightmapArray* lightmaps;
        GfxLightGrid lightGrid;
        GfxTexture* lightmapPrimaryTextures;
        GfxTexture* lightmapSecondaryTextures;
        int modelCount;
        GfxBrushModel* models;
        float mins[3];
        float maxs[3];
        unsigned int checksum;
        int materialMemoryCount;
        MaterialMemory* materialMemory;
        sunflare_t sun;
        float outdoorLookupMatrix[4][4];
        GfxImage* outdoorImage;
        unsigned int* cellCasterBits;
        GfxSceneDynModel4* sceneDynModel;
        GfxSceneDynBrush* sceneDynBrush;
        unsigned int* primaryLightEntityShadowVis;
        unsigned int* primaryLightDynEntShadowVis[2];
        char* nonSunPrimaryLightForModelDynEnt;
        GfxShadowGeometry* shadowGeom;
        GfxLightRegion* lightRegion;
        GfxWorldDpvsStatic dpvs;
        GfxWorldDpvsDynamic dpvsDyn;
        unsigned int worldLodChainCount;
        GfxWorldLodChain* worldLodChains;
        unsigned int worldLodInfoCount;
        GfxWorldLodInfo* worldLodInfos;
        unsigned int worldLodSurfaceCount;
        unsigned int* worldLodSurfaces;
        float waterDirection;
        GfxWaterBuffer waterBuffers[2];
        Material* waterMaterial;
    };

    struct Glyph
    {
        uint16_t letter;
        char x0;
        char y0;
        unsigned char dx;
        unsigned char pixelWidth;
        unsigned char pixelHeight;
        float s0;
        float t0;
        float s1;
        float t1;
    };

    struct Font_s
    {
        const char* fontName;
        int pixelHeight;
        int glyphCount;
        Material* material;
        Material* glowMaterial;
        Glyph* glyphs;
    };

    struct MenuList
    {
        const char* name;
        int menuCount;
        menuDef_t** menus;
    };

    struct rectDef_s
    {
        float x;
        float y;
        float w;
        float h;
        int horzAlign;
        int vertAlign;
    };

    struct windowDef_t
    {
        const char* name;
        rectDef_s rect;
        rectDef_s rectClient;
        const char* group;
        int style;
        int border;
        int ownerDraw;
        int ownerDrawFlags;
        float borderSize;
        int staticFlags;
        int dynamicFlags[1];
        int nextTime;
        float foreColor[4];
        float backColor[4];
        float borderColor[4];
        float outlineColor[4];
        Material* background;
    };

    struct ItemKeyHandler
    {
        int key;
        const char* action;
        ItemKeyHandler* next;
    };

    union operandInternalDataUnion
    {
        int intVal;
        float floatVal;
        const char* string;
    };

    enum expDataType
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
    };

    enum expOperationEnum
    {
        OP_NEGATE = 0x7,
        NUM_EXPRESSION_OPERATORS = 0x18,
        MAX_OPERATOR_VALUE = 0x4000,
    };

    struct Operand
    {
        expDataType dataType;
        operandInternalDataUnion internals;
    };

    union entryInternalData
    {
        Operand operand;
        expOperationEnum op;
    };

    enum expressionEntryType : int
    {
        EET_OPERATOR = 0x0,
        EET_OPERAND = 0x1,
    };

    struct expressionEntry
    {
        int type;
        entryInternalData data;
    };

    struct statement_s
    {
        int numEntries;
        expressionEntry** entries;
    };

    struct columnInfo_s
    {
        int pos;
        int width;
        int maxChars;
        int alignment;
    };

    struct listBoxDef_s
    {
        int mousePos;
        int startPos[1];
        int endPos[1];
        int drawPadding;
        float elementWidth;
        float elementHeight;
        int elementStyle;
        int numColumns;
        columnInfo_s columnInfo[16];
        const char* doubleClick;
        int notselectable;
        int noScrollBars;
        int usePaging;
        float selectBorder[4];
        float disableColor[4];
        float focusColor[4];
        Material* selectIcon;
        Material* backgroundItemListbox;
        Material* highlightTexture;
    };

    struct editFieldDef_s
    {
        float minVal;
        float maxVal;
        float defVal;
        float range;
        int maxChars;
        int maxCharsGotoNext;
        int maxPaintChars;
        int paintOffset;
    };

    struct multiDef_s
    {
        const char* dvarList[32];
        const char* dvarStr[32];
        float dvarValue[32];
        int count;
        int strDef;
    };

    union itemDefData_t
    {
        listBoxDef_s* listBox;
        editFieldDef_s* editField;
        multiDef_s* multi;
        const char* enumDvarName;
        void* data;
    };

    enum ItemDefType
    {
        ITEM_TYPE_TEXT = 0x0,
        ITEM_TYPE_BUTTON = 0x1,
        ITEM_TYPE_RADIOBUTTON = 0x2,
        ITEM_TYPE_CHECKBOX = 0x3,
        ITEM_TYPE_EDITFIELD = 0x4,
        ITEM_TYPE_COMBO = 0x5,
        ITEM_TYPE_LISTBOX = 0x6,
        ITEM_TYPE_MODEL = 0x7,
        ITEM_TYPE_OWNERDRAW = 0x8,
        ITEM_TYPE_NUMERICFIELD = 0x9,
        ITEM_TYPE_SLIDER = 0xA,
        ITEM_TYPE_YESNO = 0xB,
        ITEM_TYPE_MULTI = 0xC,
        ITEM_TYPE_DVARENUM = 0xD,
        ITEM_TYPE_BIND = 0xE,
        ITEM_TYPE_MENUMODEL = 0xF,
        ITEM_TYPE_VALIDFILEFIELD = 0x10,
        ITEM_TYPE_DECIMALFIELD = 0x11,
        ITEM_TYPE_UPREDITFIELD = 0x12,
        ITEM_TYPE_GAME_MESSAGE_WINDOW = 0x13
    };

    struct itemDef_s
    {
        windowDef_t window;
        rectDef_s textRect[1];
        int type;
        int dataType;
        int alignment;
        int fontEnum;
        int textAlignMode;
        float textalignx;
        float textaligny;
        float textscale;
        int textStyle;
        int gameMsgWindowIndex;
        int gameMsgWindowMode;
        const char* text;
        int itemFlags;
        menuDef_t* parent;
        const char* mouseEnterText;
        const char* mouseExitText;
        const char* mouseEnter;
        const char* mouseExit;
        const char* action;
        const char* onAccept;
        const char* onFocus;
        const char* leaveFocus;
        const char* dvar;
        const char* dvarTest;
        const char* onListboxSelectionChange;
        ItemKeyHandler* onKey;
        const char* enableDvar;
        int dvarFlags;
        snd_alias_list_t* focusSound;
        float special;
        int cursorPos[1];
        itemDefData_t typeData;
        int imageTrack;
        statement_s visibleExp;
        statement_s textExp;
        statement_s materialExp;
        statement_s rectXExp;
        statement_s rectYExp;
        statement_s rectWExp;
        statement_s rectHExp;
        statement_s forecolorAExp;
    };

    struct menuDef_t
    {
        windowDef_t window;
        const char* font;
        int fullScreen;
        int itemCount;
        int fontIndex;
        int cursorItem[1];
        int fadeCycle;
        float fadeClamp;
        float fadeAmount;
        float fadeInAmount;
        float blurRadius;
        const char* onOpen;
        const char* onFocus;
        const char* onClose;
        const char* onESC;
        ItemKeyHandler* onKey;
        statement_s visibleExp;
        const char* allowedBinding;
        const char* soundName;
        int imageTrack;
        float focusColor[4];
        float disableColor[4];
        statement_s rectXExp;
        statement_s rectYExp;
        itemDef_s** items;
    };

    struct LocalizeEntry
    {
        const char* value;
        const char* name;
    };

    enum weapType_t
    {
        WEAPTYPE_BULLET = 0x0,
        WEAPTYPE_GRENADE = 0x1,
        WEAPTYPE_PROJECTILE = 0x2,
        WEAPTYPE_BINOCULARS = 0x3,
        WEAPTYPE_GAS = 0x4,
        WEAPTYPE_BOMB = 0x5,
        WEAPTYPE_MINE = 0x6,

        WEAPTYPE_NUM,
    };

    enum weapClass_t
    {
        WEAPCLASS_RIFLE = 0x0,
        WEAPCLASS_MG = 0x1,
        WEAPCLASS_SMG = 0x2,
        WEAPCLASS_SPREAD = 0x3,
        WEAPCLASS_PISTOL = 0x4,
        WEAPCLASS_GRENADE = 0x5,
        WEAPCLASS_ROCKETLAUNCHER = 0x6,
        WEAPCLASS_TURRET = 0x7,
        WEAPCLASS_NON_PLAYER = 0x8,
        WEAPCLASS_GAS = 0x9,
        WEAPCLASS_ITEM = 0xA,

        WEAPCLASS_NUM,
    };

    enum PenetrateType
    {
        PENETRATE_TYPE_NONE = 0x0,
        PENETRATE_TYPE_SMALL = 0x1,
        PENETRATE_TYPE_MEDIUM = 0x2,
        PENETRATE_TYPE_LARGE = 0x3,

        PENETRATE_TYPE_NUM,
    };

    enum ImpactType
    {
        IMPACT_TYPE_NONE = 0x0,
        IMPACT_TYPE_BULLET_SMALL = 0x1,
        IMPACT_TYPE_BULLET_LARGE = 0x2,
        IMPACT_TYPE_BULLET_AP = 0x3,
        IMPACT_TYPE_SHOTGUN = 0x4,
        IMPACT_TYPE_GRENADE_BOUNCE = 0x5,
        IMPACT_TYPE_GRENADE_EXPLODE = 0x6,
        IMPACT_TYPE_RIFLE_GRENADE = 0x7,
        IMPACT_TYPE_ROCKET_EXPLODE = 0x8,
        IMPACT_TYPE_PROJECTILE_DUD = 0x9,
        IMPACT_TYPE_MORTAR_SHELL = 0xA,
        IMPACT_TYPE_TANK_SHELL = 0xB,

        IMPACT_TYPE_NUM,
    };

    enum weapInventoryType_t
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,

        WEAPINVENTORY_NUM,
    };

    enum weapFireType_t
    {
        WEAPON_FIRETYPE_FULLAUTO = 0x0,
        WEAPON_FIRETYPE_SINGLESHOT = 0x1,
        WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
        WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,

        WEAPON_FIRETYPE_NUM,
    };

    enum OffhandClass
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,

        OFFHAND_CLASS_NUM,
    };

    enum weapStance_t
    {
        WEAPSTANCE_STAND = 0x0,
        WEAPSTANCE_DUCK = 0x1,
        WEAPSTANCE_PRONE = 0x2,

        WEAPSTANCE_NUM,
    };

    enum activeReticleType_t
    {
        VEH_ACTIVE_RETICLE_NONE = 0x0,
        VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
        VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,

        VEH_ACTIVE_RETICLE_NUM,
    };

    enum weaponIconRatioType_t
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,

        WEAPON_ICON_RATIO_NUM,
    };

    enum weapClipType_t
    {
        WEAPON_CLIPTYPE_BOTTOM = 0x0,
        WEAPON_CLIPTYPE_TOP = 0x1,
        WEAPON_CLIPTYPE_LEFT = 0x2,
        WEAPON_CLIPTYPE_DP28 = 0x3,
        WEAPON_CLIPTYPE_PTRS = 0x4,
        WEAPON_CLIPTYPE_LMG = 0x5,

        WEAPON_CLIPTYPE_NUM,
    };

    enum ammoCounterClipType_t
    {
        AMMO_COUNTER_CLIP_NONE = 0x0,
        AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
        AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
        AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
        AMMO_COUNTER_CLIP_ROCKET = 0x4,
        AMMO_COUNTER_CLIP_BELTFED = 0x5,
        AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,

        AMMO_COUNTER_CLIP_NUM,
    };

    enum weapOverlayReticle_t
    {
        WEAPOVERLAYRETICLE_NONE = 0x0,
        WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,

        WEAPOVERLAYRETICLE_NUM,
    };

    enum WeapOverlayInterface_t
    {
        WEAPOVERLAYINTERFACE_NONE = 0x0,
        WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
        WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,

        WEAPOVERLAYINTERFACE_NUM,
    };

    enum weapProjExposion_t
    {
        WEAPPROJEXP_GRENADE = 0x0,
        WEAPPROJEXP_ROCKET = 0x1,
        WEAPPROJEXP_FLASHBANG = 0x2,
        WEAPPROJEXP_NONE = 0x3,
        WEAPPROJEXP_DUD = 0x4,
        WEAPPROJEXP_SMOKE = 0x5,
        WEAPPROJEXP_HEAVY = 0x6,
        WEAPPROJEXP_FIRE = 0x7,
        WEAPPROJEXP_NAPALMBLOB = 0x8,

        WEAPPROJEXP_NUM,
    };

    enum WeapStickinessType
    {
        WEAPSTICKINESS_NONE = 0x0,
        WEAPSTICKINESS_ALL = 0x1,
        WEAPSTICKINESS_GROUND = 0x2,
        WEAPSTICKINESS_GROUND_WITH_YAW = 0x3,

        WEAPSTICKINESS_NUM,
    };

    enum guidedMissileType_t
    {
        MISSILE_GUIDANCE_NONE = 0x0,
        MISSILE_GUIDANCE_SIDEWINDER = 0x1,
        MISSILE_GUIDANCE_HELLFIRE = 0x2,
        MISSILE_GUIDANCE_JAVELIN = 0x3,
        MISSILE_GUIDANCE_BALLISTIC = 0x4,

        MISSILE_GUIDANCE_NUM,
    };

    enum WeaponAnimSlot
    {
        WEAP_ANIM_ROOT = 0x0,
        WEAP_ANIM_IDLE = 0x1,
        WEAP_ANIM_EMPTY_IDLE = 0x2,
        WEAP_ANIM_FIRE = 0x3,
        WEAP_ANIM_HOLD_FIRE = 0x4,
        WEAP_ANIM_LASTSHOT = 0x5,
        WEAP_ANIM_RECHAMBER = 0x6,
        WEAP_ANIM_MELEE = 0x7,
        WEAP_ANIM_MELEE_CHARGE = 0x8,
        WEAP_ANIM_RELOAD = 0x9,
        WEAP_ANIM_RELOAD_EMPTY = 0xA,
        WEAP_ANIM_RELOAD_START = 0xB,
        WEAP_ANIM_RELOAD_END = 0xC,
        WEAP_ANIM_RAISE = 0xD,
        WEAP_ANIM_FIRST_RAISE = 0xE,
        WEAP_ANIM_DROP = 0xF,
        WEAP_ANIM_ALT_RAISE = 0x10,
        WEAP_ANIM_ALT_DROP = 0x11,
        WEAP_ANIM_QUICK_RAISE = 0x12,
        WEAP_ANIM_QUICK_DROP = 0x13,
        WEAP_ANIM_EMPTY_RAISE = 0x14,
        WEAP_ANIM_EMPTY_DROP = 0x15,
        WEAP_ANIM_SPRINT_IN = 0x16,
        WEAP_ANIM_SPRINT_LOOP = 0x17,
        WEAP_ANIM_SPRINT_OUT = 0x18,
        WEAP_ANIM_DEPLOY = 0x19,
        WEAP_ANIM_BREAKDOWN = 0x1A,
        WEAP_ANIM_DETONATE = 0x1B,
        WEAP_ANIM_NIGHTVISION_WEAR = 0x1C,
        WEAP_ANIM_NIGHTVISION_REMOVE = 0x1D,
        WEAP_ANIM_ADS_FIRE = 0x1E,
        WEAP_ANIM_ADS_LASTSHOT = 0x1F,
        WEAP_ANIM_ADS_RECHAMBER = 0x20,
        WEAP_ANIM_ADS_UP = 0x21,
        WEAP_ANIM_ADS_DOWN = 0x22,

        NUM_WEAP_ANIMS,
    };

    enum hitLocation_t
    {
        HITLOC_NONE = 0x0,
        HITLOC_HELMET = 0x1,
        HITLOC_HEAD = 0x2,
        HITLOC_NECK = 0x3,
        HITLOC_TORSO_UPR = 0x4,
        HITLOC_TORSO_LWR = 0x5,
        HITLOC_R_ARM_UPR = 0x6,
        HITLOC_L_ARM_UPR = 0x7,
        HITLOC_R_ARM_LWR = 0x8,
        HITLOC_L_ARM_LWR = 0x9,
        HITLOC_R_HAND = 0xA,
        HITLOC_L_HAND = 0xB,
        HITLOC_R_LEG_UPR = 0xC,
        HITLOC_L_LEG_UPR = 0xD,
        HITLOC_R_LEG_LWR = 0xE,
        HITLOC_L_LEG_LWR = 0xF,
        HITLOC_R_FOOT = 0x10,
        HITLOC_L_FOOT = 0x11,
        HITLOC_GUN = 0x12,

        HITLOC_COUNT,
    };

    struct snd_alias_list_name
    {
        const char* soundName;
    };

    union SndAliasCustom
    {
        snd_alias_list_name* name;
        snd_alias_list_t* sound;
    };

    union FxEffectDefRef
    {
        FxEffectDef* handle;
        const char* name;
    };

    struct DestructibleModel
    {
        XModel* model;
        float breakHealth;
        unsigned int flags;
        FxEffectDef* breakEffect;
        SndAliasCustom breakSound;
        const char* breakNotify;
        const char* loopSound;
        XModel* spawnModel[3];
    };

    struct DestructiblePiece
    {
        DestructibleModel dmodels[5];
        const char* attachBone;
        ScriptString attachBoneHash;
        unsigned char parentPiece;
        float parentDamagePercent;
        float bulletDamageScale;
        float explosiveDamageScale;
        float entityDamageTransfer;
        PhysConstraints* physConstraints;
        int health;
        SndAliasCustom damageSound;
        FxEffectDef* burnEffect;
        SndAliasCustom burnSound;
        ScriptString enableLabel;
    };

    struct DestructibleDef
    {
        const char* name;
        XModel* baseModel;
        int numPieces;
        DestructiblePiece* pieces;
        int clientOnly;
    };

    struct FlameTable
    {
        float flameVar_streamChunkGravityStart;
        float flameVar_streamChunkGravityEnd;
        float flameVar_streamChunkMaxSize;
        float flameVar_streamChunkStartSize;
        float flameVar_streamChunkEndSize;
        float flameVar_streamChunkStartSizeRand;
        float flameVar_streamChunkEndSizeRand;
        float flameVar_streamChunkDistScalar;
        float flameVar_streamChunkDistSwayScale;
        float flameVar_streamChunkDistSwayVelMax;
        float flameVar_streamChunkSpeed;
        float flameVar_streamChunkDecel;
        float flameVar_streamChunkVelocityAddScale;
        float flameVar_streamChunkDuration;
        float flameVar_streamChunkDurationScaleMaxVel;
        float flameVar_streamChunkDurationVelScalar;
        float flameVar_streamChunkSizeSpeedScale;
        float flameVar_streamChunkSizeAgeScale;
        float flameVar_streamChunkSpawnFireIntervalStart;
        float flameVar_streamChunkSpawnFireIntervalEnd;
        float flameVar_streamChunkSpawnFireMinLifeFrac;
        float flameVar_streamChunkSpawnFireMaxLifeFrac;
        float flameVar_streamChunkFireMinLifeFrac;
        float flameVar_streamChunkFireMinLifeFracStart;
        float flameVar_streamChunkFireMinLifeFracEnd;
        float flameVar_streamChunkDripsMinLifeFrac;
        float flameVar_streamChunkDripsMinLifeFracStart;
        float flameVar_streamChunkDripsMinLifeFracEnd;
        float flameVar_streamChunkRotationRange;
        float flameVar_streamSizeRandSinWave;
        float flameVar_streamSizeRandCosWave;
        float flameVar_streamDripsChunkInterval;
        float flameVar_streamDripsChunkMinFrac;
        float flameVar_streamDripsChunkRandFrac;
        float flameVar_streamSmokeChunkInterval;
        float flameVar_streamSmokeChunkMinFrac;
        float flameVar_streamSmokeChunkRandFrac;
        float flameVar_streamChunkCullDistSizeFrac;
        float flameVar_streamChunkCullMinLife;
        float flameVar_streamChunkCullMaxLife;
        float flameVar_streamFuelSizeStart;
        float flameVar_streamFuelSizeEnd;
        float flameVar_streamFuelLength;
        float flameVar_streamFuelNumSegments;
        float flameVar_streamFuelAnimLoopTime;
        float flameVar_streamFlameSizeStart;
        float flameVar_streamFlameSizeEnd;
        float flameVar_streamFlameLength;
        float flameVar_streamFlameNumSegments;
        float flameVar_streamFlameAnimLoopTime;
        float flameVar_streamPrimaryLightRadius;
        float flameVar_streamPrimaryLightRadiusFlutter;
        float flameVar_streamPrimaryLightR;
        float flameVar_streamPrimaryLightG;
        float flameVar_streamPrimaryLightB;
        float flameVar_streamPrimaryLightFlutterR;
        float flameVar_streamPrimaryLightFlutterG;
        float flameVar_streamPrimaryLightFlutterB;
        float flameVar_fireLife;
        float flameVar_fireLifeRand;
        float flameVar_fireSpeedScale;
        float flameVar_fireSpeedScaleRand;
        float flameVar_fireVelocityAddZ;
        float flameVar_fireVelocityAddZRand;
        float flameVar_fireVelocityAddSideways;
        float flameVar_fireGravity;
        float flameVar_fireGravityEnd;
        float flameVar_fireMaxRotVel;
        float flameVar_fireFriction;
        float flameVar_fireEndSizeAdd;
        float flameVar_fireStartSizeScale;
        float flameVar_fireEndSizeScale;
        float flameVar_dripsLife;
        float flameVar_dripsLifeRand;
        float flameVar_dripsSpeedScale;
        float flameVar_dripsSpeedScaleRand;
        float flameVar_dripsVelocityAddZ;
        float flameVar_dripsVelocityAddZRand;
        float flameVar_dripsVelocityAddSideways;
        float flameVar_dripsGravity;
        float flameVar_dripsGravityEnd;
        float flameVar_dripsMaxRotVel;
        float flameVar_dripsFriction;
        float flameVar_dripsEndSizeAdd;
        float flameVar_dripsStartSizeScale;
        float flameVar_dripsEndSizeScale;
        float flameVar_smokeLife;
        float flameVar_smokeLifeRand;
        float flameVar_smokeSpeedScale;
        float flameVar_smokeVelocityAddZ;
        float flameVar_smokeGravity;
        float flameVar_smokeGravityEnd;
        float flameVar_smokeMaxRotation;
        float flameVar_smokeMaxRotVel;
        float flameVar_smokeFriction;
        float flameVar_smokeEndSizeAdd;
        float flameVar_smokeStartSizeAdd;
        float flameVar_smokeOriginSizeOfsZScale;
        float flameVar_smokeOriginOfsZ;
        float flameVar_smokeFadein;
        float flameVar_smokeFadeout;
        float flameVar_smokeMaxAlpha;
        float flameVar_smokeBrightness;
        float flameVar_smokeOriginOffset;
        float flameVar_collisionSpeedScale;
        float flameVar_collisionVolumeScale;
        const char* name;
        Material* fire;
        Material* smoke;
        Material* heat;
        Material* drips;
        Material* streamFuel;
        Material* streamFuel2;
        Material* streamFlame;
        Material* streamFlame2;
        SndAliasCustom flameOffLoopSound;
        SndAliasCustom flameIgniteSound;
        SndAliasCustom flameOnLoopSound;
        SndAliasCustom flameCooldownSound;
    };

    struct WeaponDef
    {
        const char* szInternalName;
        const char* szDisplayName;
        const char* szOverlayName;
        XModel* gunXModel[16];
        XModel* handXModel;
        const char* szXAnims[35];
        const char* szModeName;
        uint16_t hideTags[8];
        uint16_t notetrackSoundMapKeys[20];
        uint16_t notetrackSoundMapValues[20];
        int playerAnimType;
        weapType_t weapType;
        weapClass_t weapClass;
        PenetrateType penetrateType;
        ImpactType impactType;
        weapInventoryType_t inventoryType;
        weapFireType_t fireType;
        weapClipType_t clipType;
        int overheatWeapon;
        float overheatRate;
        float cooldownRate;
        float overheatEndVal;
        int coolWhileFiring;
        OffhandClass offhandClass;
        weapStance_t stance;
        FxEffectDef* viewFlashEffect;
        FxEffectDef* worldFlashEffect;
        SndAliasCustom pickupSound;
        SndAliasCustom pickupSoundPlayer;
        SndAliasCustom ammoPickupSound;
        SndAliasCustom ammoPickupSoundPlayer;
        SndAliasCustom projectileSound;
        SndAliasCustom pullbackSound;
        SndAliasCustom pullbackSoundPlayer;
        SndAliasCustom fireSound;
        SndAliasCustom fireSoundPlayer;
        SndAliasCustom fireLoopSound;
        SndAliasCustom fireLoopSoundPlayer;
        SndAliasCustom fireStopSound;
        SndAliasCustom fireStopSoundPlayer;
        SndAliasCustom fireLastSound;
        SndAliasCustom fireLastSoundPlayer;
        SndAliasCustom emptyFireSound;
        SndAliasCustom emptyFireSoundPlayer;
        SndAliasCustom crackSound;
        SndAliasCustom whizbySound;
        SndAliasCustom meleeSwipeSound;
        SndAliasCustom meleeSwipeSoundPlayer;
        SndAliasCustom meleeHitSound;
        SndAliasCustom meleeMissSound;
        SndAliasCustom rechamberSound;
        SndAliasCustom rechamberSoundPlayer;
        SndAliasCustom reloadSound;
        SndAliasCustom reloadSoundPlayer;
        SndAliasCustom reloadEmptySound;
        SndAliasCustom reloadEmptySoundPlayer;
        SndAliasCustom reloadStartSound;
        SndAliasCustom reloadStartSoundPlayer;
        SndAliasCustom reloadEndSound;
        SndAliasCustom reloadEndSoundPlayer;
        SndAliasCustom rotateLoopSound;
        SndAliasCustom rotateLoopSoundPlayer;
        SndAliasCustom deploySound;
        SndAliasCustom deploySoundPlayer;
        SndAliasCustom finishDeploySound;
        SndAliasCustom finishDeploySoundPlayer;
        SndAliasCustom breakdownSound;
        SndAliasCustom breakdownSoundPlayer;
        SndAliasCustom finishBreakdownSound;
        SndAliasCustom finishBreakdownSoundPlayer;
        SndAliasCustom detonateSound;
        SndAliasCustom detonateSoundPlayer;
        SndAliasCustom nightVisionWearSound;
        SndAliasCustom nightVisionWearSoundPlayer;
        SndAliasCustom nightVisionRemoveSound;
        SndAliasCustom nightVisionRemoveSoundPlayer;
        SndAliasCustom altSwitchSound;
        SndAliasCustom altSwitchSoundPlayer;
        SndAliasCustom raiseSound;
        SndAliasCustom raiseSoundPlayer;
        SndAliasCustom firstRaiseSound;
        SndAliasCustom firstRaiseSoundPlayer;
        SndAliasCustom putawaySound;
        SndAliasCustom putawaySoundPlayer;
        SndAliasCustom overheatSound;
        SndAliasCustom overheatSoundPlayer;
        SndAliasCustom* bounceSound;
        const char* standMountedWeapdef;
        const char* crouchMountedWeapdef;
        const char* proneMountedWeapdef;
        int standMountedIndex;
        int crouchMountedIndex;
        int proneMountedIndex;
        FxEffectDef* viewShellEjectEffect;
        FxEffectDef* worldShellEjectEffect;
        FxEffectDef* viewLastShotEjectEffect;
        FxEffectDef* worldLastShotEjectEffect;
        Material* reticleCenter;
        Material* reticleSide;
        int iReticleCenterSize;
        int iReticleSideSize;
        int iReticleMinOfs;
        activeReticleType_t activeReticleType;
        float vStandMove[3];
        float vStandRot[3];
        float vDuckedOfs[3];
        float vDuckedMove[3];
        float duckedSprintOfs[3];
        float duckedSprintRot[3];
        float duckedSprintBob[2];
        float duckedSprintScale;
        float sprintOfs[3];
        float sprintRot[3];
        float sprintBob[2];
        float sprintScale;
        float vDuckedRot[3];
        float vProneOfs[3];
        float vProneMove[3];
        float vProneRot[3];
        float fPosMoveRate;
        float fPosProneMoveRate;
        float fStandMoveMinSpeed;
        float fDuckedMoveMinSpeed;
        float fProneMoveMinSpeed;
        float fPosRotRate;
        float fPosProneRotRate;
        float fStandRotMinSpeed;
        float fDuckedRotMinSpeed;
        float fProneRotMinSpeed;
        XModel* worldModel[16];
        XModel* worldClipModel;
        XModel* rocketModel;
        XModel* knifeModel;
        XModel* worldKnifeModel;
        XModel* mountedModel;
        Material* hudIcon;
        weaponIconRatioType_t hudIconRatio;
        Material* ammoCounterIcon;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
        int iStartAmmo;
        const char* szAmmoName;
        int iAmmoIndex;
        const char* szClipName;
        int iClipIndex;
        int iHeatIndex;
        int iMaxAmmo;
        int iClipSize;
        int shotCount;
        const char* szSharedAmmoCapName;
        int iSharedAmmoCapIndex;
        int iSharedAmmoCap;
        int unlimitedAmmo;
        int damage;
        int damageDuration;
        int damageInterval;
        int playerDamage;
        int iMeleeDamage;
        int iDamageType;
        int iFireDelay;
        int iMeleeDelay;
        int meleeChargeDelay;
        int iDetonateDelay;
        int iFireTime;
        int iRechamberTime;
        int iRechamberBoltTime;
        int iHoldFireTime;
        int iDetonateTime;
        int iMeleeTime;
        int meleeChargeTime;
        int iReloadTime;
        int reloadShowRocketTime;
        int iReloadEmptyTime;
        int iReloadAddTime;
        int reloadEmptyAddTime;
        int iReloadStartTime;
        int iReloadStartAddTime;
        int iReloadEndTime;
        int iDropTime;
        int iRaiseTime;
        int iAltDropTime;
        int iAltRaiseTime;
        int quickDropTime;
        int quickRaiseTime;
        int iFirstRaiseTime;
        int iEmptyRaiseTime;
        int iEmptyDropTime;
        int sprintInTime;
        int sprintLoopTime;
        int sprintOutTime;
        int deployTime;
        int breakdownTime;
        int nightVisionWearTime;
        int nightVisionWearTimeFadeOutEnd;
        int nightVisionWearTimePowerUp;
        int nightVisionRemoveTime;
        int nightVisionRemoveTimePowerDown;
        int nightVisionRemoveTimeFadeInStart;
        int fuseTime;
        int aiFuseTime;
        int requireLockonToFire;
        int noAdsWhenMagEmpty;
        int avoidDropCleanup;
        float autoAimRange;
        float aimAssistRange;
        float aimAssistRangeAds;
        int mountableWeapon;
        float aimPadding;
        float enemyCrosshairRange;
        int crosshairColorChange;
        float moveSpeedScale;
        float adsMoveSpeedScale;
        float sprintDurationScale;
        float fAdsZoomFov;
        float fAdsZoomInFrac;
        float fAdsZoomOutFrac;
        Material* overlayMaterial;
        Material* overlayMaterialLowRes;
        weapOverlayReticle_t overlayReticle;
        WeapOverlayInterface_t overlayInterface;
        float overlayWidth;
        float overlayHeight;
        float fAdsBobFactor;
        float fAdsViewBobMult;
        float fHipSpreadStandMin;
        float fHipSpreadDuckedMin;
        float fHipSpreadProneMin;
        float hipSpreadStandMax;
        float hipSpreadDuckedMax;
        float hipSpreadProneMax;
        float fHipSpreadDecayRate;
        float fHipSpreadFireAdd;
        float fHipSpreadTurnAdd;
        float fHipSpreadMoveAdd;
        float fHipSpreadDuckedDecay;
        float fHipSpreadProneDecay;
        float fHipReticleSidePos;
        int iAdsTransInTime;
        int iAdsTransOutTime;
        float fAdsIdleAmount;
        float fHipIdleAmount;
        float adsIdleSpeed;
        float hipIdleSpeed;
        float fIdleCrouchFactor;
        float fIdleProneFactor;
        float fGunMaxPitch;
        float fGunMaxYaw;
        float swayMaxAngle;
        float swayLerpSpeed;
        float swayPitchScale;
        float swayYawScale;
        float swayHorizScale;
        float swayVertScale;
        float swayShellShockScale;
        float adsSwayMaxAngle;
        float adsSwayLerpSpeed;
        float adsSwayPitchScale;
        float adsSwayYawScale;
        float adsSwayHorizScale;
        float adsSwayVertScale;
        int bRifleBullet;
        int armorPiercing;
        int bBoltAction;
        int aimDownSight;
        int bRechamberWhileAds;
        float adsViewErrorMin;
        float adsViewErrorMax;
        int bCookOffHold;
        int bClipOnly;
        int canUseInVehicle;
        int noDropsOrRaises;
        int adsFireOnly;
        int cancelAutoHolsterWhenEmpty;
        int suppressAmmoReserveDisplay;
        int enhanced;
        int laserSightDuringNightvision;
        int bayonet;
        Material* killIcon;
        weaponIconRatioType_t killIconRatio;
        int flipKillIcon;
        Material* dpadIcon;
        weaponIconRatioType_t dpadIconRatio;
        int bNoPartialReload;
        int bSegmentedReload;
        int noADSAutoReload;
        int iReloadAmmoAdd;
        int iReloadStartAdd;
        const char* szAltWeaponName;
        unsigned int altWeaponIndex;
        int iDropAmmoMin;
        int iDropAmmoMax;
        int blocksProne;
        int silenced;
        int iExplosionRadius;
        int iExplosionRadiusMin;
        int iExplosionInnerDamage;
        int iExplosionOuterDamage;
        float damageConeAngle;
        int iProjectileSpeed;
        int iProjectileSpeedUp;
        int iProjectileSpeedForward;
        int iProjectileActivateDist;
        float projLifetime;
        float timeToAccelerate;
        float projectileCurvature;
        XModel* projectileModel;
        weapProjExposion_t projExplosion;
        FxEffectDef* projExplosionEffect;
        int projExplosionEffectForceNormalUp;
        FxEffectDef* projDudEffect;
        SndAliasCustom projExplosionSound;
        SndAliasCustom projDudSound;
        SndAliasCustom mortarShellSound;
        SndAliasCustom tankShellSound;
        int bProjImpactExplode;
        WeapStickinessType stickiness;
        int hasDetonator;
        int timedDetonation;
        int rotate;
        int holdButtonToThrow;
        int freezeMovementWhenFiring;
        float lowAmmoWarningThreshold;
        float parallelBounce[31];
        float perpendicularBounce[31];
        FxEffectDef* projTrailEffect;
        float vProjectileColor[3];
        guidedMissileType_t guidedMissileType;
        float maxSteeringAccel;
        int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        SndAliasCustom projIgnitionSound;
        float fAdsAimPitch;
        float fAdsCrosshairInFrac;
        float fAdsCrosshairOutFrac;
        int adsGunKickReducedKickBullets;
        float adsGunKickReducedKickPercent;
        float fAdsGunKickPitchMin;
        float fAdsGunKickPitchMax;
        float fAdsGunKickYawMin;
        float fAdsGunKickYawMax;
        float fAdsGunKickAccel;
        float fAdsGunKickSpeedMax;
        float fAdsGunKickSpeedDecay;
        float fAdsGunKickStaticDecay;
        float fAdsViewKickPitchMin;
        float fAdsViewKickPitchMax;
        float fAdsViewKickYawMin;
        float fAdsViewKickYawMax;
        float fAdsViewKickCenterSpeed;
        float fAdsViewScatterMin;
        float fAdsViewScatterMax;
        float fAdsSpread;
        int hipGunKickReducedKickBullets;
        float hipGunKickReducedKickPercent;
        float fHipGunKickPitchMin;
        float fHipGunKickPitchMax;
        float fHipGunKickYawMin;
        float fHipGunKickYawMax;
        float fHipGunKickAccel;
        float fHipGunKickSpeedMax;
        float fHipGunKickSpeedDecay;
        float fHipGunKickStaticDecay;
        float fHipViewKickPitchMin;
        float fHipViewKickPitchMax;
        float fHipViewKickYawMin;
        float fHipViewKickYawMax;
        float fHipViewKickCenterSpeed;
        float fHipViewScatterMin;
        float fHipViewScatterMax;
        float fightDist;
        float maxDist;
        const char* aiVsAiAccuracyGraphName;
        const char* aiVsPlayerAccuracyGraphName;
        vec2_t* aiVsAiAccuracyGraphKnots;
        vec2_t* aiVsPlayerAccuracyGraphKnots;
        vec2_t* originalAiVsAiAccuracyGraphKnots;
        vec2_t* originalAiVsPlayerAccuracyGraphKnots;
        int aiVsAiAccuracyGraphKnotCount;
        int aiVsPlayerAccuracyGraphKnotCount;
        int originalAiVsAiAccuracyGraphKnotCount;
        int originalAiVsPlayerAccuracyGraphKnotCount;
        int iPositionReloadTransTime;
        float leftArc;
        float rightArc;
        float topArc;
        float bottomArc;
        float accuracy;
        float aiSpread;
        float playerSpread;
        float minTurnSpeed[2];
        float maxTurnSpeed[2];
        float pitchConvergenceTime;
        float yawConvergenceTime;
        float suppressTime;
        float maxRange;
        float fAnimHorRotateInc;
        float fPlayerPositionDist;
        const char* szUseHintString;
        const char* dropHintString;
        int iUseHintStringIndex;
        int dropHintStringIndex;
        float horizViewJitter;
        float vertViewJitter;
        const char* szScript;
        float fOOPosAnimLength[2];
        int minDamage;
        int minPlayerDamage;
        float fMaxDamageRange;
        float fMinDamageRange;
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        float locationDamageMultipliers[HITLOC_COUNT];
        const char* fireRumble;
        const char* meleeImpactRumble;
        float adsDofStart;
        float adsDofEnd;
        float hipDofStart;
        float hipDofEnd;
        const char* flameTableFirstPerson;
        const char* flameTableThirdPerson;
        FlameTable* flameTableFirstPersonPtr;
        FlameTable* flameTableThirdPersonPtr;
        FxEffectDef* tagFx_preparationEffect;
        FxEffectDef* tagFlash_preparationEffect;
    };

    struct snd_reverb_patch_t
    {
        char name[32];
        float room;
        float roomHF;
        float roomRolloffFactor;
        float decayTime;
        float decayHFRatio;
        float reflections;
        float reflectionsDelay;
        float reverb;
        float reverbDelay;
        float diffusion;
        float density;
        float HFReference;
    };

    struct snd_bus_info_t
    {
        char name[32];
        float volumeMod;
        int maxVoices;
        int restricted;
        int pausable;
        int stopOnDeath;
        int isMusic;
        snd_category_t category;
        snd_limit_type_t voiceLimitType;
    };

    struct snd_curve_t
    {
        char name[32];
        int pointCount;
        float points[8][2];
    };

    struct snd_speaker_map
    {
        int input_channel_count;
        int output_channel_count;
        float volumes[16];
    };

    struct snd_speaker_map_set
    {
        char name[32];
        snd_speaker_map channelMaps[4];
    };

    struct snd_master_effect_t
    {
        char name[32];
        float masterRingmod;
        float reverbRingmod;
        float masterLPRatio;
        float masterLPDB;
        float masterHPRatio;
        float masterHPDB;
    };

    struct SndDriverGlobals
    {
        const char* name;
        snd_reverb_patch_t reverbPatches[64];
        snd_bus_info_t buses[64];
        snd_curve_t curves[32];
        snd_speaker_map_set speakerMaps[32];
        snd_master_effect_t masterEffects[16];
    };

    struct FxIntRange
    {
        int base;
        int amplitude;
    };

    struct FxFloatRange
    {
        float base;
        float amplitude;
    };

    struct FxSpawnDefOneShot
    {
        FxIntRange count;
    };

    struct FxSpawnDefLooping
    {
        int intervalMsec;
        int count;
    };

    union FxSpawnDef
    {
        FxSpawnDefLooping looping;
        FxSpawnDefOneShot oneShot;
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
    };

    struct FxElemVec3Range
    {
        float base[3];
        float amplitude[3];
    };

    struct FxElemVelStateInFrame
    {
        FxElemVec3Range velocity;
        FxElemVec3Range totalDelta;
    };

    struct FxElemVelStateSample
    {
        FxElemVelStateInFrame local;
        FxElemVelStateInFrame world;
    };

    struct FxElemVisualState
    {
        char color[4];
        float rotationDelta;
        float rotationTotal;
        float size[2];
        float scale;
    };

    struct FxElemVisStateSample
    {
        FxElemVisualState base;
        FxElemVisualState amplitude;
    };

    enum FxElemType
    {
        FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
        FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
        FX_ELEM_TYPE_TAIL = 0x2,
        FX_ELEM_TYPE_TRAIL = 0x3,
        FX_ELEM_TYPE_CLOUD = 0x4,
        FX_ELEM_TYPE_SPARK_CLOUD = 0x5,
        FX_ELEM_TYPE_MODEL = 0x6,
        FX_ELEM_TYPE_OMNI_LIGHT = 0x7,
        FX_ELEM_TYPE_SPOT_LIGHT = 0x8,
        FX_ELEM_TYPE_SOUND = 0x9,
        FX_ELEM_TYPE_DECAL = 0xA,
        FX_ELEM_TYPE_RUNNER = 0xB,
        FX_ELEM_TYPE_COUNT = 0xC,
        FX_ELEM_TYPE_LAST_SPRITE = 0x3,
        FX_ELEM_TYPE_LAST_DRAWN = 0x8,
    };

    union FxElemVisuals
    {
        const void* anonymous;
        Material* material;
        XModel* model;
        FxEffectDefRef effectDef;
        const char* soundName;
    };

    struct FxElemMarkVisuals
    {
        Material* materials[2];
    };

    union FxElemDefVisuals
    {
        FxElemMarkVisuals* markArray;
        FxElemVisuals* array;
        FxElemVisuals instance;
    };

    struct FxTrailVertex
    {
        float pos[2];
        float normal[2];
        float texCoord;
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
    };

    struct FxElemDef
    {
        int flags;
        FxSpawnDef spawn;
        FxFloatRange spawnRange;
        FxFloatRange fadeInRange;
        FxFloatRange fadeOutRange;
        float spawnFrustumCullRadius;
        FxIntRange spawnDelayMsec;
        FxIntRange lifeSpanMsec;
        FxFloatRange spawnOrigin[3];
        FxFloatRange spawnOffsetRadius;
        FxFloatRange spawnOffsetHeight;
        FxFloatRange spawnAngles[3];
        FxFloatRange angularVelocity[3];
        FxFloatRange initialRotation;
        FxFloatRange gravity;
        FxFloatRange reflectionFactor;
        FxElemAtlas atlas;
        float windInfluence;
        char elemType;
        char visualCount;
        char velIntervalCount;
        char visStateIntervalCount;
        FxElemVelStateSample* velSamples;
        FxElemVisStateSample* visSamples;
        FxElemDefVisuals visuals;
        float collMins[3];
        float collMaxs[3];
        FxEffectDefRef effectOnImpact;
        FxEffectDefRef effectOnDeath;
        FxEffectDefRef effectEmitted;
        FxFloatRange emitDist;
        FxFloatRange emitDistVariance;
        FxTrailDef* trailDef;
        char sortOrder;
        char lightingFrac;
        char useItemClip;
        char unused[1];
    };

    struct FxEffectDef
    {
        const char* name;
        int flags;
        int totalSize;
        int msecLoopingLife;
        int elemDefCountLooping;
        int elemDefCountOneShot;
        int elemDefCountEmission;
        char efPriority;
        FxElemDef* elemDefs;
    };

    struct FxImpactEntry
    {
        FxEffectDef* nonflesh[31];
        FxEffectDef* flesh[4];
    };

    struct FxImpactTable
    {
        const char* name;
        FxImpactEntry* table;
    };

    struct RawFile
    {
        const char* name;
        int len;
        const char* buffer;
    };

    struct StringTable
    {
        const char* name;
        int columnCount;
        int rowCount;
        const char** values;
    };

    struct PackIndexEntry
    {
        unsigned int hash;
        unsigned int offset;
        unsigned int size;
    };

    struct PackIndexHeader
    {
        unsigned int magic;
        unsigned int timestamp;
        unsigned int count;
        unsigned int alignment;
        unsigned int data_start;
    };

    struct PackIndex
    {
        const char* name;
        PackIndexHeader header;
        PackIndexEntry* entries;
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
