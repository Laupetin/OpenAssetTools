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
    struct clipMap_t;
    // struct ComWorld;
    // struct GameWorldSp;
    // struct GameWorldMp;
    struct MapEnts;
    // struct FxWorld;
    // struct GfxWorld;
    struct GfxLightDef;
    struct Font_s;
    struct MenuList;
    struct menuDef_t;
    struct LocalizeEntry;
    // struct WeaponCompleteDef;
    // struct SndDriverGlobals;
    struct FxEffectDef;
    struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    struct LeaderboardDef;
    struct StructuredDataDefSet;
    struct TracerDef;
    // struct VehicleDef;
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
        // ComWorld* comWorld;
        // GameWorldSp* gameWorldSp;
        // GameWorldMp* gameWorldMp;
        MapEnts* mapEnts;
        // FxWorld* fxWorld;
        // GfxWorld* gfxWorld;
        GfxLightDef* lightDef;
        Font_s* font;
        MenuList* menuList;
        menuDef_t* menu;
        LocalizeEntry* localize;
        // WeaponCompleteDef* weapon;
        // SndDriverGlobals* sndDriverGlobals;
        FxEffectDef* fx;
        FxImpactTable* impactFx;
        RawFile* rawfile;
        StringTable* stringTable;
        LeaderboardDef* leaderboardDef;
        StructuredDataDefSet* structuredDataDefSet;
        TracerDef* tracerDef;
        // VehicleDef* vehDef;
        AddonMapEnts* addonMapEnts;
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
        //bool (__cdecl* domainFunc)(dvar_t*, DvarValue);
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
        NUM_INTERNAL_DATATYPES = 0x3,
        VAL_FUNCTION = 0x3,
        NUM_DATATYPES = 0x4,
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

    enum EventType : char
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

    struct __declspec(align(4)) rectDef_s
    {
        float x;
        float y;
        float w;
        float h;
        char horzAlign;
        char vertAlign;
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
        float disableColor[4];
        Material* background;
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
        __int16 entryCount;
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
        unsigned __int16* inds;
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
        unsigned __int16 letter;
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
        LBCOL_TYPE_COUNT = 0x6,
    };

    enum LbAggType
    {
        LBAGG_TYPE_MIN = 0x0,
        LBAGG_TYPE_MAX = 0x1,
        LBAGG_TYPE_SUM = 0x2,
        LBAGG_TYPE_LAST = 0x3,
        LBAGG_TYPE_COUNT = 0x4,
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
        DATA_COUNT = 0xA,
    };

    struct __declspec(align(4)) StructuredDataEnumEntry
    {
        const char* string;
        unsigned __int16 index;
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
        unsigned __int16 hullCount;
        unsigned __int16 firstHull;
    };

    struct TriggerHull
    {
        Bounds bounds;
        int contents;
        unsigned __int16 slabCount;
        unsigned __int16 firstSlab;
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
        unsigned __int16 triggerIndex;
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
        __int16 children[2];
    };

    struct cLeafBrushNodeLeaf_t
    {
        LeafBrush* brushes;
    };

    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        unsigned __int16 childOffset[2];
    };

    union cLeafBrushNodeData_t
    {
        cLeafBrushNodeLeaf_t leaf;
        cLeafBrushNodeChildren_t children;
    };

    struct cLeafBrushNode_s
    {
        char axis;
        __int16 leafBrushCount;
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

    struct __declspec(align(16)) CollisionAabbTree
    {
        float midPoint[3];
        unsigned __int16 materialIndex;
        unsigned __int16 childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    struct cLeaf_t
    {
        unsigned __int16 firstCollAabbIndex;
        unsigned __int16 collAabbCount;
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
        unsigned __int16 numsides;
        unsigned __int16 glassPieceIndex;
        cbrushside_t* sides;
        char* baseAdjacentSide;
        __int16 axialMaterialNum[2][3];
        char firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
    };

    struct SModelAabbNode
    {
        Bounds bounds;
        unsigned __int16 firstChild;
        unsigned __int16 childCount;
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
        unsigned __int16 brushModel;
        unsigned __int16 physicsBrushModel;
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
        unsigned __int16 flags;
        unsigned __int16 lightingHandle;
        int health;
    };

    struct DynEntityColl
    {
        unsigned __int16 sector;
        unsigned __int16 nextEntInSector;
        float linkMins[2];
        float linkMaxs[2];
    };
    
    typedef __declspec(align(128)) cbrush_t cbrush_array_t;
    typedef __declspec(align(128)) Bounds BoundsArray;

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
        unsigned __int16* triIndices;
        char* triEdgeIsWalkable;
        int borderCount;
        CollisionBorder* borders;
        int partitionCount;
        CollisionPartition* partitions;
        int aabbTreeCount;
        CollisionAabbTree* aabbTrees;
        unsigned int numSubModels;
        cmodel_t* cmodels;
        unsigned __int16 numBrushes;
        cbrush_array_t* brushes;
        BoundsArray* brushBounds;
        int* brushContents;
        MapEnts* mapEnts;
        unsigned __int16 smodelNodeCount;
        SModelAabbNode* smodelNodes;
        unsigned __int16 dynEntCount[2];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityColl* dynEntCollList[2];
        unsigned int checksum;
        char padding[48];
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
