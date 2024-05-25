#pragma once

#ifndef __IW4_ASSETS_H
#define __IW4_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace IW4
{
#endif
    enum XAssetType
    {
        ASSET_TYPE_PHYSPRESET = 0x0,
        ASSET_TYPE_PHYSCOLLMAP = 0x1,
        ASSET_TYPE_XANIMPARTS = 0x2,
        ASSET_TYPE_XMODEL_SURFS = 0x3,
        ASSET_TYPE_XMODEL = 0x4,
        ASSET_TYPE_MATERIAL = 0x5,
        ASSET_TYPE_PIXELSHADER = 0x6,
        ASSET_TYPE_VERTEXSHADER = 0x7,
        ASSET_TYPE_VERTEXDECL = 0x8,
        ASSET_TYPE_TECHNIQUE_SET = 0x9,
        ASSET_TYPE_IMAGE = 0xA,
        ASSET_TYPE_SOUND = 0xB,
        ASSET_TYPE_SOUND_CURVE = 0xC,
        ASSET_TYPE_LOADED_SOUND = 0xD,
        ASSET_TYPE_CLIPMAP_SP = 0xE,
        ASSET_TYPE_CLIPMAP_MP = 0xF,
        ASSET_TYPE_COMWORLD = 0x10,
        ASSET_TYPE_GAMEWORLD_SP = 0x11,
        ASSET_TYPE_GAMEWORLD_MP = 0x12,
        ASSET_TYPE_MAP_ENTS = 0x13,
        ASSET_TYPE_FXWORLD = 0x14,
        ASSET_TYPE_GFXWORLD = 0x15,
        ASSET_TYPE_LIGHT_DEF = 0x16,
        ASSET_TYPE_UI_MAP = 0x17,
        ASSET_TYPE_FONT = 0x18,
        ASSET_TYPE_MENULIST = 0x19,
        ASSET_TYPE_MENU = 0x1A,
        ASSET_TYPE_LOCALIZE_ENTRY = 0x1B,
        ASSET_TYPE_WEAPON = 0x1C,
        ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1D,
        ASSET_TYPE_FX = 0x1E,
        ASSET_TYPE_IMPACT_FX = 0x1F,
        ASSET_TYPE_AITYPE = 0x20,
        ASSET_TYPE_MPTYPE = 0x21,
        ASSET_TYPE_CHARACTER = 0x22,
        ASSET_TYPE_XMODELALIAS = 0x23,
        ASSET_TYPE_RAWFILE = 0x24,
        ASSET_TYPE_STRINGTABLE = 0x25,
        ASSET_TYPE_LEADERBOARD = 0x26,
        ASSET_TYPE_STRUCTURED_DATA_DEF = 0x27,
        ASSET_TYPE_TRACER = 0x28,
        ASSET_TYPE_VEHICLE = 0x29,
        ASSET_TYPE_ADDON_MAP_ENTS = 0x2A,

        ASSET_TYPE_COUNT,
        ASSET_TYPE_STRING = ASSET_TYPE_COUNT,
        ASSET_TYPE_ASSETLIST = 0x2C,

        ASSET_TYPE_FULLCOUNT
    };

    enum XFileBlock
    {
        XFILE_BLOCK_TEMP,
        XFILE_BLOCK_PHYSICAL,
        XFILE_BLOCK_RUNTIME,
        XFILE_BLOCK_VIRTUAL,
        XFILE_BLOCK_LARGE,
        XFILE_BLOCK_CALLBACK,
        XFILE_BLOCK_VERTEX,
        XFILE_BLOCK_INDEX,

        MAX_XFILE_COUNT,
    };

    struct PhysPreset;
    struct PhysCollmap;
    struct XAnimParts;
    struct XModel;
    struct Material;
    struct MaterialPixelShader;
    struct MaterialVertexShader;
    struct MaterialVertexDeclaration;
    struct MaterialTechniqueSet;
    struct GfxImage;
    struct snd_alias_list_t;
    struct SndCurve;
    struct LoadedSound;
    struct clipMap_t;
    struct ComWorld;
    struct GameWorldSp;
    struct GameWorldMp;
    struct MapEnts;
    struct FxWorld;
    struct GfxWorld;
    struct GfxLightDef;
    struct Font_s;
    struct MenuList;
    struct menuDef_t;
    struct LocalizeEntry;
    struct WeaponCompleteDef;
    // struct SndDriverGlobals;
    struct FxEffectDef;
    struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    struct LeaderboardDef;
    struct StructuredDataDefSet;
    struct TracerDef;
    struct VehicleDef;
    struct AddonMapEnts;

    union XAssetHeader
    {
        PhysPreset* physPreset;
        PhysCollmap* physCollmap;
        XAnimParts* parts;
        // XModelSurfs* modelSurfs; // NOT AN ASSET
        XModel* model;
        Material* material;
        MaterialPixelShader* pixelShader;
        MaterialVertexShader* vertexShader;
        MaterialVertexDeclaration* vertexDecl;
        MaterialTechniqueSet* techniqueSet;
        GfxImage* image;
        snd_alias_list_t* sound;
        SndCurve* sndCurve;
        LoadedSound* loadSnd;
        clipMap_t* clipMap;
        ComWorld* comWorld;
        GameWorldSp* gameWorldSp;
        GameWorldMp* gameWorldMp;
        MapEnts* mapEnts;
        FxWorld* fxWorld;
        GfxWorld* gfxWorld;
        GfxLightDef* lightDef;
        Font_s* font;
        MenuList* menuList;
        menuDef_t* menu;
        LocalizeEntry* localize;
        WeaponCompleteDef* weapon;
        // SndDriverGlobals* sndDriverGlobals; // UNUSED ON PC
        FxEffectDef* fx;
        FxImpactTable* impactFx;
        RawFile* rawfile;
        StringTable* stringTable;
        LeaderboardDef* leaderboardDef;
        StructuredDataDefSet* structuredDataDefSet;
        TracerDef* tracerDef;
        VehicleDef* vehDef;
        AddonMapEnts* addonMapEnts;
        void* data;
    };

    typedef tdef_align(16) char raw_byte16;
    typedef tdef_align(16) float raw_float16;
    typedef unsigned int raw_uint;
    typedef tdef_align(128) unsigned int raw_uint128;
    typedef unsigned char cbrushedge_t;
    typedef unsigned short r_index_t;
    typedef float vec2_t[2];
    typedef float vec3_t[3];
    typedef float vec4_t[4];

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
        bool tempDefaultToCylinder;
        bool perSurfaceSndAlias;
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
        int tempDefaultToCylinder;
        int perSurfaceSndAlias;
    };

    struct Bounds
    {
        float midPoint[3];
        float halfSize[3];
    };

    struct cplane_s
    {
        float normal[3];
        float dist;
        char type;
        char pad[3];
    };

    struct cbrushside_t
    {
        cplane_s* plane;
        uint16_t materialNum;
        char firstAdjacentSideOffset;
        char edgeCount;
    };

    struct cbrushWrapper_t
    {
        uint16_t numsides;
        uint16_t glassPieceIndex;
        cbrushside_t* sides;
        cbrushedge_t* baseAdjacentSide;
        int16_t axialMaterialNum[2][3];
        unsigned char firstAdjacentSideOffsets[2][3];
        unsigned char edgeCount[2][3];
    };

    struct BrushWrapper
    {
        Bounds bounds;
        cbrushWrapper_t brush;
        int totalEdgeCount;
        cplane_s* planes;
    };

    enum PhysicsGeomType
    {
        PHYS_GEOM_NONE = 0x0,
        PHYS_GEOM_BOX = 0x1,
        PHYS_GEOM_BRUSHMODEL = 0x2,
        PHYS_GEOM_BRUSH = 0x3,
        PHYS_GEOM_COLLMAP = 0x4,
        PHYS_GEOM_CYLINDER = 0x5,
        PHYS_GEOM_CAPSULE = 0x6,
        PHYS_GEOM_GLASS = 0x7,

        PHYS_GEOM_COUNT,
    };

    struct PhysGeomInfo
    {
        BrushWrapper* brushWrapper;
        int type;
        float orientation[3][3];
        Bounds bounds;
    };

    struct PhysMass
    {
        float centerOfMass[3];
        float momentsOfInertia[3];
        float productsOfInertia[3];
    };

    struct PhysCollmap
    {
        const char* name;
        unsigned int count;
        PhysGeomInfo* geoms;
        PhysMass mass;
        Bounds bounds;
    };

    union XAnimIndices
    {
        char* _1;
        uint16_t* _2;
        void* data;
    };

    struct XAnimNotifyInfo
    {
        uint16_t name;
        float time;
    };

    typedef unsigned char ByteVec[3];
    typedef tdef_align(4) unsigned short UShortVec[3];

    union XAnimDynamicFrames
    {
        ByteVec* _1;
        UShortVec* _2;
    };

    union XAnimDynamicIndicesTrans
    {
        char _1[1];
        uint16_t _2[1];
    };

    struct type_align(4) XAnimPartTransFrames
    {
        float mins[3];
        float size[3];
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
        char smallTrans;
        XAnimPartTransData u;
    };

    union XAnimDynamicIndicesQuat2
    {
        char _1[1];
        uint16_t _2[1];
    };

    typedef tdef_align(4) short XQuat2[2];

    struct type_align(4) XAnimDeltaPartQuatDataFrames2
    {
        XQuat2* frames;
        XAnimDynamicIndicesQuat2 indices;
    };

    union XAnimDeltaPartQuatData2
    {
        XAnimDeltaPartQuatDataFrames2 frames;
        int16_t frame0[2];
    };

    struct XAnimDeltaPartQuat2
    {
        uint16_t size;
        XAnimDeltaPartQuatData2 u;
    };

    union XAnimDynamicIndicesQuat
    {
        char _1[1];
        uint16_t _2[1];
    };

    typedef tdef_align(4) short XQuat[4];

    struct XAnimDeltaPartQuatDataFrames
    {
        XQuat* frames;
        XAnimDynamicIndicesQuat indices;
    };

    union XAnimDeltaPartQuatData
    {
        XAnimDeltaPartQuatDataFrames frames;
        int16_t frame0[4];
    };

    struct XAnimDeltaPartQuat
    {
        uint16_t size;
        XAnimDeltaPartQuatData u;
    };

    struct XAnimDeltaPart
    {
        XAnimPartTrans* trans;
        XAnimDeltaPartQuat2* quat2;
        XAnimDeltaPartQuat* quat;
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
        char flags;
        unsigned char boneCount[10];
        char notifyCount;
        char assetType;
        bool isDefault;
        unsigned int randomDataShortCount;
        unsigned int indexCount;
        float framerate;
        float frequency;
        uint16_t* names;
        char* dataByte;
        int16_t* dataShort;
        int* dataInt;
        int16_t* randomDataShort;
        char* randomDataByte;
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

    struct XSurfaceVertexInfo
    {
        int16_t vertCount[4];
        uint16_t* vertsBlend;
    };

    union GfxColor
    {
        unsigned int packed;
        char array[4];
    };

    union PackedTexCoords
    {
        unsigned int packed;
    };

    union PackedUnitVec
    {
        unsigned int packed;
        char array[4];
    };

    struct type_align(16) GfxPackedVertex
    {
        float xyz[3];
        float binormalSign;
        GfxColor color;
        PackedTexCoords texCoord;
        PackedUnitVec normal;
        PackedUnitVec tangent;
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

    typedef tdef_align(16) uint16_t r_index16_t;

    struct XSurface
    {
        char tileMode;
        bool deformed;
        uint16_t vertCount;
        uint16_t triCount;
        char zoneHandle;
        uint16_t baseTriIndex;
        uint16_t baseVertIndex;
        r_index16_t (*triIndices)[3];
        XSurfaceVertexInfo vertInfo;
        GfxPackedVertex* verts0;
        unsigned int vertListCount;
        XRigidVertList* vertList;
        int partBits[6];
    };

    struct XModelSurfs
    {
        const char* name;
        XSurface* surfs;
        uint16_t numsurfs;
        int partBits[6];
    };

    struct XModelLodInfo
    {
        float dist;
        uint16_t numsurfs;
        uint16_t surfIndex;
        XModelSurfs* modelSurfs;
        int partBits[6];
        XSurface* surfs;
        char lod;
        char smcBaseIndexPlusOne;
        char smcSubIndexMask;
        char smcBucket;
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
        Bounds bounds;
        int boneIdx;
        int contents;
        int surfFlags;
    };

    struct XBoneInfo
    {
        Bounds bounds;
        float radiusSquared;
    };

    struct DObjAnimMat
    {
        float quat[4];
        float trans[3];
        float transWeight;
    };

    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        char lodRampType;
        float scale;
        unsigned int noScalePartBits[6];
        uint16_t* boneNames;
        unsigned char* parentList;
        int16_t (*quats)[4];
        float (*trans)[3];
        unsigned char* partClassification;
        DObjAnimMat* baseMat;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        char maxLoadedLod;
        unsigned char numLods;
        unsigned char collLod;
        char flags;
        XModelCollSurf_s* collSurfs;
        int numCollSurfs;
        int contents;
        XBoneInfo* boneInfo;
        float radius;
        Bounds bounds;
        int memUsage;
        bool bad;
        PhysPreset* physPreset;
        PhysCollmap* physCollmap;
    };

    struct WaterWritable
    {
        float floatTime;
    };

    struct complex_s
    {
        float real;
        float imag;
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

    enum MaterialTextureFilter
    {
        TEXTURE_FILTER_DISABLED = 0x0,
        TEXTURE_FILTER_NEAREST = 0x1,
        TEXTURE_FILTER_LINEAR = 0x2,
        TEXTURE_FILTER_ANISO2X = 0x3,
        TEXTURE_FILTER_ANISO4X = 0x4,

        TEXTURE_FILTER_COUNT
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

    struct MaterialTextureDef
    {
        unsigned int nameHash;
        char nameStart;
        char nameEnd;
        unsigned char samplerState; // SamplerStateBits_e
        unsigned char semantic;     // TextureSemantic
        MaterialTextureDefInfo u;
    };

    struct type_align(16) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        float literal[4];
    };

    enum GfxBlend : unsigned int
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
        GFXS_BLEND_MASK = 0xF,
    };

    enum GfxBlendOp : unsigned int
    {
        GFXS_BLENDOP_DISABLED = 0x0,
        GFXS_BLENDOP_ADD = 0x1,
        GFXS_BLENDOP_SUBTRACT = 0x2,
        GFXS_BLENDOP_REVSUBTRACT = 0x3,
        GFXS_BLENDOP_MIN = 0x4,
        GFXS_BLENDOP_MAX = 0x5,
        GFXS_BLENDOP_MASK = 0x7,
    };

    enum GfxStencilFunc : unsigned int
    {
        GFXS_STENCILFUNC_NEVER = 0x0,
        GFXS_STENCILFUNC_LESS = 0x1,
        GFXS_STENCILFUNC_EQUAL = 0x2,
        GFXS_STENCILFUNC_LESSEQUAL = 0x3,
        GFXS_STENCILFUNC_GREATER = 0x4,
        GFXS_STENCILFUNC_NOTEQUAL = 0x5,
        GFXS_STENCILFUNC_GREATEREQUAL = 0x6,
        GFXS_STENCILFUNC_ALWAYS = 0x7,

        GFXS_STENCILFUNC_COUNT,
        GFXS_STENCILFUNC_MASK = 0x7
    };

    enum GfxStencilOp : unsigned int
    {
        GFXS_STENCILOP_KEEP = 0x0,
        GFXS_STENCILOP_ZERO = 0x1,
        GFXS_STENCILOP_REPLACE = 0x2,
        GFXS_STENCILOP_INCRSAT = 0x3,
        GFXS_STENCILOP_DECRSAT = 0x4,
        GFXS_STENCILOP_INVERT = 0x5,
        GFXS_STENCILOP_INCR = 0x6,
        GFXS_STENCILOP_DECR = 0x7,

        GFXS_STENCILOP_COUNT,
        GFXS_STENCILOP_MASK = 0x7
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

        GFXS0_GAMMAWRITE = 0x40000000,
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
        GFXS1_STENCIL_FRONT_PASS_MASK = 0x700,

        GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
        GFXS1_STENCIL_FRONT_FAIL_MASK = 0x3800,

        GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
        GFXS1_STENCIL_FRONT_ZFAIL_MASK = 0x1C000,

        GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
        GFXS1_STENCIL_FRONT_FUNC_MASK = 0xE0000,
        GFXS1_STENCIL_FRONT_MASK = 0xFFF00,

        GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
        GFXS1_STENCIL_BACK_PASS_MASK = 0x700000,

        GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
        GFXS1_STENCIL_BACK_FAIL_MASK = 0x3800000,

        GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
        GFXS1_STENCIL_BACK_ZFAIL_MASK = 0x1C000000,

        GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
        GFXS1_STENCIL_BACK_FUNC_MASK = 0xE0000000,
        GFXS1_STENCIL_BACK_MASK = 0xFFF00000,

        GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
        GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
    };

    struct GfxStateBits
    {
        unsigned int loadBits[2];
    };

    struct infoParm_t
    {
        const char* name;
        unsigned int clearSolid;
        unsigned int surfaceFlags;
        unsigned int contents;
        unsigned int toolFlags;
    };

    struct gcc_align(8) GfxDrawSurfFields
    {
        uint64_t objectId : 16;
        uint64_t reflectionProbeIndex : 8;
        uint64_t hasGfxEntIndex : 1;
        uint64_t customIndex : 5;
        uint64_t materialSortedIndex : 12;
        uint64_t prepass : 2;
        uint64_t useHeroLighting : 1;
        uint64_t sceneLightIndex : 8;
        uint64_t surfType : 4;
        uint64_t primarySortKey : 6;
        uint64_t unused : 1;
    };

    union GfxDrawSurf
    {
        gcc_align(8) GfxDrawSurfFields fields;
        gcc_align(8) uint64_t packed;
    };

    // The sort key is translated to a numeric value inside the material templates
    // when converting the material.
    // TODO: Try to find out which value is which sort key by investigating on materials
    enum SortKey_e
    {
        SORTKEY_OPAQUE_AMBIENT = 0,
        SORTKEY_OPAQUE = 1,
        SORTKEY_SKY = 2,
        SORTKEY_SKYBOX = 3,
        // ? = 4, // some kind of dynamic decal?
        // ? = 5, // another kind of dynamic decal?

        SORTKEY_TRANS_START = 6,

        SORTKEY_DECAL_BOTTOM_1 = 6,       // prob decal - bottom 1
        SORTKEY_DECAL_BOTTOM_2 = 7,       // prob decal - bottom 2
        SORTKEY_DECAL_BOTTOM_3 = 8,       // prob decal - bottom 3
        SORTKEY_DECAL_STATIC_DECAL = 9,   // prob decal - static decal
        SORTKEY_DECAL_MIDDLE_1 = 10,      // prob decal - middle 1
        SORTKEY_DECAL_MIDDLE_2 = 11,      // prob decal - middle 2
        SORTKEY_DECAL_MIDDLE_3 = 12,      // prob decal - middle 3
        SORTKEY_DECAL_WEAPON_IMPACT = 13, // prob decal - weapon impact
        // ? = 14, // prob decal - top 1
        // some decal = 15, // prob decal - top 2 or decal - top 3
        // some decal = 16,
        // prob decal = 17,
        // ? = 18,
        // water related = 19, // maybe underwater
        // water related = 20, // maybe transparent water
        // ? = 21,
        // - = 22,
        // - = 23,
        SORTKEY_WINDOW_INSIDE = 24,  // prob window inside
        SORTKEY_WINDOW_OUTSIDE = 25, // prob window outside
        // ? = 26, // includes motiontracker3d, impact fx, atmos
        // ? = 27,
        // ? = 28,
        // ? = 29,
        // ? = 30,
        // ? = 31,
        // - = 32,
        // ? = 33,
        // some ui/2d/fullscreen sort = 34,
        // - = 35,
        // - = 36,
        // - = 37,
        // - = 38,
        // - = 39,
        // - = 40,
        // framecolordebug = 41, // multiplicative?
        // - = 42,
        SORTKEY_DISTORTION = 43,
        // ? = 44,
        // ? = 45,
        // - = 46,
        SORTKEY_BLEND_ADDITIVE = 47,   // most likely blend / additive
        SORTKEY_EFFECT_AUTO_SORT = 48, // most likely effect - auto sort
        SORTKEY_AFTER_EFFECTS_BOTTOM = 49,
        SORTKEY_AFTER_EFFECTS_MIDDLE = 50, // prob after effects - middle
        SORTKEY_AFTER_EFFECTS_TOP = 51,    // prob after effects - top
        // - = 52,
        SORTKEY_VIEWMODEL_EFFECT = 53, // maybe viewmodel effect

        SORTKEY_MAX
    };

    // Names unknown
    // There's probably: MTL_GAMEFLAG_CASTS_SHADOW (part of iw3/t5; 0x40 there)
    enum MaterialGameFlags
    {
        MTL_GAMEFLAG_1 = 0x1,
        MTL_GAMEFLAG_2 = 0x2,
        MTL_GAMEFLAG_4 = 0x4,
        MTL_GAMEFLAG_8 = 0x8,
        MTL_GAMEFLAG_10 = 0x10,
        MTL_GAMEFLAG_20 = 0x20,
        MTL_GAMEFLAG_40 = 0x40,
        MTL_GAMEFLAG_80 = 0x80,
    };

    struct MaterialInfo
    {
        const char* name;
        unsigned char gameFlags;
        unsigned char sortKey; // SortKey_e
        unsigned char textureAtlasRowCount;
        unsigned char textureAtlasColumnCount;
        GfxDrawSurf drawSurf; // seems to be zeroed when loading
        unsigned int surfaceTypeBits;
        uint16_t hashIndex;
    };

    enum GfxCameraRegionType
    {
        CAMERA_REGION_LIT_OPAQUE = 0x0,
        CAMERA_REGION_LIT_TRANS = 0x1,
        CAMERA_REGION_EMISSIVE = 0x2,
        CAMERA_REGION_DEPTH_HACK = 0x3,

        CAMERA_REGION_COUNT,
        CAMERA_REGION_NONE = CAMERA_REGION_COUNT,
    };

    enum MaterialStateFlags
    {
        STATE_FLAG_CULL_BACK = 0x1,
        STATE_FLAG_AMBIENT = 0x2,
        STATE_FLAG_DECAL = 0x4,
        STATE_FLAG_WRITES_DEPTH = 0x8,
        STATE_FLAG_USES_DEPTH_BUFFER = 0x10,
        STATE_FLAG_USES_STENCIL_BUFFER = 0x20,
        STATE_FLAG_CULL_BACK_SHADOW = 0x40,
    };

    struct Material
    {
        MaterialInfo info;
        unsigned char stateBitsEntry[48];
        unsigned char textureCount;
        unsigned char constantCount;
        unsigned char stateBitsCount;
        unsigned char stateFlags;
        unsigned char cameraRegion; // GfxCameraRegionType
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
    };

    struct type_align(4) GfxImageLoadDef
    {
        char levelCount;
        char pad[3];
        int flags;
        int format;
        int resourceSize;
        char data[1];
    };

    union GfxTexture
    {
        // IDirect3DBaseTexture9* basemap;
        // IDirect3DTexture9* map;
        // IDirect3DVolumeTexture9* volmap;
        // IDirect3DCubeTexture9* cubemap;
        Texture* texture;
        GfxImageLoadDef* loadDef;
    };

    struct Picmip
    {
        char platform[2];
    };

    struct CardMemory
    {
        int platform[2];
    };

    enum TextureSemantic
    {
        TS_2D = 0x0,
        TS_FUNCTION = 0x1,
        TS_COLOR_MAP = 0x2,
        TS_DETAIL_MAP = 0x3,
        TS_UNUSED_2 = 0x4,
        TS_NORMAL_MAP = 0x5,
        TS_UNUSED_3 = 0x6,
        TS_UNUSED_4 = 0x7,
        TS_SPECULAR_MAP = 0x8,
        TS_UNUSED_5 = 0x9,
        TS_UNUSED_6 = 0xA,
        TS_WATER_MAP = 0xB,
    };

    struct GfxImage
    {
        GfxTexture texture;
        unsigned char mapType;
        unsigned char semantic;
        unsigned char category;
        bool useSrgbReads;
        Picmip picmip;
        bool noPicmip;
        char track;
        CardMemory cardMemory;
        uint16_t width;
        uint16_t height;
        uint16_t depth;
        bool delayLoadPixels;
        const char* name;
    };

    struct GfxPixelShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    struct GfxVertexShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    enum MaterialStreamStreamSource_e
    {
        STREAM_SRC_POSITION = 0x0,
        STREAM_SRC_COLOR = 0x1,
        STREAM_SRC_TEXCOORD_0 = 0x2,
        STREAM_SRC_NORMAL = 0x3,
        STREAM_SRC_TANGENT = 0x4,
        STREAM_SRC_OPTIONAL_BEGIN = 0x5,
        STREAM_SRC_PRE_OPTIONAL_BEGIN = 0x4,
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
        STREAM_DST_DEPTH = 0x4,
        STREAM_DST_TEXCOORD_0 = 0x5,
        STREAM_DST_TEXCOORD_1 = 0x6,
        STREAM_DST_TEXCOORD_2 = 0x7,
        STREAM_DST_TEXCOORD_3 = 0x8,
        STREAM_DST_TEXCOORD_4 = 0x9,
        STREAM_DST_TEXCOORD_5 = 0xA,
        STREAM_DST_TEXCOORD_6 = 0xB,
        STREAM_DST_TEXCOORD_7 = 0xC,

        STREAM_DST_COUNT
    };

    struct MaterialStreamRouting
    {
        unsigned char source;
        unsigned char dest;
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[13];
        void /*IDirect3DVertexDeclaration9*/* decl[16];
    };

    struct MaterialVertexDeclaration
    {
        const char* name;
        unsigned char streamCount;
        bool hasOptionalSource;
        MaterialVertexStreamRouting routing;
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

    enum MaterialTextureSource
    {
        TEXTURE_SRC_CODE_BLACK = 0x0,
        TEXTURE_SRC_CODE_WHITE = 0x1,
        TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
        TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
        TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
        TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
        TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x6,
        TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x7,
        TEXTURE_SRC_CODE_FEEDBACK = 0x8,
        TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0x9,
        TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xA,
        TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xB,
        TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xC,
        TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0xD,
        TEXTURE_SRC_CODE_OUTDOOR = 0xE,
        TEXTURE_SRC_CODE_FLOATZ = 0xF,
        TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x10,
        TEXTURE_SRC_CODE_RAW_FLOATZ = 0x11,
        TEXTURE_SRC_CODE_HALF_PARTICLES = 0x12,
        TEXTURE_SRC_CODE_HALF_PARTICLES_Z = 0x13,
        TEXTURE_SRC_CODE_CASE_TEXTURE = 0x14,
        TEXTURE_SRC_CODE_CINEMATIC_Y = 0x15,
        TEXTURE_SRC_CODE_CINEMATIC_CR = 0x16,
        TEXTURE_SRC_CODE_CINEMATIC_CB = 0x17,
        TEXTURE_SRC_CODE_CINEMATIC_A = 0x18,
        TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x19,
        TEXTURE_SRC_CODE_ALTERNATE_SCENE = 0x1A,

        TEXTURE_SRC_CODE_COUNT
    };

    enum CustomSamplers
    {
        CUSTOM_SAMPLER_REFLECTION_PROBE = 0x0,
        CUSTOM_SAMPLER_LIGHTMAP_PRIMARY = 0x1,
        CUSTOM_SAMPLER_LIGHTMAP_SECONDARY = 0x2,

        CUSTOM_SAMPLER_COUNT
    };

    struct CodeSamplerSource
    {
        const char* name;
        MaterialTextureSource source;
        CodeSamplerSource* subtable;
        int arrayCount;
        int arrayStride;
    };

    enum MaterialType
    {
        MTL_TYPE_DEFAULT = 0x0,
        MTL_TYPE_MODEL = 0x1,              // m_
        MTL_TYPE_MODEL_VERTCOL = 0x2,      // mc_
        MTL_TYPE_MODEL_VERTCOL_GREY = 0x3, // mg_
        MTL_TYPE_WORLD = 0x4,              // w_
        MTL_TYPE_WORLD_VERTCOL = 0x5,      // wc_

        MTL_TYPE_COUNT,
    };

    struct MaterialTypeInfo
    {
        const char* materialPrefix;
        const char* techniqueSetPrefix;
    };

    enum MaterialConstantSource
    {
        CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,

        CONST_SRC_CODE_LIGHT_POSITION = 0x0,
        CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
        CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
        CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
        CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
        CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0x5,
        CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x6,
        CONST_SRC_CODE_GAMETIME = 0x7,

        CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x8,

        CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x8,

        CONST_SRC_CODE_PIXEL_COST_FRACS = 0x8,
        CONST_SRC_CODE_PIXEL_COST_DECODE = 0x9,
        CONST_SRC_CODE_FILTER_TAP_0 = 0xA,
        CONST_SRC_CODE_FILTER_TAP_1 = 0xB,
        CONST_SRC_CODE_FILTER_TAP_2 = 0xC,
        CONST_SRC_CODE_FILTER_TAP_3 = 0xD,
        CONST_SRC_CODE_FILTER_TAP_4 = 0xE,
        CONST_SRC_CODE_FILTER_TAP_5 = 0xF,
        CONST_SRC_CODE_FILTER_TAP_6 = 0x10,
        CONST_SRC_CODE_FILTER_TAP_7 = 0x11,
        CONST_SRC_CODE_COLOR_MATRIX_R = 0x12,
        CONST_SRC_CODE_COLOR_MATRIX_G = 0x13,
        CONST_SRC_CODE_COLOR_MATRIX_B = 0x14,
        CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x15,
        CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x16,

        CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x17,

        CONST_SRC_CODE_FIXED_PS_BEGIN = 0x17,

        CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x17,
        CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x18,
        CONST_SRC_CODE_DOF_LERP_SCALE = 0x19,
        CONST_SRC_CODE_DOF_LERP_BIAS = 0x1A,
        CONST_SRC_CODE_DOF_ROW_DELTA = 0x1B,
        CONST_SRC_CODE_MOTION_MATRIX_X = 0x1C,
        CONST_SRC_CODE_MOTION_MATRIX_Y = 0x1D,
        CONST_SRC_CODE_MOTION_MATRIX_W = 0x1E,
        CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x1F,
        CONST_SRC_CODE_SHADOWMAP_SCALE = 0x20,
        CONST_SRC_CODE_ZNEAR = 0x21,
        CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x22,
        CONST_SRC_CODE_DEBUG_BUMPMAP = 0x23,
        CONST_SRC_CODE_MATERIAL_COLOR = 0x24,
        CONST_SRC_CODE_FOG = 0x25,
        CONST_SRC_CODE_FOG_COLOR_LINEAR = 0x26,
        CONST_SRC_CODE_FOG_COLOR_GAMMA = 0x27,
        CONST_SRC_CODE_FOG_SUN_CONSTS = 0x28,
        CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR = 0x29,
        CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA = 0x2A,
        CONST_SRC_CODE_FOG_SUN_DIR = 0x2B,
        CONST_SRC_CODE_GLOW_SETUP = 0x2C,
        CONST_SRC_CODE_GLOW_APPLY = 0x2D,
        CONST_SRC_CODE_COLOR_BIAS = 0x2E,
        CONST_SRC_CODE_COLOR_TINT_BASE = 0x2F,
        CONST_SRC_CODE_COLOR_TINT_DELTA = 0x30,
        CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA = 0x31,
        CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x32,
        CONST_SRC_CODE_ENVMAP_PARMS = 0x33,
        CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST = 0x34,
        CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x35,
        CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x36,
        CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x37,
        CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x38,
        CONST_SRC_CODE_FRAMEBUFFER_READ = 0x39,

        CONST_SRC_CODE_FIXED_PS_END = 0x3A,

        CONST_SRC_CODE_NON_PS_BEGIN = 0x3A,

        CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x3A,
        CONST_SRC_CODE_LIGHT_PROBE_AMBIENT = 0x3B,
        CONST_SRC_CODE_NEARPLANE_ORG = 0x3C,
        CONST_SRC_CODE_NEARPLANE_DX = 0x3D,
        CONST_SRC_CODE_NEARPLANE_DY = 0x3E,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x3F,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x40,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 = 0x41,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 = 0x42,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 = 0x43,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 = 0x44,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 = 0x45,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 = 0x46,
        CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0 = 0x47,
        CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1 = 0x48,
        CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x49,
        CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x4A,
        CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x4B,

        CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x4B,
        CONST_SRC_CODE_NON_PS_END = 0x4C,
        CONST_SRC_CODE_COUNT_FLOAT4 = 0x4C,

        CONST_SRC_FIRST_CODE_MATRIX = 0x4C,

        CONST_SRC_CODE_VIEW_MATRIX = 0x4C,
        CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x4D,
        CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x4E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x4F,
        CONST_SRC_CODE_PROJECTION_MATRIX = 0x50,
        CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x51,
        CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x52,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x53,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x54,
        CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x55,
        CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x56,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x57,
        CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x58,
        CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x59,
        CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5B,
        CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5C,
        CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5D,
        CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5F,
        CONST_SRC_CODE_WORLD_MATRIX0 = 0x60,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 = 0x61,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 = 0x62,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 = 0x63,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX0 = 0x64,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 = 0x65,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x66,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x67,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x68,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x69,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6B,
        CONST_SRC_CODE_WORLD_MATRIX1 = 0x6C,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 = 0x6D,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 = 0x6E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 = 0x6F,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX1 = 0x70,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 = 0x71,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x72,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x73,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x74,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x75,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x76,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x77,
        CONST_SRC_CODE_WORLD_MATRIX2 = 0x78,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 = 0x79,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 = 0x7A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 = 0x7B,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX2 = 0x7C,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 = 0x7D,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7F,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x80,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x81,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x82,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x83,

        CONST_SRC_TOTAL_COUNT,
        CONST_SRC_NONE
    };

    struct CodeConstantSource
    {
        const char* name;
        MaterialConstantSource source;
        CodeConstantSource* subtable;
        int arrayCount;
        int arrayStride;
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
        MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,  // stable
        MTL_ARG_LITERAL_VERTEX_CONST = 0x1,   // stable
        MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x2, // stable

        MTL_ARG_CODE_PRIM_BEGIN = 0x3,

        MTL_ARG_CODE_VERTEX_CONST = 0x3,  // stable object prim
        MTL_ARG_CODE_PIXEL_SAMPLER = 0x4, // stable object
        MTL_ARG_CODE_PIXEL_CONST = 0x5,   // stable

        MTL_ARG_CODE_PRIM_END = 0x6,

        MTL_ARG_MATERIAL_PIXEL_CONST = 0x6, // stable
        MTL_ARG_LITERAL_PIXEL_CONST = 0x7,  // stable

        MTL_ARG_COUNT
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
        // Guesses purely based on data analysis:
        TECHNIQUE_FLAG_1 = 0x1,   // uses resolvedPostSun code sampler // MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN
        TECHNIQUE_FLAG_2 = 0x2,   // uses resolvedScene code sampler MTL_TECHFLAG_NEEDS_RESOLVED_SCENE
        TECHNIQUE_FLAG_4 = 0x4,   // zprepass only
        TECHNIQUE_FLAG_8 = 0x8,   // build_floatz only
        TECHNIQUE_FLAG_10 = 0x10, // build_shadowmap_depth + build_shadowmap_model only
        TECHNIQUE_FLAG_20 =
            0x20, // techniques with _i_ in its name (all use texcoord[1] in decl -> other optional stream sources are not used at all so might be any optional)
        TECHNIQUE_FLAG_40 = 0x40,   // uses code constant light.spotDir or light.spotFactors
        TECHNIQUE_FLAG_80 = 0x80,   // uses floatZ sampler and does not have 0x100 flag
        TECHNIQUE_FLAG_100 = 0x100, // distortion_scale_zfeather_dtex + distortion_scale_ua_zfeather + distortion_scale_zfeather
        TECHNIQUE_FLAG_200 = 0x200, // ?
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
        TECHNIQUE_EMISSIVE_DFOG = 0x6,
        TECHNIQUE_EMISSIVE_SHADOW = 0x7,
        TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x8,

        TECHNIQUE_LIT_BEGIN = 0x9,

        TECHNIQUE_LIT = 0x9,
        TECHNIQUE_LIT_DFOG = 0xA,
        TECHNIQUE_LIT_SUN = 0xB,
        TECHNIQUE_LIT_SUN_DFOG = 0xC,
        TECHNIQUE_LIT_SUN_SHADOW = 0xD,
        TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xE,
        TECHNIQUE_LIT_SPOT = 0xF,
        TECHNIQUE_LIT_SPOT_DFOG = 0x10,
        TECHNIQUE_LIT_SPOT_SHADOW = 0x11,
        TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x12,
        TECHNIQUE_LIT_OMNI = 0x13,
        TECHNIQUE_LIT_OMNI_DFOG = 0x14,
        TECHNIQUE_LIT_OMNI_SHADOW = 0x15,
        TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x16,
        TECHNIQUE_LIT_INSTANCED = 0x17,
        TECHNIQUE_LIT_INSTANCED_DFOG = 0x18,
        TECHNIQUE_LIT_INSTANCED_SUN = 0x19,
        TECHNIQUE_LIT_INSTANCED_SUN_DFOG = 0x1A,
        TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x1B,
        TECHNIQUE_LIT_INSTANCED_SUN_SHADOW_DFOG = 0x1C,
        TECHNIQUE_LIT_INSTANCED_SPOT = 0x1D,
        TECHNIQUE_LIT_INSTANCED_SPOT_DFOG = 0x1E,
        TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x1F,
        TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW_DFOG = 0x20,
        TECHNIQUE_LIT_INSTANCED_OMNI = 0x21,
        TECHNIQUE_LIT_INSTANCED_OMNI_DFOG = 0x22,
        TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x23,
        TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW_DFOG = 0x24,

        TECHNIQUE_LIT_END = 0x25,

        TECHNIQUE_LIGHT_SPOT = 0x25,
        TECHNIQUE_LIGHT_OMNI = 0x26,
        TECHNIQUE_LIGHT_SPOT_SHADOW = 0x27,
        TECHNIQUE_FAKELIGHT_NORMAL = 0x28,
        TECHNIQUE_FAKELIGHT_VIEW = 0x29,
        TECHNIQUE_SUNLIGHT_PREVIEW = 0x2A,
        TECHNIQUE_CASE_TEXTURE = 0x2B,
        TECHNIQUE_WIREFRAME_SOLID = 0x2C,
        TECHNIQUE_WIREFRAME_SHADED = 0x2D,
        TECHNIQUE_DEBUG_BUMPMAP = 0x2E,
        TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x2F,

        TECHNIQUE_COUNT
    };

    struct MaterialTechniqueSet
    {
        const char* name;
        unsigned char worldVertFormat;
        bool hasBeenUploaded;
        unsigned char unused[1];
        MaterialTechniqueSet* remappedTechniqueSet;
        MaterialTechnique* techniques[48];
    };

    union RawFileBuffer
    {
        const char* compressedBuffer;
        const char* buffer;
    };

    struct RawFile
    {
        const char* name;
        int compressedLen;
        int len;
        RawFileBuffer data;
    };

    struct StringTableCell
    {
        const char* string;
        int hash;
    };

    struct StringTable
    {
        const char* name;
        int columnCount;
        int rowCount;
        StringTableCell* values;
    };

    struct AILSOUNDINFO
    {
        int format;
        const void* data_ptr;
        unsigned int data_len;
        unsigned int rate;
        int bits;
        int channels;
        unsigned int samples;
        unsigned int block_size;
        const void* initial_ptr;
    };

    struct MssSound
    {
        AILSOUNDINFO info;
        char* data;
    };

    struct LoadedSound
    {
        const char* name;
        MssSound sound;
    };

    struct StreamedSound
    {
        const char* dir;
        const char* name;
    };

    union SoundFileRef
    {
        LoadedSound* loadSnd;
        StreamedSound streamSnd;
    };

    enum snd_alias_type_t : char
    {
        SAT_UNKNOWN = 0x0,
        SAT_LOADED = 0x1,
        SAT_STREAMED = 0x2,
        SAT_VOICED = 0x3,
        SAT_COUNT,
    };

    struct SoundFile
    {
        snd_alias_type_t type;
        char exists;
        SoundFileRef u;
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

    struct snd_alias_t
    {
        const char* aliasName;
        const char* subtitle;
        const char* secondaryAliasName;
        const char* chainAliasName;
        const char* mixerGroup;
        SoundFile* soundFile;
        int sequence;
        float volMin;
        float volMax;
        float pitchMin;
        float pitchMax;
        float distMin;
        float distMax;
        float velocityMin;
        int flags;

        union
        {
            float slavePercentage;
            float masterPercentage;
        };

        float probability;
        float lfePercentage;
        float centerPercentage;
        int startDelay;
        SndCurve* volumeFalloffCurve;
        float envelopMin;
        float envelopMax;
        float envelopPercentage;
        SpeakerMap* speakerMap;
    };

    struct snd_alias_list_t
    {
        const char* aliasName;
        snd_alias_t* head;
        int count;
    };

    struct SndCurve
    {
        const char* filename;
        uint16_t knotCount;
        float knots[16][2];
    };

    struct LocalizeEntry
    {
        const char* value;
        const char* name;
    };

    struct StringList
    {
        int totalStrings;
        const char** strings;
    };

    union DvarLimits
    {
        struct
        {
            int stringCount;
            const char** strings;
        } enumeration;

        struct
        {
            int min;
            int max;
        } integer;

        struct
        {
            float min;
            float max;
        } value;

        struct
        {
            float min;
            float max;
        } vector;
    };

    union DvarValue
    {
        bool enabled;
        int integer;
        unsigned int unsignedInt;
        float value;
        float vector[4];
        const char* string;
        char color[4];
    };

    struct dvar_t
    {
        const char* name;
        const char* description;
        unsigned int flags;
        char type;
        bool modified;
        DvarValue current;
        DvarValue latched;
        DvarValue reset;
        DvarLimits domain;
        // bool (__cdecl* domainFunc)(dvar_t*, DvarValue);
        void* domainFunc;
        dvar_t* hashNext;
    };

    struct StaticDvar
    {
        dvar_t* dvar;
        char* dvarName;
    };

    struct StaticDvarList
    {
        int numStaticDvars;
        StaticDvar** staticDvars;
    };

    struct ExpressionString
    {
        const char* string;
    };

    struct Statement_s;

    union operandInternalDataUnion
    {
        int intVal;
        float floatVal;
        ExpressionString stringVal;
        Statement_s* function;
    };

    enum expDataType
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
        VAL_FUNCTION = 0x3,

        NUM_DATATYPES,
    };

    struct Operand
    {
        expDataType dataType;
        operandInternalDataUnion internals;
    };

    enum expressionOperatorType_e
    {
        OP_NOOP = 0x0,
        OP_RIGHTPAREN = 0x1,
        OP_MULTIPLY = 0x2,
        OP_DIVIDE = 0x3,
        OP_MODULUS = 0x4,
        OP_ADD = 0x5,
        OP_SUBTRACT = 0x6,
        OP_NOT = 0x7,
        OP_LESSTHAN = 0x8,
        OP_LESSTHANEQUALTO = 0x9,
        OP_GREATERTHAN = 0xA,
        OP_GREATERTHANEQUALTO = 0xB,
        OP_EQUALS = 0xC,
        OP_NOTEQUAL = 0xD,
        OP_AND = 0xE,
        OP_OR = 0xF,
        OP_LEFTPAREN = 0x10,
        OP_COMMA = 0x11,
        OP_BITWISEAND = 0x12,
        OP_BITWISEOR = 0x13,
        OP_BITWISENOT = 0x14,
        OP_BITSHIFTLEFT = 0x15,
        OP_BITSHIFTRIGHT = 0x16,

        OP_COUNT
    };

    enum expressionFunction_e
    {
        EXP_FUNC_STATIC_DVAR_INT = OP_COUNT,
        EXP_FUNC_STATIC_DVAR_BOOL,
        EXP_FUNC_STATIC_DVAR_FLOAT,
        EXP_FUNC_STATIC_DVAR_STRING,

        EXP_FUNC_DYN_START,

        EXP_FUNC_INT = EXP_FUNC_DYN_START,
        EXP_FUNC_STRING,
        EXP_FUNC_FLOAT,
        EXP_FUNC_SIN,
        EXP_FUNC_COS,
        EXP_FUNC_MIN,
        EXP_FUNC_MAX,
        EXP_FUNC_MILLISECONDS,
        EXP_FUNC_DVAR_INT,
        EXP_FUNC_DVAR_BOOL,
        EXP_FUNC_DVAR_FLOAT,
        EXP_FUNC_DVAR_STRING
    };

    union entryInternalData
    {
        int op;
        Operand operand;
    };

    enum expressionEntryType : int
    {
        EET_OPERATOR = 0x0,
        EET_OPERAND = 0x1,
    };

    struct expressionEntry
    {
        expressionEntryType type;
        entryInternalData data;
    };

    struct ExpressionSupportingData;

    struct Statement_s
    {
        int numEntries;
        expressionEntry* entries;
        ExpressionSupportingData* supportingData;
        int lastExecuteTime;
        Operand lastResult;
    };

    struct UIFunctionList
    {
        int totalFunctions;
        Statement_s** functions;
    };

    struct ExpressionSupportingData
    {
        UIFunctionList uifunctions;
        StaticDvarList staticDvarList;
        StringList uiStrings;
    };

    struct menuTransition
    {
        int transitionType;
        int targetField;
        int startTime;
        float startVal;
        float endVal;
        float time;
        int endTriggerType;
    };

    enum ItemFloatExpressionTarget
    {
        ITEM_FLOATEXP_TGT_RECT_X = 0x0,
        ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
        ITEM_FLOATEXP_TGT_RECT_W = 0x2,
        ITEM_FLOATEXP_TGT_RECT_H = 0x3,
        ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
        ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
        ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
        ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
        ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
        ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
        ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
        ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
        ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
        ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
        ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
        ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
        ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
        ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
        ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,

        ITEM_FLOATEXP_TGT_COUNT
    };

    struct ItemExpressionTargetBinding
    {
        int target;
        const char* name;
        const char* componentName;
    };

    struct ItemFloatExpression
    {
        int target;
        Statement_s* expression;
    };

    struct columnInfo_s
    {
        int pos;
        int width;
        int maxChars;
        int alignment;
    };

    struct MenuEventHandlerSet;

    struct ConditionalScript
    {
        MenuEventHandlerSet* eventHandlerSet;
        Statement_s* eventExpression;
    };

    struct SetLocalVarData
    {
        const char* localVarName;
        Statement_s* expression;
    };

    union EventData
    {
        const char* unconditionalScript;
        ConditionalScript* conditionalScript;
        MenuEventHandlerSet* elseScript;
        SetLocalVarData* setLocalVarData;
    };

    enum EventType : unsigned char
    {
        EVENT_UNCONDITIONAL = 0x0,
        EVENT_IF = 0x1,
        EVENT_ELSE = 0x2,
        EVENT_SET_LOCAL_VAR_BOOL = 0x3,
        EVENT_SET_LOCAL_VAR_INT = 0x4,
        EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
        EVENT_SET_LOCAL_VAR_STRING = 0x6,

        EVENT_COUNT,
    };

    struct MenuEventHandler
    {
        EventData eventData;
        EventType eventType;
    };

    struct MenuEventHandlerSet
    {
        int eventHandlerCount;
        MenuEventHandler** eventHandlers;
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
        MenuEventHandlerSet* onDoubleClick;
        int notselectable;
        int noScrollBars;
        int usePaging;
        float selectBorder[4];
        Material* selectIcon;
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

    struct newsTickerDef_s
    {
        int feedId;
        int speed;
        int spacing;
        int lastTime;
        int start;
        int end;
        float x;
    };

    struct textScrollDef_s
    {
        int startTime;
    };

    union itemDefData_t
    {
        listBoxDef_s* listBox;
        editFieldDef_s* editField;
        multiDef_s* multi;
        const char* enumDvarName;
        newsTickerDef_s* ticker;
        textScrollDef_s* scroll;
        void* data;
    };

    struct ItemKeyHandler
    {
        int key;
        MenuEventHandlerSet* action;
        ItemKeyHandler* next;
    };

    struct rectDef_s
    {
        float x;
        float y;
        float w;
        float h;
        unsigned char horzAlign;
        unsigned char vertAlign;
    };

    enum WindowDefStaticFlag : unsigned int
    {
        WINDOW_FLAG_DECORATION = 0x100000,
        WINDOW_FLAG_HORIZONTAL_SCROLL = 0x200000,
        WINDOW_FLAG_OUT_OF_BOUNDS_CLICK = 0x2000000,
        WINDOW_FLAG_SCREEN_SPACE = 0x400000,
        WINDOW_FLAG_AUTO_WRAPPED = 0x800000,
        WINDOW_FLAG_POPUP = 0x1000000,
        WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE = 0x4000000,
        WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG = 0x10000000,
        WINDOW_FLAG_HIDDEN_DURING_SCOPE = 0x20000000,
        WINDOW_FLAG_HIDDEN_DURING_UI = 0x40000000,
        WINDOW_FLAG_TEXT_ONLY_FOCUS = 0x80000000,
    };

    enum WindowDefDynamicFlag : unsigned int
    {
        WINDOW_FLAG_HOVERED = 0x1, // guessed
        WINDOW_FLAG_FOCUSED = 0x2,
        WINDOW_FLAG_VISIBLE = 0x4,
        WINDOW_FLAG_FADING_OUT = 0x10,
        WINDOW_FLAG_FADING_IN = 0x20,
        WINDOW_FLAG_80 = 0x80,
        WINDOW_FLAG_NON_DEFAULT_BACKCOLOR = 0x8000,
        WINDOW_FLAG_NON_DEFAULT_FORECOLOR = 0x10000
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
        unsigned int dynamicFlags[1];
        int nextTime;
        float foreColor[4];
        float backColor[4];
        float borderColor[4];
        float outlineColor[4];
        float disableColor[4];
        Material* background;
    };

    enum ItemDefFlag : unsigned int
    {
        ITEM_FLAG_SAVE_GAME_INFO = 0x1,
        ITEM_FLAG_CINEMATIC_SUBTITLE = 0x2,
    };

    enum ItemDefDvarFlag
    {
        ITEM_DVAR_FLAG_ENABLE = 0x1,
        ITEM_DVAR_FLAG_DISABLE = 0x2,
        ITEM_DVAR_FLAG_SHOW = 0x4,
        ITEM_DVAR_FLAG_HIDE = 0x8,
        ITEM_DVAR_FLAG_FOCUS = 0x10,
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
        ITEM_TYPE_GAME_MESSAGE_WINDOW = 0x13,
        ITEM_TYPE_NEWS_TICKER = 0x14,
        ITEM_TYPE_TEXT_SCROLL = 0x15,
        ITEM_TYPE_EMAILFIELD = 0x16,
        ITEM_TYPE_PASSWORDFIELD = 0x17
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
        MenuEventHandlerSet* mouseEnterText;
        MenuEventHandlerSet* mouseExitText;
        MenuEventHandlerSet* mouseEnter;
        MenuEventHandlerSet* mouseExit;
        MenuEventHandlerSet* action;
        MenuEventHandlerSet* accept;
        MenuEventHandlerSet* onFocus;
        MenuEventHandlerSet* leaveFocus;
        const char* dvar;
        const char* dvarTest;
        ItemKeyHandler* onKey;
        const char* enableDvar;
        const char* localVar;
        int dvarFlags;
        snd_alias_list_t* focusSound;
        float special;
        int cursorPos[1];
        itemDefData_t typeData;
        int imageTrack;
        int floatExpressionCount;
        ItemFloatExpression* floatExpressions;
        Statement_s* visibleExp;
        Statement_s* disabledExp;
        Statement_s* textExp;
        Statement_s* materialExp;
        float glowColor[4];
        bool decayActive;
        int fxBirthTime;
        int fxLetterTime;
        int fxDecayStartTime;
        int fxDecayDuration;
        int lastSoundPlayedTime;
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
        MenuEventHandlerSet* onOpen;
        MenuEventHandlerSet* onCloseRequest;
        MenuEventHandlerSet* onClose;
        MenuEventHandlerSet* onESC;
        ItemKeyHandler* onKey;
        Statement_s* visibleExp;
        const char* allowedBinding;
        const char* soundName;
        int imageTrack;
        float focusColor[4];
        Statement_s* rectXExp;
        Statement_s* rectYExp;
        Statement_s* rectWExp;
        Statement_s* rectHExp;
        Statement_s* openSoundExp;
        Statement_s* closeSoundExp;
        itemDef_s** items;
        menuTransition scaleTransition[1];
        menuTransition alphaTransition[1];
        menuTransition xTransition[1];
        menuTransition yTransition[1];
        ExpressionSupportingData* expressionData;
    };

    struct MenuList
    {
        const char* name;
        int menuCount;
        menuDef_t** menus;
    };

    enum FxElemType
    {
        FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
        FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
        FX_ELEM_TYPE_TAIL = 0x2,
        FX_ELEM_TYPE_TRAIL = 0x3,
        FX_ELEM_TYPE_CLOUD = 0x4,
        FX_ELEM_TYPE_SPARK_CLOUD = 0x5,
        FX_ELEM_TYPE_SPARK_FOUNTAIN = 0x6,
        FX_ELEM_TYPE_MODEL = 0x7,
        FX_ELEM_TYPE_OMNI_LIGHT = 0x8,
        FX_ELEM_TYPE_SPOT_LIGHT = 0x9,
        FX_ELEM_TYPE_SOUND = 0xA,
        FX_ELEM_TYPE_DECAL = 0xB,
        FX_ELEM_TYPE_RUNNER = 0xC,
        FX_ELEM_TYPE_COUNT = 0xD,
        FX_ELEM_TYPE_LAST_SPRITE = 0x3,
        FX_ELEM_TYPE_LAST_DRAWN = 0x9,
    };

    struct FxIntRange
    {
        int base;
        int amplitude;
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

    struct FxFloatRange
    {
        float base;
        float amplitude;
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

    union FxEffectDefRef
    {
        FxEffectDef* handle;
        const char* name;
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
        float invSplitDist;
        float invSplitArcDist;
        float invSplitTime;
        int vertCount;
        FxTrailVertex* verts;
        int indCount;
        uint16_t* inds;
    };

    struct FxSparkFountainDef
    {
        float gravity;
        float bounceFrac;
        float bounceRand;
        float sparkSpacing;
        float sparkLength;
        int sparkCount;
        float loopTime;
        float velMin;
        float velMax;
        float velConeFrac;
        float restSpeed;
        float boostTime;
        float boostFactor;
    };

    union FxElemExtendedDefPtr
    {
        FxTrailDef* trailDef;
        FxSparkFountainDef* sparkFountainDef;
        char* unknownDef;
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
        char elemType;
        char visualCount;
        char velIntervalCount;
        char visStateIntervalCount;
        FxElemVelStateSample* velSamples;
        FxElemVisStateSample* visSamples;
        FxElemDefVisuals visuals;
        Bounds collBounds;
        FxEffectDefRef effectOnImpact;
        FxEffectDefRef effectOnDeath;
        FxEffectDefRef effectEmitted;
        FxFloatRange emitDist;
        FxFloatRange emitDistVariance;
        FxElemExtendedDefPtr extended;
        char sortOrder;
        char lightingFrac;
        char useItemClip;
        char fadeInfo;
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
        FxElemDef* elemDefs;
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

    struct Glyph
    {
        uint16_t letter;
        char x0;
        char y0;
        char dx;
        char pixelWidth;
        char pixelHeight;
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

    enum LbColType
    {
        LBCOL_TYPE_NUMBER = 0x0,
        LBCOL_TYPE_TIME = 0x1,
        LBCOL_TYPE_LEVELXP = 0x2,
        LBCOL_TYPE_PRESTIGE = 0x3,
        LBCOL_TYPE_BIGNUMBER = 0x4,
        LBCOL_TYPE_PERCENT = 0x5,

        LBCOL_TYPE_COUNT
    };

    enum LbAggType
    {
        LBAGG_TYPE_MIN = 0x0,
        LBAGG_TYPE_MAX = 0x1,
        LBAGG_TYPE_SUM = 0x2,
        LBAGG_TYPE_LAST = 0x3,

        LBAGG_TYPE_COUNT
    };

    struct LbColumnDef
    {
        const char* name;
        int id;
        int propertyId;
        bool hidden;
        const char* statName;
        LbColType type;
        int precision;
        LbAggType agg;
    };

    struct LeaderboardDef
    {
        const char* name;
        int id;
        int columnCount;
        int xpColId;
        int prestigeColId;
        LbColumnDef* columns;
    };

    struct TracerDef
    {
        const char* name;
        Material* material;
        unsigned int drawInterval;
        float speed;
        float beamLength;
        float beamWidth;
        float screwRadius;
        float screwDist;
        float colors[5][4];
    };

    enum StructuredDataTypeCategory
    {
        DATA_INT = 0x0,
        DATA_BYTE = 0x1,
        DATA_BOOL = 0x2,
        DATA_STRING = 0x3,
        DATA_ENUM = 0x4,
        DATA_STRUCT = 0x5,
        DATA_INDEXED_ARRAY = 0x6,
        DATA_ENUM_ARRAY = 0x7,
        DATA_FLOAT = 0x8,
        DATA_SHORT = 0x9,

        DATA_COUNT
    };

    struct StructuredDataEnumEntry
    {
        const char* string;
        uint16_t index;
    };

    struct StructuredDataEnum
    {
        int entryCount;
        int reservedEntryCount;
        StructuredDataEnumEntry* entries;
    };

    union StructuredDataTypeUnion
    {
        unsigned int stringDataLength;
        int enumIndex;
        int structIndex;
        int indexedArrayIndex;
        int enumedArrayIndex;
    };

    struct StructuredDataType
    {
        StructuredDataTypeCategory type;
        StructuredDataTypeUnion u;
    };

    struct StructuredDataStructProperty
    {
        const char* name;
        StructuredDataType type;
        unsigned int offset;
    };

    struct StructuredDataStruct
    {
        int propertyCount;
        StructuredDataStructProperty* properties;
        int size;
        unsigned int bitOffset;
    };

    struct StructuredDataIndexedArray
    {
        int arraySize;
        StructuredDataType elementType;
        unsigned int elementSize;
    };

    struct StructuredDataEnumedArray
    {
        int enumIndex;
        StructuredDataType elementType;
        unsigned int elementSize;
    };

    struct StructuredDataDef
    {
        int version;
        unsigned int formatChecksum;
        int enumCount;
        StructuredDataEnum* enums;
        int structCount;
        StructuredDataStruct* structs;
        int indexedArrayCount;
        StructuredDataIndexedArray* indexedArrays;
        int enumedArrayCount;
        StructuredDataEnumedArray* enumedArrays;
        StructuredDataType rootType;
        unsigned int size;
    };

    struct StructuredDataDefSet
    {
        const char* name;
        unsigned int defCount;
        StructuredDataDef* defs;
    };

    struct TriggerModel
    {
        int contents;
        uint16_t hullCount;
        uint16_t firstHull;
    };

    struct TriggerHull
    {
        Bounds bounds;
        int contents;
        uint16_t slabCount;
        uint16_t firstSlab;
    };

    struct TriggerSlab
    {
        float dir[3];
        float midPoint;
        float halfSize;
    };

    struct MapTriggers
    {
        unsigned int count;
        TriggerModel* models;
        unsigned int hullCount;
        TriggerHull* hulls;
        unsigned int slabCount;
        TriggerSlab* slabs;
    };

    struct Stage
    {
        const char* name;
        float origin[3];
        uint16_t triggerIndex;
        char sunPrimaryLightIndex;
    };

    struct MapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
        Stage* stages;
        char stageCount;
    };

    struct AddonMapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
    };

    typedef unsigned short LeafBrush;

    struct cStaticModel_s
    {
        XModel* xmodel;
        float origin[3];
        float invScaledAxis[3][3];
        Bounds absBounds;
    };

    struct ClipMaterial
    {
        const char* name;
        int surfaceFlags;
        int contents;
    };

    struct cNode_t
    {
        cplane_s* plane;
        int16_t children[2];
    };

    struct cLeafBrushNodeLeaf_t
    {
        LeafBrush* brushes;
    };

    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        uint16_t childOffset[2];
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
        char triCount;
        char borderCount;
        char firstVertSegment;
        int firstTri;
        CollisionBorder* borders;
    };

    union CollisionAabbTreeIndex
    {
        int firstChildIndex;
        int partitionIndex;
    };

    struct type_align(16) CollisionAabbTree
    {
        float midPoint[3];
        uint16_t materialIndex;
        uint16_t childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    struct cLeaf_t
    {
        uint16_t firstCollAabbIndex;
        uint16_t collAabbCount;
        int brushContents;
        int terrainContents;
        Bounds bounds;
        int leafBrushNode;
    };

    struct cmodel_t
    {
        Bounds bounds;
        float radius;
        cLeaf_t leaf;
    };

    struct cbrush_t
    {
        uint16_t numsides;
        uint16_t glassPieceIndex;
        cbrushside_t* sides;
        char* baseAdjacentSide;
        int16_t axialMaterialNum[2][3];
        char firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
    };

    struct SModelAabbNode
    {
        Bounds bounds;
        uint16_t firstChild;
        uint16_t childCount;
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
        PhysPreset* physPreset;
        int health;
        PhysMass mass;
        int contents;
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
    };

    struct DynEntityColl
    {
        uint16_t sector;
        uint16_t nextEntInSector;
        float linkMins[2];
        float linkMaxs[2];
    };

    typedef tdef_align(128) cbrush_t cbrush_array_t;
    typedef tdef_align(128) Bounds BoundsArray;

    struct clipMap_t
    {
        const char* name;
        int isInUse;
        int planeCount;
        cplane_s* planes;
        unsigned int numStaticModels;
        cStaticModel_s* staticModelList;
        unsigned int numMaterials;
        ClipMaterial* materials;
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
        cbrush_array_t* brushes;
        BoundsArray* brushBounds;
        int* brushContents;
        MapEnts* mapEnts;
        uint16_t smodelNodeCount;
        SModelAabbNode* smodelNodes;
        uint16_t dynEntCount[2];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityColl* dynEntCollList[2];
        unsigned int checksum;
        char padding[48];
    };

    struct ComPrimaryLight
    {
        char type;
        char canUseShadowMap;
        char exponent;
        char unused;
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

    struct ComWorld
    {
        const char* name;
        int isInUse;
        unsigned int primaryLightCount;
        ComPrimaryLight* primaryLights;
    };

    struct pathnode_t;

    enum nodeType
    {
        NODE_ERROR = 0x0,
        NODE_PATHNODE = 0x1,
        NODE_COVER_STAND = 0x2,
        NODE_COVER_CROUCH = 0x3,
        NODE_COVER_CROUCH_WINDOW = 0x4,
        NODE_COVER_PRONE = 0x5,
        NODE_COVER_RIGHT = 0x6,
        NODE_COVER_LEFT = 0x7,
        NODE_AMBUSH = 0x8,
        NODE_EXPOSED = 0x9,
        NODE_CONCEALMENT_STAND = 0xA,
        NODE_CONCEALMENT_CROUCH = 0xB,
        NODE_CONCEALMENT_PRONE = 0xC,
        NODE_DOOR = 0xD,
        NODE_DOOR_INTERIOR = 0xE,
        NODE_SCRIPTED = 0xF,
        NODE_NEGOTIATION_BEGIN = 0x10,
        NODE_NEGOTIATION_END = 0x11,
        NODE_TURRET = 0x12,
        NODE_GUARD = 0x13,
        NODE_NUMTYPES = 0x14,
        NODE_DONTLINK = 0x14,
    };

    struct pathlink_s
    {
        float fDist;
        uint16_t nodeNum;
        char disconnectCount;
        char negotiationLink;
        char flags;
        char ubBadPlaceCount[3];
    };

    enum PathNodeErrorCode
    {
        PNERR_NONE = 0x0,
        PNERR_INSOLID = 0x1,
        PNERR_FLOATING = 0x2,
        PNERR_NOLINK = 0x3,
        PNERR_DUPLICATE = 0x4,
        PNERR_NOSTANCE = 0x5,
        PNERR_INVALIDDOOR = 0x6,
        PNERR_NOANGLES = 0x7,
        PNERR_BADPLACEMENT = 0x8,

        NUM_PATH_NODE_ERRORS
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

        union
        {
            float minUseDistSq;
            PathNodeErrorCode error;
        };

        int16_t wOverlapNode[2];
        uint16_t totalLinkCount;
        pathlink_s* Links;
    };

    struct pathnode_dynamic_t
    {
        void* pOwner;
        int iFreeTime;
        int iValidTime[3];
        int dangerousNodeTime[3];
        int inPlayerLOSTime;
        int16_t wLinkCount;
        int16_t wOverlapCount;
        int16_t turretEntNumber;
        char userCount;
        bool hasBadPlaceLink;
    };

    struct pathnode_transient_t
    {
        int iSearchFrame;
        pathnode_t* pNextOpen;
        pathnode_t* pPrevOpen;
        pathnode_t* pParent;
        float fCost;
        float fHeuristic;

        union
        {
            float nodeCost;
            int linkIndex;
        };
    };

    struct pathnode_t
    {
        pathnode_constant_t constant;
        pathnode_dynamic_t dynamic;
        pathnode_transient_t transient;
    };

    struct type_align(16) pathbasenode_t
    {
        float vOrigin[3];
        unsigned int type;
    };

    struct pathnode_tree_nodes_t
    {
        int nodeCount;
        uint16_t* nodes;
    };

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

    struct VehicleTrackObstacle
    {
        float origin[2];
        float radius;
    };

    struct VehicleTrackSector
    {
        float startEdgeDir[2];
        float startEdgeDist;
        float leftEdgeDir[2];
        float leftEdgeDist;
        float rightEdgeDir[2];
        float rightEdgeDist;
        float sectorLength;
        float sectorWidth;
        float totalPriorLength;
        float totalFollowingLength;
        VehicleTrackObstacle* obstacles;
        unsigned int obstacleCount;
    };

    struct VehicleTrackSegment
    {
        const char* targetName;
        VehicleTrackSector* sectors;
        unsigned int sectorCount;
        VehicleTrackSegment** nextBranches;
        unsigned int nextBranchesCount;
        VehicleTrackSegment** prevBranches;
        unsigned int prevBranchesCount;
        float endEdgeDir[2];
        float endEdgeDist;
        float totalLength;
    };

    struct VehicleTrack
    {
        VehicleTrackSegment* segments;
        unsigned int segmentCount;
    };

    struct G_GlassPiece
    {
        uint16_t damageTaken;
        uint16_t collapseTime;
        int lastStateChangeTime;
        char impactDir;
        char impactPos[2];
    };

    struct G_GlassName
    {
        char* nameStr;
        uint16_t name;
        uint16_t pieceCount;
        uint16_t* pieceIndices;
    };

    struct G_GlassData
    {
        G_GlassPiece* glassPieces;
        unsigned int pieceCount;
        uint16_t damageToWeaken;
        uint16_t damageToDestroy;
        unsigned int glassNameCount;
        G_GlassName* glassNames;
        char pad[108];
    };

    struct GameWorldSp
    {
        const char* name;
        PathData path;
        VehicleTrack vehicleTrack;
        G_GlassData* g_glassData;
    };

    struct GameWorldMp
    {
        const char* name;
        G_GlassData* g_glassData;
    };

    struct FxGlassDef
    {
        float halfThickness;
        float texVecs[2][2];
        GfxColor color;
        Material* material;
        Material* materialShattered;
        PhysPreset* physPreset;
    };

    struct FxSpatialFrame
    {
        float quat[4];
        float origin[3];
    };

    union FxGlassPiecePlace
    {
        struct
        {
            FxSpatialFrame frame;
            float radius;
        };

        unsigned int nextFree;
    };

    struct FxGlassPieceState
    {
        float texCoordOrigin[2];
        unsigned int supportMask;
        uint16_t initIndex;
        uint16_t geoDataStart;
        char defIndex;
        char pad[5];
        char vertCount;
        char holeDataCount;
        char crackDataCount;
        char fanDataCount;
        uint16_t flags;
        float areaX2;
    };

    struct FxGlassPieceDynamics
    {
        int fallTime;
        int physObjId;
        int physJointId;
        float vel[3];
        float avel[3];
    };

    struct FxGlassVertex
    {
        int16_t x;
        int16_t y;
    };

    struct FxGlassHoleHeader
    {
        uint16_t uniqueVertCount;
        char touchVert;
        char pad[1];
    };

    struct FxGlassCrackHeader
    {
        uint16_t uniqueVertCount;
        char beginVertIndex;
        char endVertIndex;
    };

    union type_align(4) FxGlassGeometryData
    {
        FxGlassVertex vert;
        FxGlassHoleHeader hole;
        FxGlassCrackHeader crack;
        char asBytes[4];
        int16_t anonymous[2];
    };

    struct FxGlassInitPieceState
    {
        FxSpatialFrame frame;
        float radius;
        float texCoordOrigin[2];
        unsigned int supportMask;
        float areaX2;
        char defIndex;
        char vertCount;
        char fanDataCount;
        char pad[1];
    };

    struct FxGlassSystem
    {
        int time;
        int prevTime;
        unsigned int defCount;
        unsigned int pieceLimit;
        unsigned int pieceWordCount;
        unsigned int initPieceCount;
        unsigned int cellCount;
        unsigned int activePieceCount;
        unsigned int firstFreePiece;
        unsigned int geoDataLimit;
        unsigned int geoDataCount;
        unsigned int initGeoDataCount;
        FxGlassDef* defs;
        FxGlassPiecePlace* piecePlaces;
        FxGlassPieceState* pieceStates;
        FxGlassPieceDynamics* pieceDynamics;
        FxGlassGeometryData* geoData;
        raw_uint* isInUse;
        raw_uint* cellBits;
        raw_byte16* visData;
        vec3_t* linkOrg;
        raw_float16* halfThickness;
        uint16_t* lightingHandles;
        FxGlassInitPieceState* initPieceStates;
        FxGlassGeometryData* initGeoData;
        bool needToCompactData;
        char initCount;
        float effectChanceAccum;
        int lastPieceDeletionTime;
    };

    struct FxWorld
    {
        const char* name;
        FxGlassSystem glassSys;
    };

    struct GfxSky
    {
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        char skySamplerState;
    };

    struct GfxWorldDpvsPlanes
    {
        int cellCount;
        cplane_s* planes;
        uint16_t* nodes;
        raw_uint* sceneEntCellBits;
    };

    struct GfxCellTreeCount
    {
        int aabbTreeCount;
    };

    typedef unsigned short StaticModelIndex;

    struct GfxAabbTree
    {
        Bounds bounds;
        uint16_t childCount;
        uint16_t surfaceCount;
        uint16_t startSurfIndex;
        uint16_t surfaceCountNoDecal;
        uint16_t startSurfIndexNoDecal;
        uint16_t smodelIndexCount;
        StaticModelIndex* smodelIndexes;
        int childrenOffset;
    };

    struct GfxCellTree
    {
        GfxAabbTree* aabbTree;
    };

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

    struct DpvsPlane
    {
        float coeffs[4];
    };

    struct GfxPortal
    {
        GfxPortalWritable writable;
        DpvsPlane plane;
        vec3_t* vertices;
        uint16_t cellIndex;
        char vertexCount;
        float hullAxis[2][3];
    };

    struct GfxCell
    {
        Bounds bounds;
        int portalCount;
        GfxPortal* portals;
        char reflectionProbeCount;
        char* reflectionProbes;
    };

    struct GfxReflectionProbe
    {
        float origin[3];
    };

    struct GfxLightmapArray
    {
        GfxImage* primary;
        GfxImage* secondary;
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

    struct GfxWorldDraw
    {
        unsigned int reflectionProbeCount;
        GfxImage** reflectionProbes;
        GfxReflectionProbe* reflectionProbeOrigins;
        GfxTexture* reflectionProbeTextures;
        int lightmapCount;
        GfxLightmapArray* lightmaps;
        GfxTexture* lightmapPrimaryTextures;
        GfxTexture* lightmapSecondaryTextures;
        GfxImage* lightmapOverridePrimary;
        GfxImage* lightmapOverrideSecondary;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        unsigned int indexCount;
        r_index_t* indices;
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
        unsigned int lastSunPrimaryLightIndex;
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
        Bounds bounds;
    };

    struct type_align(4) GfxBrushModel
    {
        GfxBrushModelWritable writable;
        Bounds bounds;
        float radius;
        uint16_t surfaceCount;
        uint16_t startSurfIndex;
        uint16_t surfaceCountNoDecal;
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
        char hasGfxEntIndex;
        char lod;
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

    struct GfxStaticModelDrawInst
    {
        GfxPackedPlacement placement;
        XModel* model;
        uint16_t cullDist;
        uint16_t lightingHandle;
        char reflectionProbeIndex;
        char primaryLightIndex;
        char flags;
        char firstMtlSkinIndex;
        GfxColor groundLighting;
        uint16_t cacheId[4];
    };

    struct GfxStaticModelInst
    {
        Bounds bounds;
        float lightingOrigin[3];
    };

    struct srfTriangles_t
    {
        unsigned int vertexLayerData;
        unsigned int firstVertex;
        uint16_t vertexCount;
        uint16_t triCount;
        unsigned int baseIndex;
    };

    struct GfxSurfaceLightingAndFlagsFields
    {
        char lightmapIndex;
        char reflectionProbeIndex;
        char primaryLightIndex;
        char flags;
    };

    union GfxSurfaceLightingAndFlags
    {
        GfxSurfaceLightingAndFlagsFields fields;
        unsigned int packed;
    };

    struct GfxSurface
    {
        srfTriangles_t tris;
        Material* material;
        GfxSurfaceLightingAndFlags laf;
    };

    struct GfxSurfaceBounds
    {
        Bounds bounds;
    };

    struct GfxWorldDpvsStatic
    {
        unsigned int smodelCount;
        unsigned int staticSurfaceCount;
        unsigned int staticSurfaceCountNoDecal;
        unsigned int litOpaqueSurfsBegin;
        unsigned int litOpaqueSurfsEnd;
        unsigned int litTransSurfsBegin;
        unsigned int litTransSurfsEnd;
        unsigned int shadowCasterSurfsBegin;
        unsigned int shadowCasterSurfsEnd;
        unsigned int emissiveSurfsBegin;
        unsigned int emissiveSurfsEnd;
        unsigned int smodelVisDataCount;
        unsigned int surfaceVisDataCount;
        char* smodelVisData[3];
        char* surfaceVisData[3];
        uint16_t* sortedSurfIndex;
        GfxStaticModelInst* smodelInsts;
        GfxSurface* surfaces;
        GfxSurfaceBounds* surfacesBounds;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxDrawSurf* surfaceMaterials;
        raw_uint128* surfaceCastsSunShadow;
        volatile int usageCount;
    };

    struct GfxWorldDpvsDynamic
    {
        unsigned int dynEntClientWordCount[2];
        unsigned int dynEntClientCount[2];
        raw_uint* dynEntCellBits[2];
        raw_byte16* dynEntVisData[2][3];
    };

    struct GfxHeroOnlyLight
    {
        char type;
        char unused[3];
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
    };

    typedef tdef_align(128) GfxCellTree GfxCellTree128;
    typedef tdef_align(4) GfxSceneDynModel GfxSceneDynModel4;

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        unsigned int surfaceCount;
        int skyCount;
        GfxSky* skies;
        unsigned int lastSunPrimaryLightIndex;
        unsigned int primaryLightCount;
        unsigned int sortKeyLitDecal;
        unsigned int sortKeyEffectDecal;
        unsigned int sortKeyEffectAuto;
        unsigned int sortKeyDistortion;
        GfxWorldDpvsPlanes dpvsPlanes;
        int /*GfxCellTreeCount*/* aabbTreeCounts;
        GfxCellTree128* aabbTrees;
        GfxCell* cells;
        GfxWorldDraw draw;
        GfxLightGrid lightGrid;
        int modelCount;
        GfxBrushModel* models;
        Bounds bounds;
        unsigned int checksum;
        int materialMemoryCount;
        MaterialMemory* materialMemory;
        sunflare_t sun;
        float outdoorLookupMatrix[4][4];
        GfxImage* outdoorImage;
        raw_uint* cellCasterBits;
        raw_uint* cellHasSunLitSurfsBits;
        GfxSceneDynModel4* sceneDynModel;
        GfxSceneDynBrush* sceneDynBrush;
        raw_uint* primaryLightEntityShadowVis;
        raw_uint* primaryLightDynEntShadowVis[2];
        char* nonSunPrimaryLightForModelDynEnt;
        GfxShadowGeometry* shadowGeom;
        GfxLightRegion* lightRegion;
        GfxWorldDpvsStatic dpvs;
        GfxWorldDpvsDynamic dpvsDyn;
        unsigned int mapVtxChecksum;
        unsigned int heroOnlyLightCount;
        GfxHeroOnlyLight* heroOnlyLights;
        char fogTypesAllowed;
    };

    enum ImpactType
    {
        IMPACT_TYPE_NONE = 0x0,
        IMPACT_TYPE_BULLET_SMALL = 0x1,
        IMPACT_TYPE_BULLET_LARGE = 0x2,
        IMPACT_TYPE_BULLET_AP = 0x3,
        IMPACT_TYPE_BULLET_EXPLODE = 0x4,
        IMPACT_TYPE_SHOTGUN = 0x5,
        IMPACT_TYPE_SHOTGUN_EXPLODE = 0x6,
        IMPACT_TYPE_GRENADE_BOUNCE = 0x7,
        IMPACT_TYPE_GRENADE_EXPLODE = 0x8,
        IMPACT_TYPE_ROCKET_EXPLODE = 0x9,
        IMPACT_TYPE_PROJECTILE_DUD = 0xA,

        IMPACT_TYPE_COUNT
    };

    enum weaponIconRatioType_t
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,

        WEAPON_ICON_RATIO_COUNT
    };

    enum weapType_t
    {
        WEAPTYPE_BULLET = 0x0,
        WEAPTYPE_GRENADE = 0x1,
        WEAPTYPE_PROJECTILE = 0x2,
        WEAPTYPE_RIOTSHIELD = 0x3,

        WEAPTYPE_NUM
    };

    enum weapClass_t
    {
        WEAPCLASS_RIFLE = 0x0,
        WEAPCLASS_SNIPER = 0x1,
        WEAPCLASS_MG = 0x2,
        WEAPCLASS_SMG = 0x3,
        WEAPCLASS_SPREAD = 0x4,
        WEAPCLASS_PISTOL = 0x5,
        WEAPCLASS_GRENADE = 0x6,
        WEAPCLASS_ROCKETLAUNCHER = 0x7,
        WEAPCLASS_TURRET = 0x8,
        WEAPCLASS_THROWINGKNIFE = 0x9,
        WEAPCLASS_NON_PLAYER = 0xA,
        WEAPCLASS_ITEM = 0xB,

        WEAPCLASS_NUM
    };

    enum PenetrateType
    {
        PENETRATE_TYPE_NONE = 0x0,
        PENETRATE_TYPE_SMALL = 0x1,
        PENETRATE_TYPE_MEDIUM = 0x2,
        PENETRATE_TYPE_LARGE = 0x3,

        PENETRATE_TYPE_COUNT
    };

    enum weapInventoryType_t
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,
        WEAPINVENTORY_EXCLUSIVE = 0x4,
        WEAPINVENTORY_SCAVENGER = 0x5,

        WEAPINVENTORYCOUNT
    };

    enum weapFireType_t
    {
        WEAPON_FIRETYPE_FULLAUTO = 0x0,
        WEAPON_FIRETYPE_SINGLESHOT = 0x1,
        WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
        WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
        WEAPON_FIRETYPE_DOUBLEBARREL = 0x5,
        WEAPON_FIRETYPECOUNT = 0x6,
        WEAPON_FIRETYPE_BURSTFIRE_FIRST = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE_LAST = 0x4,
    };

    enum OffhandClass
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,
        OFFHAND_CLASS_THROWINGKNIFE = 0x4,
        OFFHAND_CLASS_OTHER = 0x5,

        OFFHAND_CLASS_COUNT
    };

    enum weapStance_t
    {
        WEAPSTANCE_STAND = 0x0,
        WEAPSTANCE_DUCK = 0x1,
        WEAPSTANCE_PRONE = 0x2,

        WEAPSTANCE_NUM
    };

    enum activeReticleType_t
    {
        VEH_ACTIVE_RETICLE_NONE = 0x0,
        VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
        VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,

        VEH_ACTIVE_RETICLE_COUNT
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

        AMMO_COUNTER_CLIP_COUNT
    };

    enum weapOverlayReticle_t
    {
        WEAPOVERLAYRETICLE_NONE = 0x0,
        WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,

        WEAPOVERLAYRETICLE_NUM
    };

    enum WeapOverlayInterface_t
    {
        WEAPOVERLAYINTERFACE_NONE = 0x0,
        WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
        WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,

        WEAPOVERLAYINTERFACECOUNT
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

        WEAPPROJEXP_NUM
    };

    enum WeapStickinessType
    {
        WEAPSTICKINESS_NONE = 0x0,
        WEAPSTICKINESS_ALL = 0x1,
        WEAPSTICKINESS_ALL_ORIENT = 0x2,
        WEAPSTICKINESS_GROUND = 0x3,
        WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
        WEAPSTICKINESS_KNIFE = 0x5,

        WEAPSTICKINESS_COUNT
    };

    enum guidedMissileType_t
    {
        MISSILE_GUIDANCE_NONE = 0x0,
        MISSILE_GUIDANCE_SIDEWINDER = 0x1,
        MISSILE_GUIDANCE_HELLFIRE = 0x2,
        MISSILE_GUIDANCE_JAVELIN = 0x3,

        MISSILE_GUIDANCE_COUNT
    };

    enum weapAnimFiles_t
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
        WEAP_ANIM_BREACH_RAISE = 0xF,
        WEAP_ANIM_DROP = 0x10,
        WEAP_ANIM_ALT_RAISE = 0x11,
        WEAP_ANIM_ALT_DROP = 0x12,
        WEAP_ANIM_QUICK_RAISE = 0x13,
        WEAP_ANIM_QUICK_DROP = 0x14,
        WEAP_ANIM_EMPTY_RAISE = 0x15,
        WEAP_ANIM_EMPTY_DROP = 0x16,
        WEAP_ANIM_SPRINT_IN = 0x17,
        WEAP_ANIM_SPRINT_LOOP = 0x18,
        WEAP_ANIM_SPRINT_OUT = 0x19,
        WEAP_ANIM_STUNNED_START = 0x1A,
        WEAP_ANIM_STUNNED_LOOP = 0x1B,
        WEAP_ANIM_STUNNED_END = 0x1C,
        WEAP_ANIM_DETONATE = 0x1D,
        WEAP_ANIM_NIGHTVISION_WEAR = 0x1E,
        WEAP_ANIM_NIGHTVISION_REMOVE = 0x1F,
        WEAP_ANIM_ADS_FIRE = 0x20,
        WEAP_ANIM_ADS_LASTSHOT = 0x21,
        WEAP_ANIM_ADS_RECHAMBER = 0x22,
        WEAP_ANIM_ADS_UP = 0x23,
        WEAP_ANIM_ADS_DOWN = 0x24,

        NUM_WEAP_ANIMS,
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
        SURF_TYPE_RIOT_SHIELD,
        SURF_TYPE_SLUSH,

        SURF_TYPE_NUM
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
        HITLOC_SHIELD = 0x13,

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

    struct WeaponDef
    {
        const char* szOverlayName;
        XModel** gunXModel;
        XModel* handXModel;
        const char** szXAnimsRightHanded;
        const char** szXAnimsLeftHanded;
        const char* szModeName;
        uint16_t* notetrackSoundMapKeys;
        uint16_t* notetrackSoundMapValues;
        uint16_t* notetrackRumbleMapKeys;
        uint16_t* notetrackRumbleMapValues;
        int playerAnimType;
        weapType_t weapType;
        weapClass_t weapClass;
        PenetrateType penetrateType;
        weapInventoryType_t inventoryType;
        weapFireType_t fireType;
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
        SndAliasCustom fireSoundPlayerAkimbo;
        SndAliasCustom fireLoopSound;
        SndAliasCustom fireLoopSoundPlayer;
        SndAliasCustom fireStopSound;
        SndAliasCustom fireStopSoundPlayer;
        SndAliasCustom fireLastSound;
        SndAliasCustom fireLastSoundPlayer;
        SndAliasCustom emptyFireSound;
        SndAliasCustom emptyFireSoundPlayer;
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
        SndAliasCustom scanSound;
        SndAliasCustom* bounceSound;
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
        float strafeMove[3];
        float strafeRot[3];
        float vDuckedOfs[3];
        float vDuckedMove[3];
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
        XModel** worldModel;
        XModel* worldClipModel;
        XModel* rocketModel;
        XModel* knifeModel;
        XModel* worldKnifeModel;
        Material* hudIcon;
        weaponIconRatioType_t hudIconRatio;
        Material* pickupIcon;
        weaponIconRatioType_t pickupIconRatio;
        Material* ammoCounterIcon;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
        int iStartAmmo;
        const char* szAmmoName;
        int iAmmoIndex;
        const char* szClipName;
        int iClipIndex;
        int iMaxAmmo;
        int shotCount;
        const char* szSharedAmmoCapName;
        int iSharedAmmoCapIndex;
        int iSharedAmmoCap;
        int damage;
        int playerDamage;
        int iMeleeDamage;
        int iDamageType;
        int iFireDelay;
        int iMeleeDelay;
        int meleeChargeDelay;
        int iDetonateDelay;
        int iRechamberTime;
        int rechamberTimeOneHanded;
        int iRechamberBoltTime;
        int iHoldFireTime;
        int iDetonateTime;
        int iMeleeTime;
        int meleeChargeTime;
        int iReloadTime;
        int reloadShowRocketTime;
        int iReloadEmptyTime;
        int iReloadAddTime;
        int iReloadStartTime;
        int iReloadStartAddTime;
        int iReloadEndTime;
        int iDropTime;
        int iRaiseTime;
        int iAltDropTime;
        int quickDropTime;
        int quickRaiseTime;
        int iBreachRaiseTime;
        int iEmptyRaiseTime;
        int iEmptyDropTime;
        int sprintInTime;
        int sprintLoopTime;
        int sprintOutTime;
        int stunnedTimeBegin;
        int stunnedTimeLoop;
        int stunnedTimeEnd;
        int nightVisionWearTime;
        int nightVisionWearTimeFadeOutEnd;
        int nightVisionWearTimePowerUp;
        int nightVisionRemoveTime;
        int nightVisionRemoveTimePowerDown;
        int nightVisionRemoveTimeFadeInStart;
        int fuseTime;
        int aiFuseTime;
        float autoAimRange;
        float aimAssistRange;
        float aimAssistRangeAds;
        float aimPadding;
        float enemyCrosshairRange;
        float moveSpeedScale;
        float adsMoveSpeedScale;
        float sprintDurationScale;
        float fAdsZoomInFrac;
        float fAdsZoomOutFrac;
        Material* overlayMaterial;
        Material* overlayMaterialLowRes;
        Material* overlayMaterialEMP;
        Material* overlayMaterialEMPLowRes;
        weapOverlayReticle_t overlayReticle;
        WeapOverlayInterface_t overlayInterface;
        float overlayWidth;
        float overlayHeight;
        float overlayWidthSplitscreen;
        float overlayHeightSplitscreen;
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
        float adsViewErrorMin;
        float adsViewErrorMax;
        PhysCollmap* physCollmap;
        float dualWieldViewModelOffset;
        weaponIconRatioType_t killIconRatio;
        int iReloadAmmoAdd;
        int iReloadStartAdd;
        int ammoDropStockMin;
        int ammoDropClipPercentMin;
        int ammoDropClipPercentMax;
        int iExplosionRadius;
        int iExplosionRadiusMin;
        int iExplosionInnerDamage;
        int iExplosionOuterDamage;
        float damageConeAngle;
        float bulletExplDmgMult;
        float bulletExplRadiusMult;
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
        FxEffectDef* projDudEffect;
        SndAliasCustom projExplosionSound;
        SndAliasCustom projDudSound;
        WeapStickinessType stickiness;
        float lowAmmoWarningThreshold;
        float ricochetChance;
        float* parallelBounce;
        float* perpendicularBounce;
        FxEffectDef* projTrailEffect;
        FxEffectDef* projBeaconEffect;
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
        float fHipViewScatterMin;
        float fHipViewScatterMax;
        float fightDist;
        float maxDist;
        const char* aiVsAiAccuracyGraphName;
        const char* aiVsPlayerAccuracyGraphName;
        vec2_t* originalAiVsAiAccuracyGraphKnots;
        vec2_t* originalAiVsPlayerAccuracyGraphKnots;
        uint16_t originalAiVsAiAccuracyGraphKnotCount;
        uint16_t originalAiVsPlayerAccuracyGraphKnotCount;
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
        float scanSpeed;
        float scanAccel;
        int scanPauseTime;
        const char* szScript;
        float fOOPosAnimLength[2];
        int minDamage;
        int minPlayerDamage;
        float fMaxDamageRange;
        float fMinDamageRange;
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        float* locationDamageMultipliers;
        const char* fireRumble;
        const char* meleeImpactRumble;
        TracerDef* tracerType;
        float turretScopeZoomRate;
        float turretScopeZoomMin;
        float turretScopeZoomMax;
        float turretOverheatUpRate;
        float turretOverheatDownRate;
        float turretOverheatPenalty;
        SndAliasCustom turretOverheatSound;
        FxEffectDef* turretOverheatEffect;
        const char* turretBarrelSpinRumble;
        float turretBarrelSpinSpeed;
        float turretBarrelSpinUpTime;
        float turretBarrelSpinDownTime;
        SndAliasCustom turretBarrelSpinMaxSnd;
        SndAliasCustom turretBarrelSpinUpSnd[4];
        SndAliasCustom turretBarrelSpinDownSnd[4];
        SndAliasCustom missileConeSoundAlias;
        SndAliasCustom missileConeSoundAliasAtBase;
        float missileConeSoundRadiusAtTop;
        float missileConeSoundRadiusAtBase;
        float missileConeSoundHeight;
        float missileConeSoundOriginOffset;
        float missileConeSoundVolumescaleAtCore;
        float missileConeSoundVolumescaleAtEdge;
        float missileConeSoundVolumescaleCoreSize;
        float missileConeSoundPitchAtTop;
        float missileConeSoundPitchAtBottom;
        float missileConeSoundPitchTopSize;
        float missileConeSoundPitchBottomSize;
        float missileConeSoundCrossfadeTopSize;
        float missileConeSoundCrossfadeBottomSize;
        bool sharedAmmo;
        bool lockonSupported;
        bool requireLockonToFire;
        bool bigExplosion;
        bool noAdsWhenMagEmpty;
        bool avoidDropCleanup;
        bool inheritsPerks;
        bool crosshairColorChange;
        bool bRifleBullet;
        bool armorPiercing;
        bool bBoltAction;
        bool aimDownSight;
        bool bRechamberWhileAds;
        bool bBulletExplosiveDamage;
        bool bCookOffHold;
        bool bClipOnly;
        bool noAmmoPickup;
        bool adsFireOnly;
        bool cancelAutoHolsterWhenEmpty;
        bool disableSwitchToWhenEmpty;
        bool suppressAmmoReserveDisplay;
        bool laserSightDuringNightvision;
        bool markableViewmodel;
        bool noDualWield;
        bool flipKillIcon;
        bool bNoPartialReload;
        bool bSegmentedReload;
        bool blocksProne;
        bool silenced;
        bool isRollingGrenade;
        bool projExplosionEffectForceNormalUp;
        bool bProjImpactExplode;
        bool stickToPlayers;
        bool hasDetonator;
        bool disableFiring;
        bool timedDetonation;
        bool rotate;
        bool holdButtonToThrow;
        bool freezeMovementWhenFiring;
        bool thermalScope;
        bool altModeSameWeapon;
        bool turretBarrelSpinEnabled;
        bool missileConeSoundEnabled;
        bool missileConeSoundPitchshiftEnabled;
        bool missileConeSoundCrossfadeEnabled;
        bool offhandHoldIsCancelable;
    };

    struct WeaponCompleteDef
    {
        const char* szInternalName;
        WeaponDef* weapDef;
        const char* szDisplayName;
        uint16_t* hideTags;
        const char** szXAnims;
        float fAdsZoomFov;
        int iAdsTransInTime;
        int iAdsTransOutTime;
        int iClipSize;
        ImpactType impactType;
        int iFireTime;
        weaponIconRatioType_t dpadIconRatio;
        float penetrateMultiplier;
        float fAdsViewKickCenterSpeed;
        float fHipViewKickCenterSpeed;
        const char* szAltWeaponName;
        unsigned int altWeaponIndex;
        int iAltRaiseTime;
        Material* killIcon;
        Material* dpadIcon;
        int fireAnimLength;
        int iFirstRaiseTime;
        int ammoDropStockMax;
        float adsDofStart;
        float adsDofEnd;
        uint16_t aiVsAiAccuracyGraphKnotCount;
        uint16_t aiVsPlayerAccuracyGraphKnotCount;
        vec2_t* aiVsAiAccuracyGraphKnots;
        vec2_t* aiVsPlayerAccuracyGraphKnots;
        bool motionTracker;
        bool enhanced;
        bool dpadIconShowsAmmo;
    };

    struct WeaponFullDef
    {
        WeaponCompleteDef weapCompleteDef;
        WeaponDef weapDef;
        uint16_t hideTags[32];
        const char* szXAnims[37];
        XModel* gunXModel[16];
        const char* szXAnimsRightHanded[37];
        const char* szXAnimsLeftHanded[37];
        uint16_t notetrackSoundMapKeys[16];
        uint16_t notetrackSoundMapValues[16];
        uint16_t notetrackRumbleMapKeys[16];
        uint16_t notetrackRumbleMapValues[16];
        XModel* worldModel[16];
        float parallelBounce[31];
        float perpendicularBounce[31];
        float locationDamageMultipliers[20];
    };

    enum VehicleAxleType
    {
        VEH_AXLE_FRONT = 0x0,
        VEH_AXLE_REAR = 0x1,
        VEH_AXLE_ALL = 0x2,
        VEH_AXLE_COUNT = 0x3,
    };

    struct VehiclePhysDef
    {
        int physicsEnabled;
        const char* physPresetName;
        PhysPreset* physPreset;
        const char* accelGraphName;
        VehicleAxleType steeringAxle;
        VehicleAxleType powerAxle;
        VehicleAxleType brakingAxle;
        float topSpeed;
        float reverseSpeed;
        float maxVelocity;
        float maxPitch;
        float maxRoll;
        float suspensionTravelFront;
        float suspensionTravelRear;
        float suspensionStrengthFront;
        float suspensionDampingFront;
        float suspensionStrengthRear;
        float suspensionDampingRear;
        float frictionBraking;
        float frictionCoasting;
        float frictionTopSpeed;
        float frictionSide;
        float frictionSideRear;
        float velocityDependentSlip;
        float rollStability;
        float rollResistance;
        float pitchResistance;
        float yawResistance;
        float uprightStrengthPitch;
        float uprightStrengthRoll;
        float targetAirPitch;
        float airYawTorque;
        float airPitchTorque;
        float minimumMomentumForCollision;
        float collisionLaunchForceScale;
        float wreckedMassScale;
        float wreckedBodyFriction;
        float minimumJoltForNotify;
        float slipThresholdFront;
        float slipThresholdRear;
        float slipFricScaleFront;
        float slipFricScaleRear;
        float slipFricRateFront;
        float slipFricRateRear;
        float slipYawTorque;
    };

    enum VehicleType
    {
        VEH_WHEELS_4 = 0x0,
        VEH_TANK = 0x1,
        VEH_PLANE = 0x2,
        VEH_BOAT = 0x3,
        VEH_ARTILLERY = 0x4,
        VEH_HELICOPTER = 0x5,
        VEH_SNOWMOBILE = 0x6,
        VEH_TYPE_COUNT = 0x7,
    };

    struct VehicleDef
    {
        const char* name;
        VehicleType type;
        const char* useHintString;
        int health;
        int quadBarrel;
        float texScrollScale;
        float topSpeed;
        float accel;
        float rotRate;
        float rotAccel;
        float maxBodyPitch;
        float maxBodyRoll;
        float fakeBodyAccelPitch;
        float fakeBodyAccelRoll;
        float fakeBodyVelPitch;
        float fakeBodyVelRoll;
        float fakeBodySideVelPitch;
        float fakeBodyPitchStrength;
        float fakeBodyRollStrength;
        float fakeBodyPitchDampening;
        float fakeBodyRollDampening;
        float fakeBodyBoatRockingAmplitude;
        float fakeBodyBoatRockingPeriod;
        float fakeBodyBoatRockingRotationPeriod;
        float fakeBodyBoatRockingFadeoutSpeed;
        float boatBouncingMinForce;
        float boatBouncingMaxForce;
        float boatBouncingRate;
        float boatBouncingFadeinSpeed;
        float boatBouncingFadeoutSteeringAngle;
        float collisionDamage;
        float collisionSpeed;
        float killcamOffset[3];
        int playerProtected;
        int bulletDamage;
        int armorPiercingDamage;
        int grenadeDamage;
        int projectileDamage;
        int projectileSplashDamage;
        int heavyExplosiveDamage;
        VehiclePhysDef vehPhysDef;
        float boostDuration;
        float boostRechargeTime;
        float boostAcceleration;
        float suspensionTravel;
        float maxSteeringAngle;
        float steeringLerp;
        float minSteeringScale;
        float minSteeringSpeed;
        int camLookEnabled;
        float camLerp;
        float camPitchInfluence;
        float camRollInfluence;
        float camFovIncrease;
        float camFovOffset;
        float camFovSpeed;
        const char* turretWeaponName;
        WeaponCompleteDef* turretWeapon;
        float turretHorizSpanLeft;
        float turretHorizSpanRight;
        float turretVertSpanUp;
        float turretVertSpanDown;
        float turretRotRate;
        SndAliasCustom turretSpinSnd;
        SndAliasCustom turretStopSnd;
        int trophyEnabled;
        float trophyRadius;
        float trophyInactiveRadius;
        int trophyAmmoCount;
        float trophyReloadTime;
        uint16_t trophyTags[4];
        Material* compassFriendlyIcon;
        Material* compassEnemyIcon;
        int compassIconWidth;
        int compassIconHeight;
        SndAliasCustom idleLowSnd;
        SndAliasCustom idleHighSnd;
        SndAliasCustom engineLowSnd;
        SndAliasCustom engineHighSnd;
        float engineSndSpeed;
        SndAliasCustom engineStartUpSnd;
        int engineStartUpLength;
        SndAliasCustom engineShutdownSnd;
        SndAliasCustom engineIdleSnd;
        SndAliasCustom engineSustainSnd;
        SndAliasCustom engineRampUpSnd;
        int engineRampUpLength;
        SndAliasCustom engineRampDownSnd;
        int engineRampDownLength;
        SndAliasCustom suspensionSoftSnd;
        float suspensionSoftCompression;
        SndAliasCustom suspensionHardSnd;
        float suspensionHardCompression;
        SndAliasCustom collisionSnd;
        float collisionBlendSpeed;
        SndAliasCustom speedSnd;
        float speedSndBlendSpeed;
        const char* surfaceSndPrefix;
        SndAliasCustom surfaceSnds[31];
        float surfaceSndBlendSpeed;
        float slideVolume;
        float slideBlendSpeed;
        float inAirPitch;
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
