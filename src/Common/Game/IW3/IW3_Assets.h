#pragma once

#ifndef __IW3_ASSETS_H
#define __IW3_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace IW3
{
#endif
    enum XAssetType
    {
        ASSET_TYPE_XMODELPIECES = 0x0,
        ASSET_TYPE_PHYSPRESET = 0x1,
        ASSET_TYPE_XANIMPARTS = 0x2,
        ASSET_TYPE_XMODEL = 0x3,
        ASSET_TYPE_MATERIAL = 0x4,
        ASSET_TYPE_TECHNIQUE_SET = 0x5,
        ASSET_TYPE_IMAGE = 0x6,
        ASSET_TYPE_SOUND = 0x7,
        ASSET_TYPE_SOUND_CURVE = 0x8,
        ASSET_TYPE_LOADED_SOUND = 0x9,
        ASSET_TYPE_CLIPMAP = 0xA,
        ASSET_TYPE_CLIPMAP_PVS = 0xB,
        ASSET_TYPE_COMWORLD = 0xC,
        ASSET_TYPE_GAMEWORLD_SP = 0xD,
        ASSET_TYPE_GAMEWORLD_MP = 0xE,
        ASSET_TYPE_MAP_ENTS = 0xF,
        ASSET_TYPE_GFXWORLD = 0x10,
        ASSET_TYPE_LIGHT_DEF = 0x11,
        ASSET_TYPE_UI_MAP = 0x12,
        ASSET_TYPE_FONT = 0x13,
        ASSET_TYPE_MENULIST = 0x14,
        ASSET_TYPE_MENU = 0x15,
        ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
        ASSET_TYPE_WEAPON = 0x17,
        ASSET_TYPE_SNDDRIVER_GLOBALS = 0x18,
        ASSET_TYPE_FX = 0x19,
        ASSET_TYPE_IMPACT_FX = 0x1A,
        ASSET_TYPE_AITYPE = 0x1B,
        ASSET_TYPE_MPTYPE = 0x1C,
        ASSET_TYPE_CHARACTER = 0x1D,
        ASSET_TYPE_XMODELALIAS = 0x1E,
        ASSET_TYPE_RAWFILE = 0x1F,
        ASSET_TYPE_STRINGTABLE = 0x20,
        ASSET_TYPE_COUNT,

        ASSET_TYPE_STRING = ASSET_TYPE_COUNT,
        ASSET_TYPE_ASSETLIST,

        ASSET_TYPE_FULLCOUNT
    };

    enum XFileBlock
    {
        XFILE_BLOCK_TEMP,
        XFILE_BLOCK_RUNTIME,
        XFILE_BLOCK_LARGE_RUNTIME,
        XFILE_BLOCK_PHYSICAL_RUNTIME,
        XFILE_BLOCK_VIRTUAL,
        XFILE_BLOCK_LARGE,
        XFILE_BLOCK_PHYSICAL,
        XFILE_BLOCK_VERTEX,
        XFILE_BLOCK_INDEX,

        MAX_XFILE_COUNT
    };

    struct XModelPieces;
    struct PhysPreset;
    struct XAnimParts;
    struct XModel;
    struct Material;
    struct MaterialPixelShader;
    struct MaterialVertexShader;
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

    union XAssetHeader
    {
        // XModelPieces *xmodelPieces; // NOT AN ASSET
        PhysPreset* physPreset;
        XAnimParts* parts;
        XModel* model;
        Material* material;
        MaterialPixelShader* pixelShader;
        MaterialVertexShader* vertexShader;
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
        void* data;
    };

    typedef char cbrushedge_t;
    typedef float vec2_t[2];
    typedef float vec3_t[3];
    typedef float vec4_t[4];
    typedef tdef_align(128) unsigned int raw_uint128;

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
        bool tempDefaultToCylinder;
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

    union XAnimDynamicIndicesTrans
    {
        char _1[1];
        uint16_t _2[1];
    };

    union XAnimDynamicFrames
    {
        ByteVec* _1;
        UShortVec* _2;
    };

    struct XAnimPartTransFrames
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

    typedef tdef_align(4) short XQuat[2];

    union XAnimDynamicIndicesQuat
    {
        char _1[1];
        uint16_t _2[1];
    };

    struct XAnimDeltaPartQuatDataFrames
    {
        XQuat* frames;
        XAnimDynamicIndicesQuat indices;
    };

    union XAnimDeltaPartQuatData
    {
        XAnimDeltaPartQuatDataFrames frames;
        int16_t frame0[2];
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

    struct DObjAnimMat
    {
        float quat[4];
        float trans[3];
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

    struct XSurfaceVertexInfo
    {
        int16_t vertCount[4];
        uint16_t* vertsBlend;
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
        int partBits[4];
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
        float bounds[2][3];
        float offset[3];
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

    struct BrushWrapper
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int16_t axialMaterialNum[2][3];
        char* baseAdjacentSide;
        int16_t firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
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
        float momentsOfInertia[3];
        float productsOfInertia[3];
    };

    struct PhysGeomList
    {
        unsigned int count;
        PhysGeomInfo* geoms;
        PhysMass mass;
    };

    struct XModelStreamInfo
    {
        char pad;
    };

    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        char lodRampType;
        uint16_t* boneNames;
        char* parentList;
        int16_t (*quats)[4];
        float (*trans)[4];
        char* partClassification;
        DObjAnimMat* baseMat;
        XSurface* surfs;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        XModelCollSurf_s* collSurfs;
        int numCollSurfs;
        int contents;
        XBoneInfo* boneInfo;
        float radius;
        float mins[3];
        float maxs[3];
        uint16_t numLods;
        uint16_t collLod;
        XModelStreamInfo streamInfo;
        int memUsage;
        char flags;
        bool bad;
        PhysPreset* physPreset;
        PhysGeomList* physGeoms;
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
        GFXS_BLEND_MASK = 0xF,
    };

    enum GfxBlendOp
    {
        GFXS_BLENDOP_DISABLED = 0x0,
        GFXS_BLENDOP_ADD = 0x1,
        GFXS_BLENDOP_SUBTRACT = 0x2,
        GFXS_BLENDOP_REVSUBTRACT = 0x3,
        GFXS_BLENDOP_MIN = 0x4,
        GFXS_BLENDOP_MAX = 0x5,
        GFXS_BLENDOP_MASK = 0x7,
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

        GFXS0_CULL_SHIFT = 0xE,
        GFXS0_CULL_NONE = 0x4000,
        GFXS0_CULL_BACK = 0x8000,
        GFXS0_CULL_FRONT = 0xC000,
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
        GFXS1_DEPTHTEST_SHIFT = 0x2,
        GFXS1_DEPTHTEST_ALWAYS = 0x0,
        GFXS1_DEPTHTEST_LESS = 0x4,
        GFXS1_DEPTHTEST_EQUAL = 0x8,
        GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
        GFXS1_DEPTHTEST_MASK = 0xC,

        GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
        GFXS1_POLYGON_OFFSET_0 = 0x0,
        GFXS1_POLYGON_OFFSET_1 = 0x10,
        GFXS1_POLYGON_OFFSET_2 = 0x20,
        GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
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

    struct GfxStateBits
    {
        unsigned int loadBits[2];
    };

    struct type_align(16) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        float literal[4];
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

    struct gcc_align(8) GfxDrawSurfFields
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
        gcc_align(8) GfxDrawSurfFields fields;
        gcc_align(8) uint64_t packed;
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
        CAMERA_REGION_COUNT = 0x3,
        CAMERA_REGION_NONE = 0x3,
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
        char stateBitsEntry[34];
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
        char firstRow;
        char rowCount;
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

        MLT_ARG_COUNT
    };

    enum MaterialConstantSource
    {
        CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
        CONST_SRC_CODE_LIGHT_POSITION = 0x0,
        CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
        CONST_SRC_CODE_LIGHT_SPOTDIR = 0x2,
        CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x3,
        CONST_SRC_CODE_LIGHT_ATTENUATION = 0x4,
        CONST_SRC_CODE_LIGHT_FALLOFF_A = 0x5,
        CONST_SRC_CODE_LIGHT_FALLOFF_B = 0x6,
        CONST_SRC_CODE_LIGHT_SPOT_MATRIX0 = 0x7,
        CONST_SRC_CODE_LIGHT_SPOT_MATRIX1 = 0x8,
        CONST_SRC_CODE_LIGHT_SPOT_MATRIX2 = 0x9,
        CONST_SRC_CODE_LIGHT_SPOT_MATRIX3 = 0xA,
        CONST_SRC_CODE_LIGHT_SPOT_AABB = 0xB,
        CONST_SRC_CODE_LIGHT_CONE_CONTROL1 = 0xC,
        CONST_SRC_CODE_LIGHT_CONE_CONTROL2 = 0xD,
        CONST_SRC_CODE_LIGHT_SPOT_COOKIE_SLIDE_CONTROL = 0xE,
        CONST_SRC_CODE_SHADOW_PARMS = 0xF,
        CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x10,
        CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x11,
        CONST_SRC_CODE_UPSCALED_TARGET_SIZE = 0x12,
        CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x13,
        CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x14,
        CONST_SRC_CODE_DOF_LERP_SCALE = 0x15,
        CONST_SRC_CODE_DOF_LERP_BIAS = 0x16,
        CONST_SRC_CODE_DOF_ROW_DELTA = 0x17,
        CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x18,
        CONST_SRC_CODE_GAMETIME = 0x19,
        CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x1A,
        CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x1A,
        CONST_SRC_CODE_FILTER_TAP_0 = 0x1A,
        CONST_SRC_CODE_FILTER_TAP_1 = 0x1B,
        CONST_SRC_CODE_FILTER_TAP_2 = 0x1C,
        CONST_SRC_CODE_FILTER_TAP_3 = 0x1D,
        CONST_SRC_CODE_FILTER_TAP_4 = 0x1E,
        CONST_SRC_CODE_FILTER_TAP_5 = 0x1F,
        CONST_SRC_CODE_FILTER_TAP_6 = 0x20,
        CONST_SRC_CODE_FILTER_TAP_7 = 0x21,
        CONST_SRC_CODE_COLOR_MATRIX_R = 0x22,
        CONST_SRC_CODE_COLOR_MATRIX_G = 0x23,
        CONST_SRC_CODE_COLOR_MATRIX_B = 0x24,
        CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x25,
        CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x25,
        CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x25,
        CONST_SRC_CODE_SUNSHADOWMAP_PIXEL_SIZE = 0x26,
        CONST_SRC_CODE_SHADOWMAP_SCALE = 0x27,
        CONST_SRC_CODE_ZNEAR = 0x28,
        CONST_SRC_CODE_SUN_POSITION = 0x29,
        CONST_SRC_CODE_SUN_DIFFUSE = 0x2A,
        CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x2B,
        CONST_SRC_CODE_DEBUG_BUMPMAP = 0x2C,
        CONST_SRC_CODE_DEBUG_PERFORMANCE = 0x2D,
        CONST_SRC_CODE_MATERIAL_COLOR = 0x2E,
        CONST_SRC_CODE_FOG = 0x2F,
        CONST_SRC_CODE_FOG2 = 0x30,
        CONST_SRC_CODE_FOG_COLOR = 0x31,
        CONST_SRC_CODE_SUN_FOG = 0x32,
        CONST_SRC_CODE_SUN_FOG_DIR = 0x33,
        CONST_SRC_CODE_SUN_FOG_COLOR = 0x34,
        CONST_SRC_CODE_GLOW_SETUP = 0x35,
        CONST_SRC_CODE_GLOW_APPLY = 0x36,
        CONST_SRC_CODE_COLOR_BIAS = 0x37,
        CONST_SRC_CODE_COLOR_TINT_BASE = 0x38,
        CONST_SRC_CODE_COLOR_TINT_DELTA = 0x39,
        CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x3A,
        CONST_SRC_CODE_SKY_TRANSITION = 0x3B,
        CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x3C,
        CONST_SRC_CODE_DLIGHT_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x3D,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x3E,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x3F,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x40,
        CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD = 0x41,
        CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x42,
        CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x43,
        CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x44,
        CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x44,
        CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS = 0x45,
        CONST_SRC_CODE_GRID_LIGHTING_SH_0 = 0x46,
        CONST_SRC_CODE_GRID_LIGHTING_SH_1 = 0x47,
        CONST_SRC_CODE_GRID_LIGHTING_SH_2 = 0x48,
        CONST_SRC_CODE_REFLECTION_LIGHTING_SH_0 = 0x49,
        CONST_SRC_CODE_REFLECTION_LIGHTING_SH_1 = 0x4A,
        CONST_SRC_CODE_REFLECTION_LIGHTING_SH_2 = 0x4B,
        CONST_SRC_CODE_WIND_DIRECTION = 0x4C,
        CONST_SRC_CODE_MOTIONBLUR_DIRECTION_AND_MAGNITUDE = 0x4D,
        CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x4E,
        CONST_SRC_CODE_GLOW_BLOOM_SCALE = 0x4F,
        CONST_SRC_CODE_COMPOSITE_FX_OVERLAY_TEXCOORD = 0x50,
        CONST_SRC_CODE_COLOR_BIAS1 = 0x51,
        CONST_SRC_CODE_COLOR_TINT_BASE1 = 0x52,
        CONST_SRC_CODE_COLOR_TINT_DELTA1 = 0x53,
        CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x54,
        CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x55,
        CONST_SRC_CODE_FRAMEBUFFER_READ = 0x56,
        CONST_SRC_CODE_RESIZE_PARAMS1 = 0x57,
        CONST_SRC_CODE_RESIZE_PARAMS2 = 0x58,
        CONST_SRC_CODE_RESIZE_PARAMS3 = 0x59,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_0 = 0x5A,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_1 = 0x5B,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_2 = 0x5C,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_3 = 0x5D,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_4 = 0x5E,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_5 = 0x5F,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_6 = 0x60,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_7 = 0x61,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_8 = 0x62,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_9 = 0x63,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_10 = 0x64,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_11 = 0x65,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_12 = 0x66,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_13 = 0x67,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_14 = 0x68,
        CONST_SRC_CODE_VARIANT_WIND_SPRING_15 = 0x69,
        CONST_SRC_CODE_CHARACTER_CHARRED_AMOUNT = 0x6A,
        CONST_SRC_CODE_POSTFX_CONTROL0 = 0x6B,
        CONST_SRC_CODE_POSTFX_CONTROL1 = 0x6C,
        CONST_SRC_CODE_POSTFX_CONTROL2 = 0x6D,
        CONST_SRC_CODE_POSTFX_CONTROL3 = 0x6E,
        CONST_SRC_CODE_POSTFX_CONTROL4 = 0x6F,
        CONST_SRC_CODE_POSTFX_CONTROL5 = 0x70,
        CONST_SRC_CODE_POSTFX_CONTROL6 = 0x71,
        CONST_SRC_CODE_POSTFX_CONTROL7 = 0x72,
        CONST_SRC_CODE_POSTFX_CONTROL8 = 0x73,
        CONST_SRC_CODE_POSTFX_CONTROL9 = 0x74,
        CONST_SRC_CODE_POSTFX_CONTROLA = 0x75,
        CONST_SRC_CODE_POSTFX_CONTROLB = 0x76,
        CONST_SRC_CODE_POSTFX_CONTROLC = 0x77,
        CONST_SRC_CODE_POSTFX_CONTROLD = 0x78,
        CONST_SRC_CODE_POSTFX_CONTROLE = 0x79,
        CONST_SRC_CODE_POSTFX_CONTROLF = 0x7A,
        CONST_SRC_CODE_HDRCONTROL_0 = 0x7B,
        CONST_SRC_CODE_HDRCONTROL_1 = 0x7C,
        CONST_SRC_CODE_GLIGHT_POSXS = 0x7D,
        CONST_SRC_CODE_GLIGHT_POSYS = 0x7E,
        CONST_SRC_CODE_GLIGHT_POSZS = 0x7F,
        CONST_SRC_CODE_GLIGHT_FALLOFFS = 0x80,
        CONST_SRC_CODE_GLIGHT_REDS = 0x81,
        CONST_SRC_CODE_GLIGHT_GREENS = 0x82,
        CONST_SRC_CODE_GLIGHT_BLUES = 0x83,
        CONST_SRC_CODE_DLIGHT_POSITION = 0x84,
        CONST_SRC_CODE_DLIGHT_DIFFUSE = 0x85,
        CONST_SRC_CODE_DLIGHT_ATTENUATION = 0x86,
        CONST_SRC_CODE_DLIGHT_FALLOFF = 0x87,
        CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_0 = 0x88,
        CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_1 = 0x89,
        CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_2 = 0x8A,
        CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_3 = 0x8B,
        CONST_SRC_CODE_DLIGHT_SPOT_DIR = 0x8C,
        CONST_SRC_CODE_DLIGHT_SPOT_FACTORS = 0x8D,
        CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_0 = 0x8E,
        CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_1 = 0x8F,
        CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_2 = 0x90,
        CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_3 = 0x91,
        CONST_SRC_CODE_CLOUD_LAYER_CONTROL0 = 0x92,
        CONST_SRC_CODE_CLOUD_LAYER_CONTROL1 = 0x93,
        CONST_SRC_CODE_CLOUD_LAYER_CONTROL2 = 0x94,
        CONST_SRC_CODE_CLOUD_LAYER_CONTROL3 = 0x95,
        CONST_SRC_CODE_CLOUD_LAYER_CONTROL4 = 0x96,
        CONST_SRC_CODE_HERO_LIGHTING_R = 0x97,
        CONST_SRC_CODE_HERO_LIGHTING_G = 0x98,
        CONST_SRC_CODE_HERO_LIGHTING_B = 0x99,
        CONST_SRC_CODE_LIGHT_HERO_SCALE = 0x9A,
        CONST_SRC_CODE_CINEMATIC_BLUR_BOX = 0x9B,
        CONST_SRC_CODE_CINEMATIC_BLUR_BOX2 = 0x9C,
        CONST_SRC_CODE_ADSZSCALE = 0x9D,
        CONST_SRC_CODE_UI3D_UV_SETUP_0 = 0x9E,
        CONST_SRC_CODE_UI3D_UV_SETUP_1 = 0x9F,
        CONST_SRC_CODE_UI3D_UV_SETUP_2 = 0xA0,
        CONST_SRC_CODE_UI3D_UV_SETUP_3 = 0xA1,
        CONST_SRC_CODE_UI3D_UV_SETUP_4 = 0xA2,
        CONST_SRC_CODE_UI3D_UV_SETUP_5 = 0xA3,
        CONST_SRC_CODE_CHARACTER_DISSOLVE_COLOR = 0xA4,
        CONST_SRC_CODE_CAMERA_LOOK = 0xA5,
        CONST_SRC_CODE_CAMERA_UP = 0xA6,
        CONST_SRC_CODE_CAMERA_SIDE = 0xA7,
        CONST_SRC_CODE_RIMINTENSITY = 0xA8,
        CONST_SRC_CODE_GENERIC_PARAM0 = 0xA9,
        CONST_SRC_CODE_GENERIC_PARAM1 = 0xAA,
        CONST_SRC_CODE_GENERIC_PARAM2 = 0xAB,
        CONST_SRC_CODE_GENERIC_PARAM3 = 0xAC,
        CONST_SRC_CODE_GENERIC_PARAM4 = 0xAD,
        CONST_SRC_CODE_GENERIC_PARAM5 = 0xAE,
        CONST_SRC_CODE_GENERIC_PARAM6 = 0xAF,
        CONST_SRC_CODE_GENERIC_PARAM7 = 0xB0,
        CONST_SRC_CODE_GENERIC_EYEOFFSET = 0xB1,
        CONST_SRC_CODE_GENERIC_QUADINTENSITY = 0xB2,
        CONST_SRC_CODE_WEAPON_PARAM0 = 0xB3,
        CONST_SRC_CODE_WEAPON_PARAM1 = 0xB4,
        CONST_SRC_CODE_WEAPON_PARAM2 = 0xB5,
        CONST_SRC_CODE_WEAPON_PARAM3 = 0xB6,
        CONST_SRC_CODE_WEAPON_PARAM4 = 0xB7,
        CONST_SRC_CODE_WEAPON_PARAM5 = 0xB8,
        CONST_SRC_CODE_WEAPON_PARAM6 = 0xB9,
        CONST_SRC_CODE_WEAPON_PARAM7 = 0xBA,
        CONST_SRC_CODE_WEAPON_PARAM8 = 0xBB,
        CONST_SRC_CODE_WEAPON_PARAM9 = 0xBC,
        CONST_SRC_CODE_QRCODE_0 = 0xBD,
        CONST_SRC_CODE_QRCODE_1 = 0xBE,
        CONST_SRC_CODE_QRCODE_2 = 0xBF,
        CONST_SRC_CODE_QRCODE_3 = 0xC0,
        CONST_SRC_CODE_QRCODE_4 = 0xC1,
        CONST_SRC_CODE_QRCODE_5 = 0xC2,
        CONST_SRC_CODE_QRCODE_6 = 0xC3,
        CONST_SRC_CODE_QRCODE_7 = 0xC4,
        CONST_SRC_CODE_QRCODE_8 = 0xC5,
        CONST_SRC_CODE_QRCODE_9 = 0xC6,
        CONST_SRC_CODE_QRCODE_10 = 0xC7,
        CONST_SRC_CODE_QRCODE_11 = 0xC8,
        CONST_SRC_CODE_EYEOFFSET = 0xC9,
        CONST_SRC_CODE_SKY_COLOR_MULTIPLIER = 0xCA,
        CONST_SRC_CODE_EXTRA_CAM_PARAM = 0xCB,
        CONST_SRC_CODE_EMBLEM_LUT_SELECTOR = 0xCC,
        CONST_SRC_CODE_DEBUG_COLOR_OVERRIDE = 0xCD,
        CONST_SRC_CODE_DEBUG_ALPHA_OVERRIDE = 0xCE,
        CONST_SRC_CODE_DEBUG_NORMAL_OVERRIDE = 0xCF,
        CONST_SRC_CODE_DEBUG_SPECULAR_OVERRIDE = 0xD0,
        CONST_SRC_CODE_DEBUG_GLOSS_OVERRIDE = 0xD1,
        CONST_SRC_CODE_DEBUG_OCCLUSION_OVERRIDE = 0xD2,
        CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0xD3,
        CONST_SRC_CODE_COUNT_FLOAT4 = 0xD3,
        CONST_SRC_FIRST_CODE_MATRIX = 0xD3,
        CONST_SRC_CODE_WORLD_MATRIX = 0xD3,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0xD4,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0xD5,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0xD6,
        CONST_SRC_CODE_VIEW_MATRIX = 0xD7,
        CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0xD8,
        CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0xD9,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0xDA,
        CONST_SRC_CODE_PROJECTION_MATRIX = 0xDB,
        CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0xDC,
        CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0xDD,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0xDE,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0xDF,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0xE0,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0xE1,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0xE2,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0xE3,
        CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0xE4,
        CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0xE5,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0xE6,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0xE7,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0xE8,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0xE9,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0xEA,
        CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0xEB,
        CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0xEC,
        CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0xED,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0xEE,
        CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xEF,
        CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF0,
        CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF1,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF2,
        CONST_SRC_TOTAL_COUNT = 0xF3,
        CONST_SRC_NONE = 0xF4,
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
        void /*IDirect3DVertexDeclaration9*/* decl[16];
    };

    struct MaterialVertexDeclaration
    {
        char streamCount;
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
        char perPrimArgCount;
        char perObjArgCount;
        char stableArgCount;
        char customSamplerFlags;
        MaterialShaderArgument* args;
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

        TECHNIQUE_LIT_BEGIN = 0x7,

        TECHNIQUE_LIT = 0x7,
        TECHNIQUE_LIT_SUN = 0x8,
        TECHNIQUE_LIT_SUN_SHADOW = 0x9,
        TECHNIQUE_LIT_SPOT = 0xA,
        TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
        TECHNIQUE_LIT_OMNI = 0xC,
        TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
        TECHNIQUE_LIT_INSTANCED = 0xE,
        TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
        TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
        TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
        TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
        TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
        TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,

        TECHNIQUE_LIT_END = 0x15,

        TECHNIQUE_LIGHT_SPOT = 0x15,
        TECHNIQUE_LIGHT_OMNI = 0x16,
        TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
        TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
        TECHNIQUE_FAKELIGHT_VIEW = 0x19,
        TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
        TECHNIQUE_CASE_TEXTURE = 0x1B,
        TECHNIQUE_WIREFRAME_SOLID = 0x1C,
        TECHNIQUE_WIREFRAME_SHADED = 0x1D,
        TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
        TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
        TECHNIQUE_DEBUG_BUMPMAP = 0x20,
        TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
        TECHNIQUE_COUNT = 0x22,
        TECHNIQUE_TOTAL_COUNT = 0x23,
        TECHNIQUE_NONE = 0x24,
    };

    struct MaterialTechniqueSet
    {
        const char* name;
        char worldVertFormat;
        bool hasBeenUploaded;
        char unused[1];
        MaterialTechniqueSet* remappedTechniqueSet;
        MaterialTechnique* techniques[34];
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
        int16_t dimensions[3];
        int format;
        int resourceSize;
        char data[1];
    };

    union GfxTexture
    {
        // void/*IDirect3DBaseTexture9*/* basemap;
        // void/*IDirect3DTexture9*/* map;
        // void/*IDirect3DVolumeTexture9*/* volmap;
        // void/*IDirect3DCubeTexture9*/* cubemap;
        Texture* texture;
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
        MAPTYPE_COUNT = 0x6,
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

    struct SndCurve
    {
        const char* filename;
        int knotCount;
        float knots[8][2];
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

    enum snd_alias_type_t
    {
        SAT_UNKNOWN = 0x0,
        SAT_LOADED = 0x1,
        SAT_STREAMED = 0x2,
        SAT_COUNT = 0x3,
    };

    struct SoundFile
    {
        char type;
        char exists;
        SoundFileRef u;
    };

    struct snd_alias_t
    {
        const char* aliasName;
        const char* subtitle;
        const char* secondaryAliasName;
        const char* chainAliasName;
        SoundFile* soundFile;
        int sequence;
        float volMin;
        float volMax;
        float pitchMin;
        float pitchMax;
        float distMin;
        float distMax;
        int flags;
        float slavePercentage;
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
        float halfSize[3];
        uint16_t materialIndex;
        uint16_t childCount;
        CollisionAabbTreeIndex u;
    };

    struct cmodel_t
    {
        float mins[3];
        float maxs[3];
        float radius;
        cLeaf_t leaf;
    };

    struct type_align(16) cbrush_t
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int16_t axialMaterialNum[2][3];
        char* baseAdjacentSide;
        int16_t firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
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

    typedef unsigned short LeafBrush;

    typedef tdef_align(16) cbrush_t cbrush_array_t;

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
        uint16_t dynEntCount[2];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityColl* dynEntCollList[2];
        unsigned int checksum;
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
    };

    struct MapEnts
    {
        const char* name;
        char* entityString;
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
        char type;
        char canUseShadowMap;
        char unused[2];
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
        uint16_t surfaceCountNoDecal;
        uint16_t startSurfIndexNoDecal;
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

    struct GfxStaticModelDrawInst
    {
        float cullDist;
        GfxPackedPlacement placement;
        XModel* model;
        uint16_t smodelCacheIndex[4];
        char reflectionProbeIndex;
        char primaryLightIndex;
        uint16_t lightingHandle;
        char flags;
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
        unsigned int staticSurfaceCountNoDecal;
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
        char* dynEntVisData[2][3];
    };

    struct GfxWorldStreamInfo
    {
        char pad;
    };

    typedef tdef_align(4) GfxSceneDynModel GfxSceneDynModel4;

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        int indexCount;
        uint16_t* indices;
        int surfaceCount;
        GfxWorldStreamInfo streamInfo;
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        char skySamplerState;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        SunLightParseParams sunParse;
        GfxLight* sunLight;
        float sunColorFromBsp[3];
        unsigned int sunPrimaryLightIndex;
        unsigned int primaryLightCount;
        int cullGroupCount;
        unsigned int reflectionProbeCount;
        GfxReflectionProbe* reflectionProbes;
        GfxTexture* reflectionProbeTextures;
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
        const char* stringVal;
    };

    enum expDataType
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
    };

    struct Operand
    {
        expDataType dataType;
        operandInternalDataUnion internals;
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
        WEAPTYPE_NUM = 0x4,
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
        WEAPCLASS_ITEM = 0x9,
        WEAPCLASS_NUM = 0xA,
    };

    enum PenetrateType
    {
        PENETRATE_TYPE_NONE = 0x0,
        PENETRATE_TYPE_SMALL = 0x1,
        PENETRATE_TYPE_MEDIUM = 0x2,
        PENETRATE_TYPE_LARGE = 0x3,
        PENETRATE_TYPE_COUNT = 0x4,
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
        IMPACT_TYPE_ROCKET_EXPLODE = 0x7,
        IMPACT_TYPE_PROJECTILE_DUD = 0x8,
        IMPACT_TYPE_COUNT = 0x9,
    };

    enum weapInventoryType_t
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,
        WEAPINVENTORYCOUNT = 0x4,
    };

    enum weapFireType_t
    {
        WEAPON_FIRETYPE_FULLAUTO = 0x0,
        WEAPON_FIRETYPE_SINGLESHOT = 0x1,
        WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
        WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
        WEAPON_FIRETYPECOUNT = 0x5,
    };

    enum OffhandClass
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,
        OFFHAND_CLASS_COUNT = 0x4,
    };

    enum weapStance_t
    {
        WEAPSTANCE_STAND = 0x0,
        WEAPSTANCE_DUCK = 0x1,
        WEAPSTANCE_PRONE = 0x2,
        WEAPSTANCE_NUM = 0x3,
    };

    enum activeReticleType_t
    {
        VEH_ACTIVE_RETICLE_NONE = 0x0,
        VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
        VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
        VEH_ACTIVE_RETICLE_COUNT = 0x3,
    };

    enum weaponIconRatioType_t
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,
        WEAPON_ICON_RATIO_COUNT = 0x3,
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
        AMMO_COUNTER_CLIP_COUNT = 0x7,
    };

    enum weapOverlayReticle_t
    {
        WEAPOVERLAYRETICLE_NONE = 0x0,
        WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
        WEAPOVERLAYRETICLE_NUM = 0x2,
    };

    enum WeapOverlayInteface_t
    {
        WEAPOVERLAYINTERFACE_NONE = 0x0,
        WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
        WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
        WEAPOVERLAYINTERFACECOUNT = 0x3,
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
        WEAPPROJEXP_NUM = 0x7,
    };

    enum WeapStickinessType
    {
        WEAPSTICKINESS_NONE = 0x0,
        WEAPSTICKINESS_ALL = 0x1,
        WEAPSTICKINESS_GROUND = 0x2,
        WEAPSTICKINESS_GROUND_WITH_YAW = 0x3,
        WEAPSTICKINESS_COUNT = 0x4,
    };

    enum guidedMissileType_t
    {
        MISSILE_GUIDANCE_NONE = 0x0,
        MISSILE_GUIDANCE_SIDEWINDER = 0x1,
        MISSILE_GUIDANCE_HELLFIRE = 0x2,
        MISSILE_GUIDANCE_JAVELIN = 0x3,
        MISSILE_GUIDANCE_COUNT = 0x4,
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
        const char* szInternalName;
        const char* szDisplayName;
        const char* szOverlayName;
        XModel* gunXModel[16];
        XModel* handXModel;
        const char* szXAnims[33];
        const char* szModeName;
        uint16_t hideTags[8];
        uint16_t notetrackSoundMapKeys[16];
        uint16_t notetrackSoundMapValues[16];
        int playerAnimType;
        weapType_t weapType;
        weapClass_t weapClass;
        PenetrateType penetrateType;
        ImpactType impactType;
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
        XModel* worldModel[16];
        XModel* worldClipModel;
        XModel* rocketModel;
        XModel* knifeModel;
        XModel* worldKnifeModel;
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
        int iMaxAmmo;
        int iClipSize;
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
        WeapOverlayInteface_t overlayInterface;
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
        int adsFireOnly;
        int cancelAutoHolsterWhenEmpty;
        int suppressAmmoReserveDisplay;
        int enhanced;
        int laserSightDuringNightvision;
        Material* killIcon;
        weaponIconRatioType_t killIconRatio;
        int flipKillIcon;
        Material* dpadIcon;
        weaponIconRatioType_t dpadIconRatio;
        int bNoPartialReload;
        int bSegmentedReload;
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
        int bProjImpactExplode;
        WeapStickinessType stickiness;
        int hasDetonator;
        int timedDetonation;
        int rotate;
        int holdButtonToThrow;
        int freezeMovementWhenFiring;
        float lowAmmoWarningThreshold;
        float parallelBounce[29];
        float perpendicularBounce[29];
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
        // TODO: Order is accuracyGraphName[0] -> originalAccuracyGraphKnots[0] -> accuracyGraphName[1] -> ...
        // Which is currently not possible to do in code generation. Afaik this is the only place where this is the case.
        // So might be something to fix but on the other hand it might be too much work for this little inconvenience.
        // const char* accuracyGraphName[2];
        const char* accuracyGraphName0;
        const char* accuracyGraphName1;
        // float(*accuracyGraphKnots[2])[2];
        vec2_t* accuracyGraphKnots0;
        vec2_t* accuracyGraphKnots1;
        // float(*originalAccuracyGraphKnots[2])[2];
        vec2_t* originalAccuracyGraphKnots0;
        vec2_t* originalAccuracyGraphKnots1;
        int accuracyGraphKnotCount[2];
        int originalAccuracyGraphKnotCount[2];
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
        float locationDamageMultipliers[19];
        const char* fireRumble;
        const char* meleeImpactRumble;
        float adsDofStart;
        float adsDofEnd;
    };

    struct SndDriverGlobals
    {
        const char* name;
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

    union FxEffectDefRef
    {
        FxEffectDef* handle;
        const char* name;
    };

    enum FxElemType
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
        FxElemDef* elemDefs;
    };

    struct FxImpactEntry
    {
        FxEffectDef* nonflesh[29];
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

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
