#pragma once

#ifndef __CODO_ASSETS_H
#define __CODO_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace CODO
{
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

        MAX_XFILE_COUNT,
    };

    struct PhysPreset;
    struct PhysCollmap;
    struct XAnimParts;
    struct XModelSurfs;
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
    struct MapEnts;
    struct FxWorld;
    struct GfxWorld;
    struct GfxLightDef;
    struct Font_s;
    struct MenuList;
    struct menuDef_t;
    struct LocalizeEntry;
    struct WeaponCompleteDef;
    struct FxEffectDef;
    struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    struct LeaderboardDef;
    struct StructuredDataDefSet;
    struct TracerDef;
    struct VehicleDef;
    struct AddonMapEnts;
    struct WeaponAttach;
    struct NetConstStrings;
    struct MarketplaceData;
    struct Statement_s;
    struct itemDef_s;
    struct MenuEventHandlerSet;

    typedef unsigned short ScriptString;
    typedef unsigned char cbrushedge_t;

    // The following asset types do not have any data in zones and therefore are not part of the header
    // aipaths (14)
    // glass_map (17)
    // vehicle_track (18)
    // ui_map (23)
    // snddriverglobals (29)
    // aitype (32)
    // mptype (33)
    // character (34)
    // xmodelalias (35)
    union XAssetHeader
    {
        PhysPreset* physPreset;
        PhysCollmap* physCollmap;
        XAnimParts* parts;
        XModelSurfs* modelSurfs;
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
        MapEnts* mapEnts;
        FxWorld* fxWorld;
        GfxWorld* gfxWorld;
        GfxLightDef* lightDef;
        Font_s* font;
        MenuList* menuList;
        menuDef_t* menu;
        LocalizeEntry* localize;
        WeaponCompleteDef* weapon;
        FxEffectDef* fx;
        FxImpactTable* impactFx;
        RawFile* rawfile;
        StringTable* stringTable;
        LeaderboardDef* leaderboardDef;
        StructuredDataDefSet* structuredDataDefSet;
        TracerDef* tracerDef;
        VehicleDef* vehDef;
        AddonMapEnts* addonMapEnts;
        WeaponAttach* weaponAttach;
        NetConstStrings* netConstStrings;
        MarketplaceData* marketplaceData;
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

    struct Bounds
    {
        vec3_t midPoint;
        vec3_t halfSize;
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
        bool perSurfaceSndAlias;
        unsigned int unknown2C[6];
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
        uint16_t materialNum;
        unsigned char firstAdjacentSideOffset;
        unsigned char edgeCount;
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

    union XAnimDynamicFrames
    {
        ByteVec* _1;
        UShortVec* _2;
    };

    union XAnimDynamicIndicesTrans
    {
        unsigned char _1[1];
        uint16_t _2[1];
    };

    struct type_align32(4) XAnimPartTransFrames
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

    union XAnimDynamicIndicesQuat2
    {
        unsigned char _1[1];
        uint16_t _2[1];
    };

    struct type_align(4) XQuat2
    {
        int16_t value[2];
    };

    struct type_align32(4) XAnimDeltaPartQuatDataFrames2
    {
        XQuat2* frames;
        XAnimDynamicIndicesQuat2 indices;
    };

    union XAnimDeltaPartQuatData2
    {
        XAnimDeltaPartQuatDataFrames2 frames;
        XQuat2 frame0;
    };

    struct XAnimDeltaPartQuat2
    {
        uint16_t size;
        XAnimDeltaPartQuatData2 u;
    };

    union XAnimDynamicIndicesQuat
    {
        unsigned char _1[1];
        uint16_t _2[1];
    };

    struct type_align(4) XQuat
    {
        int16_t value[4];
    };

    struct XAnimDeltaPartQuatDataFrames
    {
        XQuat* frames;
        XAnimDynamicIndicesQuat indices;
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

    enum XAnimFlags
    {
        ANIM_LOOP = 0x1,
        ANIM_DELTA = 0x2,
        ANIM_DELTA_3D = 0x4,
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
        unsigned char flags;
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

    struct XSurfaceVertexInfo
    {
        int16_t vertCount[4];
        uint16_t* vertsBlend;
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
        char array[4];
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
        vec3_t trans;
        vec3_t scale;
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

    struct XSurfaceTri
    {
        uint16_t i[3];
    };

    typedef tdef_align32(16) XSurfaceTri XSurfaceTri16;

    struct XSurface
    {
        unsigned char tileMode;
        unsigned char flags;
        unsigned char unknown2;
        char zoneHandle;
        uint16_t vertCount;
        uint16_t triCount;
        uint16_t baseTriIndex;
        uint16_t baseVertIndex;
        float quantizeScale;
        XSurfaceTri16* triIndices;
        XSurfaceVertexInfo vertInfo;
        GfxPackedVertex* verts0;
        void* vb0;
        unsigned int vertListCount;
        XRigidVertList* vertList;
        void* indexBuffer;
        int partBits[6];
        unsigned int unknown4C[2];
    };

    struct XModelSurfs
    {
        const char* name;
        XSurface* surfs;
        uint16_t numsurfs;
        int partBits[6];
        unsigned int unknown24[3];
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
        vec4_t quat;
        vec3_t trans;
        float transWeight;
    };

    struct XModelQuat
    {
        int16_t v[4];
    };

    struct XModelLodInfo
    {
        float dist;
        uint16_t numsurfs;
        uint16_t surfIndex;
        XModelSurfs* modelSurfs;
        const char* unknownC;
        int partBits[6];
        unsigned int unknown28[2];
        XSurface* surfs;
        char lod;
        char smcBaseIndexPlusOne;
        char smcSubIndexMask;
        char smcBucket;
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
        unsigned int unknown24[2];
        ScriptString* boneNames;
        unsigned char* parentList;
        XModelQuat* quats;
        float* trans;
        unsigned char* partClassification;
        DObjAnimMat* baseMat;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        char maxLoadedLod;
        unsigned char numLods;
        char collLod;
        unsigned char flags;
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
        MaterialTextureDefSamplerState samplerState;
        unsigned char semantic;
        MaterialTextureDefInfo u;
    };

    struct type_align(16) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        vec4_t literal;
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
        unsigned int unused0 : 1;           // 29
        unsigned int gammaWrite : 1;        // 30
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

    enum GfxCameraRegionType
    {
        CAMERA_REGION_LIT_OPAQUE = 0x0,
        CAMERA_REGION_LIT_TRANS = 0x1,
        CAMERA_REGION_EMISSIVE = 0x2,
        CAMERA_REGION_DEPTH_HACK = 0x3,

        CAMERA_REGION_COUNT,
        CAMERA_REGION_NONE = CAMERA_REGION_COUNT,
    };

    union GfxDrawSurf
    {
        unsigned int packed[4];
    };

    struct MaterialInfo
    {
        GfxDrawSurf drawSurf;
        const char* name;
        unsigned int surfaceTypeBits;
        uint16_t hashIndex;
        unsigned short gameFlags;
        unsigned char sortKey;
        unsigned char textureAtlasRowCount;
        unsigned char textureAtlasColumnCount;
        unsigned char unknown1F;
    };

    struct Material
    {
        MaterialInfo info;
        unsigned char stateBitsEntry[49];
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

    enum MaterialVertexDeclType
    {
        VERTDECL_GENERIC = 0x0,
        VERTDECL_PACKED = 0x1,
        VERTDECL_WORLD = 0x2,
        VERTDECL_WORLD_T1N0 = 0x3,
        VERTDECL_WORLD_T1N1 = 0x4,
        VERTDECL_WORLD_T2N0 = 0x5,
        VERTDECL_WORLD_T2N1 = 0x6,
        VERTDECL_WORLD_T2N2 = 0x7,
        VERTDECL_WORLD_T3N0 = 0x8,
        VERTDECL_WORLD_T3N1 = 0x9,
        VERTDECL_WORLD_T3N2 = 0xA,
        VERTDECL_WORLD_T4N0 = 0xB,
        VERTDECL_WORLD_T4N1 = 0xC,
        VERTDECL_WORLD_T4N2 = 0xD,
        VERTDECL_POS_TEX = 0xE,
        VERTDECL_STATICMODELCACHE = 0xF,

        VERTDECL_COUNT
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[13];
        void /*IDirect3DVertexDeclaration9*/* decl[VERTDECL_COUNT];
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
        MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN = 0x1,
        MTL_TECHFLAG_NEEDS_RESOLVED_SCENE = 0x2,

        // These 3 are set so rare, it seems like they are just based on name just like other games do
        MTL_TECHFLAG_ZPREPASS = 0x4,
        MTL_TECHFLAG_BUILD_FLOATZ = 0x8,
        MTL_TECHFLAG_BUILD_SHADOW_MAP_DEPTH_OR_MODEL = 0x10,

        MTL_TECHFLAG_DECL_HAS_OPTIONAL_SOURCE = 0x20,

        MTL_TECHFLAG_USES_LIGHT_SPOT_FACTORS = 0x40,
        MTL_TECHFLAG_USES_FLOATZ = 0x80,             // uses floatZ sampler and does not have 0x100 flag
        MTL_TECHFLAG_USES_DISTORTION_FLOATZ = 0x100, // distortion_scale_zfeather_dtex + distortion_scale_ua_zfeather + distortion_scale_zfeather
        TECHNIQUE_FLAG_200 = 0x200,                  // ?
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
        TECHNIQUE_UNKNOWN_48 = 0x30,

        TECHNIQUE_COUNT
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
        unsigned char unused[2];
        MaterialTechniqueSet* remappedTechniqueSet;
        MaterialTechniqueSet* remappedTechniqueSet2; // Used when the 0x100 gameFlag of a material is set
        MaterialTechnique* techniques[TECHNIQUE_COUNT];
    };

    struct type_align32(4) GfxImageLoadDef
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
        GfxImageLoadDef* loadDef;
    };

    struct Picmip
    {
        char platform[2];
    };

    struct CardMemory
    {
        int platform[4];
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

        // These are specific to codo and unverified
        IMG_CATEGORY_UNKNOWN_1 = 0x9, // reflection probes (?)
        IMG_CATEGORY_UNKNOWN_2 = 0xA, // lightmap (?)
        IMG_CATEGORY_UNKNOWN_3 = 0xB, // compass (?)
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
        unsigned int unknownC;
        CardMemory cardMemory;
        uint16_t width;
        uint16_t height;
        uint16_t depth;
        uint16_t unknown26;
        unsigned int unknown28;
        unsigned int unknown2C;
        const char* name;
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
        unsigned int unknown3C[6];
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

    struct AILSOUNDINFO
    {
        int format;
        const void* data_ptr;
        unsigned int data_len;
        unsigned int rate;
        int bits;
        int channels;
        unsigned int unknown18;
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

    typedef unsigned short LeafBrush;

    struct ClipMaterial
    {
        const char* name;
        int surfaceFlags;
        int contents;
    };

    struct cLeafBrushNodeLeaf_t
    {
        LeafBrush* brushes;
    };

    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        unsigned short childOffset[2];
    };

    union cLeafBrushNodeData_t
    {
        cLeafBrushNodeLeaf_t leaf;
        cLeafBrushNodeChildren_t children;
    };

    struct cLeafBrushNode_s
    {
        unsigned char axis;
        short leafBrushCount;
        int contents;
        cLeafBrushNodeData_t data;
    };

    struct cbrush_t
    {
        unsigned short numsides;
        unsigned short glassPieceIndex;
        cbrushside_t* sides;
        cbrushedge_t* baseAdjacentSide;
        short axialMaterialNum[2][3];
        unsigned char firstAdjacentSideOffsets[2][3];
        unsigned char edgeCount[2][3];
    };

    typedef tdef_align32(128) cbrush_t cbrush_array_t;

    typedef tdef_align32(128) Bounds BoundsArray;

    struct ClipInfo
    {
        int planeCount;
        cplane_s* planes;
        unsigned int numMaterials;
        ClipMaterial* materials;
        unsigned int numBrushSides;
        cbrushside_t* brushsides;
        unsigned int numBrushEdges;
        cbrushedge_t* brushEdges;
        unsigned int leafbrushNodesCount;
        cLeafBrushNode_s* leafbrushNodes;
        unsigned int numLeafBrushes;
        LeafBrush* leafbrushes;
        unsigned short numBrushes;
        cbrush_array_t* brushes;
        BoundsArray* brushBounds;
        int* brushContents;
    };

    struct cStaticModel_s
    {
        XModel* xmodel;
        float origin[3];
        float invScaledAxis[3][3];
        Bounds absBounds;
    };

    struct cNode_t
    {
        cplane_s* plane;
        short children[2];
    };

    struct cLeaf_t
    {
        unsigned short firstCollAabbIndex;
        unsigned short collAabbCount;
        int brushContents;
        int terrainContents;
        Bounds bounds;
        int leafBrushNode;
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
        unsigned char firstVertSegment;
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
        unsigned short materialIndex;
        unsigned short childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    struct cmodel_t
    {
        Bounds bounds;
        float radius;
        ClipInfo* info;
        cLeaf_t leaf;
    };

    struct Stage
    {
        const char* name;
        float origin[3];
        unsigned short triggerIndex;
        unsigned char sunPrimaryLightIndex;
    };

    struct TriggerModel
    {
        int contents;
        unsigned short hullCount;
        unsigned short firstHull;
    };

    struct TriggerHull
    {
        Bounds bounds;
        int contents;
        unsigned short slabCount;
        unsigned short firstSlab;
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

    struct SModelAabbNode
    {
        Bounds bounds;
        unsigned short firstChild;
        unsigned short childCount;
    };

    enum DynEntityDrawType
    {
        DYNENT_DRAW_MODEL = 0x0,
        DYNENT_DRAW_BRUSH = 0x1,

        DYNENT_DRAW_COUNT,
    };

    enum DynEntityCollType
    {
        DYNENT_COLL_CLIENT_FIRST = 0x0,
        DYNENT_COLL_CLIENT_MODEL = 0x0,
        DYNENT_COLL_CLIENT_BRUSH = 0x1,

        DYNENT_COLL_COUNT,
    };

    enum DynEntityType
    {
        DYNENT_TYPE_INVALID = 0x0,
        DYNENT_TYPE_CLUTTER = 0x1,
        DYNENT_TYPE_DESTRUCT = 0x2,
        DYNENT_TYPE_HINGE = 0x3,
        DYNENT_TYPE_COUNT = 0x4
    };

    struct GfxPlacement
    {
        float quat[4];
        float origin[3];
    };

    struct DynEntityDef
    {
        DynEntityType type;
        GfxPlacement pose;
        XModel* xModel;
        unsigned short brushModel;
        unsigned short physicsBrushModel;
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
        unsigned short flags;
        unsigned short lightingHandle;
        int health;
    };

    struct DynEntityColl
    {
        unsigned short sector;
        unsigned short nextEntInSector;
        float linkMins[2];
        float linkMaxs[2];
    };

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
        unsigned short dynEntCount[DYNENT_COLL_COUNT];
        DynEntityDef* dynEntDefList[DYNENT_DRAW_COUNT];
        DynEntityPose* dynEntPoseList[DYNENT_DRAW_COUNT];
        DynEntityClient* dynEntClientList[DYNENT_DRAW_COUNT];
        DynEntityColl* dynEntCollList[DYNENT_COLL_COUNT];
        unsigned int checksum;
        unsigned char padding[20];
    };

    struct ComPrimaryLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        unsigned char exponent;
        unsigned char unused;
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

    struct MapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
    };

    typedef tdef_align32(16) char raw_byte16;

    typedef tdef_align32(16) float raw_float16;

    typedef unsigned int raw_uint;

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
        short x;
        short y;
    };

    struct FxGlassHoleHeader
    {
        unsigned short uniqueVertCount;
        unsigned char touchVert;
        unsigned char pad[1];
    };

    struct FxGlassCrackHeader
    {
        unsigned short uniqueVertCount;
        unsigned char beginVertIndex;
        unsigned char endVertIndex;
    };

    union type_align(4) FxGlassGeometryData
    {
        FxGlassVertex vert;
        FxGlassHoleHeader hole;
        FxGlassCrackHeader crack;
        unsigned char asBytes[4];
        short anonymous[2];
    };

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

    struct FxGlassSystem
    {
        int time;
        int unknown4;
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
        unsigned char initCount;
        float effectChanceAccum;
        int lastPieceDeletionTime;
    };

    struct FxWorld
    {
        const char* name;
        FxGlassSystem glassSys;
    };

    struct GfxReflectionProbeData
    {
        unsigned short* data;
        unsigned int count;
    };

    typedef tdef_align32(128) unsigned int raw_uint128;

    typedef unsigned char raw_byte;

    typedef unsigned short r_index_t;

    typedef unsigned short StaticModelIndex;

    struct DpvsPlane
    {
        float coeffs[4];
    };

    struct GfxReflectionProbeReferenceOrigin
    {
        float origin[3];
    };

    struct GfxReflectionProbeReference
    {
        unsigned char index;
    };

    struct GfxBrushModelWritable
    {
        Bounds bounds;
    };

    struct type_align32(4) GfxBrushModel
    {
        GfxBrushModelWritable writable;
        Bounds bounds;
        float radius;
        unsigned short surfaceCount;
        unsigned short startSurfIndex;
        uint16_t surfaceCountNoDecal;
    };

    struct srfTriangles_t
    {
        unsigned int vertexLayerData;
        unsigned int firstVertex;
        unsigned short vertexCount;
        unsigned short triCount;
        unsigned int baseIndex;
    };

    struct GfxSurfaceLightingAndFlagsFields
    {
        unsigned char lightmapIndex;
        unsigned char reflectionProbeIndex;
        unsigned char primaryLightIndex;
        unsigned char flags;
    };

    union GfxSurfaceLightingAndFlags
    {
        GfxSurfaceLightingAndFlagsFields fields;
        unsigned int packed;
    };

    struct GfxPackedPlacement
    {
        float origin[3];
        float axis[3][3];
        float scale;
    };

    struct GfxWorldSky
    {
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        unsigned char skySamplerState;
    };

    struct GfxWorldPlane
    {
        float normal[3];
        float dist;
        unsigned char type;
        unsigned char pad[3];
    };

    struct GfxWorldDpvsPlanesC
    {
        int cellCount;
        GfxWorldPlane* planes;
        unsigned short* nodes;
        unsigned int sceneEntCellBits;
    };

    struct GfxAabbTreeC
    {
        Bounds bounds;
        unsigned short childCount;
        unsigned short surfaceCount;
        unsigned short startSurfIndex;
        unsigned short surfaceCountNoDecal;
        unsigned short startSurfIndexNoDecal;
        unsigned short smodelIndexCount;
        unsigned short* smodelIndexes;
        int childrenOffset;
    };

    struct GfxCellTreeC
    {
        GfxAabbTreeC* aabbTree;
    };

    typedef tdef_align32(128) GfxCellTreeC GfxCellTreeC128;

    struct GfxPortalVertex
    {
        float v[3];
    };

    struct GfxPortalC
    {
        unsigned int unknown0[3];
        DpvsPlane plane;
        GfxPortalVertex* vertices;
        unsigned short cellIndex;
        unsigned char vertexCount;
        unsigned char unknown23;
        float hullAxis[2][3];
    };

    struct GfxWorldCellC
    {
        Bounds bounds;
        int portalCount;
        GfxPortalC* portals;
        unsigned char reflectionProbeCount;
        unsigned char* reflectionProbes;
        unsigned char reflectionProbeReferenceCount;
        unsigned char* reflectionProbeReferences;
    };

    struct GfxProbeEntryC
    {
        unsigned short* data;
        unsigned int count;
    };

    struct GfxProbeGroupC
    {
        float origin[3];
        GfxProbeEntryC* entries;
        int entryCount;
    };

    struct GfxLightmapPairC
    {
        GfxImage* primary;
        GfxImage* secondary;
    };

    struct GfxWorldVertexC
    {
        float xyz[3];
        float binormalSign;
        GfxColor color;
        float texCoord[2];
        float lmapCoord[2];
        PackedUnitVec normal;
        PackedUnitVec tangent;
    };

    struct GfxWorldDrawC
    {
        int reflectionProbeCount;
        GfxImage** reflectionProbes;
        GfxProbeGroupC* probeGroups;
        unsigned int unknownC;
        unsigned int reflectionProbeReferenceCount;
        GfxReflectionProbeReferenceOrigin* reflectionProbeReferenceOrigins;
        GfxReflectionProbeReference* reflectionProbeReferences;
        int lightmapCount;
        GfxLightmapPairC* lightmaps;
        raw_byte* lightmapPrimaryTextures;
        raw_byte* lightmapSecondaryTextures;
        GfxImage* lightmapOverridePrimary;
        GfxImage* lightmapOverrideSecondary;
        int vertexCount;
        GfxWorldVertexC* vertices;
        unsigned int unknown3C;
        int vertexLayerDataSize;
        unsigned char* vertexLayerData;
        unsigned int unknown48;
        int indexCount;
        unsigned short* indices;
    };

    struct GfxLightGridEntryC
    {
        unsigned int unknown0;
    };

    struct GfxLightGridColorsC
    {
        unsigned int unknown0[42];
    };

    struct GfxLightGridC
    {
        unsigned char hasLightRegions;
        unsigned char unknown1;
        int sunPrimaryLightIndex;
        unsigned short mins[3];
        unsigned short maxs[3];
        int rowAxis;
        int colAxis;
        unsigned short* rowDataStart;
        int rawRowDataSize;
        unsigned char* rawRowData;
        int entryCount;
        GfxLightGridEntryC* entries;
        int colorCount;
        GfxLightGridColorsC* colors;
    };

    struct GfxMaterialMemoryC
    {
        Material* material;
        int memory;
    };

    struct GfxShadowGeometryC
    {
        unsigned short surfaceCount;
        unsigned short smodelCount;
        unsigned short* sortedSurfIndex;
        unsigned short* smodelIndex;
    };

    struct GfxLightRegionAxisC
    {
        unsigned int unknown0[5];
    };

    struct GfxLightRegionHullC
    {
        float kdopMidPoint[9];
        float kdopHalfSize[9];
        int axisCount;
        GfxLightRegionAxisC* axis;
    };

    struct GfxLightRegionC
    {
        int hullCount;
        GfxLightRegionHullC* hulls;
    };

    struct GfxStaticModelInstC
    {
        Bounds bounds;
        float lightingOrigin[3];
    };

    struct GfxSurfaceC
    {
        srfTriangles_t tris;
        Material* material;
        GfxSurfaceLightingAndFlags laf;
    };

    struct GfxSurfaceBoundsC
    {
        Bounds bounds;
    };

    struct GfxStaticModelDrawInstC
    {
        GfxPackedPlacement placement;
        XModel* model;
        unsigned int unknown38[5];
    };

    struct GfxWorldDpvsStaticC
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
        raw_byte* smodelVisData[3];
        raw_byte* surfaceVisData[3];
        unsigned short* sortedSurfIndex;
        GfxStaticModelInstC* smodelInsts;
        GfxSurfaceC* surfaces;
        GfxSurfaceBoundsC* surfacesBounds;
        GfxStaticModelDrawInstC* smodelDrawInsts;
        raw_byte* surfaceMaterials;
        raw_byte* surfaceCastsSunShadow;
        int usageCount;
    };

    struct GfxLensFlareElementC
    {
        float unknown0[9];
    };

    struct sunflare_t
    {
        unsigned char hasValidData;
        Material* spriteMaterial;
        Material* flareMaterial;
        Material* lensFlareMaterial;
        Material* lensFlareMaskMaterial;
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
        unsigned int unknown68[4];
        unsigned char unknown78;
        float unknown7C;
        float unknown80;
        float unknown84[3];
        float unknown90;
        unsigned int unknown94;
        float unknown98;
        float unknown9C;
        int lensFlareElementCount;
        GfxLensFlareElementC lensFlareElements[25];
    };

    struct GfxHeroOnlyLight
    {
        unsigned char type;
        unsigned char unused[3];
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
    };

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        int surfaceCount;
        int skyCount;
        GfxWorldSky* skies;
        int lastSunPrimaryLightIndex;
        int primaryLightCount;
        unsigned int sortKeyLitDecal;
        unsigned int sortKeyEffectDecal;
        unsigned int sortKeyEffectAuto;
        unsigned int sortKeyDistortion;
        unsigned int unknown34;
        GfxWorldDpvsPlanesC dpvsPlanes;
        int* aabbTreeCounts;
        GfxCellTreeC128* aabbTrees;
        GfxWorldCellC* cells;
        GfxWorldDrawC draw;
        GfxLightGridC lightGrid;
        int modelCount;
        GfxBrushModel* models;
        Bounds bounds;
        unsigned int checksum;
        int materialMemoryCount;
        GfxMaterialMemoryC* materialMemory;
        sunflare_t sun;
        float outdoorLookupMatrix[4][4];
        GfxImage* outdoorImage;
        raw_uint* cellCasterBits;
        raw_uint* cellHasSunLitSurfsBits;
        raw_byte* sceneDynModel;
        raw_byte* sceneDynBrush;
        raw_uint* primaryLightEntityShadowVis;
        raw_uint* primaryLightDynEntShadowVis[2];
        raw_byte* nonSunPrimaryLightForModelDynEnt;
        GfxShadowGeometryC* shadowGeom;
        GfxLightRegionC* lightRegion;
        GfxWorldDpvsStaticC dpvs;
        unsigned int dynEntClientWordCount[2];
        unsigned int dynEntClientCount[2];
        raw_uint* dynEntCellBits[2];
        raw_byte* dynEntVisData[2][3];
        unsigned int unknown63C;
        unsigned int heroOnlyLightCount;
        GfxHeroOnlyLight* heroOnlyLights;
        unsigned int unknown648;
    };

    struct GfxLightImage
    {
        GfxImage* image;
        unsigned char samplerState;
    };

    struct GfxLightDef
    {
        const char* name;
        GfxLightImage attenuation;
        int lmapLookupStart;
    };

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

    struct Font_s
    {
        const char* fontName;
        int pixelHeight;
        int glyphCount;
        Material* material;
        Material* glowMaterial;
        Glyph* glyphs;
    };

    struct ExpressionString
    {
        const char* string;
    };

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

    union entryInternalData
    {
        Operand operand;
        int op;
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

    struct MenuList
    {
        const char* name;
        int menuCount;
        menuDef_t** menus;
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
        unsigned int ownerDrawFlags;
        float borderSize;
        unsigned int staticFlags;
        unsigned int dynamicFlags[1];
        int nextTime;
        float foreColor[4];
        float backColor[4];
        float borderColor[4];
        float outlineColor[4];
        float disableColor[4];
        Material* background;
        GfxImage* image;
    };

    struct Statement_s
    {
        int numEntries;
        expressionEntry* entries;
        int lastExecuteTime;
        Operand lastResult;
    };

    enum ItemType
    {
        ITEM_TYPE_TEXT = 0x0,
        ITEM_TYPE_EDITFIELD = 0x4,
        ITEM_TYPE_LISTBOX = 0x6,
        ITEM_TYPE_NUMERICFIELD = 0x9,
        ITEM_TYPE_SLIDER = 0xA,
        ITEM_TYPE_YESNO = 0xB,
        ITEM_TYPE_MULTI = 0xC,
        ITEM_TYPE_DVARENUM = 0xD,
        ITEM_TYPE_BIND = 0xE,
        ITEM_TYPE_VALIDFILEFIELD = 0x10,
        ITEM_TYPE_DECIMALFIELD = 0x11,
        ITEM_TYPE_UPREDITFIELD = 0x12,
        ITEM_TYPE_NEWS_TICKER = 0x14,
        ITEM_TYPE_TEXT_SCROLL = 0x15,
        ITEM_TYPE_EMAILFIELD = 0x16,
        ITEM_TYPE_PASSWORDFIELD = 0x17,
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
        unsigned int itemFlags;
        menuDef_t* parent;
        MenuEventHandlerSet* mouseEnterText;
        MenuEventHandlerSet* mouseExitText;
        MenuEventHandlerSet* mouseEnter;
        MenuEventHandlerSet* mouseExit;
        MenuEventHandlerSet* action;
        MenuEventHandlerSet* onAccept;
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
        Statement_s* unknown158;
        Statement_s* unknown15C;
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
        int fontIndex;
        int itemCount;
        int cursorItem[1];
        int fadeCycle;
        float fadeClamp;
        float fadeAmount;
        float fadeInAmount;
        float blurRadius;
        unsigned int unknownD0;
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
    };

    struct WeaponSndNameRef
    {
        const char* name;
    };

    struct WeaponCompleteDef
    {
        const char* szInternalName;
        const char* szDisplayName;
        const char* unknown8; // baseWeapon
        const char* unknownC;
        XModel* gunXModel[2];
        XModel* worldModel[2];
        XModel* worldClipModel;
        XModel* rocketModel;
        XModel* knifeModel;
        XModel* worldKnifeModel;
        XModel* unknown30[2];
        unsigned int unknown38[17];
        const char* szXAnims[62];
        const char* szXAnimsRightHanded[62];
        const char* szXAnimsLeftHanded[62];
        unsigned int unknown364[40];
        FxEffectDef* unknown404[16];
        unsigned short unknown444[16];
        FxEffectDef* unknown464[16];
        FxEffectDef* unknown4A4[16];
        unsigned short unknown4E4[16];
        unsigned short unknown504[16];
        FxEffectDef* unknown524[16];
        unsigned short unknown564[16];
        unsigned int unknown584[8];
        FxEffectDef* viewFlashEffect;
        FxEffectDef* worldFlashEffect;
        FxEffectDef* unknown5AC[3];
        const char* unknown5B8; // playerEffectName
        unsigned int unknown5BC[1];
        const char* unknown5C0; // playerEffectTag
        WeaponSndNameRef* pickupSound;
        WeaponSndNameRef* pickupSoundPlayer;
        WeaponSndNameRef* ammoPickupSound;
        WeaponSndNameRef* ammoPickupSoundPlayer;
        WeaponSndNameRef* projectileSound;
        WeaponSndNameRef* pullbackSound;
        WeaponSndNameRef* pullbackSoundPlayer;
        WeaponSndNameRef* fireSound;
        WeaponSndNameRef* fireSoundPlayer;
        WeaponSndNameRef* fireSoundPlayerAkimbo;
        WeaponSndNameRef* fireLoopSound;
        WeaponSndNameRef* fireLoopSoundPlayer;
        WeaponSndNameRef* fireStopSound;
        WeaponSndNameRef* fireStopSoundPlayer;
        WeaponSndNameRef* fireLastSound;
        WeaponSndNameRef* fireLastSoundPlayer;
        WeaponSndNameRef* unknown604; // fireSoundUnknown1
        WeaponSndNameRef* unknown608; // fireSoundUnknown2
        WeaponSndNameRef* emptyFireSound;
        WeaponSndNameRef* emptyFireSoundPlayer;
        WeaponSndNameRef* meleeSwipeSound;
        WeaponSndNameRef* meleeSwipeSoundPlayer;
        WeaponSndNameRef* meleeHitSound;
        WeaponSndNameRef* meleeMissSound;
        WeaponSndNameRef* rechamberSound;
        WeaponSndNameRef* rechamberSoundPlayer;
        WeaponSndNameRef* reloadSound;
        WeaponSndNameRef* reloadSoundPlayer;
        WeaponSndNameRef* reloadEmptySound;
        WeaponSndNameRef* reloadEmptySoundPlayer;
        WeaponSndNameRef* reloadStartSound;
        WeaponSndNameRef* reloadStartSoundPlayer;
        WeaponSndNameRef* reloadEndSound;
        WeaponSndNameRef* reloadEndSoundPlayer;
        WeaponSndNameRef* detonateSound;
        WeaponSndNameRef* detonateSoundPlayer;
        WeaponSndNameRef* nightVisionWearSound;
        WeaponSndNameRef* nightVisionWearSoundPlayer;
        WeaponSndNameRef* nightVisionRemoveSound;
        WeaponSndNameRef* nightVisionRemoveSoundPlayer;
        WeaponSndNameRef* altSwitchSound;
        WeaponSndNameRef* altSwitchSoundPlayer;
        WeaponSndNameRef* raiseSound;
        WeaponSndNameRef* raiseSoundPlayer;
        WeaponSndNameRef* firstRaiseSound;
        WeaponSndNameRef* firstRaiseSoundPlayer;
        WeaponSndNameRef* putawaySound;
        WeaponSndNameRef* putawaySoundPlayer;
        WeaponSndNameRef* unknown684[5];
        snd_alias_list_t* unknown698[5];
        WeaponSndNameRef** bounceSound;
        WeaponSndNameRef** unknown6B0;
        FxEffectDef* viewShellEjectEffect;
        FxEffectDef* worldShellEjectEffect;
        FxEffectDef* viewLastShotEjectEffect;
        FxEffectDef* worldLastShotEjectEffect;
        Material* reticleCenter;
        Material* reticleSide;
        int iReticleCenterSize;
        int iReticleSideSize;
        int iReticleMinOfs;
        unsigned int activeReticleType;
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
        unsigned int unknown754[9];
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
        Material* unknown7A0[6];
        unsigned int unknown7B8[1];
        Material* unknown7BC[1];
        unsigned int unknown7C0[1];
        Material* unknown7C4[1];
        unsigned int unknown7C8[2];
        Material* unknown7D0[4];
        unsigned int iStartAmmo;
        unsigned int unknown7E4[1];
        const char* szAmmoName;
        unsigned int iAmmoIndex;
        const char* szClipName;
        unsigned int iClipIndex;
        unsigned int iMaxAmmo;
        unsigned int shotCount;
        const char* szSharedAmmoCapName;
        unsigned int iSharedAmmoCapIndex;
        unsigned int iSharedAmmoCap;
        unsigned int unknown80C[1];
        unsigned int playerDamage;
        unsigned int minPlayerDamage;
        unsigned int iMeleeDamage;
        unsigned int unknown81C[3];
        int iFireDelay;
        unsigned int unknown82C;
        int meleeChargeDelay;
        unsigned int unknown834;
        int iDetonateDelay;
        unsigned int unknown83C[2];
        unsigned int iRechamberBoltTime;
        unsigned int iHoldFireTime;
        unsigned int iDetonateTime;
        unsigned int iMeleeTime;
        unsigned int meleeChargeTime;
        unsigned int unknown858[1];
        unsigned int iReloadTime;
        unsigned int unknown860[1];
        unsigned int reloadShowRocketTime;
        unsigned int iReloadEmptyTime;
        unsigned int unknown86C[1];
        unsigned int iReloadAddTime;
        unsigned int unknown874[3];
        unsigned int iReloadStartTime;
        unsigned int iReloadStartAddTime;
        unsigned int iReloadEndTime;
        unsigned int iDropTime;
        unsigned int iRaiseTime;
        unsigned int iAltDropTime;
        unsigned int quickDropTime;
        unsigned int quickRaiseTime;
        unsigned int iBreachRaiseTime;
        unsigned int iEmptyRaiseTime;
        unsigned int iEmptyDropTime;
        unsigned int sprintInTime;
        unsigned int sprintLoopTime;
        unsigned int sprintOutTime;
        unsigned int stunnedTimeBegin;
        unsigned int stunnedTimeLoop;
        unsigned int stunnedTimeEnd;
        unsigned int unknown8C4[3];
        unsigned int nightVisionWearTime;
        unsigned int nightVisionWearTimeFadeOutEnd;
        unsigned int nightVisionWearTimePowerUp;
        unsigned int nightVisionRemoveTime;
        unsigned int nightVisionRemoveTimePowerDown;
        unsigned int nightVisionRemoveTimeFadeInStart;
        unsigned int unknown8E8; // iInspectTime
        unsigned int fuseTime;
        unsigned int aiFuseTime;
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
        unsigned int overlayReticle;
        unsigned int overlayInterface;
        float overlayWidth;
        float overlayHeight;
        float overlayWidthSplitscreen;
        float overlayHeightSplitscreen;
        float fAdsBobFactor;
        float fAdsViewBobMult;
        unsigned int unknown94C[1];
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
        float fAdsViewKickCenterSpeed;
        float fHipViewKickCenterSpeed;
        unsigned int unknown9AC[2];
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
        unsigned int unknown9E8[2];
        PhysCollmap* physCollmap;
        PhysPreset* unknown9F4; // physPreset
        float dualWieldViewModelOffset;
        unsigned int unknown9FC[2];
        unsigned int iReloadAmmoAdd;
        unsigned int iReloadStartAdd;
        unsigned int unknownA0C[1];
        unsigned int ammoDropStockMin;
        unsigned int ammoDropClipPercentMin;
        unsigned int ammoDropClipPercentMax;
        unsigned int iExplosionRadius;
        unsigned int iExplosionRadiusMin;
        unsigned int iExplosionInnerDamage;
        unsigned int iExplosionOuterDamage;
        float damageConeAngle;
        float bulletExplDmgMult;
        float bulletExplRadiusMult;
        unsigned int iProjectileSpeed;
        int iProjectileSpeedUp;
        unsigned int unknownA40[4];
        int iProjectileActivateDist;
        float projLifetime;
        unsigned int unknownA58[2];
        XModel* projectileModel;
        unsigned int projExplosion;
        FxEffectDef* projExplosionEffect;
        FxEffectDef* projDudEffect;
        WeaponSndNameRef* projExplosionSound;
        WeaponSndNameRef* projDudSound;
        unsigned int stickiness;
        float lowAmmoWarningThreshold;
        float ricochetChance;
        unsigned int unknownA84[3];
        float parallelBounce[31];
        float perpendicularBounce[31];
        FxEffectDef* projTrailEffect;
        FxEffectDef* projBeaconEffect;
        float vProjectileColor[3];
        unsigned int guidedMissileType;
        float maxSteeringAccel;
        unsigned int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        WeaponSndNameRef* projIgnitionSound;
        FxEffectDef* unknownBB0; // effect2ec
        FxEffectDef* unknownBB4; // spectatorTrailEffectSc
        FxEffectDef* unknownBB8; // spectatorTrailEffectTf
        FxEffectDef* unknownBBC; // effect2ef
        FxEffectDef* unknownBC0; // impactEffect
        unsigned int unknownBC4[1];
        float fAdsAimPitch;
        float fAdsCrosshairInFrac;
        float fAdsCrosshairOutFrac;
        unsigned int adsGunKickReducedKickBullets;
        float adsGunKickReducedKickPercent;
        float fAdsGunKickPitchMin;
        float fAdsGunKickPitchMax;
        float fAdsGunKickYawMin;
        float fAdsGunKickYawMax;
        float fAdsGunKickAccel;
        float fAdsGunKickSpeedMax;
        float fAdsGunKickSpeedDecay;
        float fAdsGunKickStaticDecay;
        unsigned int unknownBFC[1];
        float fAdsViewKickPitchMin;
        float fAdsViewKickPitchMax;
        float fAdsViewKickYawMin;
        float fAdsViewKickYawMax;
        float fAdsViewScatterMin;
        float fAdsViewScatterMax;
        float fAdsSpread;
        float fAdsZoomFov;
        unsigned int unknownC20[6];
        float fHipGunKickPitchMin;
        float fHipGunKickPitchMax;
        float fHipGunKickYawMin;
        float fHipGunKickYawMax;
        float fHipGunKickAccel;
        float fHipGunKickSpeedMax;
        float fHipGunKickSpeedDecay;
        float fHipGunKickStaticDecay;
        unsigned int unknownC58[1];
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
        unsigned int unknownC8C[1];
        unsigned int iPositionReloadTransTime;
        float leftArc;
        float rightArc;
        float topArc;
        float bottomArc;
        unsigned int unknownCA4[1];
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
        unsigned int scanPauseTime;
        const char* unknownD00;
        const char* szScript;
        float fOOPosAnimLength[2];
        unsigned int unknownD10[6]; // damage
        float unknownD28[6];        // damageRange
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        unsigned int unknownD4C[21];
        const char* fireRumble;
        const char* meleeImpactRumble;
        TracerDef* unknownDA8[3];
        float turretScopeZoomRate;
        float turretScopeZoomMin;
        float turretScopeZoomMax;
        float turretOverheatUpRate;
        float turretOverheatDownRate;
        float turretOverheatPenalty;
        WeaponSndNameRef* turretOverheatSound;
        FxEffectDef* turretOverheatEffect;
        const char* turretBarrelSpinRumble;
        unsigned int turretBarrelSpinSpeed;
        unsigned int turretBarrelSpinUpTime;
        unsigned int turretBarrelSpinDownTime;
        WeaponSndNameRef* turretBarrelSpinMaxSnd;
        WeaponSndNameRef* turretBarrelSpinUpSnd[4];
        WeaponSndNameRef* turretBarrelSpinDownSnd[4];
        WeaponSndNameRef* missileConeSoundAlias;
        WeaponSndNameRef* missileConeSoundAliasAtBase;
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
        unsigned int unknownE44[3];
        WeaponSndNameRef* unknownE50; // loopSoundNpc
        WeaponSndNameRef* unknownE54; // loopSoundPlayer
        WeaponSndNameRef* unknownE58; // startSoundNpc
        WeaponSndNameRef* unknownE5C; // startSoundPlayer
        WeaponSndNameRef* unknownE60; // stopSoundNpc
        WeaponSndNameRef* unknownE64; // stopSoundPlayer
        unsigned int iClipSize;
        unsigned int unknownE6C;
        int iFireTime;
        unsigned int unknownE74[4];
        const char* szAltWeaponName;
        unsigned int unknownE88[3];
        const char* unknownE94[2]; // projectileWeaponNames
        unsigned int unknownE9C[1];
        Material* killIcon;
        Material* unknownEA4;
        Material* dpadIcon;
        Material* unknownEAC;
        unsigned int unknownEB0[2];
        unsigned int ammoDropStockMax;
        unsigned int unknownEBC[2];
        unsigned short aiVsAiAccuracyGraphKnotCount;
        unsigned short aiVsPlayerAccuracyGraphKnotCount;
        vec2_t* aiVsAiAccuracyGraphKnots;
        vec2_t* aiVsPlayerAccuracyGraphKnots;
        unsigned char unknownED0[3];
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
        unsigned char unknownEDD;
        bool aimDownSight;
        bool bRechamberWhileAds;
        unsigned char unknownEE0[3];
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
        unsigned char unknownEEE;
        bool bNoPartialReload;
        bool bSegmentedReload;
        unsigned char unknownEF1;
        bool blocksProne;
        bool silenced;
        bool isRollingGrenade;
        unsigned char unknownEF5[2];
        bool bProjImpactExplode;
        unsigned char unknownEF8;
        bool stickToPlayers;
        unsigned char unknownEFA;
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
        unsigned char unknownF08[32];
        const char* unknownF28[3]; // grenadeLauncherWeaponNames
        unsigned int unknownF34[27];
        const char* unknownFA0; // menuRunnerName
        unsigned int unknownFA4[3];
        const char* unknownFB0; // weaponAbilities
        unsigned int unknownFB4[26];
    };

    enum FxElemType : unsigned char
    {
        FX_ELEM_TYPE_SPRITE_BILLBOARD,
        FX_ELEM_TYPE_SPRITE_ORIENTED,
        FX_ELEM_TYPE_SPRITE_ROTATED,
        FX_ELEM_TYPE_TAIL,
        FX_ELEM_TYPE_LINE,
        FX_ELEM_TYPE_TRAIL,
        FX_ELEM_TYPE_CLOUD,
        FX_ELEM_TYPE_SPARK_CLOUD,
        FX_ELEM_TYPE_SPARK_FOUNTAIN,
        FX_ELEM_TYPE_MODEL,
        FX_ELEM_TYPE_OMNI_LIGHT,
        FX_ELEM_TYPE_SPOT_LIGHT,
        FX_ELEM_TYPE_SOUND,
        FX_ELEM_TYPE_DECAL,
        FX_ELEM_TYPE_RUNNER,

        FX_ELEM_TYPE_COUNT
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
        unsigned char behavior;
        unsigned char index;
        unsigned char fps;
        unsigned char loopCount;
        unsigned char colIndexBits;
        unsigned char rowIndexBits;
        short entryCount;
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
        unsigned char color[4];
        float rotationDelta;
        float rotationTotal;
        float size[2];
        float scale;
        float unknown18[2];
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
        FxElemType elemType;
        unsigned char visualCount;
        unsigned char velIntervalCount;
        unsigned char visStateIntervalCount;
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
        unsigned char sortOrder;
        unsigned char lightingFrac;
        unsigned char useItemClip;
        unsigned char fadeInfo;
        unsigned int unknownFC;
        const char* unknown100;
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
        FxEffectDef* nonflesh[31];
        FxEffectDef* flesh[8];
    };

    struct FxImpactTable
    {
        const char* name;
        FxImpactEntry* table;
    };

    struct LocalizeEntry
    {
        const char* value;
        const char* name;
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

    enum LbColType
    {
        LBCOL_TYPE_NUMBER,
        LBCOL_TYPE_TIME,
        LBCOL_TYPE_LEVELXP,
        LBCOL_TYPE_PRESTIGE,
        LBCOL_TYPE_BIGNUMBER,
        LBCOL_TYPE_PERCENT,

        LBCOL_TYPE_COUNT
    };

    enum LbAggType
    {
        LBAGG_TYPE_MIN,
        LBAGG_TYPE_MAX,
        LBAGG_TYPE_SUM,
        LBAGG_TYPE_LAST,

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

    enum StructuredDataTypeCategory
    {
        DATA_INT,
        DATA_BYTE,
        DATA_BOOL,
        DATA_STRING,
        DATA_ENUM,
        DATA_STRUCT,
        DATA_INDEXED_ARRAY,
        DATA_ENUM_ARRAY,
        DATA_FLOAT,
        DATA_SHORT,

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
        unsigned int unknown10[2];
    };

    struct StructuredDataStruct
    {
        int propertyCount;
        StructuredDataStructProperty* properties;
        int size;
        unsigned int bitOffset;
        unsigned int unknown10;
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
        int unknown34Count;
        int* unknown38; // unknown34Count * 4 bytes
    };

    struct StructuredDataDefSet
    {
        const char* name;
        unsigned int defCount;
        StructuredDataDef* defs;
    };

    struct TracerDef
    {
        const char* name;
        Material* material;
        FxEffectDef* effect;
        unsigned int drawInterval;
        float speed;
        float beamLength;
        float beamWidth;
        float screwRadius;
        float screwDist;
        float colors[5][4];
    };

    enum VehicleType
    {
        VEH_WHEELS_4,
        VEH_TANK,
        VEH_PLANE,
        VEH_BOAT,
        VEH_ARTILLERY,
        VEH_HELICOPTER,
        VEH_SNOWMOBILE,
        VEH_TYPE_COUNT
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
        // vehPhysDef
        int physicsEnabled;
        const char* physPresetName;
        PhysPreset* physPreset;
        const char* accelGraphName;
        unsigned int unknownB8[54];
        const char* turretWeaponName;
        WeaponCompleteDef* turretWeapon;
        float turretHorizSpanLeft;
        float turretHorizSpanRight;
        float turretVertSpanUp;
        float turretVertSpanDown;
        float turretRotRate;
        WeaponSndNameRef* turretSpinSnd;
        WeaponSndNameRef* turretStopSnd;
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
        WeaponSndNameRef* idleLowSnd;
        WeaponSndNameRef* idleHighSnd;
        WeaponSndNameRef* engineLowSnd;
        WeaponSndNameRef* engineHighSnd;
        float engineSndSpeed;
        WeaponSndNameRef* engineStartUpSnd;
        int engineStartUpLength;
        WeaponSndNameRef* engineShutdownSnd;
        WeaponSndNameRef* engineIdleSnd;
        WeaponSndNameRef* engineSustainSnd;
        WeaponSndNameRef* engineRampUpSnd;
        int engineRampUpLength;
        WeaponSndNameRef* engineRampDownSnd;
        int engineRampDownLength;
        WeaponSndNameRef* suspensionSoftSnd;
        float suspensionSoftCompression;
        WeaponSndNameRef* suspensionHardSnd;
        float suspensionHardCompression;
        WeaponSndNameRef* collisionSnd;
        float collisionBlendSpeed;
        WeaponSndNameRef* speedSnd;
        float speedSndBlendSpeed;
        const char* surfaceSndPrefix;
        WeaponSndNameRef* surfaceSnds[31];
        float surfaceSndBlendSpeed;
        float slideVolume;
        float slideBlendSpeed;
        float inAirPitch;
        unsigned int unknown2C8[19];
    };

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

    // Overrides the value of a member of a weapon at a specific offset.
    // value is a string for the types 0-3, 10-15, 40, 41, 43 and 44 (type is the first byte) and a plain 4 byte value for all the others.
    // The 2 low bytes of unknown4 contain the offset of the overridden member in the weapon (e.g. 0x5a4 is viewFlashEffect).
    union WeaponAttachValue
    {
        const char* string;
        int intValue;
        float floatValue;
    };

    struct WeaponAttachEntry
    {
        unsigned char type;
        unsigned char unknown1;
        unsigned short unknown2;
        unsigned int unknown4;
        WeaponAttachValue value;
    };

    struct WeaponAttach
    {
        const char* name;
        int entryCount;
        WeaponAttachEntry* entries;
    };

    struct NetConstStrings
    {
        const char* name;
        int unknown4;
        int unknown8;
        int stringCount;
        const char** strings;
    };

    struct MarketplaceDataStringEntry
    {
        unsigned int unknown0;
        const char* string;
    };

    struct MarketplaceDataEntry28
    {
        unsigned int unknown0[2];
        const char* strings[5];
        ScriptString unknown1C;
        unsigned short unknown1E;
        unsigned short unknown20;
        unsigned short unknown22Count;
        unsigned short* unknown24;
    };

    struct MarketplaceDataEntry1C
    {
        unsigned char unknown0[0xe];
        unsigned char unknownECount;
        unsigned char unknownF;
        unsigned int* unknown10;
        unsigned char unknown14Count;
        unsigned char unknown15[3];
        unsigned short* unknown18;
    };

    struct MarketplaceDataStrings3
    {
        const char* strings[3];
    };

    struct MarketplaceDataEntryC
    {
        unsigned int unknown0;
        unsigned short unknown4Count;
        unsigned short unknown6;
        unsigned short* unknown8;
    };

    struct MarketplaceDataUShortList
    {
        int count;
        unsigned short* values;
    };

    struct MarketplaceDataListSet
    {
        MarketplaceDataUShortList lists[20];
    };

    struct MarketplaceDataStringEntry2
    {
        unsigned int unknown0;
        const char* string;
        const char* unknown8;
    };

    struct MarketplaceDataStringEntry10
    {
        unsigned int unknown0;
        const char* string;
        unsigned int unknown8[2];
    };

    struct MarketplaceDataEntry90
    {
        unsigned int unknown0[2];
        const char* strings[7];
        unsigned int unknown24[8];
        unsigned short unknown44Count;
        unsigned short unknown46;
        MarketplaceDataStringEntry10* unknown48;
        unsigned int unknown4C[17];
    };

    struct MarketplaceDataStringArrayEntry
    {
        unsigned int unknown0;
        const char* string;
        const char** strings;
        unsigned short stringCount;
        unsigned short unknownE;
    };

    struct MarketplaceDataEntry34
    {
        unsigned int unknown0[9];
        int unknown24Count;
        unsigned short* unknown28;
        int unknown2CCount;
        unsigned short* unknown30;
    };

    struct MarketplaceDataEntry14
    {
        unsigned int unknown0;
        const char* strings[4];
    };

    struct MarketplaceDataEntry8
    {
        unsigned short count;
        unsigned short unknown2;
        MarketplaceDataStringEntry2* entries;
    };

    struct MarketplaceDataBytes6
    {
        unsigned char unknown0[6];
    };

    struct MarketplaceDataBytes18
    {
        unsigned char unknown0[0x18];
    };

    struct MarketplaceData
    {
        const char* name;
        MarketplaceDataStringEntry* unknown4;
        int unknown8Count;
        unsigned short* unknownC;
        int unknown10Count;
        MarketplaceDataEntry28* unknown14;
        int unknown18Count;
        unsigned short* unknown1C;
        int unknown20Count;
        unsigned int unknown24[33];
        unsigned short* unknownA8;
        int unknownACCount;
        unsigned int unknownB0[2500];
        MarketplaceDataEntry1C* unknown27C0;
        int unknown27C4Count;
        MarketplaceDataStrings3* unknown27C8;
        int unknown27CCCount;
        unsigned short* unknown27D0;
        int unknown27D4Count;
        MarketplaceDataBytes18* unknown27D8;
        int unknown27DCCount;
        unsigned short* unknown27E0;
        int unknown27E4Count;
        MarketplaceDataEntryC* unknown27E8;
        int unknown27ECCount;
        MarketplaceDataListSet unknown27F0[14];
        MarketplaceDataUShortList* unknown30B0;
        MarketplaceDataUShortList* unknown30B4;
        int unknown30B8Count;
        MarketplaceDataEntry90* unknown30BC;
        int unknown30C0Count;
        MarketplaceDataBytes6* unknown30C4;
        int unknown30C8Count;
        MarketplaceDataStringArrayEntry* unknown30CC;
        int unknown30D0Count;
        MarketplaceDataEntry34* unknown30D4;
        int unknown30D8Count;
        MarketplaceDataEntry14* unknown30DC;
        int unknown30E0Count;
        MarketplaceDataEntry8* unknown30E4;
        int unknown30E8Count;
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
