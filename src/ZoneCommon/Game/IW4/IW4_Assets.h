#pragma once

#ifndef __IW4_ASSETS_H
#define __IW4_ASSETS_H

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
    // struct clipMap_t;
    // struct ComWorld;
    // struct GameWorldSp;
    // struct GameWorldMp;
    // struct MapEnts;
    // struct FxWorld;
    // struct GfxWorld;
    // struct GfxLightDef;
    // struct Font_s;
    // struct MenuList;
    // struct menuDef_t;
    // struct LocalizeEntry;
    // struct WeaponCompleteDef;
    // struct SndDriverGlobals;
    // struct FxEffectDef;
    // struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    // struct LeaderboardDef;
    // struct StructuredDataDefSet;
    // struct TracerDef;
    // struct VehicleDef;
    // struct AddonMapEnts;

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
        // clipMap_t* clipMap;
        // ComWorld* comWorld;
        // GameWorldSp* gameWorldSp;
        // GameWorldMp* gameWorldMp;
        // MapEnts* mapEnts;
        // FxWorld* fxWorld;
        // GfxWorld* gfxWorld;
        // GfxLightDef* lightDef;
        // Font_s* font;
        // MenuList* menuList;
        // menuDef_t* menu;
        // LocalizeEntry* localize;
        // WeaponCompleteDef* weapon;
        // SndDriverGlobals* sndDriverGlobals;
        // FxEffectDef* fx;
        // FxImpactTable* impactFx;
        RawFile* rawfile;
        StringTable* stringTable;
        // LeaderboardDef* leaderboardDef;
        // StructuredDataDefSet* structuredDataDefSet;
        // TracerDef* tracerDef;
        // VehicleDef* vehDef;
        // AddonMapEnts* addonMapEnts;
        void* data;
    };

    typedef char cbrushedge_t;
    typedef float vec3_t[3];

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
        unsigned __int16 materialNum;
        char firstAdjacentSideOffset;
        char edgeCount;
    };

    struct cbrushWrapper_t
    {
        unsigned __int16 numsides;
        unsigned __int16 glassPieceIndex;
        cbrushside_t* sides;
        cbrushedge_t* baseAdjacentSide;
        __int16 axialMaterialNum[2][3];
        char firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
    };

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
        char* _1;
        unsigned __int16* _2;
        void* data;
    };

    struct XAnimNotifyInfo
    {
        unsigned __int16 name;
        float time;
    };

    union XAnimDynamicFrames
    {
        char (*_1)[3];
        unsigned __int16 (*_2)[3];
    };

    union XAnimDynamicIndices
    {
        char _1[1];
        unsigned __int16 _2[1];
    };

    struct __declspec(align(4)) XAnimPartTransFrames
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
        unsigned __int16 size;
        char smallTrans;
        XAnimPartTransData u;
    };

    struct __declspec(align(4)) XAnimDeltaPartQuatDataFrames2
    {
        __int16 (*frames)[2];
        XAnimDynamicIndices indices;
    };

    union XAnimDeltaPartQuatData2
    {
        XAnimDeltaPartQuatDataFrames2 frames;
        __int16 frame0[2];
    };

    struct XAnimDeltaPartQuat2
    {
        unsigned __int16 size;
        XAnimDeltaPartQuatData2 u;
    };

    struct XAnimDeltaPartQuatDataFrames
    {
        __int16 (*frames)[4];
        XAnimDynamicIndices indices;
    };

    union XAnimDeltaPartQuatData
    {
        XAnimDeltaPartQuatDataFrames frames;
        __int16 frame0[4];
    };

    struct XAnimDeltaPartQuat
    {
        unsigned __int16 size;
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
        unsigned __int16 dataByteCount;
        unsigned __int16 dataShortCount;
        unsigned __int16 dataIntCount;
        unsigned __int16 randomDataByteCount;
        unsigned __int16 randomDataIntCount;
        unsigned __int16 numframes;
        char flags;
        char boneCount[10];
        char notifyCount;
        char assetType;
        bool isDefault;
        unsigned int randomDataShortCount;
        unsigned int indexCount;
        float framerate;
        float frequency;
        unsigned __int16* names;
        char* dataByte;
        __int16* dataShort;
        int* dataInt;
        __int16* randomDataShort;
        char* randomDataByte;
        int* randomDataInt;
        XAnimIndices indices;
        XAnimNotifyInfo* notify;
        XAnimDeltaPart* deltaPart;
    };

    struct XSurfaceVertexInfo
    {
        __int16 vertCount[4];
        unsigned __int16* vertsBlend;
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

    struct __declspec(align(16)) GfxPackedVertex
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
        unsigned __int16 mins[3];
        unsigned __int16 maxs[3];
    };

    struct __declspec(align(16)) XSurfaceCollisionNode
    {
        XSurfaceCollisionAabb aabb;
        unsigned __int16 childBeginIndex;
        unsigned __int16 childCount;
    };

    struct XSurfaceCollisionLeaf
    {
        unsigned __int16 triangleBeginIndex;
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
        unsigned __int16 boneOffset;
        unsigned __int16 vertCount;
        unsigned __int16 triOffset;
        unsigned __int16 triCount;
        XSurfaceCollisionTree* collisionTree;
    };

    typedef __declspec(align(16)) unsigned __int16 r_index16_t;

    struct XSurface
    {
        char tileMode;
        bool deformed;
        unsigned __int16 vertCount;
        unsigned __int16 triCount;
        char zoneHandle;
        unsigned __int16 baseTriIndex;
        unsigned __int16 baseVertIndex;
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
        unsigned __int16 numsurfs;
        int partBits[6];
    };

    struct XModelLodInfo
    {
        float dist;
        unsigned __int16 numsurfs;
        unsigned __int16 surfIndex;
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
        char numBones;
        char numRootBones;
        char numsurfs;
        char lodRampType;
        float scale;
        unsigned int noScalePartBits[6];
        unsigned __int16* boneNames;
        char* parentList;
        __int16* quats;
        float* trans;
        char* partClassification;
        DObjAnimMat* baseMat;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        char maxLoadedLod;
        char numLods;
        char collLod;
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

    struct MaterialTextureDef
    {
        unsigned int nameHash;
        char nameStart;
        char nameEnd;
        char samplerState;
        char semantic;
        MaterialTextureDefInfo u;
    };

    struct __declspec(align(16)) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        float literal[4];
    };

    struct GfxStateBits
    {
        unsigned int loadBits[2];
    };

    struct GfxDrawSurfFields
    {
        unsigned __int64 objectId : 16;
        unsigned __int64 reflectionProbeIndex : 8;
        unsigned __int64 hasGfxEntIndex : 1;
        unsigned __int64 customIndex : 5;
        unsigned __int64 materialSortedIndex : 12;
        unsigned __int64 prepass : 2;
        unsigned __int64 useHeroLighting : 1;
        unsigned __int64 sceneLightIndex : 8;
        unsigned __int64 surfType : 4;
        unsigned __int64 primarySortKey : 6;
        unsigned __int64 unused : 1;
    };

    union GfxDrawSurf
    {
        GfxDrawSurfFields fields;
        unsigned __int64 packed;
    };

    struct MaterialInfo
    {
        const char* name;
        char gameFlags;
        char sortKey;
        char textureAtlasRowCount;
        char textureAtlasColumnCount;
        GfxDrawSurf drawSurf;
        unsigned int surfaceTypeBits;
        unsigned __int16 hashIndex;
    };

    struct Material
    {
        MaterialInfo info;
        char stateBitsEntry[48];
        char textureCount;
        char constantCount;
        char stateBitsCount;
        char stateFlags;
        char cameraRegion;
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
    };

    struct __declspec(align(4)) GfxImageLoadDef
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
        char mapType;
        char semantic;
        char category;
        bool useSrgbReads;
        Picmip picmip;
        bool noPicmip;
        char track;
        CardMemory cardMemory;
        unsigned __int16 width;
        unsigned __int16 height;
        unsigned __int16 depth;
        bool delayLoadPixels;
        const char* name;
    };

    struct GfxPixelShaderLoadDef
    {
        unsigned int* program;
        unsigned __int16 programSize;
        unsigned __int16 loadForRenderer;
    };

    struct GfxVertexShaderLoadDef
    {
        unsigned int* program;
        unsigned __int16 programSize;
        unsigned __int16 loadForRenderer;
    };

    struct MaterialStreamRouting
    {
        char source;
        char dest;
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[13];
        void/*IDirect3DVertexDeclaration9*/* decl[16];
    };

    struct MaterialVertexDeclaration
    {
        const char* name;
        char streamCount;
        bool hasOptionalSource;
        MaterialVertexStreamRouting routing;
    };

    struct MaterialVertexShaderProgram
    {
        void/*IDirect3DVertexShader9*/* vs;
        GfxVertexShaderLoadDef loadDef;
    };

    struct MaterialVertexShader
    {
        const char* name;
        MaterialVertexShaderProgram prog;
    };

    struct MaterialPixelShaderProgram
    {
        void/*IDirect3DPixelShader9*/* ps;
        GfxPixelShaderLoadDef loadDef;
    };

    struct MaterialPixelShader
    {
        const char* name;
        MaterialPixelShaderProgram prog;
    };

    struct MaterialArgumentCodeConst
    {
        unsigned __int16 index;
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
        MTL_ARG_COUNT = 0x8,
    };

    struct MaterialShaderArgument
    {
        unsigned __int16 type;
        unsigned __int16 dest;
        MaterialArgumentDef u;
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
        unsigned __int16 flags;
        unsigned __int16 passCount;
        MaterialPass passArray[1];
    };

    struct MaterialTechniqueSet
    {
        const char* name;
        char worldVertFormat;
        bool hasBeenUploaded;
        char unused[1];
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

    struct _AILSOUNDINFO
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
        _AILSOUNDINFO info;
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
        unsigned __int16 knotCount;
        float knots[16][2];
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
