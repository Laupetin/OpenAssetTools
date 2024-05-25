#pragma once

#ifndef __IW5_ASSETS_H
#define __IW5_ASSETS_H

#ifndef __zonecodegenerator
#ifndef __cplusplus
#define __ida
#endif
#endif

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
#ifndef __ida
namespace IW5
{
#endif
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
        ASSET_TYPE_CLIPMAP = 0xE,
        ASSET_TYPE_COMWORLD = 0xF,
        ASSET_TYPE_GLASSWORLD = 0x10,
        ASSET_TYPE_PATHDATA = 0x11,
        ASSET_TYPE_VEHICLE_TRACK = 0x12,
        ASSET_TYPE_MAP_ENTS = 0x13,
        ASSET_TYPE_FXWORLD = 0x14,
        ASSET_TYPE_GFXWORLD = 0x15,
        ASSET_TYPE_LIGHT_DEF = 0x16,
        ASSET_TYPE_UI_MAP = 0x17,
        ASSET_TYPE_FONT = 0x18,
        ASSET_TYPE_MENULIST = 0x19,
        ASSET_TYPE_MENU = 0x1A,
        ASSET_TYPE_LOCALIZE_ENTRY = 0x1B,
        ASSET_TYPE_ATTACHMENT = 0x1C,
        ASSET_TYPE_WEAPON = 0x1D,
        ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1E,
        ASSET_TYPE_FX = 0x1F,
        ASSET_TYPE_IMPACT_FX = 0x20,
        ASSET_TYPE_SURFACE_FX = 0x21,
        ASSET_TYPE_AITYPE = 0x22,
        ASSET_TYPE_MPTYPE = 0x23,
        ASSET_TYPE_CHARACTER = 0x24,
        ASSET_TYPE_XMODELALIAS = 0x25,
        ASSET_TYPE_RAWFILE = 0x26,
        ASSET_TYPE_SCRIPTFILE = 0x27,
        ASSET_TYPE_STRINGTABLE = 0x28,
        ASSET_TYPE_LEADERBOARD = 0x29,
        ASSET_TYPE_STRUCTURED_DATA_DEF = 0x2A,
        ASSET_TYPE_TRACER = 0x2B,
        ASSET_TYPE_VEHICLE = 0x2C,
        ASSET_TYPE_ADDON_MAP_ENTS = 0x2D,
        ASSET_TYPE_COUNT,

        ASSET_TYPE_STRING = ASSET_TYPE_COUNT,
        ASSET_TYPE_ASSETLIST = 0x2F,

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
        XFILE_BLOCK_SCRIPT,

        MAX_XFILE_COUNT
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
    struct LocalizeEntry;
    struct WeaponAttachment;
    struct WeaponCompleteDef;
    struct FxEffectDef;
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

    typedef unsigned short ScriptString;

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
        LocalizeEntry* localize;
        WeaponAttachment* attachment;
        WeaponCompleteDef* weapon;
        // SndDriverGlobals* sndDriverGlobals; // UNUSED ON PC
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
        void* data;
    };

    typedef float vec2_t[2];
    typedef float vec3_t[3];
    typedef float vec4_t[4];

    typedef tdef_align(16) uint16_t r_index16_t;
    typedef tdef_align(16) char raw_byte16;
    typedef tdef_align(16) float raw_float16;
    typedef tdef_align(128) unsigned int raw_uint128;
    typedef unsigned char raw_byte;
    typedef unsigned int raw_uint;
    typedef unsigned short r_index_t;

    enum PhysPresetScaling
    {
        PHYSPRESET_SCALING_LINEAR = 0x0,
        PHYSPRESET_SCALING_QUADRATIC = 0x1,
        PHYSPRESET_SCALING_COUNT = 0x2
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
        float minPitch;
        float maxPitch;
        PhysPresetScaling volumeType;
        PhysPresetScaling pitchType;
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
        unsigned short size;
        unsigned char smallTrans;
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
        short frame0[2];
    };

    struct XAnimDeltaPartQuat2
    {
        unsigned short size;
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
        short frame0[4];
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
        unsigned char boneCount[10];
        unsigned char notifyCount;
        unsigned char assetType;
        bool isDefault;
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

    struct DObjSkelMat
    {
        float axis[3][4];
        float origin[4];
    };

    struct XSurfaceVertexInfo
    {
        short vertCount[4];
        unsigned short* vertsBlend;
    };

    union PackedUnitVec
    {
        unsigned int packed;
        char array[4];
    };

    union PackedTexCoords
    {
        unsigned int packed;
    };

    struct GfxQuantizedNoColorVertex
    {
        short xyz[3];
        short binormalSign;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
    };

    union GfxColor
    {
        unsigned int packed;
        unsigned char array[4];
    };

    struct GfxQuantizedVertex
    {
        short xyz[3];
        short binormalSign;
        PackedUnitVec normal;
        PackedUnitVec tangent;
        PackedTexCoords texCoord;
        GfxColor color;
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

    union GfxVertexUnion0
    {
        GfxQuantizedNoColorVertex* quantizedNoColorVerts0;
        GfxQuantizedVertex* quantizedVerts0;
        GfxPackedVertex* packedVerts0;
        void* verts0;
    };

    struct XSurfaceCollisionAabb
    {
        unsigned short mins[3];
        unsigned short maxs[3];
    };

    struct type_align(16) XSurfaceCollisionNode
    {
        XSurfaceCollisionAabb aabb;
        unsigned short childBeginIndex;
        unsigned short childCount;
    };

    struct XSurfaceCollisionLeaf
    {
        unsigned short triangleBeginIndex;
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
        unsigned short boneOffset;
        unsigned short vertCount;
        unsigned short triOffset;
        unsigned short triCount;
        XSurfaceCollisionTree* collisionTree;
    };

    struct XSurface
    {
        unsigned char tileMode;
        unsigned char flags;
        unsigned short vertCount;
        unsigned short triCount;
        char zoneHandle;
        uint16_t baseTriIndex;
        uint16_t baseVertIndex;
        float quantizeScale;
        r_index16_t (*triIndices)[3];
        XSurfaceVertexInfo vertInfo;
        GfxVertexUnion0 verts0;
        unsigned int vertListCount;
        XRigidVertList* vertList;
        int partBits[6];
    };

    struct XModelSurfs
    {
        const char* name;
        XSurface* surfs;
        unsigned short numsurfs;
        int partBits[6];
    };

    struct DObjAnimMat
    {
        float quat[4];
        float trans[3];
        float transWeight;
    };

    struct XModelLodInfo
    {
        float dist;
        unsigned short numsurfs;
        unsigned short surfIndex;
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

    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        float scale;
        unsigned int noScalePartBits[6];
        ScriptString* boneNames;
        unsigned char* parentList;
        short (*quats)[4];
        float (*trans)[3];
        unsigned char* partClassification;
        DObjAnimMat* baseMat;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        char maxLoadedLod;
        unsigned char numLods;
        unsigned char collLod;
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

    struct gcc_align(8) GfxDrawSurfFields
    {
        uint64_t unused : 1;
        uint64_t primarySortKey : 6;
        uint64_t surfType : 4;
        uint64_t viewModelRender : 1;
        uint64_t sceneLightIndex : 8;
        uint64_t useHeroLighting : 1;
        uint64_t prepass : 2;
        uint64_t materialSortedIndex : 12;
        uint64_t customIndex : 5;
        uint64_t hasGfxEntIndex : 1;
        uint64_t reflectionProbeIndex : 8;
        uint64_t objectId : 15;
    };

    union GfxDrawSurf
    {
        gcc_align(8) GfxDrawSurfFields fields;
        gcc_align(8) uint64_t packed;
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
        TS_DISPLACEMENT_MAP = 0xC
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
        unsigned char samplerState;
        unsigned char semantic;
        MaterialTextureDefInfo u;
    };

    struct type_align(16) MaterialConstantDef
    {
        unsigned int nameHash;
        char name[12];
        float literal[4];
    };

    struct GfxStateBits
    {
        unsigned int loadBits[2];
    };

    struct Material
    {
        MaterialInfo info;
        unsigned char stateBitsEntry[54];
        unsigned char textureCount;
        unsigned char constantCount;
        unsigned char stateBitsCount;
        unsigned char stateFlags;
        unsigned char cameraRegion;
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
        const char** subMaterials;
    };

    struct GfxShaderLoadDef
    {
        unsigned char* cachedPart;
        unsigned char* physicalPart;
        unsigned short cachedPartSize;
        unsigned short physicalPartSize;
    };

    struct GfxPixelShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    struct MaterialPixelShaderProgram
    {
        void* ps;
        GfxPixelShaderLoadDef loadDef;
    };

    struct MaterialPixelShader
    {
        const char* name;
        MaterialPixelShaderProgram prog;
    };

    struct GfxVertexShaderLoadDef
    {
        unsigned int* program;
        uint16_t programSize;
        uint16_t loadForRenderer;
    };

    struct MaterialVertexShaderProgram
    {
        void* vs;
        GfxVertexShaderLoadDef loadDef;
    };

    struct MaterialVertexShader
    {
        const char* name;
        MaterialVertexShaderProgram prog;
    };

    struct MaterialStreamRouting
    {
        char source;
        char dest;
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[13];
        void* decl[16];
    };

    struct MaterialVertexDeclaration
    {
        const char* name;
        char streamCount;
        bool hasOptionalSource;
        MaterialVertexStreamRouting routing;
    };

    enum MaterialShaderArgumentType
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

        MTL_ARG_COUNT
    };

    struct MaterialArgumentCodeConst
    {
        unsigned short index;
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

    struct MaterialShaderArgument
    {
        unsigned short type;
        unsigned short dest;
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

    struct MaterialTechnique
    {
        const char* name;
        unsigned short flags;
        unsigned short passCount;
        MaterialPass passArray[1];
    };

    struct MaterialTechniqueSet
    {
        const char* name;
        unsigned char worldVertFormat;
        unsigned char unused[2];
        MaterialTechniqueSet* remappedTechniqueSet;
        MaterialTechnique* techniques[54];
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
#ifndef __ida
        Texture* texture;
#endif
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

    struct GfxImage
    {
        GfxTexture texture;
        char mapType;
        char semantic;
        char category;
        unsigned char flags;
        Picmip picmip;
        bool noPicmip;
        char track;
        CardMemory cardMemory;
        unsigned short width;
        unsigned short height;
        unsigned short depth;
        unsigned char levelCount;
        const char* name;
    };

    enum snd_alias_type_t
    {
        SAT_UNKNOWN = 0x0,
        SAT_LOADED = 0x1,
        SAT_STREAMED = 0x2,
        SAT_VOICED = 0x3,
        SAT_COUNT = 0x4
    };

    struct StreamedSound
    {
        const char* dir;
        const char* name;
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

    union SoundFileRef
    {
        LoadedSound* loadSnd;
        StreamedSound streamSnd;
    };

    struct SoundFile
    {
        unsigned char type;
        unsigned char exists;
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

    struct SndCurve
    {
        const char* filename;
        unsigned short knotCount;
        float knots[16][2];
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

    typedef tdef_align(128) cbrush_t cbrush_array_t;
    typedef tdef_align(128) Bounds BoundsArray;

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
        DynEntityHingeDef* hinge;
        PhysMass mass;
        int contents;
    };

    struct DynEntityPose
    {
        GfxPlacement pose;
        float radius;
    };

    struct Hinge
    {
        float angle;
        float quat[4];
        float angularVel;
        float torqueAccum;
        bool active;
        float autoDisableTimeLeft;
        DynEntityHingeDef* def;
        PhysPreset* physPreset;
        float centerOfMassRelToAxisOriginAtAngleZero[3];
    };

    struct DynEntityClient
    {
        int physObjId;
        unsigned short flags;
        unsigned short lightingHandle;
        int health;
        Hinge* hinge;
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
        unsigned short dynEntCount[2];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityColl* dynEntCollList[2];
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

    struct ComWorld
    {
        const char* name;
        int isInUse;
        unsigned int primaryLightCount;
        ComPrimaryLight* primaryLights;
    };

    struct G_GlassName
    {
        char* nameStr;
        unsigned short name;
        unsigned short pieceCount;
        unsigned short* pieceIndices;
    };

    struct G_GlassPiece
    {
        unsigned short damageTaken;
        unsigned short collapseTime;
        int lastStateChangeTime;
        unsigned char impactDir;
        unsigned char impactPos[2];
    };

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

    struct GlassWorld
    {
        const char* name;
        G_GlassData* g_glassData;
    };

    struct pathlink_s
    {
        float fDist;
        unsigned short nodeNum;
        unsigned char disconnectCount;
        unsigned char negotiationLink;
        unsigned char flags;
        unsigned char ubBadPlaceCount[3];
    };

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
        NODE_DONTLINK = 0x14
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
        NUM_PATH_NODE_ERRORS = 0x9
    };

    struct pathnode_constant_t
    {
        nodeType type;
        unsigned short spawnflags;
        ScriptString targetname;
        ScriptString script_linkName;
        ScriptString script_noteworthy;
        ScriptString target;
        ScriptString animscript;
        int animscriptfunc;
        float vOrigin[3];
        float fAngle;
        float forward[2];
        float minUseDistSq;
        PathNodeErrorCode error;
        short wOverlapNode[2];
        unsigned short totalLinkCount;
        pathlink_s* Links;
    };

    struct pathnode_dynamic_t
    {
        void* pOwner;
        int iFreeTime;
        int iValidTime[3];
        int dangerousNodeTime[3];
        int inPlayerLOSTime;
        short wLinkCount;
        short wOverlapCount;
        short turretEntNumber;
        unsigned char userCount;
        bool hasBadPlaceLink;
    };

    struct pathnode_t;

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
        unsigned short* nodes;
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
        const char* name;
        unsigned int nodeCount;
        pathnode_t* nodes;
        pathbasenode_t* basenodes;
        unsigned int chainNodeCount;
        unsigned short* chainNodeForNode;
        unsigned short* nodeForChainNode;
        int visBytes;
        unsigned char* pathVis;
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
        const char* name;
        VehicleTrackSegment* segments;
        unsigned int segmentCount;
    };

    struct ClientTriggerAabbNode
    {
        Bounds bounds;
        unsigned short firstChild;
        unsigned short childCount;
    };

    struct ClientTriggers
    {
        MapTriggers trigger;
        unsigned short numClientTriggerNodes;
        ClientTriggerAabbNode* clientTriggerAabbTree;
        unsigned int triggerStringLength;
        char* triggerString;
        unsigned short* triggerStringOffsets;
        unsigned char* triggerType;
        vec3_t* origins;
        float* scriptDelay;
        short* audioTriggers;
    };

    struct MapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
        ClientTriggers clientTrigger;
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
        unsigned short* lightingHandles;
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

    struct GfxSky
    {
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        unsigned char skySamplerState;
    };

    struct GfxWorldDpvsPlanes
    {
        int cellCount;
        cplane_s* planes;
        unsigned short* nodes;
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
        unsigned short childCount;
        unsigned short surfaceCount;
        unsigned short startSurfIndex;
        unsigned short surfaceCountNoDecal;
        unsigned short startSurfIndexNoDecal;
        unsigned short smodelIndexCount;
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
        unsigned char recursionDepth;
        unsigned char hullPointCount;
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
        unsigned short cellIndex;
        unsigned char vertexCount;
        float hullAxis[2][3];
    };

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

    struct GfxReflectionProbe
    {
        float origin[3];
    };

    struct GfxReflectionProbeReferenceOrigin
    {
        float origin[3];
    };

    struct GfxReflectionProbeReference
    {
        unsigned char index;
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
        void* worldVb;
    };

    struct GfxWorldVertexLayerData
    {
        unsigned char* data;
        void* layerVb;
    };

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
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        unsigned int indexCount;
        r_index_t* indices;
    };

    struct type_align(4) GfxLightGridEntry
    {
        unsigned short colorsIndex;
        unsigned char primaryLightIndex;
        unsigned char needsTrace;
    };

    struct type_align(4) GfxLightGridColors
    {
        unsigned char rgb[56][3];
    };

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
        unsigned short surfaceCount;
        unsigned short startSurfIndex;
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
        unsigned char hasGfxEntIndex;
        unsigned char lod;
        unsigned short surfId;
    };

    struct GfxSceneDynModel
    {
        XModelDrawInfo info;
        unsigned short dynEntId;
    };

    struct BModelDrawInfo
    {
        unsigned short surfId;
    };

    struct type_align(4) GfxSceneDynBrush
    {
        BModelDrawInfo info;
        unsigned short dynEntId;
    };

    struct GfxShadowGeometry
    {
        unsigned short surfaceCount;
        unsigned short smodelCount;
        unsigned short* sortedSurfIndex;
        unsigned short* smodelIndex;
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

    struct GfxStaticModelInst
    {
        Bounds bounds;
        float lightingOrigin[3];
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
        unsigned short cullDist;
        unsigned short lightingHandle;
        unsigned char reflectionProbeIndex;
        unsigned char primaryLightIndex;
        unsigned char flags;
        unsigned char firstMtlSkinIndex;
        GfxColor groundLighting;
        uint16_t cacheId[4];
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
        raw_byte* smodelVisData[3];
        raw_byte* surfaceVisData[3];
        unsigned short* sortedSurfIndex;
        GfxStaticModelInst* smodelInsts;
        GfxSurface* surfaces;
        GfxSurfaceBounds* surfacesBounds;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxDrawSurf* surfaceMaterials;
        raw_uint128* surfaceCastsSunShadow;
        volatile long usageCount;
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

    typedef tdef_align(128) GfxCellTree GfxCellTree128;

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
        GfxSceneDynModel* sceneDynModel;
        GfxSceneDynBrush* sceneDynBrush;
        raw_uint* primaryLightEntityShadowVis;
        raw_uint* primaryLightDynEntShadowVis[2];
        raw_byte* nonSunPrimaryLightForModelDynEnt;
        GfxShadowGeometry* shadowGeom;
        GfxLightRegion* lightRegion;
        GfxWorldDpvsStatic dpvs;
        GfxWorldDpvsDynamic dpvsDyn;
        unsigned int mapVtxChecksum;
        unsigned int heroOnlyLightCount;
        GfxHeroOnlyLight* heroOnlyLights;
        unsigned char fogTypesAllowed;
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
        GfxLightImage cucoloris;
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

    struct MenuList
    {
        const char* name;
        int menuCount;
        menuDef_t** menus;
    };

    struct Statement_s;

    enum expDataType
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
        VAL_FUNCTION = 0x3,

        NUM_DATATYPES
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
        EXP_FUNC_LOCAL_CLIENT_UI_MILLISECONDS,
        EXP_FUNC_DVAR_INT,
        EXP_FUNC_DVAR_BOOL,
        EXP_FUNC_DVAR_FLOAT,
        EXP_FUNC_DVAR_STRING
    };

    enum expressionEntryType : int
    {
        EET_OPERATOR = 0x0,
        EET_OPERAND = 0x1,
    };

    union entryInternalData
    {
        int op;
        Operand operand;
    };

    struct expressionEntry
    {
        int type;
        entryInternalData data;
    };

    struct UIFunctionList
    {
        int totalFunctions;
        Statement_s** functions;
    };

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

    struct dvar_t
    {
        const char* name;
        const char* description;
        unsigned short flags;
        unsigned char type;
        bool modified;
        DvarValue current;
        DvarValue latched;
        DvarValue reset;
        DvarLimits domain;
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

    struct StringList
    {
        int totalStrings;
        const char** strings;
    };

    struct ExpressionSupportingData
    {
        UIFunctionList uifunctions;
        StaticDvarList staticDvarList;
        StringList uiStrings;
    };

    struct ExpressionPersistentState
    {
        int flags;
        int playerDataKey[4];
        int lastExecuteTime[4];
        Operand lastResult[4];
    };

    struct Statement_s
    {
        int numEntries;
        expressionEntry* entries;
        ExpressionSupportingData* supportingData;
        ExpressionPersistentState persistentState;
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

        EVENT_COUNT
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

    struct ItemKeyHandler
    {
        int key;
        MenuEventHandlerSet* action;
        ItemKeyHandler* next;
    };

    struct menuTransition
    {
        int transitionType;
        int startTime;
        float startVal;
        float endVal;
        float time;
        int endTriggerType;
    };

    struct menuData_t
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
        int cursorItem[1];
        menuTransition scaleTransition[1];
        menuTransition alphaTransition[1];
        menuTransition xTransition[1];
        menuTransition yTransition[1];
        ExpressionSupportingData* expressionData;
        unsigned char priority;
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

    // This is data from IW4, could be different for IW5, to be investigated
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

    // This is data from IW4, could be different for IW5, to be investigated
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

    // This is data from IW4, could be different for IW5, to be investigated
    enum ItemDefFlag : unsigned int
    {
        ITEM_FLAG_SAVE_GAME_INFO = 0x1,
        ITEM_FLAG_CINEMATIC_SUBTITLE = 0x2,
    };

    // This is data from IW4, could be different for IW5, to be investigated
    enum ItemDefDvarFlag
    {
        ITEM_DVAR_FLAG_ENABLE = 0x1,
        ITEM_DVAR_FLAG_DISABLE = 0x2,
        ITEM_DVAR_FLAG_SHOW = 0x4,
        ITEM_DVAR_FLAG_HIDE = 0x8,
        ITEM_DVAR_FLAG_FOCUS = 0x10,
    };

    struct columnInfo_s
    {
        int xpos;
        int ypos;
        int width;
        int height;
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
        MenuEventHandlerSet* onDoubleClick;
        int notselectable;
        int noScrollBars;
        int usePaging;
        float selectBorder[4];
        Material* selectIcon;
        Statement_s* elementHeightExp;
    };

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
        int cursorPos[1];
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

    struct menuDef_t
    {
        menuData_t* data;
        windowDef_t window;
        int itemCount;
        itemDef_s** items;
    };

    struct LocalizeEntry
    {
        const char* value;
        const char* name;
    };

    enum AttachmentType
    {
        ATTACHMENT_SCOPE = 0x0,
        ATTACHMENT_UNDERBARREL = 0x1,
        ATTACHMENT_OTHER = 0x2,

        ATTACHMENT_COUNT
    };

    enum weapType_t
    {
        WEAPTYPE_NONE = 0x0,
        WEAPTYPE_BULLET = 0x1,
        WEAPTYPE_GRENADE = 0x2,
        WEAPTYPE_PROJECTILE = 0x3,
        WEAPTYPE_RIOTSHIELD = 0x4,

        WEAPTYPE_COUNT
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

        WEAPCLASS_COUNT
    };

    enum PenetrateType
    {
        PENETRATE_TYPE_NONE = 0x0,
        PENETRATE_TYPE_SMALL = 0x1,
        PENETRATE_TYPE_MEDIUM = 0x2,
        PENETRATE_TYPE_LARGE = 0x3,

        PENETRATE_TYPE_COUNT
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

    enum weapFireType_t
    {
        WEAPON_FIRETYPE_FULLAUTO = 0x0,
        WEAPON_FIRETYPE_SINGLESHOT = 0x1,
        WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
        WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
        WEAPON_FIRETYPE_DOUBLEBARREL = 0x5,

        WEAPON_FIRETYPE_COUNT
    };

    struct AttAmmoGeneral
    {
        PenetrateType penetrateType;
        float penetrateMultiplier;
        ImpactType impactType;
        weapFireType_t fireType;
        TracerDef* tracerType;
        bool rifleBullet;
        bool armorPiercing;
    };

    struct AttSight
    {
        bool aimDownSight;
        bool adsFire;
        bool rechamberWhileAds;
        bool noAdsWhenMagEmpty;
        bool canHoldBreath;
        bool canVariableZoom;
        bool hideRailWithThisScope;
    };

    struct AttReload
    {
        bool noPartialReload;
        bool segmentedReload;
    };

    struct AttAddOns
    {
        bool motionTracker;
        bool silenced;
    };

    struct AttGeneral
    {
        bool boltAction;
        bool inheritsPerks;
        float enemyCrosshairRange;
        Material* reticleCenter;
        Material* reticleSide;
        int reticleCenterSize;
        int reticleSideSize;
        float moveSpeedScale;
        float adsMoveSpeedScale;
    };

    struct AttAimAssist
    {
        float autoAimRange;
        float aimAssistRange;
        float aimAssistRangeAds;
    };

    struct AttAmmunition
    {
        int maxAmmo;
        int startAmmo;
        int clipSize;
        int shotCount;
        int reloadAmmoAdd;
        int reloadStartAdd;
    };

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

    struct AttIdleSettings
    {
        float hipIdleAmount;
        float hipIdleSpeed;
        float idleCrouchFactor;
        float idleProneFactor;
        float adsIdleLerpStartTime;
        float adsIdleLerpTime;
    };

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

    struct AttGunKick
    {
        int hipGunKickReducedKickBullets;
        float hipGunKickReducedKickPercent;
        float hipGunKickPitchMin;
        float hipGunKickPitchMax;
        float hipGunKickYawMin;
        float hipGunKickYawMax;
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
        float adsGunKickAccel;
        float adsGunKickSpeedMax;
        float adsGunKickSpeedDecay;
        float adsGunKickStaticDecay;
    };

    struct AttViewKick
    {
        float hipViewKickPitchMin;
        float hipViewKickPitchMax;
        float hipViewKickYawMin;
        float hipViewKickYawMax;
        float hipViewKickCenterSpeed;
        float adsViewKickPitchMin;
        float adsViewKickPitchMax;
        float adsViewKickYawMin;
        float adsViewKickYawMax;
        float adsViewKickCenterSpeed;
    };

    enum weapOverlayReticle_t
    {
        WEAPOVERLAYRETICLE_NONE = 0x0,
        WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,

        WEAPOVERLAYRETICLE_COUNT
    };

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

    struct AttADSOverlay
    {
        ADSOverlay overlay;
        bool thermalScope;
    };

    enum weaponIconRatioType_t
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,

        WEAPON_ICON_RATIO_COUNT
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

    struct AttUI
    {
        Material* dpadIcon;
        Material* ammoCounterIcon;
        weaponIconRatioType_t dpadIconRatio;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
    };

    struct AttRumbles
    {
        const char* fireRumble;
        const char* meleeImpactRumble;
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

        WEAPPROJEXP_COUNT
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
        SndAliasCustom projExplosionSound;
        FxEffectDef* projDudEffect;
        SndAliasCustom projDudSound;
        bool projImpactExplode;
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        FxEffectDef* projTrailEffect;
        int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        SndAliasCustom projIgnitionSound;
    };

    typedef tdef_align(4) AttSight AttSight4;

    struct WeaponAttachment
    {
        const char* szInternalName;
        const char* szDisplayName;
        AttachmentType type;
        weapType_t weaponType;
        weapClass_t weapClass;
        XModel** worldModels;
        XModel** viewModels;
        XModel** reticleViewModels;
        AttAmmoGeneral* ammoGeneral;
        AttSight4* sight;
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

    enum weapInventoryType_t
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,
        WEAPINVENTORY_EXCLUSIVE = 0x4,
        WEAPINVENTORY_SCAVENGER = 0x5,

        WEAPINVENTORY_COUNT
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

        WEAPSTANCE_COUNT
    };

    enum activeReticleType_t
    {
        VEH_ACTIVE_RETICLE_NONE = 0x0,
        VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
        VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,

        VEH_ACTIVE_RETICLE_COUNT
    };

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
        int blastFrontTime;
        int blastRightTime;
        int blastBackTime;
        int blastLeftTime;
        int raiseInterruptableTime;
        int firstRaiseInterruptableTime;
        int reloadInterruptableTime;
        int reloadEmptyInterruptableTime;
        int fireInterruptableTime;
    };

    enum WeapOverlayInterface_t
    {
        WEAPOVERLAYINTERFACE_NONE = 0x0,
        WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
        WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,

        WEAPOVERLAYINTERFACE_COUNT
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

    enum weapAnimFiles_t : unsigned int
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
        WEAP_ANIM_BLAST_FRONT = 0x23,
        WEAP_ANIM_BLAST_RIGHT = 0x24,
        WEAP_ANIM_BLAST_BACK = 0x25,
        WEAP_ANIM_BLAST_LEFT = 0x26,
        WEAP_ANIM_ADS_UP = 0x27,
        WEAP_ANIM_ADS_DOWN = 0x28,
        WEAP_ALT_ANIM_ADJUST = 0x29,

        WEAP_ANIM_COUNT
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

        HITLOC_COUNT
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
        SndAliasCustom changeVariableZoomSound;
        SndAliasCustom* bounceSound;
        SndAliasCustom* rollingSound;
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
        WeapOverlayInterface_t overlayInterface;
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
        unsigned short originalAiVsAiAccuracyGraphKnotCount;
        unsigned short originalAiVsPlayerAccuracyGraphKnotCount;
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
        bool canVariableZoom;
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
        ScriptString stowTag;
        XModel* stowOffsetModel;
    };

    union WeaponAttachmentCombination
    {
        struct
        {
            // Specifies the index as a number
            // since there can only be one scope
            unsigned short scope : 3;
            // Specifies the index as a number
            // since there can only be one under barrel
            unsigned short underBarrel : 2;
            // Specifies all other attachments as a bit array
            unsigned short other : 4;
        };

        unsigned short fields;
    };

    struct AnimOverrideEntry
    {
        WeaponAttachmentCombination attachment1;
        WeaponAttachmentCombination attachment2;
        const char* overrideAnim;
        const char* altmodeAnim;
        weapAnimFiles_t animTreeType;
        int animTime;
        int altTime;
    };

    enum SoundOverrideTypes : unsigned int
    {
        SNDTYPE_NONE = 0x0,
        SNDTYPE_FIRE = 0x1,
        SNDTYPE_PLAYER_FIRE = 0x2,
        SNDTYPE_PLAYER_AKIMBO = 0x3,
        SNDTYPE_PLAYER_LASTSHOT = 0x4,

        SNDTYPE_PLAYER_COUNT
    };

    struct SoundOverrideEntry
    {
        WeaponAttachmentCombination attachment1;
        WeaponAttachmentCombination attachment2;
        SndAliasCustom overrideSound;
        SndAliasCustom altmodeSound;
        SoundOverrideTypes soundType;
    };

    enum FxOverrideTypes : unsigned int
    {
        FXTYPE_NONE = 0x0,
        FXTYPE_VIEW_FLASH = 0x1,
        FXTYPE_WORLD_FLASH = 0x2,
        FXTYPE_VIEW_SHELL_EJECT = 0x3,
        FXTYPE_WORLD_SHELL_EJECT = 0x4,

        FXTYPE_COUNT
    };

    struct FxOverrideEntry
    {
        WeaponAttachmentCombination attachment1;
        WeaponAttachmentCombination attachment2;
        FxEffectDef* overrideFx;
        FxEffectDef* altmodeFx;
        FxOverrideTypes fxType;
    };

    struct ReloadStateTimerEntry
    {
        WeaponAttachmentCombination attachment;
        short unused;
        int reloadAddTime;
        int reloadStartAddTime;
    };

    struct NoteTrackToSoundEntry
    {
        WeaponAttachmentCombination attachment;
        short unused;
        ScriptString* notetrackSoundMapKeys;
        ScriptString* notetrackSoundMapValues;
    };

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
        unsigned int numFxOverrides;
        FxOverrideEntry* fxOverrides;
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
        const char* szXAnims[42];
        XModel* gunXModel[16];
        const char* szXAnimsRightHanded[42];
        const char* szXAnimsLeftHanded[42];
        uint16_t notetrackSoundMapKeys[24];
        uint16_t notetrackSoundMapValues[24];
        uint16_t notetrackRumbleMapKeys[16];
        uint16_t notetrackRumbleMapValues[16];
        XModel* worldModel[16];
        float parallelBounce[31];
        float perpendicularBounce[31];
        float locationDamageMultipliers[20];
        WeaponAttachment* scopes[6];
        WeaponAttachment* underBarrels[3];
        WeaponAttachment* others[4];
    };

    struct FxFloatRange
    {
        float base;
        float amplitude;
    };

    struct FxIntRange
    {
        int base;
        int amplitude;
    };

    struct FxElemVec3Range
    {
        float base[3];
        float amplitude[3];
    };

    struct FxSpawnDefLooping
    {
        int intervalMsec;
        int count;
    };

    struct FxSpawnDefOneShot
    {
        FxIntRange count;
    };

    union FxSpawnDef
    {
        FxSpawnDefLooping looping;
        FxSpawnDefOneShot oneShot;
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
    };

    struct FxElemVisStateSample
    {
        FxElemVisualState base;
        FxElemVisualState amplitude;
    };

    struct FxElemMarkVisuals
    {
        Material* materials[2];
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
        GfxLightDef* lightDef;
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
        unsigned short* inds;
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

    struct FxSpotLightDef
    {
        float fovInnerFraction;
        float startRadius;
        float endRadius;
        float brightness;
        float maxLength;
        int exponent;
    };

    union FxElemExtendedDefPtr
    {
        FxTrailDef* trailDef;
        FxSparkFountainDef* sparkFountainDef;
        FxSpotLightDef* spotLightDef;
        char* unknownDef;
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
        FX_ELEM_TYPE_LAST_DRAWN = 0x9
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
        unsigned char elemType;
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
        int randomSeed;
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
        float occlusionQueryDepthBias;
        int occlusionQueryFadeIn;
        int occlusionQueryFadeOut;
        FxFloatRange occlusionQueryScaleRange;
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

    struct SurfaceFxEntry
    {
        FxEffectDef* surfaceEffect[31];
    };

    struct SurfaceFxTable
    {
        const char* name;
        SurfaceFxEntry* table;
    };

    struct RawFile
    {
        const char* name;
        int compressedLen;
        int len;
        const char* buffer;
    };

    struct ScriptFile
    {
        const char* name;
        int compressedLen;
        int len;
        int bytecodeLen;
        const char* buffer;
        unsigned char* bytecode;
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
        LBCOL_TYPE_NUMBER = 0x0,
        LBCOL_TYPE_TIME = 0x1,
        LBCOL_TYPE_LEVELXP = 0x2,
        LBCOL_TYPE_PRESTIGE = 0x3,
        LBCOL_TYPE_BIGNUMBER = 0x4,
        LBCOL_TYPE_PERCENT = 0x5,
        LBCOL_TYPE_TIME_FULL = 0x6,

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
        int uiCalColX;
        int uiCalColY;
    };

    enum LbUpdateType
    {
        LBUPDATE_TYPE_NORMAL = 0x0,
        LBUPDATE_TYPE_RANK = 0x1,
        LBUPDATE_TYPE_COMBINE = 0x2,

        LBUPDATE_TYPE_COUNT
    };

    enum LbTrackType
    {
        TRK_ALLTIME = 0x0,
        TRK_WEEKLY = 0x1,
        TRK_MONTHLY = 0x2,
        TRK_PRESTIGE_ALLTIME = 0x3,
        TRK_PRESTIGE_WEEKLY = 0x4,
        TRK_PRESTIGE_MONTHLY = 0x5,
        TRK_DAILY = 0x6,
        TRK_PRESTIGE_DAILY = 0x7,

        TRK_COUNT
    };

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

    struct StructuredDataEnumEntry
    {
        const char* string;
        unsigned short index;
    };

    struct StructuredDataEnum
    {
        int entryCount;
        int reservedEntryCount;
        StructuredDataEnumEntry* entries;
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

    enum StructuredDataValidationType
    {
        VALIDATION_NONE = 0x0,
        VALIDATION_CONSTANT = 0x1,
        VALIDATION_INCREASECLAMP = 0x2,
        VALIDATION_INCREASESTRICT = 0x3,
        VALIDATION_DELTACLAMP = 0x4,
        VALIDATION_DELTASTRICT = 0x5,
        VALIDATION_XP = 0x6,

        VALIDATION_COUNT
    };

    struct StructuredDataStructProperty
    {
        const char* name;
        StructuredDataType type;
        unsigned int offset;
        StructuredDataValidationType validation;
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

    enum VehicleType
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

        VEH_TYPE_COUNT
    };

    enum VehicleAxleType
    {
        VEH_AXLE_FRONT = 0x0,
        VEH_AXLE_REAR = 0x1,
        VEH_AXLE_ALL = 0x2,

        VEH_AXLE_COUNT
    };

    enum VehCamZOffsetMode
    {
        VEHCAM_ZMODE_WORLD = 0x0,
        VEHCAM_ZMODE_VEHICLE = 0x1,
        VEHCAM_ZMODE_VIEW = 0x2,
        VEHCAM_ZMODE_COUNT = 0x3
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
        int fakeBodyStabilizer;
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
        int camLookEnabled;
        int camRelativeControl;
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
        SndAliasCustom turretSpinSnd;
        SndAliasCustom turretStopSnd;
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
        SndAliasCustom idleLowSnd;
        SndAliasCustom idleHighSnd;
        SndAliasCustom engineLowSnd;
        SndAliasCustom engineHighSnd;
        float engineSndSpeed;
        ScriptString audioOriginTag;
        SndAliasCustom idleLowSndAlt;
        SndAliasCustom idleHighSndAlt;
        SndAliasCustom engineLowSndAlt;
        SndAliasCustom engineHighSndAlt;
        float engineSndSpeedAlt;
        ScriptString audioOriginTagAlt;
        SndAliasCustom turretSpinSndAlt;
        SndAliasCustom turretStopSndAlt;
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

    struct cmodel2_t
    {
        Bounds bounds;
        float radius;
        ClipInfo* info;
        cLeaf_t leaf;
    };

    struct AddonMapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
        MapTriggers trigger;
        ClipInfo* info;
        unsigned int numSubModels;
        cmodel2_t* cmodels;
        GfxBrushModel* models;
    };

#ifndef __zonecodegenerator
#ifndef __ida
}
#endif
#endif

#endif

// EOF
