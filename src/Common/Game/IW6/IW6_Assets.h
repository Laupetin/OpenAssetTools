#pragma once

#ifndef __IW6_ASSETS_H
#define __IW6_ASSETS_H

#ifndef __zonecodegenerator
#ifndef __cplusplus
#define __ida
#endif
#endif

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
#ifndef __ida
namespace IW6
{
#endif
#endif
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
        XFILE_BLOCK_SCRIPT,

        MAX_XFILE_COUNT
    };

    struct PhysPreset;
    struct PhysCollmap;
    struct XAnimParts;
    struct XModelSurfs;
    struct XModel;
    struct Material;
    struct MaterialVertexShader;
    struct MaterialPixelShader;
    struct MaterialVertexDeclaration;
    struct MaterialTechniqueSet;
    struct GfxImage;
    struct snd_alias_list_t;
    struct SndCurve;
    struct LoadedSound;
    struct clipMap_t;
    struct ComWorld;
    struct GlassWorld;
    struct PathData;
    struct VehicleTrack;
    struct MapEnts;
    struct FxWorld;
    struct GfxWorld;
    struct GfxLightDef;
    struct Font_s;
    struct MenuList;
    struct menuDef_t;
    struct AnimationClass;
    struct LocalizeEntry;
    struct WeaponAttachment;
    struct WeaponCompleteDef;
    struct SndDriverGlobals;
    struct FxEffectDef;
    struct Statement_s;
    struct FxImpactTable;
    struct SurfaceFxTable;
    struct RawFile;
    struct ScriptFile;
    struct StringTable;
    struct LeaderboardDef;
    struct StructuredDataDefSet;
    struct TracerDef;
    struct VehicleDef;
    struct AddonMapEnts;
    struct NetConstStrings;
    struct ReverbPreset;
    struct LuaFile;
    struct ScriptableDef;
    struct EquipmentSoundTable;
    struct DopplerPreset;

    typedef unsigned short ScriptString;

    union XAssetHeader
    {
        PhysPreset* physPreset;
        PhysCollmap* physCollmap;
        XAnimParts* parts;
        XModelSurfs* modelSurfs;
        XModel* model;
        Material* material;
        MaterialVertexShader* vertexShader;
        MaterialPixelShader* pixelShader;
        MaterialVertexDeclaration* vertexDecl;
        MaterialTechniqueSet* techniqueSet;
        GfxImage* image;
        snd_alias_list_t* sound;
        SndCurve* sndCurve;
        SndCurve* lpfCurve;
        SndCurve* reverbCurve;
        LoadedSound* loadSnd;
        clipMap_t* clipMap;
        ComWorld* comWorld;
        GlassWorld* glassWorld;
        PathData* pathData;
        VehicleTrack* vehicleTrack;
        MapEnts* mapEnts;
        FxWorld* fxWorld;
        GfxWorld* gfxWorld;
        GfxLightDef* lightDef;
        Font_s* font;
        MenuList* menuList;
        menuDef_t* menu;
        AnimationClass* animClass;
        LocalizeEntry* localize;
        WeaponAttachment* attachment;
        WeaponCompleteDef* weapon;
        SndDriverGlobals* sndDriverGlobals;
        FxEffectDef* fx;
        FxImpactTable* impactFx;
        SurfaceFxTable* surfaceFx;
        RawFile* rawfile;
        ScriptFile* scriptfile;
        StringTable* stringTable;
        LeaderboardDef* leaderboardDef;
        StructuredDataDefSet* structuredDataDefSet;
        TracerDef* tracerDef;
        VehicleDef* vehDef;
        AddonMapEnts* addonMapEnts;
        NetConstStrings* netConstStrings;
        ReverbPreset* reverbPreset;
        LuaFile* luaFile;
        ScriptableDef* scriptable;
        EquipmentSoundTable* equipSndTable;
        DopplerPreset* dopplerPreset;
        void* data;
    };

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

    typedef tdef_align32(16) char raw_byte16;
    typedef tdef_align32(16) float raw_float16;
    typedef tdef_align32(128) unsigned int raw_uint128;
    typedef unsigned char raw_byte;
    typedef unsigned int raw_uint;
    typedef unsigned short r_index_t;

    enum PhysPresetScaling : int
    {
        PHYSPRESET_SCALING_LINEAR = 0x0,
        PHYSPRESET_SCALING_QUADRATIC = 0x1,
        PHYSPRESET_SCALING_COUNT = 0x2,
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
        float minMomentum;
        float maxMomentum;
        float minVolume;
        float maxVolume;
        float minPitch;
        float maxPitch;
        PhysPresetScaling volumeType;
        PhysPresetScaling pitchType;
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
        float minMomentum;
        float maxMomentum;
        float minVolume;
        float maxVolume;
        float minPitch;
        float maxPitch;
        PhysPresetScaling volumeType;
        PhysPresetScaling pitchType;
        bool tempDefaultToCylinder;
        bool perSurfaceSndAlias;
    };

    struct Bounds
    {
        vec3_t midPoint;
        vec3_t halfSize;
    };

    struct cplane_s
    {
        float normal[3];
        float dist;
        unsigned char type;
        unsigned char pad[3];
    };

    struct cbrushside_t
    {
        cplane_s* plane;
        unsigned short materialNum;
        unsigned char firstAdjacentSideOffset;
        unsigned char edgeCount;
    };

    typedef unsigned char cbrushedge_t;

    struct cbrushWrapper_t
    {
        unsigned short numsides;
        unsigned short glassPieceIndex;
        cbrushside_t* sides;
        cbrushedge_t* baseAdjacentSide;
        short axialMaterialNum[2][3];
        unsigned char firstAdjacentSideOffsets[2][3];
        unsigned char edgeCount[2][3];
    };

    typedef cbrushWrapper_t cbrush_t;

    struct BrushWrapper
    {
        Bounds bounds;
        cbrushWrapper_t brush;
        int totalEdgeCount;
        cplane_s* planes;
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
        unsigned char* _1;
        unsigned short* _2;
        void* data;
    };

    struct XAnimNotifyInfo
    {
        ScriptString name;
        float time;
    };

    typedef unsigned char ByteVec[3];
    typedef tdef_align32(4) unsigned short UShortVec[3];

    union XAnimDynamicFrames
    {
        ByteVec* _1;
        UShortVec* _2;
    };

    union XAnimDynamicIndices
    {
        uint8_t _1[1];
        uint16_t _2[1];
    };

    struct type_align32(4) XAnimPartTransFrames
    {
        float mins[3];
        float size[3];
        XAnimDynamicFrames frames;
        XAnimDynamicIndices indices;
    };

    union XAnimPartTransData
    {
        XAnimPartTransFrames frames;
        vec3_t frame0;
    };

    struct XAnimPartTrans
    {
        unsigned short size;
        unsigned char smallTrans;
        XAnimPartTransData u;
    };

    struct type_align(4) XQuat2
    {
        int16_t value[2];
    };

    struct type_align32(4) XAnimDeltaPartQuatDataFrames2
    {
        XQuat2* frames;
        XAnimDynamicIndices indices;
    };

    union XAnimDeltaPartQuatData2
    {
        XAnimDeltaPartQuatDataFrames2 frames;
        XQuat2 frame0;
    };

    struct XAnimDeltaPartQuat2
    {
        unsigned short size;
        XAnimDeltaPartQuatData2 u;
    };

    struct type_align(4) XQuat
    {
        int16_t value[4];
    };

    struct XAnimDeltaPartQuatDataFrames
    {
        XQuat* frames;
        XAnimDynamicIndices indices;
    };

    union XAnimDeltaPartQuatData
    {
        XAnimDeltaPartQuatDataFrames frames;
        XQuat frame0;
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
        unsigned short dataByteCount;
        unsigned short dataShortCount;
        unsigned short dataIntCount;
        unsigned short randomDataByteCount;
        unsigned short randomDataIntCount;
        unsigned short numframes;
        unsigned char flags;
        unsigned char boneCount[12];
        unsigned char notifyCount;
        unsigned char assetType;
        unsigned char ikType;
        unsigned int randomDataShortCount;
        unsigned int indexCount;
        float framerate;
        float frequency;
        ScriptString* names;
        unsigned char* dataByte;
        short* dataShort;
        int* dataInt;
        short* randomDataShort;
        unsigned char* randomDataByte;
        int* randomDataInt;
        XAnimIndices indices;
        XAnimNotifyInfo* notify;
        XAnimDeltaPart* deltaPart;
    };

    struct XSurfaceCollisionAabb
    {
        unsigned short mins[3];
        unsigned short maxs[3];
    };

    /* 11141 */
    struct XSurfaceCollisionNode
    {
        XSurfaceCollisionAabb aabb;
        unsigned short childBeginIndex;
        unsigned short childCount;
    };

    /* 11142 */
    struct XSurfaceCollisionLeaf
    {
        unsigned short triangleBeginIndex;
    };

    /* 11143 */
    struct XSurfaceCollisionTree
    {
        float trans[3];
        float scale[3];
        unsigned int nodeCount;
        XSurfaceCollisionNode* nodes;
        unsigned int leafCount;
        XSurfaceCollisionLeaf* leafs;
    };

    /* 11144 */
    struct XRigidVertList
    {
        unsigned short boneOffset;
        unsigned short vertCount;
        unsigned short triOffset;
        unsigned short triCount;
        XSurfaceCollisionTree* collisionTree;
    };

    /* 11130 */
    union PackedUnitVec
    {
        unsigned int packed;
    };

    /* 11131 */
    union PackedTexCoords
    {
        unsigned int packed;
    };

    /* 11132 */
    struct GfxQuantizedNoColorVertex
    {
        short xyz[3];
        short binormalSign;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
    };

    /* 11133 */
    union GfxColor
    {
        unsigned int packed;
        unsigned char array[4];
    };

    /* 11134 */
    struct GfxQuantizedNoColorMotionVertex
    {
        short xyz[3];
        short binormalSignAndHeight;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
        GfxColor pieceIndex;
    };

    /* 11135 */
    struct GfxQuantizedVertex
    {
        short xyz[3];
        short binormalSign;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
        GfxColor color;
    };

    /* 11136 */
    struct GfxQuantizedMotionVertex
    {
        short xyz[3];
        short binormalSignAndHeight;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
        GfxColor pieceIndex;
    };

    /* 11137 */
    struct GfxPackedVertex
    {
        float xyz[3];
        float binormalSign;
        GfxColor color;
        PackedTexCoords texCoord;
        PackedUnitVec normal;
        PackedUnitVec tangent;
    };

    /* 11138 */
    struct GfxPackedMotionVertex
    {
        float xyz[3];
        float binormalSignAndHeight;
        GfxColor pieceIndex;
        PackedTexCoords texCoord;
        PackedUnitVec normal;
        PackedUnitVec tangent;
    };

    /* 11139 */
    union GfxVertexUnion0
    {
        GfxQuantizedNoColorVertex* quantizedNoColorVerts0;
        GfxQuantizedNoColorMotionVertex* quantizedNoColorMotionVerts0;
        GfxQuantizedVertex* quantizedVerts0;
        GfxQuantizedMotionVertex* quantizedMotionVerts0;
        GfxPackedVertex* packedVerts0;
        GfxPackedMotionVertex* packedMotionVerts0;
        void* verts0;
    };

    /* 2959 */
    struct D3DResource
    {
        unsigned int Common;
        unsigned int ReferenceCount;
        unsigned int Fence;
        unsigned int ReadFence;
        unsigned int Identifier;
        unsigned int BaseFlush;
    };

    /* 3564 */
    struct GPUVERTEX_FETCH_CONSTANT
    {
        unsigned int Type : 2;
        unsigned int BaseAddress : 30;
        unsigned int Endian : 2;
        unsigned int Size : 24;
        unsigned int AddressClamp : 1;
        unsigned int Unused: 1;
        unsigned int RequestSize : 2;
        unsigned int ClampDisable : 2;
    };

    /* 4101 */
    struct D3DVertexBuffer : D3DResource
    {
        GPUVERTEX_FETCH_CONSTANT Format;
    };

    /* 4102 */
    struct D3DIndexBuffer : D3DResource
    {
        unsigned int Address;
        unsigned int Size;
    };

    /* 11145 */
    struct XSurface
    {
        unsigned char flags;
        unsigned char tileMode;
        unsigned short vertCount;
        unsigned short triCount;
        unsigned char rigidVertListCount;
        unsigned char pad;
        short blendVertCounts[4];
        //GfxVertexUnion0 verts0;
        GfxPackedVertex *verts0;
        unsigned short* triIndices;
        D3DVertexBuffer vb0;
        D3DIndexBuffer indexBuffer;
        XRigidVertList* rigidVertLists;
        unsigned short* blendVerts;
        unsigned int vertexLightingIndex;
        float quantizeScale;
        int partBits[6];
    };

    /* 11146 */
    struct XModelSurfs
    {
        const char* name;
        XSurface* surfs;
        unsigned short numsurfs;
        int partBits[6];
    };

    /* 11149 */
    union XBoneRadius
    {
        float radiusSquared;
        unsigned int radiusSquaredAsInt;
    };

    /* 11150 */
    struct XBoneInfo
    {
        Bounds bounds;
        XBoneRadius ___u1;
    };

    /* 11128 */
    struct DObjAnimMat
    {
        float quat[4];
        float trans[3];
        float transWeight;
    };

    /* 11129 */
    struct ReactiveMotionModelPart
    {
        float center[3];
        float stiffness;
    };

    /* 11147 */
    struct XModelLodInfo
    {
        float dist;
        unsigned short numsurfs;
        unsigned short surfIndex;
        XModelSurfs* modelSurfs;
        int partBits[6];
        XSurface* surfs;
    };

    /* 11148 */
    struct XModelCollSurf_s
    {
        Bounds bounds;
        int boneIdx;
        int contents;
        int surfFlags;
    };

    /* 11158 */
    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        unsigned char numReactiveMotionParts;
        float scale;
        unsigned int noScalePartBits[6];
        ScriptString* boneNames;
        unsigned char* parentList;
        short* quats;
        float* trans;
        unsigned char* partClassification;
        DObjAnimMat* baseMat;
        ReactiveMotionModelPart* reactiveMotionParts;
        Material** materialHandles;
        XModelLodInfo lodInfo[6];
        char maxLoadedLod;
        char numLods;
        char collLod;
        unsigned char flags;
        XModelCollSurf_s* collSurfs;
        int numCollSurfs;
        int contents;
        XBoneInfo* boneInfo;
        float radius;
        Bounds bounds;
        unsigned short* invHighMipRadius;
        int memUsage;
        PhysPreset* physPreset;
        PhysCollmap* physCollmap;
        float quantization;
    };

    /* 11098 */
    struct GfxDrawSurfFields
    {
        uint64_t objectId : 15;
        uint64_t reflectionProbeIndex : 8;
        uint64_t hasGfxEntIndex : 1;
        uint64_t customIndex : 5;
        uint64_t materialSortedIndex : 12;
        uint64_t prepass : 2;
        uint64_t useHeroLighting : 1;
        uint64_t sceneLightIndex : 8;
        uint64_t viewModelRender : 1;
        uint64_t surfType : 4;
        uint64_t primarySortKey : 6;
        uint64_t unused : 1;
    };

    /* 11099 */
    union GfxDrawSurf
    {
        GfxDrawSurfFields fields;
        uint64_t packed;
    };

    /* 11100 */
    struct MaterialInfo
    {
        const char* name;
        unsigned char gameFlags;
        unsigned char sortKey;
        unsigned char textureAtlasRowCount;
        unsigned char textureAtlasColumnCount;
        unsigned char textureAtlasFrameBlend;
        GfxDrawSurf drawSurf;
        unsigned int surfaceTypeBits;
    };

    /* 11101 */
    struct MaterialStreamRouting
    {
        unsigned char source;
        unsigned char dest;
    };

    /* 11102 */
    union MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[15];
        void* decl[26];
    };

    /* 11103 */
    struct MaterialVertexDeclaration
    {
        unsigned char streamCount;
        bool hasOptionalSource;
        MaterialVertexStreamRouting routing;
    };

    /* 11104 */
    struct GfxVertexShaderLoadDef
    {
        unsigned char* cachedPart;
        unsigned char* physicalPart;
        unsigned short cachedPartSize;
        unsigned short physicalPartSize;
        unsigned int microCodeCrc;
    };

    /* 11105 */
    union MaterialVertexShaderProgram
    {
        void* vs;
        GfxVertexShaderLoadDef loadDef;
    };

    /* 11106 */
    struct MaterialVertexShader
    {
        const char* name;
        MaterialVertexShaderProgram prog;
    };

    /* 11107 */
    struct GfxPixelShaderLoadDef
    {
        unsigned char* cachedPart;
        unsigned char* physicalPart;
        unsigned short cachedPartSize;
        unsigned short physicalPartSize;
        unsigned int microCodeCrc;
    };

    /* 11108 */
    union MaterialPixelShaderProgram
    {
        void* ps;
        GfxPixelShaderLoadDef loadDef;
    };

    /* 11109 */
    struct MaterialPixelShader
    {
        const char* name;
        MaterialPixelShaderProgram prog;
    };

    /* 11110 */
    struct MaterialArgumentCodeConst
    {
        unsigned short index;
        unsigned char firstRow;
        unsigned char rowCount;
    };

    /* 11111 */
    union MaterialArgumentDef
    {
        const float* literalConst;
        MaterialArgumentCodeConst codeConst;
        unsigned int codeSampler;
        unsigned int nameHash;
    };

    /* 1310 */
    enum MaterialShaderArgumentType : unsigned short
    {
        MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
        MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
        MTL_ARG_MATERIAL_VERTEX_SAMPLER = 0x2,
        MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x3,

        MTL_ARG_CODE_PRIM_BEGIN = 0x4,

        MTL_ARG_CODE_VERTEX_CONST = 0x4,
        MTL_ARG_CODE_PIXEL_SAMPLER = 0x5,
        MTL_ARG_CODE_PIXEL_CONST = 0x6,

        MTL_ARG_CODE_PRIM_END = 0x7,

        MTL_ARG_MATERIAL_PIXEL_CONST = 0x7,
        MTL_ARG_LITERAL_PIXEL_CONST = 0x8,

        MTL_ARG_COUNT = 0x9,
    };

    /* 11112 */
    struct MaterialShaderArgument
    {
        MaterialShaderArgumentType type;
        unsigned short dest;
        MaterialArgumentDef u;
    };

    /* 11113 */
    struct MaterialPass
    {
        MaterialVertexDeclaration* vertexDecl;
        MaterialVertexShader* vertexShaderArray[26];
        MaterialVertexShader* vertexShader;
        MaterialPixelShader* pixelShader;
        unsigned char perPrimArgCount;
        unsigned char perObjArgCount;
        unsigned char stableArgCount;
        unsigned char customSamplerFlags;
        unsigned char precompiledIndex;
        unsigned char stageConfig;
        MaterialShaderArgument* args;
    };

    /* 858 */
    enum MaterialTechniqueType : int32_t
    {
        TECHNIQUE_STANDARD_BEGIN = 0x0,
        TECHNIQUE_DEPTH_PREPASS = 0x0,
        TECHNIQUE_BUILD_FLOAT_Z = 0x1,
        TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
        TECHNIQUE_UNLIT = 0x3,
        TECHNIQUE_EMISSIVE = 0x4,
        TECHNIQUE_EMISSIVE_DFOG = 0x5,
        TECHNIQUE_EMISSIVE_SHADOW = 0x6,
        TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x7,
        TECHNIQUE_LIT_BEGIN = 0x8,
        TECHNIQUE_LIT = 0x8,
        TECHNIQUE_LIT_DFOG = 0x9,
        TECHNIQUE_LIT_SUN = 0xA,
        TECHNIQUE_LIT_SUN_DFOG = 0xB,
        TECHNIQUE_LIT_SUN_SHADOW = 0xC,
        TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xD,
        TECHNIQUE_LIT_SPOT = 0xE,
        TECHNIQUE_LIT_SPOT_DFOG = 0xF,
        TECHNIQUE_LIT_SPOT_SHADOW = 0x10,
        TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x11,
        TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS = 0x12,
        TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x13,
        TECHNIQUE_LIT_OMNI = 0x14,
        TECHNIQUE_LIT_OMNI_DFOG = 0x15,
        TECHNIQUE_LIT_OMNI_SHADOW = 0x16,
        TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x17,
        TECHNIQUE_LIT_END = 0x18,
        TECHNIQUE_LIGHT_SPOT = 0x18,
        TECHNIQUE_LIGHT_OMNI = 0x19,
        TECHNIQUE_LIGHT_SPOT_SHADOW = 0x1A,
        TECHNIQUE_LIGHT_SPOT_SHADOW_CUCOLORIS = 0x1B,
        TECHNIQUE_FAKELIGHT_NORMAL = 0x1C,
        TECHNIQUE_FAKELIGHT_VIEW = 0x1D,
        TECHNIQUE_SUNLIGHT_PREVIEW = 0x1E,
        TECHNIQUE_CASE_TEXTURE = 0x1F,
        TECHNIQUE_WIREFRAME_SOLID = 0x20,
        TECHNIQUE_WIREFRAME_SHADED = 0x21,
        TECHNIQUE_THERMAL = 0x22,
        TECHNIQUE_DEBUG_TEXEL_DENSITY = 0x23,
        TECHNIQUE_DEBUG_BUMPMAP = 0x24,
        TECHNIQUE_STANDARD_LAST = 0x24,
        TECHNIQUE_COUNT = 0x25,
        TECHNIQUE_TOTAL_COUNT = 0x26,
        TECHNIQUE_NONE = 0x27,
    };

    /* 1298 */
    enum MaterialWorldVertexFormat : int32_t
    {
        MTL_WORLDVERT_T1 = 0x0,
        MTL_WORLDVERT_T2N1 = 0x1,
        MTL_WORLDVERT_T2N2 = 0x2,
        MTL_WORLDVERT_T3N1 = 0x3,
        MTL_WORLDVERT_T3N2 = 0x4,
        MTL_WORLDVERT_T3N3 = 0x5,
        MTL_WORLDVERT_T4N1 = 0x6,
        MTL_WORLDVERT_T4N2 = 0x7,
        MTL_WORLDVERT_T4N3 = 0x8,
        MTL_WORLDVERT_T5N1 = 0x9,
        MTL_WORLDVERT_T5N2 = 0xA,
        MTL_WORLDVERT_T5N3 = 0xB,
        MTL_WORLDVERT_COUNT = 0xC,
    };

    /* 1300 */
    enum MaterialStreamStreamSource_e : int32_t
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
        STREAM_SRC_VERTEX_LIT_0 = 0x7,
        STREAM_SRC_VERTEX_LIT_1 = 0x8,
        STREAM_SRC_VERTEX_LIT_2 = 0x9,
        STREAM_SRC_NORMAL_TRANSFORM_0 = 0xA,
        STREAM_SRC_NORMAL_TRANSFORM_1 = 0xB,
        STREAM_SRC_COUNT = 0xC,
    };

    /* 1301 */
    enum MaterialStreamDestination_e : int32_t
    {
        STREAM_DST_POSITION = 0x0,
        STREAM_DST_NORMAL = 0x1,
        STREAM_DST_COLOR_0 = 0x2,
        STREAM_DST_COLOR_1 = 0x3,
        STREAM_DST_COLOR_LAST = 0x4,
        STREAM_DST_COLOR_END = 0x3,
        STREAM_DST_DEPTH = 0x4,
        STREAM_DST_TEXCOORD_0 = 0x5,
        STREAM_DST_TEXCOORD_1 = 0x6,
        STREAM_DST_TEXCOORD_2 = 0x7,
        STREAM_DST_TEXCOORD_3 = 0x8,
        STREAM_DST_TEXCOORD_4 = 0x9,
        STREAM_DST_TEXCOORD_5 = 0xA,
        STREAM_DST_TEXCOORD_6 = 0xB,
        STREAM_DST_TEXCOORD_7 = 0xC,
        STREAM_DST_TEXCOORD_8 = 0xD,
        STREAM_DST_TEXCOORD_9 = 0xE,
        STREAM_DST_COUNT = 0xF,
        STREAM_DST_TEXCOORD_COUNT = 0xA,
        STREAM_DST_COLOR_COUNT = 0x2,
        STREAM_DST_CUSTOM = 0xF,
    };

    /* 1293 */
    enum MaterialConstantSource : int32_t
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
        CONST_SRC_CODE_GENERIC_MATERIAL_DATA = 0x8,
        CONST_SRC_CODE_EYEOFFSET = 0x9,
        CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0xA,
        CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0xA,
        CONST_SRC_CODE_PIXEL_COST_FRACS = 0xA,
        CONST_SRC_CODE_PIXEL_COST_DECODE = 0xB,
        CONST_SRC_CODE_FILTER_TAP_0 = 0xC,
        CONST_SRC_CODE_FILTER_TAP_1 = 0xD,
        CONST_SRC_CODE_FILTER_TAP_2 = 0xE,
        CONST_SRC_CODE_FILTER_TAP_3 = 0xF,
        CONST_SRC_CODE_FILTER_TAP_4 = 0x10,
        CONST_SRC_CODE_FILTER_TAP_5 = 0x11,
        CONST_SRC_CODE_FILTER_TAP_6 = 0x12,
        CONST_SRC_CODE_FILTER_TAP_7 = 0x13,
        CONST_SRC_CODE_COLOR_MATRIX_R = 0x14,
        CONST_SRC_CODE_COLOR_MATRIX_G = 0x15,
        CONST_SRC_CODE_COLOR_MATRIX_B = 0x16,
        CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x17,
        CONST_SRC_CODE_RENDER_SOURCE_SIZE = 0x18,
        CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x19,
        CONST_SRC_CODE_FIXED_PS_BEGIN = 0x19,
        CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x19,
        CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x1A,
        CONST_SRC_CODE_DOF_LERP_SCALE = 0x1B,
        CONST_SRC_CODE_DOF_LERP_BIAS = 0x1C,
        CONST_SRC_CODE_DOF_ROW_DELTA = 0x1D,
        CONST_SRC_CODE_MOTION_MATRIX_X = 0x1E,
        CONST_SRC_CODE_MOTION_MATRIX_Y = 0x1F,
        CONST_SRC_CODE_MOTION_MATRIX_W = 0x20,
        CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x21,
        CONST_SRC_CODE_SHADOWMAP_SCALE = 0x22,
        CONST_SRC_CODE_ZNEAR = 0x23,
        CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x24,
        CONST_SRC_CODE_RIM_LIGHT_0_DIR = 0x25,
        CONST_SRC_CODE_RIM_LIGHT_0_COLOR = 0x26,
        CONST_SRC_CODE_RIM_LIGHT_1_DIR = 0x27,
        CONST_SRC_CODE_RIM_LIGHT_1_COLOR = 0x28,
        CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE = 0x29,
        CONST_SRC_CODE_DEBUG_BUMPMAP = 0x2A,
        CONST_SRC_CODE_MATERIAL_COLOR = 0x2B,
        CONST_SRC_CODE_FOG = 0x2C,
        CONST_SRC_CODE_FOG_COLOR_LINEAR = 0x2D,
        CONST_SRC_CODE_FOG_COLOR_GAMMA = 0x2E,
        CONST_SRC_CODE_FOG_SUN_CONSTS = 0x2F,
        CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR = 0x30,
        CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA = 0x31,
        CONST_SRC_CODE_FOG_SUN_DIR = 0x32,
        CONST_SRC_CODE_FOG_HEIGHT = 0x33,
        CONST_SRC_CODE_GLOW_SETUP = 0x34,
        CONST_SRC_CODE_GLOW_APPLY = 0x35,
        CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE = 0x36,
        CONST_SRC_CODE_COLOR_BIAS = 0x37,
        CONST_SRC_CODE_COLOR_TINT_BASE = 0x38,
        CONST_SRC_CODE_COLOR_TINT_DELTA = 0x39,
        CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA = 0x3A,
        CONST_SRC_CODE_MCOLOR_BIAS_AND_RANGE1 = 0x3B,
        CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE1 = 0x3C,
        CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT1 = 0x3D,
        CONST_SRC_CODE_MCOLOR_BIAS2_AND_RANGE_SCALE = 0x3E,
        CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE2 = 0x3F,
        CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT2 = 0x40,
        CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x41,
        CONST_SRC_CODE_ENVMAP_PARMS = 0x42,
        CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST = 0x43,
        CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x44,
        CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x45,
        CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x46,
        CONST_SRC_CODE_SNOWAMBIENTCOLOR = 0x47,
        CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x48,
        CONST_SRC_CODE_FRAMEBUFFER_READ = 0x49,
        CONST_SRC_CODE_THERMAL_COLOR_OFFSET = 0x4A,
        CONST_SRC_CODE_THERMAL_FADE_CONTROL = 0x4B,
        CONST_SRC_CODE_THERMAL_FADE_COLOR = 0x4C,
        CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS = 0x4D,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX_R0_PS = 0x4E,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX_R1_PS = 0x4F,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX_R2_PS = 0x50,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX_R3_PS = 0x51,
        CONST_SRC_CODE_FIXED_PS_END = 0x52,
        CONST_SRC_CODE_NON_PS_BEGIN = 0x52,
        CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x52,
        CONST_SRC_CODE_LIGHT_PROBE_AMBIENT = 0x53,
        CONST_SRC_CODE_NEARPLANE_ORG = 0x54,
        CONST_SRC_CODE_NEARPLANE_DX = 0x55,
        CONST_SRC_CODE_NEARPLANE_DY = 0x56,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x57,
        CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x58,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 = 0x59,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 = 0x5A,
        CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 = 0x5B,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 = 0x5C,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 = 0x5D,
        CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 = 0x5E,
        CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0 = 0x5F,
        CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1 = 0x60,
        CONST_SRC_CODE_REACTIVEMOTION_CENTERS = 0x61,
        CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x71,
        CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x72,
        CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x73,
        CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x73,
        CONST_SRC_CODE_NON_PS_END = 0x74,
        CONST_SRC_CODE_COUNT_FLOAT4 = 0x74,
        CONST_SRC_FIRST_CODE_MATRIX = 0x74,
        CONST_SRC_CODE_VIEW_MATRIX = 0x74,
        CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x75,
        CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x76,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x77,
        CONST_SRC_CODE_PROJECTION_MATRIX = 0x78,
        CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x79,
        CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x7A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x7B,
        CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x7C,
        CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x7D,
        CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x7E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x7F,
        CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x80,
        CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x81,
        CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x82,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x83,
        CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x84,
        CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x85,
        CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x86,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x87,
        CONST_SRC_CODE_WORLD_MATRIX0 = 0x88,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 = 0x89,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 = 0x8A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 = 0x8B,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX0 = 0x8C,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 = 0x8D,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x8E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x8F,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x90,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x91,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x92,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x93,
        CONST_SRC_CODE_WORLD_MATRIX1 = 0x94,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 = 0x95,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 = 0x96,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 = 0x97,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX1 = 0x98,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 = 0x99,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x9A,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x9B,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x9C,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x9D,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x9E,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x9F,
        CONST_SRC_CODE_WORLD_MATRIX2 = 0xA0,
        CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 = 0xA1,
        CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 = 0xA2,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 = 0xA3,
        CONST_SRC_CODE_WORLD_VIEW_MATRIX2 = 0xA4,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 = 0xA5,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0xA6,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0xA7,
        CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 = 0xA8,
        CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0xA9,
        CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0xAA,
        CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0xAB,
        CONST_SRC_TOTAL_COUNT = 0xAC,
        CONST_SRC_NONE = 0xAD,
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
        TEXTURE_SRC_CODE_LIGHT_CUCOLORIS = 0xE,
        TEXTURE_SRC_CODE_OUTDOOR = 0xF,
        TEXTURE_SRC_CODE_FLOATZ = 0x10,
        TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x11,
        TEXTURE_SRC_CODE_RAW_FLOATZ = 0x12,
        TEXTURE_SRC_CODE_HALF_PARTICLES = 0x13,
        TEXTURE_SRC_CODE_HALF_PARTICLES_Z = 0x14,
        TEXTURE_SRC_CODE_CASE_TEXTURE = 0x15,
        TEXTURE_SRC_CODE_CINEMATIC_Y = 0x16,
        TEXTURE_SRC_CODE_CINEMATIC_CR = 0x17,
        TEXTURE_SRC_CODE_CINEMATIC_CB = 0x18,
        TEXTURE_SRC_CODE_CINEMATIC_A = 0x19,
        TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1A,
        TEXTURE_SRC_CODE_PIP_SCENE = 0x1B,
        TEXTURE_SRC_CODE_COLOR_MANIPULATION = 0x1C,
        TEXTURE_SRC_CODE_STREAMING_LOADING = 0x1D,

        TEXTURE_SRC_CODE_COUNT
    };

    enum TechniqueFlags
    {
        MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN = 0x1,
        MTL_TECHFLAG_NEEDS_RESOLVED_SCENE = 0x2,

        MTL_TECHFLAG_ZPREPASS = 0x4,
        MTL_TECHFLAG_BUILD_FLOATZ = 0x8,
        MTL_TECHFLAG_BUILD_SHADOW_MAP_DEPTH_OR_MODEL = 0x10,

        MTL_TECHFLAG_DECL_HAS_OPTIONAL_SOURCE = 0x20,

        MTL_TECHFLAG_USES_LIGHT_SPOT_FACTORS = 0x40,
        MTL_TECHFLAG_USES_FLOATZ = 0x80,
        MTL_TECHFLAG_USES_DISTORTION_FLOATZ = 0x100,
        TECHNIQUE_FLAG_200 = 0x200,
    };

    /* 11114 */
    struct MaterialTechnique
    {
        const char* name;
        unsigned short flags;
        unsigned short passCount;
        MaterialPass passArray[1];
    };

    /* 11115 */
    struct MaterialTechniqueSet
    {
        const char* name;
        unsigned char worldVertFormat;
        unsigned char unused[2];
        MaterialTechnique* techniques[37];
    };

    /* 2960 */
    struct GPUTEXTURESIZE_1D
    {
        unsigned int Width : 24;
    };

    /* 2961 */
    struct GPUTEXTURESIZE_2D
    {
        unsigned int Width : 13;
        unsigned int Height : 13;
    };

    /* 2962 */
    struct GPUTEXTURESIZE_3D
    {
        unsigned int Width : 11;
        unsigned int Height : 11;
        unsigned int Depth : 10;
    };

    /* 2963 */
    struct GPUTEXTURESIZE_STACK
    {
        unsigned int Width : 13;
        unsigned int Height : 13;
        unsigned int Depth : 6;
    };

    /* 2965 */
    struct GPUTEXTURE_FETCH_CONSTANT
    {
        unsigned int Type : 2;
        unsigned int SignX : 2;
        unsigned int SignY : 2;
        unsigned int SignZ : 2;
        unsigned int SignW : 2;
        unsigned int ClampX : 3;
        unsigned int ClampY : 3;
        unsigned int ClampZ : 3;
        unsigned int Unused: 3;
        unsigned int Pitch : 9;
        unsigned int Tiled : 1;
        unsigned int DataFormat : 6;
        unsigned int Endian : 2;
        unsigned int RequestSize : 2;
        unsigned int Stacked : 1;
        unsigned int ClampPolicy : 1;
        unsigned int BaseAddress : 20;
        union
        {
            GPUTEXTURESIZE_1D OneD;
            GPUTEXTURESIZE_2D TwoD;
            GPUTEXTURESIZE_3D ThreeD;
            GPUTEXTURESIZE_STACK Stack;
        } Size;
        unsigned int NumFormat : 1;
        unsigned int SwizzleX : 3;
        unsigned int SwizzleY : 3;
        unsigned int SwizzleZ : 3;
        unsigned int SwizzleW : 3;
        int ExpAdjust : 6;
        unsigned int MagFilter : 2;
        unsigned int MinFilter : 2;
        unsigned int MipFilter : 2;
        unsigned int AnisoFilter : 3;
        unsigned int Unused2: 3;
        unsigned int BorderSize : 1;
        unsigned int VolMagFilter : 1;
        unsigned int VolMinFilter : 1;
        unsigned int MinMipLevel : 4;
        unsigned int MaxMipLevel : 4;
        unsigned int MagAnisoWalk : 1;
        unsigned int MinAnisoWalk : 1;
        int LODBias : 10;
        int GradExpAdjustH : 5;
        int GradExpAdjustV : 5;
        unsigned int BorderColor : 2;
        unsigned int ForceBCWToMax : 1;
        unsigned int TriClamp : 2;
        int AnisoBias : 4;
        unsigned int Dimension : 2;
        unsigned int PackedMips : 1;
        unsigned int MipAddress : 20;
    };

    /* 2967 */
    struct D3DBaseTexture : D3DResource
    {
        unsigned int MipFlush;
        GPUTEXTURE_FETCH_CONSTANT Format;
    };

    /* 2968 */
    struct D3DCubeTexture : D3DBaseTexture
    {
    };

    /* 3284 */
    struct D3DLineTexture : D3DBaseTexture
    {
    };

    /* 4892 */
    struct D3DTexture : D3DBaseTexture
    {
    };

    /* 3983 */
    struct D3DVolumeTexture : D3DBaseTexture
    {
    };

    /* 11116 */
    union GfxTexture
    {
        D3DBaseTexture basemap;
        D3DLineTexture linemap;
        D3DTexture map;
        D3DVolumeTexture volmap;
        D3DCubeTexture cubemap;
    };

    /* 11117 */
    struct CardMemory
    {
        int platform[1];
    };

    /* 11118 */
    struct GfxImageStreamData
    {
        unsigned short width;
        unsigned short height;
        unsigned int pixelSize;
    };

    /* 1303 */
    enum ImageCategory : unsigned char
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

    /* 1511 */
    enum ImageMapType : unsigned char
    {
        MAPTYPE_NONE = 0x0,
        MAPTYPE_INVALID1 = 0x1,
        MAPTYPE_1D = 0x2,
        MAPTYPE_2D = 0x3,
        MAPTYPE_3D = 0x4,
        MAPTYPE_CUBE = 0x5,
        MAPTYPE_ARRAY = 0x6,
        MAPTYPE_COUNT = 0x7,
    };

    /* 11119 */
    struct GfxImage
    {
        GfxTexture texture;
        int format;
        ImageMapType mapType;
        unsigned char semantic;
        ImageCategory category;
        unsigned char flags;
        CardMemory cardMemory;
        unsigned short width;
        unsigned short height;
        unsigned short depth;
        unsigned char levelCount;
        unsigned char cached;
        unsigned char* pixels;
        GfxImageStreamData streams[4];
        const char* name;
    };

    /* 11120 */
    struct WaterWritable
    {
        float floatTime;
    };

    /* 11121 */
    struct water_t
    {
        WaterWritable writable;
        float* H0X;
        float* H0Y;
        float* wTerm;
        int M;
        int N;
        float Lx;
        float Lz;
        float gravity;
        float windvel;
        float winddir[2];
        float amplitude;
        GfxImage* image;
    };

    /* 11122 */
    union MaterialTextureDefInfo
    {
        GfxImage* image;
        water_t* water;
    };

    /* 1302 */
    enum TextureSemantic : unsigned char
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
        TS_OCEANFLOW_DISPLACEMENT_MAP = 0xA,
        TS_WATER_MAP = 0xB,
        TS_OCEAN_DISPLACEMENT_MAP = 0xC,
        TS_DISPLACEMENT_MAP = 0xD,
        TS_PARALLAX_MAP = 0xE,

        TS_COUNT = 0xF,
    };

    /* 946 */
    enum GfxCameraRegionType : int32_t
    {
        CAMERA_REGION_LIT_OPAQUE = 0x0,
        CAMERA_REGION_LIT_TRANS = 0x1,
        CAMERA_REGION_EMISSIVE = 0x2,
        CAMERA_REGION_DEPTH_HACK = 0x3,
        CAMERA_REGION_LIGHT_MAP_OPAQUE = 0x4,
        CAMERA_REGION_COUNT = 0x5,
        CAMERA_REGION_NONE = 0x5,
    };

    struct MaterialTextureDefSamplerState
    {
        unsigned char filter : 3;
        unsigned char mipMap : 2;
        unsigned char clampU : 1;
        unsigned char clampV : 1;
        unsigned char clampW : 1;
    };

    /* 11123 */
    struct MaterialTextureDef
    {
        unsigned int nameHash;
        char nameStart;
        char nameEnd;
        MaterialTextureDefSamplerState samplerState;
        TextureSemantic semantic;
        MaterialTextureDefInfo u;
    };

    /* 11124 */
    struct MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        float literal[4];
    };

    /* 1285 */
    enum GfxStencilOp : int32_t
    {
        GFXS_STENCILOP_KEEP = 0x0,
        GFXS_STENCILOP_ZERO = 0x1,
        GFXS_STENCILOP_REPLACE = 0x2,
        GFXS_STENCILOP_INCRSAT = 0x3,
        GFXS_STENCILOP_DECRSAT = 0x4,
        GFXS_STENCILOP_INVERT = 0x5,
        GFXS_STENCILOP_INCR = 0x6,
        GFXS_STENCILOP_DECR = 0x7,
        GFXS_STENCILOP_COUNT = 0x8,
    };

    /* 1287 */
    enum GfxStencilFunc : int32_t
    {
        GFXS_STENCILFUNC_NEVER = 0x0,
        GFXS_STENCILFUNC_LESS = 0x1,
        GFXS_STENCILFUNC_EQUAL = 0x2,
        GFXS_STENCILFUNC_LESSEQUAL = 0x3,
        GFXS_STENCILFUNC_GREATER = 0x4,
        GFXS_STENCILFUNC_NOTEQUAL = 0x5,
        GFXS_STENCILFUNC_GREATEREQUAL = 0x6,
        GFXS_STENCILFUNC_ALWAYS = 0x7,
        GFXS_STENCILFUNC_COUNT = 0x8,
    };

    /* 1282 */
    enum GfxBlend : int32_t
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

    /* 1283 */
    enum GfxBlendOp : int32_t
    {
        GFXS_BLENDOP_DISABLED = 0x0,
        GFXS_BLENDOP_ADD = 0x1,
        GFXS_BLENDOP_SUBTRACT = 0x2,
        GFXS_BLENDOP_REVSUBTRACT = 0x3,
        GFXS_BLENDOP_MIN = 0x4,
        GFXS_BLENDOP_MAX = 0x5,
        GFXS_BLENDOP_MASK = 0x7,
    };

    /* 1280 */
    enum TextureFilter : int32_t
    {
        TEXTURE_FILTER_DISABLED = 0x0,
        TEXTURE_FILTER_NEAREST = 0x1,
        TEXTURE_FILTER_LINEAR = 0x2,
        TEXTURE_FILTER_ANISO2X = 0x3,
        TEXTURE_FILTER_ANISO4X = 0x4,
        TEXTURE_FILTER_CMP = 0x5,
        TEXTURE_FILTER_ANISO16X = 0x6,
    };

    /* 1281 */
    enum SamplerFilter : int32_t
    {
        SAMPLER_FILTER_SHIFT = 0x0,
        SAMPLER_FILTER_NEAREST = 0x1,
        SAMPLER_FILTER_LINEAR = 0x2,
        SAMPLER_FILTER_ANISO2X = 0x3,
        SAMPLER_FILTER_ANISO4X = 0x4,
        SAMPLER_FILTER_CMP = 0x5,
        SAMPLER_FILTER_ANISO16X = 0x6,
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

    /* 11125 */
    struct GfxStateBits
    {
        unsigned int loadBits[3];
    };

    /* 11126 */
    struct Material
    {
        MaterialInfo info;
        unsigned char stateBitsEntry[37];
        unsigned char textureCount;
        unsigned char constantCount;
        unsigned char stateBitsCount;
        unsigned char stateFlags;
        unsigned char cameraRegion;
        unsigned char materialType;
        unsigned char layerCount;
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
        const char** subMaterials;
    };

    /* 1820 */
    enum ERiffAudioFormat : char
    {
        SND_FORMAT_INVALID = 0x0,
        SND_FORMAT_PCM = 0x1,
        SND_FORMAT_XMA2 = 0x2,
        SND_FORMAT_ADPCM = 0x3,
        SND_FORMAT_XWMA = 0x4,
        SND_FORMAT_MP3 = 0x5,
        SND_FORMAT_WIIUDSP = 0x6,
        SND_FORMAT_FLAC = 0x7,
        SND_FORMAT_COUNT = 0x8,
    };

    /* 11172 */
    struct type_align32(4) RiffAudioInfo
    {
        unsigned int sampleRate;
        unsigned int dataByteCount;
        unsigned int numSamples;
        unsigned int avgBytesPerSec;
        unsigned short channels;
        unsigned short numBits;
        unsigned short blockAlign;
        unsigned short BlockCount;
        unsigned int PlayBegin;
        unsigned int PlayLength;
        unsigned int LoopBegin;
        unsigned int LoopLength;
        unsigned int BytesPerBlock;
        ERiffAudioFormat format;
    };

    /* 11173 */
    struct XA2Sound
    {
        unsigned char* data;
        unsigned int* seekTable;
        RiffAudioInfo format;
        int loadedSize;
        int seekTableSize;
    };

    /* 11174 */
    struct LoadedSound
    {
        const char* name;
        XA2Sound sound;
    };

    /* 11175 */
    struct StreamFileNameRaw
    {
        const char* dir;
        const char* name;
    };

    /* 11176 */
    struct StreamFileNamePacked
    {
        unsigned int offset;
        unsigned int length;
    };

    /* 11177 */
    union StreamFileInfo
    {
        StreamFileNameRaw raw;
        StreamFileNamePacked packed;
    };

    /* 11178 */
    struct StreamFileName
    {
        unsigned int fileIndex;
        StreamFileInfo info;
    };

    /* 11179 */
    struct StreamedSound
    {
        StreamFileName filename;
        unsigned int totalMsec;
    };

    /* 11180 */
    struct PrimedSound
    {
        StreamFileName filename;
        LoadedSound* loadedPart;
        int dataOffset;
        int totalSize;
        unsigned int primedCrc;
    };

    /* 11181 */
    union SoundFileRef
    {
        LoadedSound* loadSnd;
        StreamedSound streamSnd;
        PrimedSound primedSnd;
    };

    /* 1015 */
    enum snd_alias_type_t : uint8_t
    {
        SAT_UNKNOWN = 0x0,
        SAT_LOADED = 0x1,
        SAT_STREAMED = 0x2,
        SAT_PRIMED = 0x3,
        SAT_COUNT = 0x4,
    };

    /* 11182 */
    struct SoundFile
    {
        snd_alias_type_t type;
        unsigned char exists;
        SoundFileRef u;
    };

    /* 11183 */
    struct SndCurve
    {
        bool isDefault;
        const char* filename;
        unsigned short knotCount;
        float knots[16][2];
    };

    /* 11184 */
    struct SpeakerLevels
    {
        int speaker;
        int numLevels;
        float levels[2];
    };

    /* 11185 */
    struct ChannelMap
    {
        int speakerCount;
        SpeakerLevels speakers[6];
    };

    /* 11186 */
    struct SpeakerMap
    {
        bool isDefault;
        const char* name;
        ChannelMap channelMaps[2][2];
    };

    /* 11187 */
    struct OcclusionShape
    {
        bool isDefault;
        const char* name;
        int shapeKind;
        float innerAngle;
        float outerAngle;
        float occludedVolume;
    };

    /* 11188 */
    struct DopplerPreset
    {
        const char* p_name;
        float speedOfSound;
        float playerVelocityScale;
        float minPitch;
        float maxPitch;
        float smoothing;
    };

    /* 11189 */
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
        int volModIndex;
        float pitchMin;
        float pitchMax;
        float distMin;
        float distMax;
        float velocityMin;
        int flags;
        unsigned char masterPriority;
        float masterPercentage;
        float slavePercentage;
        float probability;
        float lfePercentage;
        float centerPercentage;
        int startDelay;
        SndCurve* volumeFalloffCurve;
        SndCurve* lpfCurve;
        SndCurve* reverbSendCurve;
        float envelopMin;
        float envelopMax;
        float envelopPercentage;
        SpeakerMap* speakerMap;
        float wetMixOverride;
        OcclusionShape* occlusionShape;
        unsigned char allowDoppler;
        DopplerPreset* dopplerPreset;
    };

    /* 11190 */
    struct snd_alias_list_t
    {
        const char* aliasName;
        snd_alias_t* head;
        int count;
    };

    /* 1508 */
    enum DynEntityType : int
    {
        DYNENT_TYPE_INVALID = 0x0,
        DYNENT_TYPE_CLUTTER = 0x1,
        DYNENT_TYPE_DESTRUCT = 0x2,
        DYNENT_TYPE_HINGE = 0x3,
        DYNENT_TYPE_SCRIPTABLEINST = 0x4,
        DYNENT_TYPE_SCRIPTABLEPHYSICS = 0x5,
        DYNENT_TYPE_COUNT = 0x6,
    };

    /* 11823 */
    struct GfxPlacement
    {
        float quat[4];
        float origin[3];
    };

    /* 11824 */
    struct DynEntityHingeDef
    {
        float axisOrigin[3];
        float axisDir[3];
        bool isLimited;
        float angleMin;
        float angleMax;
        float momentOfInertia;
        float friction;
    };

    /* 11825 */
    struct DynEntityDef
    {
        DynEntityType type;
        GfxPlacement pose;
        XModel* baseModel;
        unsigned short brushModel;
        unsigned short physicsBrushModel;
        unsigned short scriptableIndex;
        unsigned short health;
        FxEffectDef* destroyFx;
        PhysPreset* physPreset;
        DynEntityHingeDef* hinge;
        PhysMass mass;
        int contents;
    };

    /* 11826 */
    struct DynEntityPose
    {
        GfxPlacement pose;
        float radius;
    };

    /* 11827 */
    struct Hinge
    {
        float angle;
        float quat[4];
        float angularVel;
        float torqueAccum;
        bool active;
        float autoDisableTimeLeft;
        const DynEntityHingeDef* def;
        const PhysPreset* physPreset;
        float centerOfMassRelToAxisOriginAtAngleZero[3];
    };

    /* 11828 */
    struct DynEntityClient
    {
        int physObjId;
        unsigned short flags;
        unsigned short lightingHandle;
        unsigned short health;
        Hinge* hinge;
        XModel* activeModel;
    };

    /* 11829 */
    struct DynEntityColl
    {
        unsigned short sector;
        unsigned short nextEntInSector;
        float linkMins[2];
        float linkMaxs[2];
    };

    /* 11830 */
    struct ScriptableEventStateChangeDef
    {
        unsigned char targetIndex;
        unsigned char delayStreamIndex;
        unsigned short delayMin;
        unsigned short delayMax;
    };

    /* 11831 */
    struct ScriptableEventModelDef
    {
        XModel* model;
    };

    /* 11087 */
    struct FxFloatRange
    {
        float base;
        float amplitude;
    };

    /* 11160 */
    union FxEffectDefRef
    {
        FxEffectDef* handle;
        const char* name;
    };

    /* 11161 */
    struct type_align32(4) GfxLightImage
    {
        GfxImage* image;
        unsigned char samplerState;
    };

    /* 11162 */
    struct GfxLightDef
    {
        const char* name;
        GfxLightImage attenuation;
        GfxLightImage cucoloris;
        int lmapLookupStart;
    };

    /* 11163 */
    union FxElemVisuals
    {
        const void* anonymous;
        Material* material;
        XModel* model;
        FxEffectDefRef effectDef;
        const char* soundName;
        const char* vectorFieldName;
        GfxLightDef* lightDef;
    };

    /* 11127 */
    struct FxElemMarkVisuals
    {
        Material* materials[2];
    };

    /* 11164 */
    union FxElemDefVisuals
    {
        FxElemMarkVisuals* markArray;
        FxElemVisuals* array;
        FxElemVisuals instance;
    };

    /* 11165 */
    struct FxTrailVertex
    {
        float pos[2];
        float normal[2];
        float texCoord[2];
    };

    /* 11166 */
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
        unsigned short* inds;
    };

    /* 11167 */
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

    /* 11168 */
    struct FxSpotLightDef
    {
        float fovInnerFraction;
        float startRadius;
        float endRadius;
        float brightness;
        float maxLength;
        int exponent;
    };

    /* 11169 */
    struct FxFlareDef
    {
        float position;
        int angularRotCount;
        int flags;
        FxFloatRange depthScaleRange;
        FxFloatRange depthScaleValue;
        FxFloatRange radialRot;
        FxFloatRange radialScaleX;
        FxFloatRange radialScaleY;
        float dir[3];
        int intensityXIntervalCount;
        int intensityYIntervalCount;
        int srcCosIntensityIntervalCount;
        int srcCosScaleIntervalCount;
        float* intensityX;
        float* intensityY;
        float* srcCosIntensity;
        float* srcCosScale;
    };

    /* 11170 */
    union FxElemExtendedDefPtr
    {
        FxTrailDef* trailDef;
        FxSparkFountainDef* sparkFountainDef;
        FxSpotLightDef* spotLightDef;
        FxFlareDef* flareDef;
        void* unknownDef;
    };

    /* 11088 */
    struct FxSpawnDefLooping
    {
        int intervalMsec;
        int count;
    };

    /* 11089 */
    struct FxIntRange
    {
        int base;
        int amplitude;
    };

    /* 11090 */
    struct FxSpawnDefOneShot
    {
        FxIntRange count;
    };

    /* 11091 */
    union FxSpawnDef
    {
        FxSpawnDefLooping looping;
        FxSpawnDefOneShot oneShot;
    };

    /* 11092 */
    struct FxElemAtlas
    {
        unsigned char behavior;
        unsigned char index;
        unsigned char fps;
        unsigned char loopCount;
        unsigned char colIndexBits;
        unsigned char rowIndexBits;
        short entryCount;
    };

    /* 11093 */
    struct FxElemVec3Range
    {
        float base[3];
        float amplitude[3];
    };

    /* 11094 */
    struct FxElemVelStateInFrame
    {
        FxElemVec3Range velocity;
        FxElemVec3Range totalDelta;
    };

    /* 11095 */
    const struct FxElemVelStateSample
    {
        FxElemVelStateInFrame local;
        FxElemVelStateInFrame world;
    };

    /* 11096 */
    struct FxElemVisualState
    {
        unsigned char color[4];
        float rotationDelta;
        float rotationTotal;
        float size[2];
        float scale;
    };

    /* 11097 */
    const struct FxElemVisStateSample
    {
        FxElemVisualState base;
        FxElemVisualState amplitude;
    };

    /* 11171 */
    const struct FxElemDef
    {
        int flags;
        int flags2;
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
        unsigned char elemType;
        unsigned char visualCount;
        unsigned char velIntervalCount;
        unsigned char visStateIntervalCount;
        const FxElemVelStateSample* velSamples;
        const FxElemVisStateSample* visSamples;
        FxElemDefVisuals visuals;
        Bounds collBounds;
        FxEffectDefRef effectOnImpact;
        FxEffectDefRef effectOnDeath;
        FxEffectDefRef effectEmitted;
        FxFloatRange emitDist;
        FxFloatRange emitDistVariance;
        FxElemExtendedDefPtr extended;
        unsigned char sortOrder;
        unsigned char lightingFrac;
        unsigned char useItemClip;
        unsigned char fadeInfo;
        int randomSeed;
    };

    /* 1337 */
    enum FxElemClass : int32_t
    {
        FX_ELEM_CLASS_SPRITE = 0x0,
        FX_ELEM_CLASS_NONSPRITE = 0x1,
        FX_ELEM_CLASS_CLOUD = 0x2,
        FX_ELEM_CLASS_COUNT = 0x3,
    };

    /* 1341 */
    enum FxElemType : int32_t
    {
        FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
        FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
        FX_ELEM_TYPE_TAIL = 0x2,
        FX_ELEM_TYPE_TRAIL = 0x3,
        FX_ELEM_TYPE_FLARE = 0x4,
        FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 0x5,
        FX_ELEM_TYPE_CLOUD = 0x6,
        FX_ELEM_TYPE_SPARK_CLOUD = 0x7,
        FX_ELEM_TYPE_SPARK_FOUNTAIN = 0x8,
        FX_ELEM_TYPE_MODEL = 0x9,
        FX_ELEM_TYPE_OMNI_LIGHT = 0xA,
        FX_ELEM_TYPE_SPOT_LIGHT = 0xB,
        FX_ELEM_TYPE_SOUND = 0xC,
        FX_ELEM_TYPE_DECAL = 0xD,
        FX_ELEM_TYPE_RUNNER = 0xE,
        FX_ELEM_TYPE_VECTORFIELD = 0xF,
        FX_ELEM_TYPE_COUNT = 0x10,
        FX_ELEM_TYPE_LAST_SPRITE = 0x5,
        FX_ELEM_TYPE_LAST_DRAWN = 0xB,
    };

    /* 11159 */
    struct FxEffectDef
    {
        const char* name;
        int flags;
        int totalSize;
        int msecLoopingLife;
        int elemDefCountLooping;
        int elemDefCountOneShot;
        int elemDefCountEmission;
        float occlusionQueryDepthBias;
        int occlusionQueryFadeIn;
        int occlusionQueryFadeOut;
        FxFloatRange occlusionQueryScaleRange;
        FxElemDef* elemDefs;
    };

    /* 11832 */
    struct ScriptableEventFxDef
    {
        FxEffectDef* handle;
        ScriptString tagName;
        unsigned short loopTime;
        unsigned char loopTimeStreamIndex;
        bool tagUseAngles;
    };

    /* 11833 */
    struct ScriptableEventSoundDef
    {
        snd_alias_list_t* alias;
        bool looping;
    };

    /* 11834 */
    struct ScriptableEventAnimationDef
    {
        const char* animName;
        bool override;
        unsigned char animEntryIndex;
        unsigned short timeOffsetMin;
        unsigned short timeOffsetMax;
        unsigned short playbackRateMin;
        unsigned short playbackRateMax;
        unsigned short blendTime;
    };

    /* 11835 */
    struct ScriptableEventExplodeDef
    {
        unsigned short forceMin;
        unsigned short forceMax;
        unsigned short radius;
        unsigned short damageMin;
        unsigned short damageMax;
        bool aiAvoid;
    };

    /* 11836 */
    struct ScriptableEventHealthDef
    {
        unsigned short amount;
        unsigned short interval;
        unsigned short badPlaceRadius;
        unsigned char streamIndex;
    };

    /* 11837 */
    struct ScriptableEventPhysicsDef
    {
        XModel* model;
    };

    /* 11838 */
    struct ScriptableEventLightSettingsDef
    {
        unsigned char color[4];
        unsigned char lightIndexConstIndex;
        unsigned char transStateStreamIndex;
        unsigned char useColor;
        unsigned char useStateTransitionTime;
        unsigned short intensityScaleMin;
        unsigned short intensityScaleMax;
        unsigned short radiusScaleMin;
        unsigned short radiusScaleMax;
        const char* noteworthy;
        unsigned short transitionTimeMin;
        unsigned short transitionTimeMax;
    };

    /* 11839 */
    struct ScriptableEventSunlightSettingsDef
    {
        unsigned char color[4];
        unsigned char transStateStreamIndex;
        unsigned char flags;
        unsigned short intensityScaleMin;
        unsigned short intensityScaleMax;
        unsigned short pitchMin;
        unsigned short pitchMax;
        unsigned short headingMin;
        unsigned short headingMax;
        unsigned short transitionTimeMin;
        unsigned short transitionTimeMax;
    };

    /* 11840 */
    union ScriptableEventDataUnion
    {
        ScriptableEventStateChangeDef stateChange;
        ScriptableEventModelDef setModel;
        ScriptableEventFxDef playFx;
        ScriptableEventSoundDef playSound;
        ScriptableEventAnimationDef playAnim;
        ScriptableEventExplodeDef doExplosion;
        ScriptableEventHealthDef healthDrain;
        ScriptableEventPhysicsDef physicsLaunch;
        ScriptableEventLightSettingsDef lightSettings;
        ScriptableEventSunlightSettingsDef sunlightSettings;
    };

    /* 1491 */
    enum ScriptableEventType : int
    {
        SCRIPTABLE_EVENT_MODEL = 0x0,
        SCRIPTABLE_EVENT_FX = 0x1,
        SCRIPTABLE_EVENT_SOUND = 0x2,
        SCRIPTABLE_EVENT_ANIMATION = 0x3,
        SCRIPTABLE_EVENT_EXPLODE = 0x4,
        SCRIPTABLE_EVENT_HEALTHDRAIN = 0x5,
        SCRIPTABLE_EVENT_PHYSICSLAUNCH = 0x6,
        SCRIPTABLE_EVENT_LIGHTSETTINGS = 0x7,
        SCRIPTABLE_EVENT_STATECHANGE = 0x8,
        SCRIPTABLE_EVENT_COUNT = 0x9,
    };

    /* 11841 */
    struct ScriptableEventDef
    {
        ScriptableEventType type;
        ScriptableEventDataUnion data;
    };

    /* 11842 */
    struct ScriptableStateDef
    {
        ScriptString name;
        ScriptString tagName;
        ScriptableEventDef* onEnterEvents;
        unsigned char onEnterEventCount;
        unsigned char damageFlags;
        unsigned short maxHealth;
    };

    /* 11843 */
    struct ScriptablePartDef
    {
        ScriptableStateDef* states;
        ScriptString name;
        unsigned char stateCount;
        unsigned char flags;
        unsigned char eventStreamTimeRemainIndex;
        unsigned char eventStreamNextChangeTimeIndex;
    };

    /* 11844 */
    struct ScriptableDef
    {
        const char* name;
        XModel* baseModel;
        const char* baseCollisionBrush;
        const char* destroyedCollisionBrush;
        ScriptablePartDef* parts;
        unsigned char partCount;
        unsigned char flags;
        unsigned char serverPartCount;
        unsigned char eventStreamSize;
        unsigned char eventConstantsSize;
    };

    /* 11845 */
    struct ScriptableInstancePartState
    {
        unsigned char flags;
        unsigned char stateIndex;
        unsigned short curHealth;
    };

    /* 11846 */
    struct ScriptableInstance
    {
        ScriptableDef* def;
        float origin[3];
        float angles[3];
        ScriptString targetname;
        unsigned short preBrushModel;
        unsigned short postBrushModel;
        unsigned char flags;
        XModel* currentModel;
        unsigned int currentPartBits[6];
        ScriptableInstancePartState* partStates;
        unsigned char* eventStreamBuf;
        unsigned char* eventConstantsBuf;
        unsigned short updateNextInstance;
        unsigned short linkedObject;
        unsigned short damageOwner;
    };

    /* 11847 */
    struct ScriptableAnimationEntry
    {
        const char* animName;
        uint64_t runtimeBuf;
    };

    /* 11848 */
    struct ScriptableMapEnts
    {
        unsigned int instanceStateSize;
        unsigned int instanceCount;
        ScriptableInstance* instances;
        unsigned int animEntryCount;
        ScriptableAnimationEntry* animEntries;
        unsigned int replicatedInstanceCount;
    };

    /* 11798 */
    struct cLeafBrushNodeLeaf_t
    {
        unsigned short* brushes;
    };

    /* 11799 */
    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        unsigned short childOffset[2];
    };

    /* 11800 */
    union cLeafBrushNodeData_t
    {
        cLeafBrushNodeLeaf_t leaf;
        cLeafBrushNodeChildren_t children;
    };

    /* 11801 */
    struct cLeafBrushNode_s
    {
        unsigned char axis;
        short leafBrushCount;
        int contents;
        cLeafBrushNodeData_t data;
    };

    /* 11797 */
    struct ClipMaterial
    {
        const char* name;
        int surfaceFlags;
        int contents;
    };

    /* 11802 */
    struct ClipInfo
    {
        int planeCount;
        cplane_s* planes;
        unsigned int numMaterials;
        ClipMaterial* materials;
        unsigned int numBrushSides;
        cbrushside_t* brushsides;
        unsigned int numBrushEdges;
        unsigned char* brushEdges;
        unsigned int leafbrushNodesCount;
        cLeafBrushNode_s* leafbrushNodes;
        unsigned int numLeafBrushes;
        unsigned short* leafbrushes;
        unsigned short numBrushes;
        cbrush_t* brushes;
        Bounds* brushBounds;
        int* brushContents;
    };

    /* 11803 */
    struct cStaticModel_s
    {
        XModel* xmodel;
        float origin[3];
        float invScaledAxis[3][3];
        Bounds absBounds;
    };

    /* 11804 */
    struct cNode_t
    {
        cplane_s* plane;
        short children[2];
    };

    /* 11805 */
    struct cLeaf_t
    {
        unsigned short firstCollAabbIndex;
        unsigned short collAabbCount;
        int brushContents;
        int terrainContents;
        Bounds bounds;
        int leafBrushNode;
    };

    /* 11806 */
    struct CollisionBorder
    {
        float distEq[3];
        float zBase;
        float zSlope;
        float start;
        float length;
    };

    /* 11807 */
    struct CollisionPartition
    {
        unsigned char triCount;
        unsigned char borderCount;
        unsigned char firstVertSegment;
        int firstTri;
        CollisionBorder* borders;
    };

    /* 11808 */
    union CollisionAabbTreeIndex
    {
        int firstChildIndex;
        int partitionIndex;
    };

    /* 11809 */
    struct CollisionAabbTree
    {
        float midPoint[3];
        unsigned short materialIndex;
        unsigned short childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    /* 11810 */
    struct cmodel_t
    {
        Bounds bounds;
        float radius;
        ClipInfo* info;
        cLeaf_t leaf;
    };

    /* 11811 */
    struct TriggerModel
    {
        int contents;
        unsigned short hullCount;
        unsigned short firstHull;
    };

    /* 11812 */
    struct TriggerHull
    {
        Bounds bounds;
        int contents;
        unsigned short slabCount;
        unsigned short firstSlab;
    };

    /* 11813 */
    struct TriggerSlab
    {
        float dir[3];
        float midPoint;
        float halfSize;
    };

    /* 11814 */
    struct MapTriggers
    {
        unsigned int count;
        TriggerModel* models;
        unsigned int hullCount;
        TriggerHull* hulls;
        unsigned int slabCount;
        TriggerSlab* slabs;
    };

    /* 11815 */
    struct ClientTriggerAabbNode
    {
        Bounds bounds;
        unsigned short firstChild;
        unsigned short childCount;
    };

    /* 11816 */
    struct ClientTriggers
    {
        MapTriggers trigger;
        unsigned short numClientTriggerNodes;
        ClientTriggerAabbNode* clientTriggerAabbTree;
        unsigned int triggerStringLength;
        char* triggerString;
        short* visionSetTriggers;
        unsigned char* triggerType;
        float (*origins)[3];
        float* scriptDelay;
        short* audioTriggers;
        short* blendLookup;
        short* npcTriggers;
    };

    /* 11817 */
    struct ClientTriggerBlendNode
    {
        float pointA[3];
        float pointB[3];
        unsigned short triggerA;
        unsigned short triggerB;
    };

    /* 11818 */
    struct ClientTriggerBlend
    {
        unsigned short numClientTriggerBlendNodes;
        ClientTriggerBlendNode* blendNodes;
    };

    /* 11474 */
    const struct SpawnPointEntityRecord
    {
        unsigned short index;
        ScriptString name;
        ScriptString target;
        ScriptString script_noteworthy;
        float origin[3];
        float angles[3];
    };

    /* 11819 */
    struct SpawnPointRecordList
    {
        unsigned short spawnsCount;
        SpawnPointEntityRecord* spawns;
    };

    /* 11820 */
    struct MapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
        ClientTriggers clientTrigger;
        ClientTriggerBlend clientTriggerBlend;
        SpawnPointRecordList spawnList;
    };

    /* 11821 */
    struct Stage
    {
        const char* name;
        float origin[3];
        unsigned short triggerIndex;
        unsigned char sunPrimaryLightIndex;
    };

    /* 11822 */
    struct SModelAabbNode
    {
        Bounds bounds;
        unsigned short firstChild;
        unsigned short childCount;
    };

    /* 11849 */
    struct clipMap_t
    {
        const char* name;
        int isInUse;
        ClipInfo info;
        ClipInfo* pInfo;
        unsigned int numStaticModels;
        cStaticModel_s* staticModelList;
        unsigned int numNodes;
        cNode_t* nodes;
        unsigned int numLeafs;
        cLeaf_t* leafs;
        unsigned int vertCount;
        vec3_t* verts;
        int triCount;
        unsigned short* triIndices;
        unsigned char* triEdgeIsWalkable;
        int borderCount;
        CollisionBorder* borders;
        int partitionCount;
        CollisionPartition* partitions;
        int aabbTreeCount;
        CollisionAabbTree* aabbTrees;
        unsigned int numSubModels;
        cmodel_t* cmodels;
        MapEnts* mapEnts;
        Stage* stages;
        unsigned char stageCount;
        MapTriggers stageTrigger;
        unsigned short smodelNodeCount;
        SModelAabbNode* smodelNodes;
        unsigned short dynEntCount[2];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityColl* dynEntCollList[2];
        ScriptableMapEnts scriptableMapEnts;
        unsigned int checksum;
    };

    /* 11850 */
    struct ComPrimaryLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        unsigned char exponent;
        unsigned char unused;
        float color[3];
        float dir[3];
        float up[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        float cosHalfFovExpanded;
        float rotationLimit;
        float translationLimit;
        const char* defName;
    };

    /* 11851 */
    struct ComWorld
    {
        const char* name;
        int isInUse;
        unsigned int primaryLightCount;
        ComPrimaryLight* primaryLights;
    };

    /* 11852 */
    struct G_GlassPiece
    {
        unsigned short damageTaken;
        unsigned short collapseTime;
        int lastStateChangeTime;
        unsigned char impactDir;
        unsigned char impactPos[2];
    };

    /* 11853 */
    struct G_GlassName
    {
        char* nameStr;
        ScriptString name;
        unsigned short pieceCount;
        unsigned short* pieceIndices;
    };

    /* 11854 */
    struct G_GlassData
    {
        G_GlassPiece* glassPieces;
        unsigned int pieceCount;
        unsigned short damageToWeaken;
        unsigned short damageToDestroy;
        unsigned int glassNameCount;
        G_GlassName* glassNames;
        unsigned char pad[108];
    };

    /* 11855 */
    struct GlassWorld
    {
        const char* name;
        G_GlassData* g_glassData;
    };

    /* 11856 */
    struct pathbasenode_t
    {
        float vOrigin[3];
        unsigned int type;
    };

    /* 11858 */
    struct pathnode_tree_nodes_t
    {
        int nodeCount;
        unsigned short* nodes;
    };

    struct pathnode_tree_t;

    /* 11859 */
    union pathnode_tree_info_t
    {
        pathnode_tree_t* child[2];
        pathnode_tree_nodes_t s;
    };

    /* 11857 */
    struct pathnode_tree_t
    {
        int axis;
        float dist;
        pathnode_tree_info_t u;
    };

    /* 11860 */
    struct PathDynamicNodeGroup
    {
        unsigned short parentIndex;
        int nodeTreeCount;
        pathnode_tree_t* nodeTree;
    };

    /* 11003 */
    struct pathnode_yaworient_t
    {
        float fLocalAngle;
        float localForward[2];
    };

    /* 11004 */
    struct pathlink_s
    {
        float fDist;
        unsigned short nodeNum;
        unsigned char disconnectCount;
        unsigned char negotiationLink;
        unsigned char flags;
        unsigned char ubBadPlaceCount[3];
    };

    /* 11005 */
    union pathnode_angles_t
    {
        pathnode_yaworient_t yaw_orient;
        float angles[3];
    };

    /* 11006 */
    union pathnode_parent_t
    {
        unsigned short parentIndex;
        ScriptString parentName;
    };

    /* 1697 */
    enum PathNodeErrorCode : int
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
        NUM_PATH_NODE_ERRORS = 0x9,
    };

    /* 11007 */
    union pathnode_error_t
    {
        float minUseDistSq;
        PathNodeErrorCode error;
    };

    /* 11008 */
    struct pathnode_constant_t
    {
        unsigned short type;
        unsigned int spawnflags;
        ScriptString targetname;
        ScriptString script_linkName;
        ScriptString script_noteworthy;
        ScriptString target;
        ScriptString animscript;
        int animscriptfunc;
        float vLocalOrigin[3];
        pathnode_angles_t ___u9;
        pathnode_parent_t ___u10;
        pathnode_error_t ___u11;
        short wOverlapNode[2];
        unsigned short totalLinkCount;
        pathlink_s* Links;
    };

    /* 11009 */
    struct SentientHandle
    {
        unsigned short number;
        unsigned short infoIndex;
    };

    /* 11010 */
    struct pathnode_dynamic_t
    {
        SentientHandle pOwner;
        int iFreeTime;
        int iValidTime[3];
        short wLinkCount;
        short wOverlapCount;
        short turretEntNumber;
        unsigned char userCount;
        unsigned char hasBadPlaceLink;
        int spreadUsedTime[2];
        short flags;
        short dangerousCount;
        int recentUseProxTime;
    };

    /* 11012 */
    union pathnode_transient_cost_t
    {
        float nodeCost;
        int linkIndex;
    };

    struct pathnode_t;

    /* 11013 */
    struct pathnode_transient_t
    {
        int iSearchFrame;
        pathnode_t* pNextOpen;
        pathnode_t* pPrevOpen;
        pathnode_t* pParent;
        float fCost;
        float fHeuristic;
        pathnode_transient_cost_t ___u6;
    };

    /* 11011 */
    struct pathnode_t
    {
        pathnode_constant_t constant;
        pathnode_dynamic_t dynamic;
        pathnode_transient_t transient;
    };

    /* 11861 */
    struct PathData
    {
        const char* name;
        unsigned int nodeCount;
        pathnode_t* nodes;
        pathbasenode_t* basenodes;
        bool parentIndexResolved;
        unsigned short version;
        int visBytes;
        unsigned char* pathVis;
        int nodeTreeCount;
        pathnode_tree_t* nodeTree;
        int dynamicNodeGroupCount;
        PathDynamicNodeGroup* dynamicNodeGroups;
        int exposureBytes;
        unsigned char* pathExposure;
        int noPeekVisBytes;
        unsigned char* pathNoPeekVis;
        int zoneCount;
        int zonesBytes;
        unsigned char* pathZones;
        int dynStatesBytes;
        unsigned char* pathDynStates;
    };

    /* 11044 */
    struct VehicleTrackObstacle
    {
        float origin[2];
        float radius;
    };

    /* 11045 */
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

    /* 11046 */
    const struct VehicleTrackSegment
    {
        const char* targetName;
        VehicleTrackSector* sectors;
        unsigned int sectorCount;
        const VehicleTrackSegment** nextBranches;
        unsigned int nextBranchesCount;
        const VehicleTrackSegment** prevBranches;
        unsigned int prevBranchesCount;
        float endEdgeDir[2];
        float endEdgeDist;
        float totalLength;
    };

    /* 11862 */
    struct VehicleTrack
    {
        const char* name;
        VehicleTrackSegment* segments;
        unsigned int segmentCount;
    };

    /* 11863 */
    struct FxGlassDef
    {
        float halfThickness;
        float texVecs[2][2];
        GfxColor color;
        Material* material;
        Material* materialShattered;
        PhysPreset* physPreset;
        FxEffectDef* pieceBreakEffect;
        FxEffectDef* shatterEffect;
        FxEffectDef* shatterSmallEffect;
        FxEffectDef* crackDecalEffect;
        float invHighMipRadius;
        float shatteredInvHighMipRadius;
        int numCrackRings;
    };

    /* 11357 */
    struct FxSpatialFrame
    {
        float quat[4];
        float origin[3];
    };

    /* 11864 */
    struct FxGlassPieceFrame
    {
        FxSpatialFrame frame;
        float radius;
    };

    /* 11865 */
    union FxGlassPiecePlace
    {
        FxGlassPieceFrame __s0;
        unsigned int nextFree;
    };

    /* 11866 */
    struct FxGlassPieceState
    {
        float texCoordOrigin[2];
        unsigned int supportMask;
        unsigned short initIndex;
        unsigned short geoDataStart;
        unsigned char defIndex;
        unsigned char pad[5];
        unsigned char vertCount;
        unsigned char holeDataCount;
        unsigned char crackDataCount;
        unsigned char fanDataCount;
        unsigned short flags;
        float areaX2;
    };

    /* 11867 */
    struct FxGlassPieceDynamics
    {
        int fallTime;
        int physObjId;
        int physJointId;
        float vel[3];
        float avel[3];
    };

    /* 11868 */
    struct FxGlassVertex
    {
        short x;
        short y;
    };

    /* 11869 */
    struct FxGlassHoleHeader
    {
        unsigned short uniqueVertCount;
        unsigned char touchVert;
        unsigned char pad[1];
    };

    /* 11870 */
    struct FxGlassCrackHeader
    {
        unsigned short uniqueVertCount;
        unsigned char beginVertIndex;
        unsigned char endVertIndex;
    };

    /* 11871 */
    union FxGlassGeometryData
    {
        FxGlassVertex vert;
        FxGlassHoleHeader hole;
        FxGlassCrackHeader crack;
        unsigned char asBytes[4];
        short anonymous[2];
    };

    /* 11872 */
    struct FxGlassInitPieceState
    {
        FxSpatialFrame frame;
        float radius;
        float texCoordOrigin[2];
        unsigned int supportMask;
        float areaX2;
        unsigned char defIndex;
        unsigned char vertCount;
        unsigned char fanDataCount;
        unsigned char pad[1];
    };

    /* 11873 */
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
        unsigned int* isInUse;
        unsigned int* cellBits;
        unsigned char* visData;
        float (*linkOrg)[3];
        float* halfThickness;
        unsigned short* lightingHandles;
        FxGlassInitPieceState* initPieceStates;
        FxGlassGeometryData* initGeoData;
        bool needToCompactData;
        unsigned char initCount;
        float effectChanceAccum;
        int lastPieceDeletionTime;
    };

    /* 11874 */
    struct FxWorld
    {
        const char* name;
        FxGlassSystem glassSys;
    };

    /* 11910 */
    struct srfTriangles_t
    {
        unsigned int vertexLayerData;
        unsigned int firstVertex;
        unsigned short vertexCount;
        unsigned short triCount;
        unsigned int baseIndex;
    };

    /* 11911 */
    struct GfxSurfaceLightingAndFlagsFields
    {
        unsigned char lightmapIndex;
        unsigned char reflectionProbeIndex;
        unsigned char primaryLightIndex;
        unsigned char flags;
    };

    /* 11912 */
    union GfxSurfaceLightingAndFlags
    {
        GfxSurfaceLightingAndFlagsFields fields;
        unsigned int packed;
    };

    /* 11913 */
    struct GfxSurface
    {
        srfTriangles_t tris;
        Material* material;
        GfxSurfaceLightingAndFlags laf;
    };

    /* 11914 */
    struct GfxSurfaceBounds
    {
        Bounds bounds;
        unsigned short mipRadius;
        unsigned char invHighMipRadius[5];
    };

    /* 11915 */
    struct GfxPackedPlacement
    {
        float origin[3];
        PackedUnitVec axis[3];
        float scale;
    };

    /* 11916 */
    struct GfxStaticModelVertexLighting
    {
        unsigned char ambientColorUint8[4];
        unsigned char highlightColorUint8[4];
        unsigned char highlightDir[4];
    };

    /* 11917 */
    struct GfxStaticModelVertexLightingInfo
    {
        GfxStaticModelVertexLighting* lightingValues;
        D3DVertexBuffer lightingValuesVb;
        unsigned int numLightingValues;
    };

    /* 11918 */
    struct GfxStaticModelDrawInst
    {
        GfxPackedPlacement placement;
        XModel* model;
        GfxColor groundLighting;
        GfxStaticModelVertexLightingInfo vertexLightingInfo;
        unsigned short lightingHandle;
        unsigned short cullDist;
        unsigned short flags;
        unsigned short staticModelId;
        unsigned char primaryLightIndex;
        unsigned char reflectionProbeIndex;
        unsigned char firstMtlSkinIndex;
    };

    /* 11909 */
    struct GfxStaticModelInst
    {
        Bounds bounds;
        float lightingOrigin[3];
    };

    /* 11919 */
    struct GfxWorldDpvsStatic
    {
        unsigned int smodelCount;
        unsigned int staticSurfaceCount;
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
        unsigned char* smodelVisData[3];
        unsigned char* surfaceVisData[3];
        unsigned short* sortedSurfIndex;
        GfxStaticModelInst* smodelInsts;
        GfxSurface* surfaces;
        GfxSurfaceBounds* surfacesBounds;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxDrawSurf* surfaceMaterials;
        unsigned int* surfaceCastsSunShadow;
        volatile int usageCount;
    };

    /* 11920 */
    struct GfxWorldDpvsDynamic
    {
        unsigned int dynEntClientWordCount[2];
        unsigned int dynEntClientCount[2];
        unsigned int* dynEntCellBits[2];
        unsigned char* dynEntVisData[2][3];
    };

    /* 11921 */
    struct GfxHeroOnlyLight
    {
        unsigned char type;
        unsigned char unused[3];
        float color[3];
        float dir[3];
        float up[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
    };

    /* 11901 */
    struct XModelDrawInfo
    {
        unsigned char hasGfxEntIndex;
        unsigned char lod;
        unsigned short surfId;
    };

    /* 11902 */
    struct GfxSceneDynModel
    {
        XModelDrawInfo info;
        unsigned short dynEntId;
    };

    /* 11903 */
    struct BModelDrawInfo
    {
        unsigned short surfId;
    };

    /* 11904 */
    struct GfxSceneDynBrush
    {
        BModelDrawInfo info;
        unsigned short dynEntId;
    };

    /* 11905 */
    struct GfxShadowGeometry
    {
        unsigned short surfaceCount;
        unsigned short smodelCount;
        unsigned short* sortedSurfIndex;
        unsigned short* smodelIndex;
    };

    /* 11906 */
    struct GfxLightRegionAxis
    {
        float dir[3];
        float midPoint;
        float halfSize;
    };

    /* 11907 */
    struct GfxLightRegionHull
    {
        float kdopMidPoint[9];
        float kdopHalfSize[9];
        unsigned int axisCount;
        GfxLightRegionAxis* axis;
    };

    /* 11908 */
    struct GfxLightRegion
    {
        unsigned int hullCount;
        GfxLightRegionHull* hulls;
    };

    /* 11875 */
    struct type_align32(4) GfxSky
    {
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        unsigned char skySamplerState;
    };

    /* 11876 */
    struct GfxWorldDpvsPlanes
    {
        int cellCount;
        cplane_s* planes;
        unsigned short* nodes;
        unsigned int* sceneEntCellBits;
    };

    /* 11877 */
    struct GfxCellTreeCount
    {
        int aabbTreeCount;
    };

    /* 11878 */
    struct GfxAabbTree
    {
        Bounds bounds;
        unsigned short childCount;
        unsigned short surfaceCount;
        unsigned short startSurfIndex;
        unsigned short smodelIndexCount;
        unsigned short* smodelIndexes;
        int childrenOffset;
    };

    /* 11879 */
    struct GfxCellTree
    {
        GfxAabbTree* aabbTree;
    };

    struct GfxPortal;

    /* 11881 */
    struct GfxPortalWritable
    {
        bool isQueued;
        bool isAncestor;
        unsigned char recursionDepth;
        unsigned char hullPointCount;
        float (*hullPoints)[2];
        GfxPortal* queuedParent;
    };

    /* 11882 */
    struct DpvsPlane
    {
        float coeffs[4];
    };

    /* 11880 */
    struct GfxPortal
    {
        GfxPortalWritable writable;
        DpvsPlane plane;
        float (*vertices)[3];
        unsigned short cellIndex;
        unsigned char vertexCount;
        float hullAxis[2][3];
    };

    /* 11883 */
    struct GfxCell
    {
        Bounds bounds;
        int portalCount;
        GfxPortal* portals;
        unsigned char reflectionProbeCount;
        unsigned char* reflectionProbes;
        unsigned char reflectionProbeReferenceCount;
        unsigned char* reflectionProbeReferences;
    };

    /* 11884 */
    struct GfxReflectionProbeVolumeData
    {
        float volumePlanes[6][4];
    };

    /* 11885 */
    struct GfxReflectionProbe
    {
        float origin[3];
        GfxReflectionProbeVolumeData* probeVolumes;
        unsigned int probeVolumeCount;
    };

    /* 11886 */
    struct GfxReflectionProbeReferenceOrigin
    {
        float origin[3];
    };

    /* 11887 */
    struct GfxReflectionProbeReference
    {
        unsigned char index;
    };

    /* 11888 */
    struct GfxLightmapArray
    {
        GfxImage* primary;
        GfxImage* secondary;
    };

    /* 11889 */
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

    /* 11890 */
    struct GfxWorldVertexData
    {
        GfxWorldVertex* vertices;
        D3DVertexBuffer worldVb;
    };

    /* 11891 */
    struct GfxWorldVertexLayerData
    {
        unsigned char* data;
        D3DVertexBuffer layerVb;
    };

    /* 11892 */
    struct GfxWorldDraw
    {
        unsigned int reflectionProbeCount;
        GfxImage** reflectionProbes;
        GfxReflectionProbe* reflectionProbeOrigins;
        GfxTexture* reflectionProbeTextures;
        unsigned int reflectionProbeReferenceCount;
        GfxReflectionProbeReferenceOrigin* reflectionProbeReferenceOrigins;
        GfxReflectionProbeReference* reflectionProbeReferences;
        int lightmapCount;
        GfxLightmapArray* lightmaps;
        GfxTexture* lightmapPrimaryTextures;
        GfxTexture* lightmapSecondaryTextures;
        GfxImage* lightmapOverridePrimary;
        GfxImage* lightmapOverrideSecondary;
        unsigned int trisType;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        unsigned int indexCount;
        unsigned short* indices;
        D3DIndexBuffer indexBuffer;
    };

    /* 11893 */
    struct GfxLightGridEntry
    {
        unsigned short colorsIndex;
        unsigned char primaryLightIndex;
        unsigned char needsTrace;
    };

    /* 11894 */
    struct GfxLightGridColors
    {
        unsigned char rgb[56][3];
    };

    /* 11895 */
    struct GfxLightGridTree
    {
        unsigned char maxDepth;
        int nodeCount;
        int leafCount;
        int coordMinGridSpace[3];
        int coordMaxGridSpace[3];
        int coordHalfSizeGridSpace[3];
        int defaultColorIndexBitCount;
        int defaultLightIndexBitCount;
        unsigned int* p_nodeTable;
        int leafTableSize;
        unsigned char* p_leafTable;
    };

    /* 11896 */
    struct GfxLightGrid
    {
        bool hasLightRegions;
        bool useSkyForLowZ;
        unsigned int lastSunPrimaryLightIndex;
        unsigned short mins[3];
        unsigned short maxs[3];
        unsigned int rowAxis;
        unsigned int colAxis;
        unsigned short* rowDataStart;
        unsigned int rawRowDataSize;
        unsigned char* rawRowData;
        unsigned int entryCount;
        GfxLightGridEntry* entries;
        unsigned int colorCount;
        GfxLightGridColors* colors;
        unsigned int missingGridColorIndex;
        int tableVersion;
        int paletteVersion;
        char rangeExponent8BitsEncoding;
        char rangeExponent12BitsEncoding;
        char rangeExponent16BitsEncoding;
        unsigned char stageCount;
        float* stageLightingContrastGain;
        unsigned int paletteEntryCount;
        int* paletteEntryAddress;
        unsigned int paletteBitstreamSize;
        unsigned char* paletteBitstream;
        GfxLightGridColors skyLightGridColors;
        GfxLightGridColors defaultLightGridColors;
        GfxLightGridTree tree;
    };

    /* 11897 */
    struct GfxBrushModelWritable
    {
        Bounds bounds;
    };

    /* 11898 */
    struct GfxBrushModel
    {
        GfxBrushModelWritable writable;
        Bounds bounds;
        float radius;
        unsigned short startSurfIndex;
        unsigned short surfaceCount;
    };

    /* 11899 */
    struct MaterialMemory
    {
        Material* material;
        int memory;
    };

    /* 11900 */
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

    /* 11922 */
    struct type_align32(4) GfxWorld
    {
        const char* name;
        const char* baseName;
        unsigned int bspVersion;
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
        GfxCellTreeCount* aabbTreeCounts;
        GfxCellTree* aabbTrees;
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
        unsigned int* cellCasterBits;
        unsigned int* cellHasSunLitSurfsBits;
        GfxSceneDynModel* sceneDynModel;
        GfxSceneDynBrush* sceneDynBrush;
        unsigned int* primaryLightEntityShadowVis;
        unsigned int* primaryLightDynEntShadowVis[2];
        unsigned char* nonSunPrimaryLightForModelDynEnt;
        GfxShadowGeometry* shadowGeom;
        GfxLightRegion* lightRegion;
        GfxWorldDpvsStatic dpvs;
        GfxWorldDpvsDynamic dpvsDyn;
        unsigned int mapVtxChecksum;
        unsigned int heroOnlyLightCount;
        GfxHeroOnlyLight* heroOnlyLights;
        unsigned char fogTypesAllowed;
    };

    /* 11489 */
    struct Glyph
    {
        unsigned short letter;
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

    /* 11490 */
    struct Font_s
    {
        const char* fontName;
        int pixelHeight;
        int glyphCount;
        Material* material;
        Material* glowMaterial;
        Glyph* glyphs;
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

    /* 11953 */
    struct ItemFloatExpression
    {
        int target;
        Statement_s* expression;
    };

    /* 11924 */
    struct ExpressionString
    {
        const char* string;
    };

    /* 11926 */
    union operandInternalDataUnion
    {
        int intVal;
        float floatVal;
        ExpressionString stringVal;
        Statement_s* function;
    };

    /* 1103 */
    enum expDataType : int
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
        NUM_INTERNAL_DATATYPES = 0x3,
        VAL_FUNCTION = 0x3,
        NUM_DATATYPES = 0x4,
    };

    /* 11927 */
    struct Operand
    {
        operandInternalDataUnion internals;
        expDataType dataType;
    };

    /* 1117 */
    enum operationEnum : int
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
        OP_STATICDVARINT = 0x17,
        OP_FIRSTFUNCTIONCALL = 0x17,
        OP_STATICDVARBOOL = 0x18,
        OP_STATICDVARFLOAT = 0x19,
        OP_STATICDVARSTRING = 0x1A,
        OP_TOINT = 0x1B,
        OP_TOSTRING = 0x1C,
        OP_TOFLOAT = 0x1D,
        LAST_COMMONLY_CALLED_FUNCTION = 0x1D,
        OP_SIN = 0x1E,
        OP_COS = 0x1F,
        OP_MIN = 0x20,
        OP_MAX = 0x21,
        OP_MILLISECONDS = 0x22,
        OP_LOCALCLIENTUIMILLISECONDS = 0x23,
        OP_DVARINT = 0x24,
        OP_DVARBOOL = 0x25,
        OP_DVARFLOAT = 0x26,
        OP_DVARSTRING = 0x27,
        OP_UIACTIVE = 0x28,
        OP_FLASHBANGED = 0x29,
        OP_USINGVEHICLE = 0x2A,
        OP_MISSILECAM = 0x2B,
        OP_SCOPED = 0x2C,
        OP_SCOPEDTHERMAL = 0x2D,
        OP_SCOREBOARDVISIBLE = 0x2E,
        OP_INKILLCAM = 0x2F,
        OP_INKILLCAM_NPC = 0x30,
        OP_PLAYERFIELD = 0x31,
        OP_GET_PLAYER_PERK = 0x32,
        OP_SELECTINGLOCATION = 0x33,
        OP_SELECTINGDIRECTION = 0x34,
        OP_TEAMFIELD = 0x35,
        OP_OTHERTEAMFIELD = 0x36,
        OP_MARINESFIELD = 0x37,
        OP_OPFORFIELD = 0x38,
        OP_MENUISOPEN = 0x39,
        OP_WRITINGDATA = 0x3A,
        OP_INLOBBY = 0x3B,
        OP_INGAMELOBBY = 0x3C,
        OP_INPRIVATEPARTY = 0x3D,
        OP_PRIVATEPARTYHOST = 0x3E,
        OP_PRIVATEPARTYHOSTINLOBBY = 0x3F,
        OP_ALONEINPARTY = 0x40,
        OP_ADSJAVELIN = 0x41,
        OP_WEAPLOCKBLINK = 0x42,
        OP_WEAPATTACKTOP = 0x43,
        OP_WEAPATTACKDIRECT = 0x44,
        OP_WEAPLOCKING = 0x45,
        OP_WEAPLOCKED = 0x46,
        OP_WEAPLOCKTOOCLOSE = 0x47,
        OP_WEAPLOCKSCREENPOSX = 0x48,
        OP_WEAPLOCKSCREENPOSY = 0x49,
        OP_SECONDSASTIME = 0x4A,
        OP_TABLELOOKUP = 0x4B,
        OP_TABLELOOKUPBYROW = 0x4C,
        OP_TABLEGETROWNUM = 0x4D,
        OP_LOCALIZESTRING = 0x4E,
        OP_LOCALVARINT = 0x4F,
        OP_LOCALVARBOOL = 0x50,
        OP_LOCALVARFLOAT = 0x51,
        OP_LOCALVARSTRING = 0x52,
        OP_TIMELEFT = 0x53,
        OP_SECONDSASCOUNTDOWN = 0x54,
        OP_GAMEMSGWNDACTIVE = 0x55,
        OP_GAMETYPENAME = 0x56,
        OP_GAMETYPE = 0x57,
        OP_GAMETYPEDESCRIPTION = 0x58,
        OP_SCORE = 0x59,
        OP_FOLLOWING = 0x5A,
        OP_SPECTATINGFREE = 0x5B,
        OP_KEYBINDING = 0x5C,
        OP_ACTIONSLOTUSABLE = 0x5D,
        OP_HUDFADE = 0x5E,
        OP_MAXPLAYERS = 0x5F,
        OP_ACCEPTINGINVITE = 0x60,
        OP_ISINTERMISSION = 0x61,
        OP_GAMEHOST = 0x62,
        OP_PARTYHASMISSINGMAPPACK = 0x63,
        OP_PARTYMISSINGMAPPACKERROR = 0x64,
        OP_ANYNEWMAPPACKS = 0x65,
        OP_AMISELECTED = 0x66,
        OP_PARTYSTATUSSTRING = 0x67,
        OP_ATTACHED_CONTROLLER_COUNT = 0x68,
        OP_IS_SPLIT_SCREEN_ONLINE_POSSIBLE = 0x69,
        OP_SPLITSCREENPLAYERCOUNT = 0x6A,
        OP_GETPLAYERDATA = 0x6B,
        OP_GETPLAYERDATASPLITSCREEN = 0x6C,
        OP_GET_MATCHRULES_DATA = 0x6D,
        OP_GET_SAVED_MATCHRULES_METADATA = 0x6E,
        OP_LEVEL_FOR_EXPERIENCE_MP = 0x6F,
        OP_LEVEL_FOR_EXPERIENCE_SO = 0x70,
        OP_IS_ITEM_UNLOCKED = 0x71,
        OP_IS_ITEM_UNLOCKEDSPLITSCREEN = 0x72,
        OP_IS_CARDICON_UNLOCKED = 0x73,
        OP_IS_CARDTITLE_UNLOCKED = 0x74,
        OP_IS_CARDICON_NEW = 0x75,
        OP_IS_CARDTITLE_NEW = 0x76,
        OP_IS_CARDICON_UNLOCKED_SPLITSCREEN = 0x77,
        OP_IS_CARDTITLE_UNLOCKED_SPLITSCREEN = 0x78,
        OP_IS_CARDICON_NEW_SPLITSCREEN = 0x79,
        OP_IS_CARDTITLE_NEW_SPLITSCREEN = 0x7A,
        OP_IS_PROFILEITEM_UNLOCKED = 0x7B,
        OP_IS_PROFILEITEM_UNLOCKED_SPLITSCREEN = 0x7C,
        OP_IS_PROFILEITEM_NEW = 0x7D,
        OP_IS_PROFILEITEM_NEW_SPLITSCREEN = 0x7E,
        OP_DEBUG_PRINT = 0x7F,
        OP_GETPLAYERDATA_ANYBOOLTRUE = 0x80,
        OP_GETPROFILE_ANYBOOLTRUE = 0x81,
        OP_WEAPON_CLASS_NEW = 0x82,
        OP_WEAPONNAME = 0x83,
        OP_ISRELOADING = 0x84,
        OP_SAVE_GAME_AVAILABLE = 0x85,
        OP_UNLOCKED_ITEM_COUNT = 0x86,
        OP_UNLOCKED_ITEM_COUNT_SPLITSCREEN = 0x87,
        OP_UNLOCKED_ITEM = 0x88,
        OP_UNLOCKED_ITEM_SPLITSCREEN = 0x89,
        OP_RADAR_IS_JAMMED = 0x8A,
        OP_RADAR_JAM_INTENSITY = 0x8B,
        OP_RADAR_IS_ENABLED = 0x8C,
        OP_EMP_JAMMED = 0x8D,
        OP_PLAYERADS = 0x8E,
        OP_WEAPON_HEAT_ACTIVE = 0x8F,
        OP_WEAPON_HEAT_VALUE = 0x90,
        OP_WEAPON_HEAT_OVERHEATED = 0x91,
        OP_SPLASH_TEXT = 0x92,
        OP_SPLASH_DESCRIPTION = 0x93,
        OP_SPLASH_MATERIAL = 0x94,
        OP_SPLASH_HAS_ICON = 0x95,
        OP_SPLASH_ROWNUM = 0x96,
        OP_GETFOCUSED_NAME = 0x97,
        OP_GETFOCUSED_X = 0x98,
        OP_GETFOCUSED_Y = 0x99,
        OP_GETFOCUSED_W = 0x9A,
        OP_GETFOCUSED_H = 0x9B,
        OP_GETITEMDEF_X = 0x9C,
        OP_GETITEMDEF_Y = 0x9D,
        OP_GETITEMDEF_W = 0x9E,
        OP_GETITEMDEF_H = 0x9F,
        OP_PLAYLISTFIELD = 0xA0,
        OP_SCOREBOARD_EXTERNALMUTE_NOTICE = 0xA1,
        OP_CLIENT_MATCH_DATA = 0xA2,
        OP_CLIENT_MATCH_DATA_DEF = 0xA3,
        OP_GET_MAP_NAME = 0xA4,
        OP_GET_MAP_IMAGE = 0xA5,
        OP_GET_MAP_CUSTOM = 0xA6,
        OP_GET_MIGRATION_STATUS = 0xA7,
        OP_GET_PLAYERCARD_INFO = 0xA8,
        OP_IS_OFFLINE_PROFILE_SELECTED = 0xA9,
        OP_COOP_PLAYERFIELD = 0xAA,
        OP_IS_COOP = 0xAB,
        OP_GETPARTYSTATUS = 0xAC,
        OP_GETSEARCHPARAMS = 0xAD,
        OP_GETTIMEPLAYED = 0xAE,
        OP_IS_SELECTED_PLAYER_FRIEND = 0xAF,
        OP_GETCHARBYINDEX = 0xB0,
        OP_GETPLAYERPROFILEDATA = 0xB1,
        OP_GETPLAYERPROFILEDATASPLITSCREEN = 0xB2,
        OP_IS_PROFILE_SIGNED_IN = 0xB3,
        OP_GET_WAIT_POPUP_STATUS = 0xB4,
        OP_GETNATTYPE = 0xB5,
        OP_GETLOCALIZEDNATTYPE = 0xB6,
        OP_GET_ADJUSTED_SAFEAREA_HORIZONTAL = 0xB7,
        OP_GET_ADJUSTED_SAFEAREA_VERTICAL = 0xB8,
        OP_CONNECTION_INFO = 0xB9,
        OP_OFFLINE_PROFILE_CAN_SAVE = 0xBA,
        OP_USER_WITHOUT_OFFLINE_PROFILE = 0xBB,
        OP_ALL_SPLITSCREEN_PROFILES_CAN_SAVE = 0xBC,
        OP_ALL_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0xBD,
        OP_DO_WE_HAVE_MAP_PACK = 0xBE,
        OP_MAY_INVITE_PLAYER_TO_PARTY = 0xBF,
        OP_GETPATCHNOTES = 0xC0,
        OP_GETGAMEINFOS = 0xC1,
        OP_COOP_READY = 0xC2,
        OP_VOTE_CAST = 0xC3,
        OP_VOTE_PASSED = 0xC4,
        OP_GET_MAP_VOTE_MAP_IMAGE = 0xC5,
        OP_GET_MAP_VOTE_MAP_NAME = 0xC6,
        OP_GET_MAP_VOTE_GAME_TYPE_NAME = 0xC7,
        OP_IS_FRIEND_INVITABLE = 0xC8,
        OP_IS_FRIEND_JOINABLE = 0xC9,
        OP_GET_SORTED_CHALLENGE_INDEX = 0xCA,
        OP_GET_SORTED_CHALLENGE_NAME = 0xCB,
        OP_GET_SORTED_CHALLENGE_COUNT = 0xCC,
        OP_GET_FILTER_CHALLENGE_COUNT = 0xCD,
        OP_GET_FILTER_CHALLENGE_LOCKED_COUNT = 0xCE,
        OP_GET_FILTER_CHALLENGE_COMPLETE_COUNT = 0xCF,
        OP_IS_SORTED_CHALLENGE_TIERED = 0xD0,
        OP_GET_CHALLENGE_FILTER_CACHE_COUNT = 0xD1,
        OP_GET_CHALLENGE_FILTER_CACHE_COMPLETE_COUNT = 0xD2,
        OP_IS_COOP_SEARCHING = 0xD3,
        OP_IS_COOP_PUBLIC = 0xD4,
        OP_GET_COOP_DISPLAYABLE_GROUP_NUM = 0xD5,
        OP_COOP_HAS_REQUIRED_ONLINE_FILES = 0xD6,
        OP_GET_TEXTWIDTH = 0xD7,
        OP_GET_TEXTHEIGHT = 0xD8,
        OP_DEVELOPER = 0xD9,
        OP_IS_USING_AIRBURST_WEAPON = 0xDA,
        OP_GET_AIRBURST_METERS = 0xDB,
        OP_GET_CROSSHAIR_TRACE_METERS = 0xDC,
        OP_GET_FACEBOOK_STATUS_TEXT = 0xDD,
        OP_IS_FACEBOOK_LOGGED_IN = 0xDE,
        OP_IS_FACEBOOK_CHECKING = 0xDF,
        OP_IS_FACEBOOK_ALLOWED = 0xE0,
        OP_GETPRIVATELOBBYSTATUS = 0xE1,
        OP_INCLUDEDINMAPROTATION = 0xE2,
        OP_SELECT = 0xE3,
        OP_IS_DEMO_PLAYING = 0xE4,
        OP_GET_USER_GROUP_TEXT = 0xE5,
        OP_GET_USER_GROUP_COMMON_INTEREST_TOTAL = 0xE6,
        OP_IS_DEMO_FOLLOW_CAMERA = 0xE7,
        OP_IS_DEMO_FREE_CAMERA = 0xE8,
        OP_IS_DEMO_CAPTURING_SCREENSHOT = 0xE9,
        OP_PARTY_HOST_WAITING_ON_MEMBERS = 0xEA,
        OP_POPUP_PARTY_MEMBER_AWAY = 0xEB,
        OP_SELECTED_PARTY_MEMBER_AWAY = 0xEC,
        OP_GAMETIME = 0xED,
        OP_GAMEENDTIME = 0xEE,
        OP_HAS_FOCUS = 0xEF,
        OP_MENU_HAS_FOCUS = 0xF0,
        OP_GET_DEMO_SEGMENT_COUNT = 0xF1,
        OP_GET_DEMO_SEGMENT_INFORMATION = 0xF2,
        OP_IS_CLIP_MODIFIED = 0xF3,
        OP_IS_USING_RECIPE = 0xF4,
        OP_IS_GUEST = 0xF5,
        OP_GET_FACEBOOK_HELP_TEXT = 0xF6,
        OP_IS_ELITE_CLAN_ALLOWED = 0xF7,
        OP_IS_ENTITLEMENTS_ALLOWED = 0xF8,
        OP_IS_USERGROUPS_ALLOWED = 0xF9,
        OP_IS_WAITING_FOR_ONLINE_SERVICES = 0xFA,
        OP_GET_TEXTWIDTHMODCASE = 0xFB,
        OP_GET_SAVE_SCREEN_TITLE = 0xFC,
        OP_GET_SAVE_SCREEN_DESCRIPTION = 0xFD,
        OP_GET_ONLINEVAULT_SELECTEDITEM_DATA = 0xFE,
        OP_ONLINEVAULT_IS_RESTRICTED = 0xFF,
        OP_IS_CONTENTSERVER_TASK_IN_PROGRESS = 0x100,
        OP_IS_CONTENTSERVER_GET_TASK_PROGRESS = 0x101,
        OP_GET_RECENTGAMES_SELECTEDITEM_DATA = 0x102,
        OP_GAMETYPENAME_ABBREVIATED = 0x103,
        OP_GET_MAP_VOTE_GAME_TYPE_NAME_ABBREVIATED = 0x104,
        OP_IS_USER_SIGNED_IN_TO_LIVE = 0x105,
        OP_USER_CAN_PLAY_ONLINE = 0x106,
        OP_GET_PAST_TITLE_RANK = 0x107,
        OP_GET_FEEDER_DATA = 0x108,
        OP_PARTY_CLIENTS_UP_TO_DATE = 0x109,
        OP_TRUNCATETEXTWITHELLIPSIS = 0x10A,
        OP_UI_STARTED = 0x10B,
        OP_CAN_RENDER_CLIP = 0x10C,
        OP_GET_PREVIEW_MAP_CUSTOM = 0x10D,
        OP_GET_DLC_MAPS_AVAILABLE_COUNT = 0x10E,
        OP_IS_USER_SIGNED_IN = 0x10F,
        OP_USINGINTERMISSIONTIMER = 0x110,
        OP_ISUSINGCUSTOMMAPROTATION = 0x111,
        OP_MENU_IS_TOPMOST = 0x112,
        OP_FACEBOOK_IS_PLATFORM_FRIEND = 0x113,
        OP_ELITE_CLAN_IS_PLATFORM_FRIEND = 0x114,
        OP_ELITE_CLAN_IS_ME = 0x115,
        OP_ELITE_CLAN_IS_LEADER = 0x116,
        OP_IS_USER_SIGNED_IN_FOR_VAULT = 0x117,
        OP_GET_USING_MATCHRULES_DATA = 0x118,
        OP_CAN_USER_ACCESS_ONLINEVAULT = 0x119,
        OP_FRIEND_GET_GAMERTAG = 0x11A,
        OP_RECENTPLAYER_GET_GAMERTAG = 0x11B,
        OP_LIVEPARTY_GET_GAMERTAG = 0x11C,
        OP_FACEBOOK_GET_GAMERTAG = 0x11D,
        OP_ELITECLAN_GET_GAMERTAG = 0x11E,
        OP_LIVEPARTY_IS_ME = 0x11F,
        OP_LIVEPARTY_IS_LOCAL = 0x120,
        OP_DOUBLECLICK_WAS_RIGHT_CLICK = 0x121,
        OP_IS_DEMO_CLIP_RECORDING = 0x122,
        OP_GET_INDEX_FROM_STRING = 0x123,
        OP_GET_STRING_WIHTOUT_INDEX = 0x124,
        OP_ELITECLAN_GET_NAME = 0x125,
        OP_ELITECLAN_GET_HELP = 0x126,
        OP_ELITECLAN_GET_MOTD = 0x127,
        OP_ELITECLAN_IS_MEMBER = 0x128,
        OP_ELITECLAN_IS_EMBLEM_OK = 0x129,
        OP_FACEBOOKFRIENDS_SHOW_NEXT = 0x12A,
        OP_FACEBOOKFRIENDS_SHOW_PREV = 0x12B,
        OP_GET_ONLINEVAULT_FRIEND_GAMERTAG = 0x12C,
        OP_GET_OBJECTIVE_LIST_HEIGHT = 0x12D,
        OP_IS_CLIENT_DEMO_ENABLED = 0x12E,
        OP_IS_USER_SIGNED_IN_TO_DEMONWARE = 0x12F,
        OP_IS_CUSTOM_CLASS_RESTRICTED = 0x130,
        OP_IS_WEAPON_RESTRICTED = 0x131,
        OP_ANY_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0x132,
        OP_IS_GUEST_SPLITSCREEN = 0x133,
        OP_IS_ITEM_UNLOCKED_BY_CLIENT = 0x134,
        OP_IS_ANY_USER_SIGNED_IN_TO_LIVE = 0x135,
        OP_GET_PAST_TITLE_PRESTIGE = 0x136,
        OP_SPLITSCREENACTIVEGAMEPADCOUNT = 0x137,
        OP_SHOW_FRIEND_PLAYERCARD = 0x138,
        OP_GET_FRIEND_PLAYERCARD_PRESENCE = 0x139,
        OP_SHOW_RECENT_PLAYERS_GROUP_ICON = 0x13A,
        OP_GET_WRAPPED_TEXT_HEIGHT = 0x13B,
        OP_CAN_SAVE = 0x13C,
        OP_GET_GAME_INVITES_COUNT = 0x13D,
        OP_IS_SPLITSCREEN_GAMER_LIVE_ENABLED = 0x13E,
        OP_SO_COOP_SHOW_COMMON_GROUP_ICON = 0x13F,
        OP_STRIP_COLORS_FROM_STRING = 0x140,
        OP_CAN_USER_ACCESS_THEATRE = 0x141,
        OP_IS_CHALLENGE_PERIODIC = 0x142,
        OP_GET_CHALLENGE_DATA = 0x143,
        OP_IS_ELITE_APP_PRESENT = 0x144,
        OP_ELITE_CLAN_SELECTED_IS_ME = 0x145,
        OP_ENOUGH_STORAGE_SPACE_FOR_CLIENT_DEMO = 0x146,
        OP_IS_USER_SIGNED_IN_FOR_COMMERCE = 0x147,
        OP_GET_FACEBOOK_MENU_TEXT = 0x148,
        OP_GET_FACEBOOK_IS_POSTING = 0x149,
        OP_MEET_PLAYER_IS_PLATFORM_FRIEND = 0x14A,
        OP_IS_SELECTED_PLAYER_GUEST = 0x14B,
        OP_GET_SPLITSCREEN_CONTROLLER_CLIENT_NUM = 0x14C,
        OP_IS_CLIENT_DEMO_ENABLED_SPLITSCREEN = 0x14D,
        OP_ITEM_CAN_TAKE_FOCUS = 0x14E,
        OP_GET_TIME_SINCE_LAST_DOUBLECLICK = 0x14F,
        OP_IS_SERVER_LIST_REFRESHING = 0x150,
        OP_IS_RECIPE_NAME_VALID = 0x151,
        OP_RECIPE_EXISTS = 0x152,
        OP_GET_FACEBOOK_OPTIONS_HELP_TEXT = 0x153,
        OP_DO_WE_HAVE_ALL_MAP_PACKS = 0x154,
        OP_IS_THERE_NEW_ELITE_ITEMS = 0x155,
        OP_IS_PAYING_SUBSCRIBER = 0x156,
        OP_LOCALUSER_HASMISSINGMAPPACK = 0x157,
        OP_LOCALUSER_MISSINGMAPPACKERROR = 0x158,
        OP_GET_FIRST_SPECOPS_DLC_MAP = 0x159,
        OP_LOCALUSER_MISSINGMAPNAME = 0x15A,
        OP_SHOW_STORE_NEW = 0x15B,
        OP_USINGBOTS = 0x15C,
        OP_BOTSALLOWED = 0x15D,
        OP_ISCOLORBLIND = 0x15E,
        OP_BOTSCONNECTTYPE = 0x15F,
        OP_BOTSDIFFICULTY = 0x160,
        OP_SPECTATINGTHIRD = 0x161,
        OP_GETPLAYERNAME = 0x162,
        OP_SQUAD_SHOW_REPORT = 0x163,
        OP_GETRANKEDPLAYERDATA = 0x164,
        OP_GETPRIVATEPLAYERDATA = 0x165,
        OP_GETCOOPPLAYERDATA = 0x166,
        OP_GETCOMMONPLAYERDATA = 0x167,
        OP_GETRANKEDPLAYERDATASPLITSCREEN = 0x168,
        OP_GETPRIVATEPLAYERDATASPLITSCREEN = 0x169,
        OP_GETCOOPPLAYERDATASPLITSCREEN = 0x16A,
        OP_GETCOMMONPLAYERDATASPLITSCREEN = 0x16B,

        OP_COUNT,

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
        EXP_FUNC_LOCAL_CLIENT_UI_MILLISECONDS,
        EXP_FUNC_DVAR_INT,
        EXP_FUNC_DVAR_BOOL,
        EXP_FUNC_DVAR_FLOAT,
        EXP_FUNC_DVAR_STRING
    };

    /* 11928 */
    union entryInternalData
    {
        operationEnum op;
        Operand operand;
    };

    enum expressionEntryType : int
    {
        EET_OPERATOR = 0x0,
        EET_OPERAND = 0x1,
    };

    /* 11929 */
    struct expressionEntry
    {
        entryInternalData data;
        expressionEntryType type;
    };

    /* 11930 */
    struct UIFunctionList
    {
        int totalFunctions;
        Statement_s** functions;
    };

    struct DvarLimitsEnum
    {
        int stringCount;
        char** strings;
    };

    struct DvarLimitsInt
    {
        int min;
        int max;
    };

    struct DvarLimitsFloat
    {
        float min;
        float max;
    };

    /* 11485 */
    union DvarLimits
    {
        DvarLimitsEnum enumeration;
        DvarLimitsInt integer;
        DvarLimitsFloat value;
        DvarLimitsFloat vector;
    };

    /* 11480 */
    union DvarValue
    {
        bool enabled;
        int integer;
        unsigned int unsignedInt;
        float value;
        float vector[4];
        const char* string;
        unsigned char color[4];
    };

    /* 11486 */
    struct dvar_t
    {
        const char* name;
        const char* description;
        unsigned int flags;
        unsigned char type;
        bool modified;
        DvarValue current;
        DvarValue latched;
        DvarValue reset;
        DvarLimits domain;
        dvar_t* hashNext;
    };

    /* 11931 */
    struct StaticDvar
    {
        const dvar_t* dvar;
        char* dvarName;
    };

    /* 11932 */
    struct StaticDvarList
    {
        int numStaticDvars;
        StaticDvar** staticDvars;
    };

    /* 11933 */
    struct StringList
    {
        int totalStrings;
        const char** strings;
    };

    /* 11934 */
    struct ExpressionSupportingData
    {
        UIFunctionList uifunctions;
        StaticDvarList staticDvarList;
        StringList uiStrings;
    };

    /* 11935 */
    struct ExpressionPersistentState
    {
        int flags;
        int playerDataKey[4];
        int lastExecuteTime[4];
        Operand lastResult[4];
    };

    /* 11925 */
    struct Statement_s
    {
        int numEntries;
        expressionEntry* entries;
        ExpressionSupportingData* supportingData;
        ExpressionPersistentState persistentState;
    };

    struct MenuEventHandlerSet;

    /* 11936 */
    struct ConditionalScript
    {
        MenuEventHandlerSet* eventHandlerSet;
        Statement_s* eventExpression;
    };

    /* 11937 */
    struct SetLocalVarData
    {
        const char* localVarName;
        Statement_s* expression;
    };

    /* 11938 */
    union EventData
    {
        const char* unconditionalScript;
        ConditionalScript* conditionalScript;
        MenuEventHandlerSet* elseScript;
        SetLocalVarData* setLocalVarData;
    };

    /* 1794 */
    enum EventType : unsigned char
    {
        EVENT_UNCONDITIONAL = 0x0,
        EVENT_IF = 0x1,
        EVENT_ELSE = 0x2,

        EVENT_SET_LOCAL_VAR_BOOL = 0x3,
        EVENT_SET_LOCAL_VAR_INT = 0x4,
        EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
        EVENT_SET_LOCAL_VAR_STRING = 0x6,

        EVENT_COUNT = 0x7,
    };

    /* 11939 */
    struct type_align32(4) MenuEventHandler
    {
        EventData eventData;
        EventType eventType;
    };

    /* 11923 */
    struct MenuEventHandlerSet
    {
        int eventHandlerCount;
        MenuEventHandler** eventHandlers;
    };

    /* 11940 */
    struct ItemKeyHandler
    {
        int key;
        MenuEventHandlerSet* action;
        ItemKeyHandler* next;
    };

    /* 11941 */
    struct menuTransition
    {
        int transitionType;
        int startTime;
        float startVal;
        float endVal;
        float time;
        int endTriggerType;
    };

    /* 11942 */
    struct type_align32(4) menuData_t
    {
        int fullScreen;
        int fadeCycle;
        float fadeClamp;
        float fadeAmount;
        float fadeInAmount;
        float blurRadius;
        MenuEventHandlerSet* onOpen;
        MenuEventHandlerSet* onCloseRequest;
        MenuEventHandlerSet* onClose;
        MenuEventHandlerSet* onESC;
        MenuEventHandlerSet* onFocusDueToClose;
        ItemKeyHandler* onKey;
        Statement_s* visibleExp;
        const char* allowedBinding;
        const char* soundName;
        float focusColor[4];
        Statement_s* rectXExp;
        Statement_s* rectYExp;
        Statement_s* rectWExp;
        Statement_s* rectHExp;
        Statement_s* openSoundExp;
        Statement_s* closeSoundExp;
        Statement_s* soundLoopExp;
        int cursorItem[4];
        menuTransition scaleTransition[4];
        menuTransition alphaTransition[4];
        menuTransition xTransition[4];
        menuTransition yTransition[4];
        ExpressionSupportingData* expressionData;
        unsigned char priority;
    };

    /* 11943 */
    struct type_align32(4) rectDef_s
    {
        float x;
        float y;
        float w;
        float h;
        unsigned char horzAlign;
        unsigned char vertAlign;
    };

    // This is data from IW4, could be different for IW6, to be investigated
    enum WindowDefStaticFlag : unsigned int
    {
        WINDOW_FLAG_DECORATION = 0x100000,
        WINDOW_FLAG_HORIZONTAL_SCROLL = 0x200000,
        WINDOW_FLAG_OUT_OF_BOUNDS_CLICK = 0x2000000,
        WINDOW_FLAG_SCREEN_SPACE = 0x400000,
        WINDOW_FLAG_AUTO_WRAPPED = 0x800000,
        WINDOW_FLAG_POPUP = 0x1000000,
        WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE = 0x4000000,
        WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG = 0x10000000, // confirmed
        WINDOW_FLAG_HIDDEN_DURING_SCOPE = 0x20000000,      // confirmed
        WINDOW_FLAG_HIDDEN_DURING_UI = 0x40000000,         // confirmed
        WINDOW_FLAG_TEXT_ONLY_FOCUS = 0x80000000,
    };

    // This is data from IW4, could be different for IW6, to be investigated
    enum WindowDefDynamicFlag : unsigned int
    {
        WINDOW_FLAG_HOVERED = 0x1, // guessed
        WINDOW_FLAG_FOCUSED = 0x2,
        WINDOW_FLAG_VISIBLE = 0x4, // confirmed
        WINDOW_FLAG_FADING_OUT = 0x10,
        WINDOW_FLAG_FADING_IN = 0x20,
        WINDOW_FLAG_80 = 0x80,
        WINDOW_FLAG_NON_DEFAULT_BACKCOLOR = 0x8000,
        WINDOW_FLAG_NON_DEFAULT_FORECOLOR = 0x10000
    };

    /* 11944 */
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
        int dynamicFlags[4];
        int nextTime;
        float foreColor[4];
        float backColor[4];
        float borderColor[4];
        float outlineColor[4];
        float disableColor[4];
        Material* background;
    };

    /* 11946 */
    struct columnInfo_s
    {
        int xpos;
        int ypos;
        int width;
        int height;
        int maxChars;
        int alignment;
    };

    /* 11947 */
    struct listBoxDef_s
    {
        int startPos[4];
        int endPos[4];
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
        Statement_s* elementHeightExp;
    };

    /* 11948 */
    struct editFieldDef_s
    {
        float minVal;
        float maxVal;
        float stepVal;
        float range;
        int maxChars;
        int maxCharsGotoNext;
        int maxPaintChars;
        int paintOffset;
    };

    /* 11949 */
    struct multiDef_s
    {
        const char* dvarList[32];
        const char* dvarStr[32];
        float dvarValue[32];
        int count;
        int strDef;
    };

    /* 11950 */
    struct newsTickerDef_s
    {
        int feedId;
        int speed;
        int spacing;
    };

    /* 11951 */
    struct textScrollDef_s
    {
        int startTime;
    };

    /* 11952 */
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

    struct menuDef_t;

    enum ItemDefType : int
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

    // This is data from IW4, could be different for IW6, to be investigated
    enum ItemDefFlag : unsigned int
    {
        ITEM_FLAG_SAVE_GAME_INFO = 0x1,
        ITEM_FLAG_CINEMATIC_SUBTITLE = 0x2,
    };

    // This is data from IW4, could be different for IW6, to be investigated
    enum ItemDefDvarFlag
    {
        ITEM_DVAR_FLAG_ENABLE = 0x1,
        ITEM_DVAR_FLAG_DISABLE = 0x2,
        ITEM_DVAR_FLAG_SHOW = 0x4,
        ITEM_DVAR_FLAG_HIDE = 0x8,
        ITEM_DVAR_FLAG_FOCUS = 0x10,
    };

    /* 11954 */
    struct itemDef_t
    {
        windowDef_t window;
        rectDef_s textRect[4];
        ItemDefType type;
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
        MenuEventHandlerSet* hasFocus;
        MenuEventHandlerSet* leaveFocus;
        const char* dvar;
        const char* dvarTest;
        ItemKeyHandler* onKey;
        const char* enableDvar;
        const char* localVar;
        int dvarFlags;
        snd_alias_list_t* focusSound;
        float special;
        int cursorPos[4];
        itemDefData_t typeData;
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
        Statement_s* textAlignYExp;
    };

    /* 11945 */
    struct menuDef_t
    {
        menuData_t* data;
        windowDef_t window;
        int itemCount;
        itemDef_t** items;
    };

    /* 11955 */
    struct MenuList
    {
        const char* name;
        int menuCount;
        menuDef_t** menus;
    };

    /* 11956 */
    struct AnimationAimSet
    {
        ScriptString name;
        ScriptString rootName;
        ScriptString animName[8];
        unsigned int rootIndex;
        unsigned int animIndices[8];
    };

    /* 11957 */
    struct AnimationEntry
    {
        ScriptString alias;
        ScriptString animName;
    };

    /* 11958 */
    struct AnimationState
    {
        ScriptString name;
        ScriptString notify;
        float blendTime;
        unsigned char flags;
        unsigned char entryCount;
        AnimationAimSet* aimSet;
        AnimationEntry* animEntries;
        unsigned int* animIndices;
    };

    /* 11959 */
    struct AnimationStateMachine
    {
        ScriptString name;
        unsigned short stateCount;
        unsigned short aimSetCount;
        AnimationState* states;
        AnimationAimSet* aimSets;
    };

    /* 1852 */
    enum AnimationController : int
    {
        ANIMCTRL_NONE = 0x0,
        ANIMCTRL_PLAYER = 0x1,
        ANIMCTRL_DOG = 0x2,
        ANIMCTRL_NUM = 0x3,
    };

    /* 11960 */
    struct AnimationClass
    {
        const char* className;
        AnimationStateMachine* stateMachine;
        AnimationController animCtrl;
        ScriptString animTree;
        unsigned short soundCount;
        unsigned short effectCount;
        ScriptString* soundNotes;
        ScriptString* soundNames;
        ScriptString* effectNotes;
        FxEffectDef** effectDefs;
        ScriptString* effectTags;
    };

    /* 11961 */
    struct LocalizeEntry
    {
        const char* value;
        const char* name;
    };

    /* 988 */
    enum AttachmentType : int
    {
        ATTACHMENT_SCOPE = 0x0,
        ATTACHMENT_UNDERBARREL = 0x1,
        ATTACHMENT_OTHER = 0x2,
        ATTACHMENT_COUNT = 0x3,
    };

    /* 975 */
    enum weapType_t : int
    {
        WEAPTYPE_NONE = 0x0,
        WEAPTYPE_BULLET = 0x1,
        WEAPTYPE_GRENADE = 0x2,
        WEAPTYPE_PROJECTILE = 0x3,
        WEAPTYPE_RIOTSHIELD = 0x4,
        WEAPTYPE_NUM = 0x5,
    };

    /* 976 */
    enum weapClass_t : int
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
        WEAPCLASS_NUM = 0xC,
    };

    /* 11191 */
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

    /* 978 */
    enum PenetrateType : int
    {
        PENETRATE_TYPE_NONE = 0x0,
        PENETRATE_TYPE_SMALL = 0x1,
        PENETRATE_TYPE_MEDIUM = 0x2,
        PENETRATE_TYPE_LARGE = 0x3,
        PENETRATE_TYPE_COUNT = 0x4,
    };

    /* 979 */
    enum ImpactType : int
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
        IMPACT_TYPE_COUNT = 0xB,
    };

    /* 950 */
    enum weapFireType_t : int
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

    /* 11192 */
    struct type_align32(4) AttAmmoGeneral
    {
        PenetrateType penetrateType;
        float penetrateMultiplier;
        ImpactType impactType;
        weapFireType_t fireType;
        TracerDef* tracerType;
        bool rifleBullet;
        bool armorPiercing;
    };

    /* 11193 */
    struct AttSight
    {
        bool aimDownSight;
        bool adsFire;
        bool rechamberWhileAds;
        bool noAdsWhenMagEmpty;
        bool canHoldBreath;
        bool canVariableZoom;
        bool hideRailWithThisScope;
        bool useScopeDrift;
    };

    /* 11194 */
    struct AttReload
    {
        bool noPartialReload;
        bool segmentedReload;
    };

    /* 11195 */
    struct AttAddOns
    {
        bool motionTracker;
        bool silenced;
        bool riotShield;
    };

    /* 11196 */
    struct AttGeneral
    {
        bool boltAction;
        bool inheritsPerks;
        bool reticleSpin45;
        float enemyCrosshairRange;
        Material* reticleCenter;
        Material* reticleSide;
        int reticleCenterSize;
        int reticleSideSize;
        float moveSpeedScale;
        float adsMoveSpeedScale;
    };

    /* 11197 */
    struct AttAimAssist
    {
        float autoAimRange;
        float aimAssistRange;
        float aimAssistRangeAds;
    };

    /* 11198 */
    struct AttAmmunition
    {
        int maxAmmo;
        int startAmmo;
        int clipSize;
        int shotCount;
        int reloadAmmoAdd;
        int reloadStartAdd;
    };

    /* 11199 */
    struct AttDamage
    {
        int damage;
        int minDamage;
        int meleeDamage;
        float maxDamageRange;
        float minDamageRange;
        int playerDamage;
        int minPlayerDamage;
    };

    /* 11200 */
    struct AttLocationDamage
    {
        float locNone;
        float locHelmet;
        float locHead;
        float locNeck;
        float locTorsoUpper;
        float locTorsoLower;
        float locRightArmUpper;
        float locRightArmLower;
        float locRightHand;
        float locLeftArmUpper;
        float locLeftArmLower;
        float locLeftHand;
        float locRightLegUpper;
        float locRightLegLower;
        float locRightFoot;
        float locLeftLegUpper;
        float locLeftLegLower;
        float locLeftFoot;
        float locGun;
    };

    /* 11201 */
    struct AttIdleSettings
    {
        float hipIdleAmount;
        float hipIdleSpeed;
        float idleCrouchFactor;
        float idleProneFactor;
        float adsIdleLerpStartTime;
        float adsIdleLerpTime;
    };

    /* 11202 */
    struct AttADSSettings
    {
        float adsSpread;
        float adsAimPitch;
        float adsTransInTime;
        float adsTransOutTime;
        int adsReloadTransTime;
        float adsCrosshairInFrac;
        float adsCrosshairOutFrac;
        float adsZoomFov;
        float adsZoomInFrac;
        float adsZoomOutFrac;
        float adsBobFactor;
        float adsViewBobMult;
        float adsViewErrorMin;
        float adsViewErrorMax;
    };

    /* 11203 */
    struct AttScopeDriftSettings
    {
        float fScopeDriftDelay;
        float fScopeDriftLerpInTime;
        float fScopeDriftSteadyTime;
        float fScopeDriftLerpOutTime;
        float fScopeDriftSteadyFactor;
        float fScopeDriftUnsteadyFactor;
    };

    /* 11204 */
    struct AttHipSpread
    {
        float hipSpreadStandMin;
        float hipSpreadDuckedMin;
        float hipSpreadProneMin;
        float hipSpreadMax;
        float hipSpreadDuckedMax;
        float hipSpreadProneMax;
        float hipSpreadFireAdd;
        float hipSpreadTurnAdd;
        float hipSpreadMoveAdd;
        float hipSpreadDecayRate;
        float hipSpreadDuckedDecay;
        float hipSpreadProneDecay;
    };

    /* 11205 */
    struct AttGunKick
    {
        int hipGunKickReducedKickBullets;
        float hipGunKickReducedKickPercent;
        float hipGunKickPitchMin;
        float hipGunKickPitchMax;
        float hipGunKickYawMin;
        float hipGunKickYawMax;
        float hipGunKickMagMin;
        float hipGunKickAccel;
        float hipGunKickSpeedMax;
        float hipGunKickSpeedDecay;
        float hipGunKickStaticDecay;
        int adsGunKickReducedKickBullets;
        float adsGunKickReducedKickPercent;
        float adsGunKickPitchMin;
        float adsGunKickPitchMax;
        float adsGunKickYawMin;
        float adsGunKickYawMax;
        float adsGunKickMagMin;
        float adsGunKickAccel;
        float adsGunKickSpeedMax;
        float adsGunKickSpeedDecay;
        float adsGunKickStaticDecay;
    };

    /* 11206 */
    struct AttViewKick
    {
        float hipViewKickPitchMin;
        float hipViewKickPitchMax;
        float hipViewKickYawMin;
        float hipViewKickYawMax;
        float hipViewKickMagMin;
        float hipViewKickCenterSpeed;
        float adsViewKickPitchMin;
        float adsViewKickPitchMax;
        float adsViewKickYawMin;
        float adsViewKickYawMax;
        float adsViewKickMagMin;
        float adsViewKickCenterSpeed;
    };

    /* 980 */
    enum weapOverlayReticle_t : int
    {
        WEAPOVERLAYRETICLE_NONE = 0x0,
        WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
        WEAPOVERLAYRETICLE_NUM = 0x2,
    };

    /* 11207 */
    struct ADSOverlay
    {
        Material* shader;
        Material* shaderLowRes;
        Material* shaderEMP;
        Material* shaderEMPLowRes;
        weapOverlayReticle_t reticle;
        float width;
        float height;
        float widthSplitscreen;
        float heightSplitscreen;
    };

    /* 11208 */
    struct type_align32(4) AttADSOverlay
    {
        ADSOverlay overlay;
        bool thermalScope;
    };

    /* 981 */
    enum weaponIconRatioType_t : int
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,
        WEAPON_ICON_RATIO_COUNT = 0x3,
    };

    /* 982 */
    enum ammoCounterClipType_t : int
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

    /* 11209 */
    struct AttUI
    {
        Material* dpadIcon;
        Material* ammoCounterIcon;
        weaponIconRatioType_t dpadIconRatio;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
    };

    /* 11210 */
    struct AttRumbles
    {
        const char* fireRumble;
        const char* meleeImpactRumble;
    };

    /* 977 */
    enum weapProjExposion_t : int
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

    /* 11211 */
    struct AttProjectile
    {
        int explosionRadius;
        int explosionInnerDamage;
        int explosionOuterDamage;
        float damageConeAngle;
        int projectileSpeed;
        int projectileSpeedUp;
        int projectileActivateDist;
        float projectileLifetime;
        XModel* projectileModel;
        weapProjExposion_t projExplosionType;
        FxEffectDef* projExplosionEffect;
        bool projExplosionEffectForceNormalUp;
        snd_alias_list_t* projExplosionSound;
        FxEffectDef* projDudEffect;
        snd_alias_list_t* projDudSound;
        bool projImpactExplode;
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        FxEffectDef* projTrailEffect;
        int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        snd_alias_list_t* projIgnitionSound;
    };

    /* 11212 */
    struct type_align32(4) WeaponAttachment
    {
        const char* szInternalName;
        const char* szDisplayName;
        AttachmentType type;
        weapType_t weaponType;
        weapClass_t weapClass;
        XModel** worldModels;
        XModel** viewModels;
        XModel** reticleViewModels;
        AttAmmoGeneral* ammogeneral;
        AttSight* sight;
        AttReload* reload;
        AttAddOns* addOns;
        AttGeneral* general;
        AttAimAssist* aimAssist;
        AttAmmunition* ammunition;
        AttDamage* damage;
        AttLocationDamage* locationDamage;
        AttIdleSettings* idleSettings;
        AttADSSettings* adsSettings;
        AttADSSettings* adsSettingsMain;
        AttScopeDriftSettings* scopeDriftSettings;
        AttScopeDriftSettings* scopeDriftSettingsMain;
        AttHipSpread* hipSpread;
        AttGunKick* gunKick;
        AttViewKick* viewKick;
        AttADSOverlay* adsOverlay;
        AttUI* ui;
        AttRumbles* rumbles;
        AttProjectile* projectile;
        float ammunitionScale;
        float damageScale;
        float damageScaleMin;
        float stateTimersScale;
        float fireTimersScale;
        float idleSettingsScale;
        float adsSettingsScale;
        float adsSettingsScaleMain;
        float hipSpreadScale;
        float gunKickScale;
        float viewKickScale;
        float viewCenterScale;
        int loadIndex;
        bool hideIronSightsWithThisAttachment;
        bool shareAmmoWithAlt;
    };

    /* 983 */
    enum weapInventoryType_t : int
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,
        WEAPINVENTORY_EXCLUSIVE = 0x4,
        WEAPINVENTORY_SCAVENGER = 0x5,
        WEAPINVENTORYCOUNT = 0x6,
    };

    /* 822 */
    enum OffhandClass : int
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,
        OFFHAND_CLASS_THROWINGKNIFE = 0x4,
        OFFHAND_CLASS_OTHER = 0x5,
        OFFHAND_CLASS_COUNT = 0x6,
    };

    /* 984 */
    enum weapStance_t : int
    {
        WEAPSTANCE_STAND = 0x0,
        WEAPSTANCE_DUCK = 0x1,
        WEAPSTANCE_PRONE = 0x2,
        WEAPSTANCE_NUM = 0x3,
    };

    /* 986 */
    enum activeReticleType_t : int
    {
        VEH_ACTIVE_RETICLE_NONE = 0x0,
        VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
        VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
        VEH_ACTIVE_RETICLE_COUNT = 0x3,
    };

    /* 11231 */
    struct StateTimers
    {
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
        int primeTime;
        int blastFrontTime;
        int blastRightTime;
        int blastBackTime;
        int blastLeftTime;
        int slideInTime;
        int slideLoopTime;
        int slideOutTime;
    };

    /* 985 */
    enum WeapOverlayInteface_t : int
    {
        WEAPOVERLAYINTERFACE_NONE = 0x0,
        WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
        WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
        WEAPOVERLAYINTERFACECOUNT = 0x3,
    };

    /* 825 */
    enum WeapStickinessType : int
    {
        WEAPSTICKINESS_NONE = 0x0,
        WEAPSTICKINESS_ALL = 0x1,
        WEAPSTICKINESS_ALL_ORIENT = 0x2,
        WEAPSTICKINESS_GROUND = 0x3,
        WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
        WEAPSTICKINESS_KNIFE = 0x5,
        WEAPSTICKINESS_COUNT = 0x6,
    };

    /* 987 */
    enum guidedMissileType_t : int
    {
        MISSILE_GUIDANCE_NONE = 0x0,
        MISSILE_GUIDANCE_SIDEWINDER = 0x1,
        MISSILE_GUIDANCE_HELLFIRE = 0x2,
        MISSILE_GUIDANCE_JAVELIN = 0x3,
        MISSILE_GUIDANCE_COUNT = 0x4,
    };

    enum hitLocation_t : int32_t
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

        SURF_TYPE_COUNT
    };

    /* 11232 */
    struct WeaponDef
    {
        const char* szOverlayName;
        XModel** gunXModel;
        XModel* handXModel;
        const char** szXAnimsRightHanded;
        const char** szXAnimsLeftHanded;
        const char* szModeName;
        ScriptString* notetrackSoundMapKeys;
        ScriptString* notetrackSoundMapValues;
        ScriptString* notetrackRumbleMapKeys;
        ScriptString* notetrackRumbleMapValues;
        ScriptString* notetrackFXMapKeys;
        FxEffectDef** notetrackFXMapValues;
        ScriptString* notetrackFXMapTagValues;
        int playerAnimType;
        weapType_t weapType;
        weapClass_t weapClass;
        PenetrateType penetrateType;
        weapInventoryType_t inventoryType;
        weapFireType_t fireType;
        OffhandClass offhandClass;
        weapStance_t stance;
        unsigned char rattleSoundType;
        FxEffectDef* viewFlashEffect;
        FxEffectDef* worldFlashEffect;
        FxEffectDef* viewFlashADSEffect;
        snd_alias_list_t* pickupSound;
        snd_alias_list_t* pickupSoundPlayer;
        snd_alias_list_t* ammoPickupSound;
        snd_alias_list_t* ammoPickupSoundPlayer;
        snd_alias_list_t* projectileSound;
        snd_alias_list_t* pullbackSound;
        snd_alias_list_t* pullbackSoundPlayer;
        snd_alias_list_t* fireSound;
        snd_alias_list_t* fireSoundPlayer;
        snd_alias_list_t* fireSoundPlayerAkimbo;
        snd_alias_list_t* fireLoopSound;
        snd_alias_list_t* fireLoopSoundPlayer;
        snd_alias_list_t* fireStopSound;
        snd_alias_list_t* fireStopSoundPlayer;
        snd_alias_list_t* fireLastSound;
        snd_alias_list_t* fireLastSoundPlayer;
        snd_alias_list_t* emptyFireSound;
        snd_alias_list_t* emptyFireSoundPlayer;
        snd_alias_list_t* meleeSwipeSound;
        snd_alias_list_t* meleeSwipeSoundPlayer;
        snd_alias_list_t* meleeHitSound;
        snd_alias_list_t* meleeMissSound;
        snd_alias_list_t* rechamberSound;
        snd_alias_list_t* rechamberSoundPlayer;
        snd_alias_list_t* reloadSound;
        snd_alias_list_t* reloadSoundPlayer;
        snd_alias_list_t* reloadEmptySound;
        snd_alias_list_t* reloadEmptySoundPlayer;
        snd_alias_list_t* reloadStartSound;
        snd_alias_list_t* reloadStartSoundPlayer;
        snd_alias_list_t* reloadEndSound;
        snd_alias_list_t* reloadEndSoundPlayer;
        snd_alias_list_t* detonateSound;
        snd_alias_list_t* detonateSoundPlayer;
        snd_alias_list_t* nightVisionWearSound;
        snd_alias_list_t* nightVisionWearSoundPlayer;
        snd_alias_list_t* nightVisionRemoveSound;
        snd_alias_list_t* nightVisionRemoveSoundPlayer;
        snd_alias_list_t* altSwitchSound;
        snd_alias_list_t* altSwitchSoundPlayer;
        snd_alias_list_t* raiseSound;
        snd_alias_list_t* raiseSoundPlayer;
        snd_alias_list_t* firstRaiseSound;
        snd_alias_list_t* firstRaiseSoundPlayer;
        snd_alias_list_t* putawaySound;
        snd_alias_list_t* putawaySoundPlayer;
        snd_alias_list_t* scanSound;
        snd_alias_list_t* changeVariableZoomSound;
        snd_alias_list_t** bounceSound;
        snd_alias_list_t** rollingSound;
        FxEffectDef* viewShellEjectEffect;
        FxEffectDef* worldShellEjectEffect;
        FxEffectDef* viewLastShotEjectEffect;
        FxEffectDef* worldLastShotEjectEffect;
        FxEffectDef* viewMagEjectEffect;
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
        StateTimers stateTimers;
        StateTimers akimboStateTimers;
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
        ADSOverlay overlay;
        WeapOverlayInteface_t overlayInterface;
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
        float adsIdleLerpStartTime;
        float adsIdleLerpTime;
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
        float fScopeDriftDelay;
        float fScopeDriftLerpInTime;
        float fScopeDriftSteadyTime;
        float fScopeDriftLerpOutTime;
        float fScopeDriftSteadyFactor;
        float fScopeDriftUnsteadyFactor;
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
        snd_alias_list_t* projExplosionSound;
        snd_alias_list_t* projDudSound;
        WeapStickinessType stickiness;
        float lowAmmoWarningThreshold;
        float ricochetChance;
        bool riotShieldEnableDamage;
        int riotShieldHealth;
        float riotShieldDamageMult;
        float* parallelBounce;
        float* perpendicularBounce;
        FxEffectDef* projTrailEffect;
        FxEffectDef* projBeaconEffect;
        float vProjectileColor[3];
        guidedMissileType_t guidedMissileType;
        float maxSteeringAccel;
        int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        snd_alias_list_t* projIgnitionSound;
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
        unsigned int iUseHintStringIndex;
        unsigned int dropHintStringIndex;
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
        bool turretADSEnabled;
        float turretADSTime;
        float turretFov;
        float turretFovADS;
        float turretScopeZoomRate;
        float turretScopeZoomMin;
        float turretScopeZoomMax;
        float turretOverheatUpRate;
        float turretOverheatDownRate;
        float turretOverheatPenalty;
        snd_alias_list_t* turretOverheatSound;
        FxEffectDef* turretOverheatEffect;
        const char* turretBarrelSpinRumble;
        float turretBarrelSpinSpeed;
        float turretBarrelSpinUpTime;
        float turretBarrelSpinDownTime;
        snd_alias_list_t* turretBarrelSpinMaxSnd;
        snd_alias_list_t* turretBarrelSpinUpSnd[4];
        snd_alias_list_t* turretBarrelSpinDownSnd[4];
        snd_alias_list_t* missileConeSoundAlias;
        snd_alias_list_t* missileConeSoundAliasAtBase;
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
        ScriptString knifeAttachTagOverride;
        bool knifeAlwaysAttached;
        bool meleeOverrideValues;
        float aim_automelee_lerp;
        float aim_automelee_range;
        float aim_automelee_region_height;
        float aim_automelee_region_width;
        float player_meleeHeight;
        float player_meleeRange;
        float player_meleeWidth;
        bool sharedAmmo;
        bool lockonSupported;
        bool requireLockonToFire;
        bool isAirburstWeapon;
        bool bigExplosion;
        bool noAdsWhenMagEmpty;
        bool avoidDropCleanup;
        bool inheritsPerks;
        bool crosshairColorChange;
        bool bRifleBullet;
        bool armorPiercing;
        bool bBoltAction;
        bool aimDownSight;
        bool canHoldBreath;
        bool meleeOnly;
        bool canVariableZoom;
        bool bRechamberWhileAds;
        bool bBulletExplosiveDamage;
        bool bCookOffHold;
        bool reticleSpin45;
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
        bool projExplosionEffectInheritParentDirection;
        bool bProjImpactExplode;
        bool bProjTrajectoryEvents;
        bool bProjWhizByEnabled;
        bool stickToPlayers;
        bool stickToVehicles;
        bool stickToTurrets;
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
        bool doNotAllowAttachmentsToOverrideSpread;
        bool useFastReloadAnims;
        bool useScopeDrift;
        bool oldWeapon;
        ScriptString stowTag;
        XModel* stowOffsetModel;
    };

    /* 11233 */
    struct AnimOverrideEntry
    {
        unsigned short attachment1;
        unsigned short attachment2;
        const char* overrideAnim;
        const char* altmodeAnim;
        unsigned int animTreeType;
        int animTime;
        int altTime;
    };

    /* 2002 */
    enum SoundOverrideTypes : unsigned int
    {
        SNDTYPE_NONE = 0x0,
        SNDTYPE_FIRE = 0x1,
        SNDTYPE_PLAYER_FIRE = 0x2,
        SNDTYPE_PLAYER_AKIMBO = 0x3,
        SNDTYPE_PLAYER_LASTSHOT = 0x4,

        NUM_SOUND_OVERRIDE_TYPES = 0x5,
    };

    /* 11234 */
    struct SoundOverrideEntry
    {
        unsigned short attachment1;
        unsigned short attachment2;
        snd_alias_list_t* overrideSound;
        snd_alias_list_t* altmodeSound;
        SoundOverrideTypes soundType;
    };

    /* 2003 */
    enum FXOverrideTypes : unsigned int
    {
        FXTYPE_NONE = 0x0,
        FXTYPE_VIEW_FLASH = 0x1,
        FXTYPE_WORLD_FLASH = 0x2,
        FXTYPE_VIEW_SHELL_EJECT = 0x3,
        FXTYPE_WORLD_SHELL_EJECT = 0x4,
        FXTYPE_VIEW_MAG_EJECT = 0x5,
        FXTYPE_VIEW_FLASH_ADS = 0x6,
        NUM_FX_OVERRIDE_TYPES = 0x7,
    };

    /* 11235 */
    struct FXOverrideEntry
    {
        unsigned short attachment1;
        unsigned short attachment2;
        FxEffectDef* overrideFX;
        FxEffectDef* altmodeFX;
        FXOverrideTypes fxType;
    };

    /* 11236 */
    struct ReloadStateTimerEntry
    {
        int attachment;
        int reloadAddTime;
        int reloadStartAddTime;
    };

    /* 11237 */
    struct NoteTrackToSoundEntry
    {
        int attachment;
        ScriptString* notetrackSoundMapKeys;
        ScriptString* notetrackSoundMapValues;
    };

    /* 804 */
    enum weapAnimFiles_t : int32_t
    {
        WEAP_ANIM_ROOT = 0x0,
        WEAP_ANIM_IDLE = 0x1,
        WEAP_ANIM_EMPTY_IDLE = 0x2,
        WEAP_ANIM_FIRE = 0x3,
        WEAP_ANIM_HOLD_FIRE = 0x4,
        WEAP_ANIM_LASTSHOT = 0x5,
        WEAP_ANIM_RECHAMBER = 0x6,
        WEAP_ANIM_MELEE_SWIPE = 0x7,
        WEAP_ANIM_MELEE_HIT = 0x8,
        WEAP_ANIM_MELEE_FATAL = 0x9,
        WEAP_ANIM_MELEE_MISS = 0xA,
        WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT = 0xB,
        WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL = 0xC,
        WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS = 0xD,
        WEAP_ANIM_RELOAD = 0xE,
        WEAP_ANIM_RELOAD_EMPTY = 0xF,
        WEAP_ANIM_RELOAD_START = 0x10,
        WEAP_ANIM_RELOAD_END = 0x11,
        WEAP_ANIM_FAST_RELOAD = 0x12,
        WEAP_ANIM_FAST_RELOAD_EMPTY = 0x13,
        WEAP_ANIM_FAST_RELOAD_START = 0x14,
        WEAP_ANIM_FAST_RELOAD_END = 0x15,
        WEAP_ANIM_RAISE = 0x16,
        WEAP_ANIM_FIRST_RAISE = 0x17,
        WEAP_ANIM_BREACH_RAISE = 0x18,
        WEAP_ANIM_DROP = 0x19,
        WEAP_ANIM_ALT_RAISE = 0x1A,
        WEAP_ANIM_ALT_DROP = 0x1B,
        WEAP_ANIM_QUICK_RAISE = 0x1C,
        WEAP_ANIM_QUICK_DROP = 0x1D,
        WEAP_ANIM_EMPTY_RAISE = 0x1E,
        WEAP_ANIM_EMPTY_DROP = 0x1F,
        WEAP_ANIM_SPRINT_IN = 0x20,
        WEAP_ANIM_SPRINT_IN_CANCEL = 0x21,
        WEAP_ANIM_SPRINT_LOOP = 0x22,
        WEAP_ANIM_SPRINT_OUT = 0x23,
        WEAP_ANIM_STUNNED_START = 0x24,
        WEAP_ANIM_STUNNED_LOOP = 0x25,
        WEAP_ANIM_STUNNED_END = 0x26,
        WEAP_ANIM_DETONATE = 0x27,
        WEAP_ANIM_NIGHTVISION_WEAR = 0x28,
        WEAP_ANIM_NIGHTVISION_REMOVE = 0x29,
        WEAP_ANIM_ADS_FIRE = 0x2A,
        WEAP_ANIM_ADS_LASTSHOT = 0x2B,
        WEAP_ANIM_ADS_RECHAMBER = 0x2C,
        WEAP_ANIM_BLAST_FRONT = 0x2D,
        WEAP_ANIM_BLAST_RIGHT = 0x2E,
        WEAP_ANIM_BLAST_BACK = 0x2F,
        WEAP_ANIM_BLAST_LEFT = 0x30,
        WEAP_ANIM_SLIDE_IN = 0x31,
        WEAP_ANIM_SLIDE_LOOP = 0x32,
        WEAP_ANIM_SLIDE_OUT = 0x33,
        WEAP_ANIM_RECOIL_SETTLE = 0x34,
        WEAP_ANIM_SWIM_LOOP = 0x35,
        WEAP_ANIM_MANTLE_UP_64 = 0x36,
        WEAP_ANIM_MANTLE_UP_56 = 0x37,
        WEAP_ANIM_MANTLE_UP_48 = 0x38,
        WEAP_ANIM_MANTLE_UP_40 = 0x39,
        WEAP_ANIM_MANTLE_UP_32 = 0x3A,
        WEAP_ANIM_MANTLE_UP_24 = 0x3B,
        WEAP_ANIM_MANTLE_UP_16 = 0x3C,
        WEAP_ANIM_MANTLE_OVER_64 = 0x3D,
        WEAP_ANIM_MANTLE_OVER_56 = 0x3E,
        WEAP_ANIM_MANTLE_OVER_48 = 0x3F,
        WEAP_ANIM_MANTLE_OVER_40 = 0x40,
        WEAP_ANIM_MANTLE_OVER_32 = 0x41,
        WEAP_ANIM_MANTLE_OVER_24 = 0x42,
        WEAP_ANIM_MANTLE_OVER_16 = 0x43,
        WEAP_ANIM_ADS_UP = 0x44,
        WEAP_ANIM_ADS_DOWN = 0x45,
        WEAP_ANIM_RECOIL = 0x46,
        WEAP_ALT_ANIM_ADJUST = 0x47,
        WEAP_ANIM_ADDITIVE_ADS_ROOT = 0x48,
        WEAP_ANIM_ADDITIVE_ADS_UP = 0x49,
        WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT = 0x4A,
        WEAP_ANIM_ADDITIVE_DRAG_LEFT = 0x4B,
        WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT = 0x4C,
        WEAP_ANIM_ADDITIVE_DRAG_RIGHT = 0x4D,
        WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT = 0x4E,
        WEAP_ANIM_ADDITIVE_DRAG_UP = 0x4F,
        WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT = 0x50,
        WEAP_ANIM_ADDITIVE_DRAG_DOWN = 0x51,
        WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT = 0x52,
        WEAP_ANIM_ADDITIVE_SWIM_FORWARD = 0x53,
        WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT = 0x54,
        WEAP_ANIM_ADDITIVE_SWIM_BACKWARD = 0x55,
        WEAP_ANIM_ADDITIVE_JUMP_ROOT = 0x56,
        WEAP_ANIM_ADDITIVE_JUMP = 0x57,
        WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT = 0x58,
        WEAP_ANIM_ADDITIVE_JUMP_LAND = 0x59,
        WEAP_ANIM_ADDITIVE_WALK_ROOT = 0x5A,
        WEAP_ANIM_ADDITIVE_WALK = 0x5B,
        NUM_WEAP_ANIMS = 0x5C,
        WEAP_ANIM_VIEWMODEL_START = 0x1,
        WEAP_ANIM_VIEWMODEL_END = 0x44,
        WEAP_ANIM_ADDITIVE_START = 0x48,
        WEAP_ANIM_ADDITIVE_END = 0x5C,

        WEAP_ANIM_COUNT
    };

    union WeaponAttachmentCombination
    {
        struct
        {
            uint32_t weaponIdx : 8;
            uint32_t weaponVariation : 4;
            uint32_t weaponScopes : 3;
            uint32_t weaponUnderBarrels : 3;
            uint32_t weaponOthers : 8;
            uint32_t scopeVariation : 3;
            uint32_t padding : 3;
        };

        unsigned short fields;
    };

    /* 11238 */
    struct WeaponCompleteDef
    {
        const char* szInternalName;
        WeaponDef* weapDef;
        const char* szDisplayName;
        ScriptString* hideTags;
        WeaponAttachment** scopes;
        WeaponAttachment** underBarrels;
        WeaponAttachment** others;
        const char** szXAnims;
        unsigned int numAnimOverrides;
        AnimOverrideEntry* animOverrides;
        unsigned int numSoundOverrides;
        SoundOverrideEntry* soundOverrides;
        unsigned int numFXOverrides;
        FXOverrideEntry* fxOverrides;
        unsigned int numReloadStateTimerOverrides;
        ReloadStateTimerEntry* reloadOverrides;
        unsigned int numNotetrackOverrides;
        NoteTrackToSoundEntry* notetrackOverrides;
        float fAdsZoomFov;
        int iAdsTransInTime;
        int iAdsTransOutTime;
        int iClipSize;
        ImpactType impactType;
        int iFireTime;
        int iFireTimeAkimbo;
        weaponIconRatioType_t dpadIconRatio;
        float penetrateMultiplier;
        float fAdsViewKickCenterSpeed;
        float fHipViewKickCenterSpeed;
        const char* szAltWeaponName;
        int altWeapon;
        int iAltRaiseTime;
        int iAltRaiseTimeAkimbo;
        Material* killIcon;
        Material* dpadIcon;
        int fireAnimLength;
        int fireAnimLengthAkimbo;
        int iFirstRaiseTime;
        int iFirstRaiseTimeAkimbo;
        int ammoDropStockMax;
        float adsDofStart;
        float adsDofEnd;
        unsigned short aiVsAiAccuracyGraphKnotCount;
        unsigned short aiVsPlayerAccuracyGraphKnotCount;
        float* aiVsAiAccuracyGraphKnots;
        float* aiVsPlayerAccuracyGraphKnots;
        bool motionTracker;
        bool enhanced;
        bool dpadIconShowsAmmo;
        const char* szAdsrBaseSetting;
    };

    /* 10740 */
    struct XAUDIO2FX_REVERB_I3DL2_PARAMETERS
    {
        float WetDryMix;
        int Room;
        int RoomHF;
        float RoomRolloffFactor;
        float DecayTime;
        float DecayHFRatio;
        int Reflections;
        float ReflectionsDelay;
        int Reverb;
        float ReverbDelay;
        float Diffusion;
        float Density;
        float HFReference;
    };

    /* 11962 */
    struct XaReverbSettings
    {
        int presetOverridden;
        XAUDIO2FX_REVERB_I3DL2_PARAMETERS reverbSettings;
    };

    /* 11503 */
    struct SndGlobalSettings_t
    {
        float globalVolumeModifier;
    };

    /* 11963 */
    struct SndDriverGlobals
    {
        const char* name;
        SndGlobalSettings_t settings;
        XaReverbSettings* reverbSettings;
    };

    /* 11964 */
    struct FxImpactEntry
    {
        FxEffectDef* nonflesh[31];
        FxEffectDef* flesh[4];
    };

    /* 11965 */
    struct FxImpactTable
    {
        const char* name;
        FxImpactEntry* table;
    };

    /* 11966 */
    struct SurfaceFxEntry
    {
        FxEffectDef* surfaceEffect[31];
    };

    /* 11967 */
    struct SurfaceFxTable
    {
        const char* name;
        SurfaceFxEntry* table;
    };

    /* 11968 */
    struct RawFile
    {
        const char* name;
        int compressedLen;
        int len;
        const char* buffer;
    };

    /* 11969 */
    struct ScriptFile
    {
        const char* name;
        int compressedLen;
        int len;
        int bytecodeLen;
        const char* buffer;
        unsigned char* bytecode;
    };

    /* 11334 */
    struct StringTableCell
    {
        const char* string;
        int hash;
    };

    /* 11335 */
    struct StringTable
    {
        const char* name;
        int columnCount;
        int rowCount;
        StringTableCell* values;
    };

    /* 1235 */
    enum StatsGroup : int
    {
        STATSGROUP_RANKED = 0x0,
        STATSGROUP_PRIVATE = 0x1,
        STATSGROUP_COOP = 0x2,
        STATSGROUP_COMMON = 0x3,
        STATSGROUP_COUNT = 0x4,
        STATSGROUP_IGNORE = 0x5,
    };

    /* 1247 */
    enum LbColType : int
    {
        LBCOL_TYPE_NUMBER = 0x0,
        LBCOL_TYPE_TIME = 0x1,
        LBCOL_TYPE_LEVELXP = 0x2,
        LBCOL_TYPE_PRESTIGE = 0x3,
        LBCOL_TYPE_BIGNUMBER = 0x4,
        LBCOL_TYPE_PERCENT = 0x5,
        LBCOL_TYPE_TIME_FULL = 0x6,
        LBCOL_TYPE_COUNT = 0x7,
    };

    /* 1248 */
    enum LbAggType : int
    {
        LBAGG_TYPE_MIN = 0x0,
        LBAGG_TYPE_MAX = 0x1,
        LBAGG_TYPE_SUM = 0x2,
        LBAGG_TYPE_LAST = 0x3,
        LBAGG_TYPE_COUNT = 0x4,
    };

    /* 11970 */
    struct LbColumnDef
    {
        const char* name;
        int id;
        int propertyId;
        bool hidden;
        StatsGroup statsGroup;
        const char* statName;
        LbColType type;
        int precision;
        LbAggType agg;
        int uiCalColX;
        int uiCalColY;
    };

    /* 1249 */
    enum LbUpdateType : int
    {
        LBUPDATE_TYPE_NORMAL = 0x0,
        LBUPDATE_TYPE_RANK = 0x1,
        LBUPDATE_TYPE_COMBINE = 0x2,
        LBUPDATE_TYPE_COUNT = 0x3,
    };

    enum LbTrackTypes : int
    {
        LBTRK_TYPE_ALLTIME = 0x0,
        LBTRK_TYPE_WEEKLY = 0x1,
        LBTRK_TYPE_MONTHLY = 0x2,
        LBTRK_TYPE_PRESTIGE_ALLTIME = 0x3,
        LBTRK_TYPE_PRESTIGE_WEEKLY = 0x4,
        LBTRK_TYPE_PRESTIGE_MONTHLY = 0x5,
        LBTRK_TYPE_DAILY = 0x6,
        LBTRK_TYPE_PRESTIGE_DAILY = 0x7,

        LBTRK_TYPE_COUNT = 0x8,
    };

    /* 11971 */
    struct LeaderboardDef
    {
        const char* name;
        int id;
        int columnCount;
        int xpColId;
        int prestigeColId;
        LbColumnDef* columns;
        LbUpdateType updateType;
        int trackTypes;
    };

    /* 11972 */
    struct type_align32(4) StructuredDataEnumEntry
    {
        const char* string;
        unsigned short index;
    };

    /* 11973 */
    struct StructuredDataEnum
    {
        int entryCount;
        int reservedEntryCount;
        StructuredDataEnumEntry* entries;
    };

    /* 11974 */
    union StructuredDataTypeUnion
    {
        unsigned int stringDataLength;
        int enumIndex;
        int structIndex;
        int indexedArrayIndex;
        int enumedArrayIndex;
    };

    /* 1154 */
    enum StructuredDataTypeCategory : int
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
        DATA_COUNT = 0xA,
    };

    /* 11975 */
    struct StructuredDataType
    {
        StructuredDataTypeCategory type;
        StructuredDataTypeUnion u;
    };

    /* 1184 */
    enum StructuredDataValidationType : int
    {
        VALIDATION_NONE = 0x0,
        VALIDATION_CONSTANT = 0x1,
        VALIDATION_INCREASECLAMP = 0x2,
        VALIDATION_INCREASESTRICT = 0x3,
        VALIDATION_DELTACLAMP = 0x4,
        VALIDATION_DELTASTRICT = 0x5,
        VALIDATION_XP = 0x6,
        VALIDATION_COUNT = 0x7,
    };

    /* 11976 */
    struct StructuredDataStructProperty
    {
        const char* name;
        StructuredDataType type;
        unsigned int offset;
        StructuredDataValidationType validation;
    };

    /* 11977 */
    struct StructuredDataStruct
    {
        int propertyCount;
        StructuredDataStructProperty* properties;
        int size;
        unsigned int bitOffset;
    };

    /* 11978 */
    struct StructuredDataIndexedArray
    {
        int arraySize;
        StructuredDataType elementType;
        unsigned int elementSize;
    };

    /* 11979 */
    struct StructuredDataEnumedArray
    {
        int enumIndex;
        StructuredDataType elementType;
        unsigned int elementSize;
    };

    /* 11980 */
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

    /* 11981 */
    struct StructuredDataDefSet
    {
        const char* name;
        unsigned int defCount;
        StructuredDataDef* defs;
    };

    /* 1097 */
    enum VehicleType : int
    {
        VEH_WHEELS_4 = 0x0,
        VEH_TANK = 0x1,
        VEH_PLANE = 0x2,
        VEH_BOAT = 0x3,
        VEH_ARTILLERY = 0x4,
        VEH_HELICOPTER = 0x5,
        VEH_SNOWMOBILE = 0x6,
        VEH_SUBMARINE = 0x7,
        VEH_UGV = 0x8,
        VEH_TYPE_COUNT = 0x9,
    };

    /* 1099 */
    enum VehicleAxleType : int
    {
        VEH_AXLE_FRONT = 0x0,
        VEH_AXLE_REAR = 0x1,
        VEH_AXLE_ALL = 0x2,
        VEH_AXLE_COUNT = 0x3,
    };

    /* 11069 */
    const struct VehiclePhysDef
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

    /* 1098 */
    enum VehCamZOffsetMode : int
    {
        VEHCAM_ZMODE_WORLD = 0x0,
        VEHCAM_ZMODE_VEHICLE = 0x1,
        VEHCAM_ZMODE_VIEW = 0x2,
        VEHCAM_ZMODE_COUNT = 0x3,
    };

    /* 2775 */
    enum VehicleTurretFireType : int
    {
        VEH_TURRET_SINGLE_FIRE = 0x0,
        VEH_TURRET_DUAL_FIRE = 0x1,
        VEH_TURRET_ALT_FIRE = 0x2,
        VEH_TURRET_FIRE_TYPE_COUNT = 0x3,
    };

    /* 11982 */
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
        int vehHelicopterLockAltitude;
        int vehHelicopterOffsetFromMesh;
        float vehHelicopterAltitudeOffset;
        float vehHelicopterPitchOffset;
        float vehHelicopterBoundsRadius;
        float vehHelicopterMaxSpeed;
        float vehHelicopterMaxSpeedVertical;
        float vehHelicopterMaxAccel;
        float vehHelicopterMaxAccelVertical;
        float vehHelicopterDecelerationFwd;
        float vehHelicopterDecelerationSide;
        float vehHelicopterDecelerationUp;
        float vehHelicopterMaxYawRate;
        float vehHelicopterMaxYawAccel;
        float vehHelicopterTiltFromVelocity;
        float vehHelicopterTiltFromControllerAxes;
        float vehHelicopterTiltFromAcceleration;
        float vehHelicopterTiltFromDeceleration;
        float vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt;
        float vehHelicopterTiltFromFwdAndYaw;
        float vehHelicopterTiltMomentum;
        float vehHelicopterTiltSpeed;
        float vehHelicopterMaxPitch;
        float vehHelicopterMaxRoll;
        float vehHelicopterHoverSpeedThreshold;
        float vehHelicopterJitterJerkyness;
        float vehHelicopterLookaheadTime;
        int vehHelicopterSoftCollisions;
        int vehHelicopterUseGroundFX;
        FxEffectDef* vehHelicopterGroundFx;
        FxEffectDef* vehHelicopterGroundWaterFx;
        float vehHelicopterGroundFxDefaultRepeatRate;
        float vehHelicopterGroundFxSlowestRepeatRate;
        float vehHelicopterGroundFxFastestRepeatRate;
        float vehHelicopterGroundFxMinGroundDist;
        float vehHelicopterGroundFxMaxGroundDist;
        int camLookEnabled;
        int camRelativeControl;
        int camRemoteDrive;
        float camLerp;
        float camHeight;
        float camRadius;
        float camPitchInfluence;
        float camYawInfluence;
        float camRollInfluence;
        float camFovIncrease;
        float camFovOffset;
        float camFovSpeed;
        float camReturnSpeed;
        float camReturnLerp;
        int vehCam_UseGDT;
        float vehCam_anglesPitch;
        float vehCam_anglesYaw;
        float vehCam_anglesRoll;
        float vehCam_offsetX;
        float vehCam_offsetY;
        float vehCam_offsetZ;
        float vehCam_radius;
        float vehCam_speedInfluence;
        float vehCam_pitchTurnRate;
        float vehCam_pitchClamp;
        float vehCam_yawTurnRate;
        float vehCam_yawClamp;
        VehCamZOffsetMode vehCam_zOffsetMode;
        float vehCam_anglesPitch3P;
        float vehCam_anglesYaw3P;
        float vehCam_anglesRoll3P;
        float vehCam_offsetX3P;
        float vehCam_offsetY3P;
        float vehCam_offsetZ3P;
        float vehCam_radius3P;
        float vehCam_speedInfluence3P;
        float vehCam_pitchTurnRate3P;
        float vehCam_pitchClamp3P;
        float vehCam_yawTurnRate3P;
        float vehCam_yawClamp3P;
        VehCamZOffsetMode vehCam_zOffsetMode3P;
        const char* turretWeaponName;
        WeaponCompleteDef* turretWeapon;
        float turretHorizSpanLeft;
        float turretHorizSpanRight;
        float turretVertSpanUp;
        float turretVertSpanDown;
        float turretHorizResistLeft;
        float turretHorizResistRight;
        float turretVertResistUp;
        float turretVertResistDown;
        float turretRotRate;
        VehicleTurretFireType turretFireType;
        snd_alias_list_t* turretSpinSnd;
        snd_alias_list_t* turretStopSnd;
        int trophyEnabled;
        float trophyRadius;
        float trophyInactiveRadius;
        int trophyAmmoCount;
        float trophyReloadTime;
        ScriptString trophyTags[4];
        FxEffectDef* trophyExplodeFx;
        FxEffectDef* trophyFlashFx;
        Material* compassFriendlyIcon;
        Material* compassEnemyIcon;
        Material* compassFriendlyAltIcon;
        Material* compassEnemyAltIcon;
        int compassIconWidth;
        int compassIconHeight;
        snd_alias_list_t* idleLowSnd;
        snd_alias_list_t* idleHighSnd;
        snd_alias_list_t* engineLowSnd;
        snd_alias_list_t* engineHighSnd;
        float engineSndSpeed;
        ScriptString audioOriginTag;
        snd_alias_list_t* idleLowSndAlt;
        snd_alias_list_t* idleHighSndAlt;
        snd_alias_list_t* engineLowSndAlt;
        snd_alias_list_t* engineHighSndAlt;
        float engineSndSpeedAlt;
        ScriptString audioOriginTagAlt;
        snd_alias_list_t* turretSpinSndAlt;
        snd_alias_list_t* turretStopSndAlt;
        snd_alias_list_t* engineStartUpSnd;
        int engineStartUpLength;
        snd_alias_list_t* engineShutdownSnd;
        snd_alias_list_t* engineIdleSnd;
        snd_alias_list_t* engineSustainSnd;
        snd_alias_list_t* engineRampUpSnd;
        int engineRampUpLength;
        snd_alias_list_t* engineRampDownSnd;
        int engineRampDownLength;
        snd_alias_list_t* suspensionSoftSnd;
        float suspensionSoftCompression;
        snd_alias_list_t* suspensionHardSnd;
        float suspensionHardCompression;
        snd_alias_list_t* collisionSnd;
        float collisionBlendSpeed;
        snd_alias_list_t* speedSnd;
        float speedSndBlendSpeed;
        const char* surfaceSndPrefix;
        snd_alias_list_t* surfaceSnds[31];
        float surfaceSndBlendSpeed;
        float slideVolume;
        float slideBlendSpeed;
        float inAirPitch;
    };

    /* 11983 */
    struct AddonMapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
        ClipInfo* info;
        unsigned int numSubModels;
        cmodel_t* cmodels;
        GfxBrushModel* models;
    };

    /* 1771 */
    enum NetConstStringType : int
    {
        NETCONSTSTRINGTYPE_XMODEL = 0x0,
        NETCONSTSTRINGTYPE_MATERIAL = 0x1,
        NETCONSTSTRINGTYPE_RUMBLE = 0x2,
        NETCONSTSTRINGTYPE_VEHICLES = 0x3,
        NETCONSTSTRINGTYPE_FX = 0x4,
        NETCONSTSTRINGTYPE_LOCSTRING = 0x5,
        NETCONSTSTRINGTYPE_SOUNDALIAS = 0x6,
        NETCONSTSTRINGTYPE_SOUNDALIAS_LOOPING = 0x7,
        NETCONSTSTRINGTYPE_SHOCK = 0x8,
        NETCONSTSTRINGTYPE_SCRIPTMENU = 0x9,
        NETCONSTSTRINGTYPE_CLIENT_TAGS = 0xA,
        NETCONSTSTRINGTYPE_HEADICON = 0xB,
        NETCONSTSTRINGTYPE_STATUSICON = 0xC,
        NETCONSTSTRINGTYPE_NAMEPLATE = 0xD,
        NETCONSTSTRINGTYPE_MINIMAPICON = 0xE,
        NETCONSTSTRINGTYPE_LOCSELMAT = 0xF,
        NETCONSTSTRINGTYPE_WEAPON = 0x10,
        NETCONSTSTRINGTYPE_HINTSTRING = 0x11,
        NETCONSTSTRINGTYPE_ANIM = 0x12,
        NETCONSTSTRINGTYPE_TAGS = 0x13,
        NETCONSTSTRINGTYPE_ANIMCLASS = 0x14,
        NETCONSTSTRINGTYPE_LUI = 0x15,
        NETCONSTSTRINGTYPE_ASSET_COUNT = 0x16,
        NETCONSTSTRINGTYPE_CODINFO_DVAR = 0x16,
        NETCONSTSTRINGTYPE_NETWORK_DVAR = 0x17,
        NETCONSTSTRINGTYPE_COUNT = 0x18,
        NETCONSTSTRINGTYPE_NONE = 0x18,
    };

    /* 1772 */
    enum NetConstStringSource : int
    {
        NETCONSTSTRINGSOURCE_MAP = 0x0,
        NETCONSTSTRINGSOURCE_PRE_MAP = 0x1,
        NETCONSTSTRINGSOURCE_COMMON = 0x2,
        NETCONSTSTRINGSOURCE_COUNT = 0x3,
        NETCONSTSTRINGSOURCE_NONE = 0x3,
    };

    /* 11984 */
    struct NetConstStrings
    {
        const char* name;
        NetConstStringType stringType;
        NetConstStringSource sourceType;
        unsigned int entryCount;
        const char** stringList;
    };

    /* 11337 */
    struct ReverbPreset
    {
        const char* p_name;
        float earlyTime;
        float lateTime;
        float earlyGain;
        float lateGain;
        float lateGainProx;
        float returnGain;
        float earlyLpf;
        float lateLpf;
        float inputLpf;
        float dampLpf;
        float wallReflect;
        float dryGain;
        float earlySize;
        float lateSize;
        float diffusion;
        float rearLevel;
    };

    /* 11985 */
    struct LuaFile
    {
        const char* name;
        int len;
        const unsigned char* buffer;
    };

    /* 11986 */
    struct EquipmentClothData
    {
        char* szName;
    };

    /* 11987 */
    struct EquipmentWeaponRattleData
    {
        char* szName;
        float priority;
    };

    /* 11988 */
    struct EquipmentSndChance
    {
        float rattleChance;
        float accentChance;
        float silentChance;
    };

    /* 11989 */
    struct EquipmentChanceMoveTypes
    {
        EquipmentSndChance* chances;
    };

    /* 11990 */
    struct EquipmentChanceRattleTypes
    {
        EquipmentChanceMoveTypes* chances;
    };

    /* 11991 */
    struct EquipmentSoundSet
    {
        snd_alias_list_t* soundPLR;
        snd_alias_list_t* soundNPC;
    };

    /* 11992 */
    struct EquipSoundSetMoveTypes
    {
        EquipmentSoundSet* soundSets;
    };

    /* 11993 */
    struct EquipmentSoundTable
    {
        const char* szName;
        unsigned int numClothTypes;
        unsigned int numWeaponRattleTypes;
        unsigned int numMoveTypes;
        EquipmentClothData* clothTypes;
        EquipmentWeaponRattleData* weaponRattleTypes;
        EquipmentChanceRattleTypes* chancesPLR;
        EquipmentChanceRattleTypes* chancesNPC;
        EquipSoundSetMoveTypes* mvmtClothSoundSets;
        EquipSoundSetMoveTypes* mvmtRattleSoundSets;
        EquipSoundSetMoveTypes mvmtAccentSoundSets;
    };



#ifndef __zonecodegenerator
#ifndef __ida
}
#endif
#endif

#endif

// EOF
