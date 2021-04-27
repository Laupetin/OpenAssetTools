#pragma once

#ifndef __T5_ASSETS_H
#define __T5_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace T5
{
#endif

    struct PhysPreset;
    struct PhysConstraints;
    struct DestructibleDef;
    struct XAnimParts;
    struct XModel;
    struct Material;
    struct MaterialTechniqueSet;
    struct GfxImage;
    struct SndBank;
    struct SndPatch;
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
    struct WeaponVariantDef;
    struct SndDriverGlobals;
    struct FxEffectDef;
    struct FxImpactTable;
    struct RawFile;
    struct StringTable;
    struct PackIndex;
    struct XGlobals;
    struct ddlRoot_t;
    struct Glasses;
    struct EmblemSet;

    enum XAssetType
    {
        ASSET_TYPE_XMODELPIECES = 0x0,
        ASSET_TYPE_PHYSPRESET = 0x1,
        ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
        ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
        ASSET_TYPE_XANIMPARTS = 0x4,
        ASSET_TYPE_XMODEL = 0x5,
        ASSET_TYPE_MATERIAL = 0x6,
        ASSET_TYPE_TECHNIQUE_SET = 0x7,
        ASSET_TYPE_IMAGE = 0x8,
        ASSET_TYPE_SOUND = 0x9,
        ASSET_TYPE_SOUND_PATCH = 0xA,
        ASSET_TYPE_CLIPMAP = 0xB,
        ASSET_TYPE_CLIPMAP_PVS = 0xC,
        ASSET_TYPE_COMWORLD = 0xD,
        ASSET_TYPE_GAMEWORLD_SP = 0xE,
        ASSET_TYPE_GAMEWORLD_MP = 0xF,
        ASSET_TYPE_MAP_ENTS = 0x10,
        ASSET_TYPE_GFXWORLD = 0x11,
        ASSET_TYPE_LIGHT_DEF = 0x12,
        ASSET_TYPE_UI_MAP = 0x13,
        ASSET_TYPE_FONT = 0x14,
        ASSET_TYPE_MENULIST = 0x15,
        ASSET_TYPE_MENU = 0x16,
        ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
        ASSET_TYPE_WEAPON = 0x18,
        ASSET_TYPE_WEAPONDEF = 0x19,
        ASSET_TYPE_WEAPON_VARIANT = 0x1A,
        ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
        ASSET_TYPE_FX = 0x1C,
        ASSET_TYPE_IMPACT_FX = 0x1D,
        ASSET_TYPE_AITYPE = 0x1E,
        ASSET_TYPE_MPTYPE = 0x1F,
        ASSET_TYPE_MPBODY = 0x20,
        ASSET_TYPE_MPHEAD = 0x21,
        ASSET_TYPE_CHARACTER = 0x22,
        ASSET_TYPE_XMODELALIAS = 0x23,
        ASSET_TYPE_RAWFILE = 0x24,
        ASSET_TYPE_STRINGTABLE = 0x25,
        ASSET_TYPE_PACK_INDEX = 0x26,
        ASSET_TYPE_XGLOBALS = 0x27,
        ASSET_TYPE_DDL = 0x28,
        ASSET_TYPE_GLASSES = 0x29,
        ASSET_TYPE_EMBLEMSET = 0x2A,
        ASSET_TYPE_COUNT = 0x2B,
        ASSET_TYPE_STRING = 0x2B,
        ASSET_TYPE_ASSETLIST = 0x2C,

        ASSET_TYPE_FULL_COUNT
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

        MAX_XFILE_COUNT
    };

    union XAssetHeader
    {
        //XModelPieces *xmodelPieces;  // Not an asset
        PhysPreset *physPreset;
        PhysConstraints *physConstraints;
        DestructibleDef *destructibleDef;
        XAnimParts *parts;
        XModel *model;
        Material *material;
        //MaterialPixelShader *pixelShader; // Not an asset
        //MaterialVertexShader *vertexShader; // Not an asset
        MaterialTechniqueSet *techniqueSet;
        GfxImage *image;
        SndBank *sound;
        SndPatch *soundPatch;
        clipMap_t *clipMap;
        ComWorld *comWorld;
        GameWorldSp *gameWorldSp;
        GameWorldMp *gameWorldMp;
        MapEnts *mapEnts;
        GfxWorld *gfxWorld;
        GfxLightDef *lightDef;
        Font_s *font;
        MenuList *menuList;
        menuDef_t *menu;
        LocalizeEntry *localize;
        WeaponVariantDef *weapon;
        SndDriverGlobals *sndDriverGlobals;
        FxEffectDef *fx;
        FxImpactTable *impactFx;
        RawFile *rawfile;
        StringTable *stringTable;
        PackIndex *packIndex;
        XGlobals *xGlobals;
        ddlRoot_t *ddlRoot;
        Glasses *glasses;
        //TextureList *textureList; // Not an asset
        EmblemSet *emblemSet;
        void *data;
    };

    typedef tdef_align(16) char char16;
    typedef tdef_align(32) char byte32;
    typedef tdef_align(128) char byte128;

    typedef tdef_align(4) char char_align4;
    typedef tdef_align(128) char char_align128;

    typedef tdef_align(16) char raw_byte16;
    typedef tdef_align(128) char raw_byte128;

    typedef tdef_align(128) float float_align128;

    typedef char cbrushedge_t;
    typedef float vec2_t[2];
    typedef float vec3_t[3];
    typedef tdef_align(128) unsigned int raw_uint128;

    struct PhysPreset
    {
        const char* name;
        int flags;
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
        float centerOfMassOffset[3];
        float buoyancyBoxMin[3];
        float buoyancyBoxMax[3];
    };

    enum ConstraintType
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
        CONSTRAINT_LIGHT = 0x9,

        NUM_CONSTRAINT_TYPES
    };

    enum AttachPointType
    {
        ATTACH_POINT_WORLD = 0x0,
        ATTACH_POINT_DYNENT = 0x1,
        ATTACH_POINT_ENT = 0x2,
        ATTACH_POINT_BONE = 0x3,
    };

    struct PhysConstraint
    {
        unsigned __int16 targetname;
        ConstraintType type;
        AttachPointType attach_point_type1;
        int target_index1;
        unsigned __int16 target_ent1;
        const char* target_bone1;
        AttachPointType attach_point_type2;
        int target_index2;
        unsigned __int16 target_ent2;
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
        int centity_num[4];
    };

    struct PhysConstraints
    {
        const char* name;
        unsigned int count;
        PhysConstraint data[16];
    };

    struct DestructibleStage
    {
        unsigned __int16 showBone;
        float breakHealth;
        float maxTime;
        unsigned int flags;
        FxEffectDef* breakEffect;
        const char* breakSound;
        const char* breakNotify;
        const char* loopSound;
        XModel* spawnModel[3];
        PhysPreset* physPreset;
    };

    struct DestructiblePiece
    {
        DestructibleStage stages[5];
        char parentPiece;
        float parentDamagePercent;
        float bulletDamageScale;
        float explosiveDamageScale;
        float meleeDamageScale;
        float impactDamageScale;
        float entityDamageTransfer;
        PhysConstraints* physConstraints;
        int health;
        const char* damageSound;
        FxEffectDef* burnEffect;
        const char* burnSound;
        unsigned __int16 enableLabel;
        int hideBones[5];
    };

    struct DestructibleDef
    {
        const char* name;
        XModel* model;
        XModel* pristineModel;
        int numPieces;
        DestructiblePiece* pieces;
        int clientOnly;
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
        unsigned __int16 dataByteCount;
        unsigned __int16 dataShortCount;
        unsigned __int16 dataIntCount;
        unsigned __int16 randomDataByteCount;
        unsigned __int16 randomDataIntCount;
        unsigned __int16 numframes;
        bool bLoop;
        bool bDelta;
        bool bLeftHandGripIK;
        bool bStreamable;
        unsigned int streamedFileSize;
        char boneCount[10];
        char notifyCount;
        char assetType;
        bool isDefault;
        unsigned int randomDataShortCount;
        unsigned int indexCount;
        float framerate;
        float frequency;
        float primedLength;
        float loopEntryTime;
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

    struct DObjAnimMat
    {
        float quat[4];
        float trans[3];
        float transWeight;
    };

    struct XSurfaceVertexInfo
    {
        __int16 vertCount[4];
        unsigned __int16* vertsBlend;
        float* tensionData;
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

    struct XSurfaceCollisionLeaf
    {
        unsigned __int16 triangleBeginIndex;
    };

    struct XSurfaceCollisionAabb
    {
        unsigned __int16 mins[3];
        unsigned __int16 maxs[3];
    };

    struct type_align(16) XSurfaceCollisionNode
    {
        XSurfaceCollisionAabb aabb;
        unsigned __int16 childBeginIndex;
        unsigned __int16 childCount;
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

    struct XSurface
    {
        char tileMode;
        char vertListCount;
        unsigned __int16 flags;
        unsigned __int16 vertCount;
        unsigned __int16 triCount;
        unsigned __int16 baseTriIndex;
        unsigned __int16 baseVertIndex;
        unsigned __int16* triIndices;
        XSurfaceVertexInfo vertInfo;
        GfxPackedVertex* verts0;
        void/*IDirect3DVertexBuffer9*/* vb0;
        XRigidVertList* vertList;
        void/*IDirect3DIndexBuffer9*/* indexBuffer;
        int partBits[5];
    };

    struct XModelLodInfo
    {
        float dist;
        unsigned __int16 numsurfs;
        unsigned __int16 surfIndex;
        int partBits[5];
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

    struct __declspec(align(4)) XBoneInfo
    {
        float bounds[2][3];
        float offset[3];
        float radiusSquared;
        char collmap;
    };

    struct XModelHighMipBounds
    {
        float center[3];
        float himipRadiusSq;
    };

    struct XModelStreamInfo
    {
        XModelHighMipBounds* highMipBounds;
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
        int cflags;
        int sflags;
    };

    struct BrushWrapper
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int axial_cflags[2][3];
        int axial_sflags[2][3];
        unsigned int numverts;
        float(*verts)[3];
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

    struct PhysGeomList
    {
        unsigned int count;
        PhysGeomInfo* geoms;
        int contents;
    };

    struct Collmap
    {
        PhysGeomList* geomList;
    };

    struct XModel
    {
        const char* name;
        char numBones;
        char numRootBones;
        char numsurfs;
        char lodRampType;
        unsigned __int16* boneNames;
        char* parentList;
        __int16* quats;
        float* trans;
        char* partClassification;
        DObjAnimMat* baseMat;
        XSurface* surfs;
        Material** materialHandles;
        XModelLodInfo lodInfo[4];
        char lodDistAutoGenerated;
        XModelCollSurf_s* collSurfs;
        int numCollSurfs;
        int contents;
        XBoneInfo* boneInfo;
        float radius;
        float mins[3];
        float maxs[3];
        __int16 numLods;
        __int16 collLod;
        XModelStreamInfo streamInfo;
        int memUsage;
        int flags;
        bool bad;
        PhysPreset* physPreset;
        char numCollmaps;
        Collmap* collmaps;
        PhysConstraints* physConstraints;
    };

    struct GfxDrawSurfFields
    {
        unsigned __int64 objectId : 16;
        unsigned __int64 fade : 4;
        unsigned __int64 customIndex : 5;
        unsigned __int64 reflectionProbeIndex : 3;
        unsigned __int64 hdrBits : 1;
        unsigned __int64 glightRender : 1;
        unsigned __int64 dlightRender : 1;
        unsigned __int64 materialSortedIndex : 12;
        unsigned __int64 primaryLightIndex : 8;
        unsigned __int64 surfType : 4;
        unsigned __int64 prepass : 2;
        unsigned __int64 noDynamicShadow : 1;
        unsigned __int64 primarySortKey : 6;
    };

    union GfxDrawSurf
    {
        GfxDrawSurfFields fields;
        unsigned __int64 packed;
    };

    struct __declspec(align(8)) MaterialInfo
    {
        const char* name;
        unsigned int gameFlags;
        char pad;
        char sortKey;
        char textureAtlasRowCount;
        char textureAtlasColumnCount;
        GfxDrawSurf drawSurf;
        unsigned int surfaceTypeBits;
        unsigned int layeredSurfaceTypes;
        unsigned __int16 hashIndex;
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
        char isMatureContent;
        char pad[3];
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
        char stateBitsEntry[130];
        char textureCount;
        char constantCount;
        char stateBitsCount;
        char stateFlags;
        char cameraRegion;
        char maxStreamedMips;
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
    };

    struct MaterialStreamRouting
    {
        char source;
        char dest;
    };

    struct MaterialVertexStreamRouting
    {
        MaterialStreamRouting data[16];
        void/*IDirect3DVertexDeclaration9*/* decl[18];
    };

    struct MaterialVertexDeclaration
    {
        char streamCount;
        bool hasOptionalSource;
        bool isLoaded;
        MaterialVertexStreamRouting routing;
    };

    struct __declspec(align(4)) GfxVertexShaderLoadDef
    {
        unsigned int* program;
        unsigned __int16 programSize;
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

    struct __declspec(align(4)) GfxPixelShaderLoadDef
    {
        unsigned int* program;
        unsigned __int16 programSize;
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
        const float* literalConst;
        MaterialArgumentCodeConst codeConst;
        unsigned int codeSampler;
        unsigned int nameHash;
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
        char unused[1];
        unsigned __int16 techsetFlags;
        MaterialTechnique* techniques[130];
    };

    struct GfxImageLoadDef
    {
        char levelCount;
        char flags;
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

    struct GfxImage
    {
        GfxTexture texture;
        char mapType;
        char semantic;
        char category;
        bool delayLoadPixels;
        Picmip picmip;
        bool noPicmip;
        char track;
        CardMemory cardMemory;
        unsigned __int16 width;
        unsigned __int16 height;
        unsigned __int16 depth;
        char levelCount;
        char streaming;
        unsigned int baseSize;
        char* pixels;
        unsigned int loadedSize;
        char skippedMipLevels;
        const char* name;
        unsigned int hash;
    };

    enum snd_asset_format
    {
        SND_ASSET_FORMAT_PCMS16 = 0x0,
        SND_ASSET_FORMAT_PCMS24 = 0x1,
        SND_ASSET_FORMAT_PCMS32 = 0x2,
        SND_ASSET_FORMAT_IEEE = 0x3,
        SND_ASSET_FORMAT_XMA4 = 0x4,
        SND_ASSET_FORMAT_MP3 = 0x5,
        SND_ASSET_FORMAT_MSADPCM = 0x6,
        SND_ASSET_FORMAT_WMA = 0x7,
    };

    enum snd_asset_channel
    {
        SND_ASSET_CHANNEL_L = 0x1,
        SND_ASSET_CHANNEL_R = 0x2,
        SND_ASSET_CHANNEL_C = 0x4,
        SND_ASSET_CHANNEL_LFE = 0x8,
        SND_ASSET_CHANNEL_LS = 0x10,
        SND_ASSET_CHANNEL_RS = 0x20,
        SND_ASSET_CHANNEL_LB = 0x40,
        SND_ASSET_CHANNEL_RB = 0x80,
    };

    enum snd_asset_flags
    {
        SND_ASSET_FLAG_DEFAULT = 0x0,
        SND_ASSET_FLAG_LOOPING = 0x1,
        SND_ASSET_FLAG_PAD_LOOP_BUFFER = 0x2,
    };

    struct snd_asset
    {
        unsigned int version;
        unsigned int frame_count;
        unsigned int frame_rate;
        unsigned int channel_count;
        unsigned int header_size;
        unsigned int block_size;
        unsigned int buffer_size;
        snd_asset_format format;
        snd_asset_channel channel_flags;
        snd_asset_flags flags;
        unsigned int seek_table_count;
        unsigned int* seek_table;
        unsigned int data_size;
        char* data;
    };

    struct LoadedSound
    {
        const char* name;
        snd_asset sound;
    };

    struct PrimedSound
    {
        const char* name;
        char* buffer;
        unsigned int size;
    };

    struct StreamedSound
    {
        char* filename;
        PrimedSound* primeSnd;
    };

    union SoundFileRef
    {
        LoadedSound* loadSnd;
        StreamedSound* streamSnd;
    };

    struct __declspec(align(4)) SoundFile
    {
        SoundFileRef u;
        char type;
        char exists;
    };

    struct __declspec(align(2)) snd_alias_t
    {
        const char* name;
        unsigned int id;
        const char* subtitle;
        const char* secondaryname;
        SoundFile* soundFile;
        unsigned int flags;
        unsigned int duck;
        unsigned int contextType;
        unsigned int contextValue;
        unsigned __int16 fluxTime;
        unsigned __int16 startDelay;
        unsigned __int16 reverbSend;
        unsigned __int16 centerSend;
        unsigned __int16 volMin;
        unsigned __int16 volMax;
        unsigned __int16 teamVolMod;
        unsigned __int16 pitchMin;
        unsigned __int16 pitchMax;
        unsigned __int16 teamPitchMod;
        unsigned __int16 distMin;
        unsigned __int16 distMax;
        unsigned __int16 distReverbMax;
        unsigned __int16 envelopMin;
        unsigned __int16 envelopMax;
        unsigned __int16 envelopPercentage;
        char minPriorityThreshold;
        char maxPriorityThreshold;
        char probability;
        char occlusionLevel;
        char occlusionWetDry;
        char minPriority;
        char maxPriority;
        char pan;
        char dryCurve;
        char wetCurve;
        char dryMinCurve;
        char wetMinCurve;
        char limitCount;
        char entityLimitCount;
        char snapshotGroup;
    };

    struct snd_alias_list_t
    {
        const char* name;
        unsigned int id;
        snd_alias_t* head;
        int count;
        int sequence;
    };

    struct SndIndexEntry
    {
        unsigned __int16 value;
        unsigned __int16 next;
    };

    struct snd_radverb
    {
        char name[32];
        unsigned int id;
        float smoothing;
        float earlyTime;
        float lateTime;
        float earlyGain;
        float lateGain;
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
    };

    struct snd_snapshot
    {
        char name[32];
        unsigned int id;
        char occlusionName[32];
        unsigned int occlusionId;
        float fadeIn;
        float fadeOut;
        float distance;
        unsigned int fadeInCurve;
        unsigned int fadeOutCurve;
        float attenuation[64];
    };

    struct SndBank
    {
        const char* name;
        unsigned int aliasCount;
        snd_alias_list_t* alias;
        SndIndexEntry* aliasIndex;
        unsigned int packHash;
        unsigned int packLocation;
        unsigned int radverbCount;
        snd_radverb* radverbs;
        unsigned int snapshotCount;
        snd_snapshot* snapshots;
    };

    struct SndPatch
    {
        char* name;
        unsigned int elementCount;
        unsigned int* elements;
        unsigned int fileCount;
        SoundFile* files;
    };

    struct cStaticModelWritable
    {
        unsigned __int16 nextModelInWorldSector;
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
        __int16 children[2];
    };

    struct __declspec(align(4)) cLeaf_s
    {
        unsigned __int16 firstCollAabbIndex;
        unsigned __int16 collAabbCount;
        int brushContents;
        int terrainContents;
        float mins[3];
        float maxs[3];
        int leafBrushNode;
        __int16 cluster;
    };

    struct cLeafBrushNodeChildren_t
    {
        float dist;
        float range;
        unsigned __int16 childOffset[2];
    };

    struct cLeafBrushNodeLeaf_t
    {
        unsigned __int16* brushes;
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
        unsigned __int16 materialIndex;
        unsigned __int16 childCount;
        float halfSize[3];
        CollisionAabbTreeIndex u;
    };

    struct cmodel_t
    {
        float mins[3];
        float maxs[3];
        float radius;
        cLeaf_s leaf;
    };

    struct type_align(16) cbrush_t
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        int axial_cflags[2][3];
        int axial_sflags[2][3];
        unsigned int numverts;
        float(*verts)[3];
    };

    enum DynEntityType
    {
        DYNENT_TYPE_INVALID = 0x0,
        DYNENT_TYPE_CLUTTER = 0x1,
        DYNENT_TYPE_DESTRUCT = 0x2,
        DYNENT_TYPE_COUNT = 0x3,
    };

    struct GfxPlacement
    {
        float quat[4];
        float origin[3];
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

    struct DynEntityDef
    {
        DynEntityType type;
        GfxPlacement pose;
        XModel* xModel;
        XModel* destroyedxModel;
        unsigned __int16 brushModel;
        unsigned __int16 physicsBrushModel;
        FxEffectDef* destroyFx;
        unsigned int destroySound;
        XModelPieces* destroyPieces;
        PhysPreset* physPreset;
        __int16 physConstraints[4];
        int health;
        int flags;
        int contents;
        unsigned __int16 targetname;
        unsigned __int16 target;
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
        unsigned __int16 burnTime;
        unsigned __int16 fadeTime;
        int physicsStartTime;
    };

    struct DynEntityServer
    {
        unsigned __int16 flags;
        int health;
    };

    struct DynEntityColl
    {
        unsigned __int16 sector;
        unsigned __int16 nextEntInSector;
        float linkMins[3];
        float linkMaxs[3];
        int contents;
    };

    struct par_t
    {
        float p[3];
        float p0[3];
        float p_prev[3];
        int flags;
    };

    enum rope_constraint_e
    {
        ROPE_PAIR_CONSTRAINT = 0x0,
        ROPE_WORLD_CONSTRAINT = 0x1,
        ROPE_DENTITY_CONSTRAINT = 0x2,
        ROPE_CENTITY_CONSTRAINT = 0x3,
    };

    struct __declspec(align(4)) constraint_t
    {
        float p[3];
        rope_constraint_e type;
        int entity_index;
        int bone_name_hash;
        char pi1;
        char pi2;
    };

    struct rope_frame_verts_t
    {
        int num_verts;
        float v[50][3];
    };

    struct rope_client_verts_t
    {
        rope_frame_verts_t frame_verts[2];
        unsigned int frame_index;
    };

    struct __declspec(align(4)) rope_t
    {
        par_t m_particles[25];
        constraint_t m_constraints[30];
        int m_entity_anchors[3];
        int m_num_particles;
        int m_num_constraints;
        int m_num_entity_anchors;
        int m_num_draw_verts;
        rope_client_verts_t m_client_verts;
        float m_min[3];
        float m_max[3];
        float m_start[3];
        float m_end[3];
        int m_in_use;
        int m_visible;
        int m_dist_constraint;
        int m_flags;
        Material* m_material;
        float m_seglen;
        float m_length;
        float m_width;
        float m_scale;
        float m_force_scale;
        int m_health;
        int m_frame;
        int m_stable_count;
        int m_static_rope;
        unsigned __int16 m_lightingHandle;
    };

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
        unsigned int numNodes;
        cNode_t* nodes;
        unsigned int numLeafs;
        cLeaf_s* leafs;
        unsigned int leafbrushNodesCount;
        cLeafBrushNode_s* leafbrushNodes;
        unsigned int numLeafBrushes;
        unsigned __int16* leafbrushes;
        unsigned int numLeafSurfaces;
        unsigned int* leafsurfaces;
        unsigned int vertCount;
        float(*verts)[3];
        unsigned int numBrushVerts;
        float(*brushVerts)[3];
        unsigned int nuinds;
        unsigned __int16* uinds;
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
        cbrush_t* brushes;
        int numClusters;
        int clusterBytes;
        char* visibility;
        int vised;
        MapEnts* mapEnts;
        cbrush_t* box_brush;
        cmodel_t box_model;
        unsigned __int16 originalDynEntCount;
        unsigned __int16 dynEntCount[4];
        DynEntityDef* dynEntDefList[2];
        DynEntityPose* dynEntPoseList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityServer* dynEntServerList[2];
        DynEntityColl* dynEntCollList[4];
        int num_constraints;
        PhysConstraint* constraints;
        int max_ropes;
        rope_t* ropes;
        unsigned int checksum;
    };

    struct ComPrimaryLight
    {
        char type;
        char canUseShadowMap;
        char exponent;
        char priority;
        __int16 cullDist;
        char _pad[2];
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        float cosHalfFovExpanded;
        float rotationLimit;
        float translationLimit;
        float mipDistance;
        float diffuseColor[4];
        float specularColor[4];
        float attenuation[4];
        float falloff[4];
        float angle[4];
        float aAbB[4];
        float cookieControl0[4];
        float cookieControl1[4];
        float cookieControl2[4];
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
        __int16 waterheight;
        char flooroffset;
        char shoredist;
        char color[4];
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
        char state;
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

    struct pathnode_t;

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
        NODE_COVER_PILLAR = 0xA,
        NODE_CONCEALMENT_STAND = 0xB,
        NODE_CONCEALMENT_CROUCH = 0xC,
        NODE_CONCEALMENT_PRONE = 0xD,
        NODE_REACQUIRE = 0xE,
        NODE_BALCONY = 0xF,
        NODE_SCRIPTED = 0x10,
        NODE_NEGOTIATION_BEGIN = 0x11,
        NODE_NEGOTIATION_END = 0x12,
        NODE_TURRET = 0x13,
        NODE_GUARD = 0x14,
        NODE_NUMTYPES = 0x15,
        NODE_DONTLINK = 0x15,
    };

    struct pathlink_s
    {
        float fDist;
        unsigned __int16 nodeNum;
        char disconnectCount;
        char negotiationLink;
        char ubBadPlaceCount[4];
    };

    struct pathnode_constant_t
    {
        nodeType type;
        unsigned __int16 spawnflags;
        unsigned __int16 targetname;
        unsigned __int16 script_linkName;
        unsigned __int16 script_noteworthy;
        unsigned __int16 target;
        unsigned __int16 animscript;
        int animscriptfunc;
        float vOrigin[3];
        float fAngle;
        float forward[2];
        float fRadius;
        float minUseDistSq;
        __int16 wOverlapNode[2];
        __int16 wChainId;
        __int16 wChainDepth;
        __int16 wChainParent;
        unsigned __int16 totalLinkCount;
        pathlink_s* Links;
    };

    struct SentientHandle
    {
        unsigned __int16 number;
        unsigned __int16 infoIndex;
    };

    struct pathnode_dynamic_t
    {
        SentientHandle pOwner;
        int iFreeTime;
        int iValidTime[3];
        int inPlayerLOSTime;
        __int16 wLinkCount;
        __int16 wOverlapCount;
        __int16 turretEntNumber;
        __int16 userCount;
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

    struct pathnode_tree_t;

    struct pathnode_tree_nodes_t
    {
        int nodeCount;
        unsigned __int16* nodes;
    };

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
        unsigned __int16* chainNodeForNode;
        unsigned __int16* nodeForChainNode;
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
        char* entityString;
        int numEntityChars;
    };

    struct GfxStreamingAabbTree
    {
        unsigned __int16 firstItem;
        unsigned __int16 itemCount;
        unsigned __int16 firstChild;
        unsigned __int16 childCount;
        float mins[3];
        float maxs[3];
    };

    struct GfxWorldStreamInfo
    {
        int aabbTreeCount;
        GfxStreamingAabbTree* aabbTrees;
        int leafRefCount;
        int* leafRefs;
    };

    struct GfxWorldSunColor
    {
        unsigned int control;
        float angles[3];
        float ambientColor[4];
        float sunDiffuseColor[4];
        float sunSpecularColor[4];
        float skyColor[4];
        float groundColor[4];
        float exposure;
        float sunShadowSampleSizeNear;
        float skyboxHDRScale;
    };

    struct SunLightParseParams
    {
        char name[64];
        float treeScatterIntensity;
        float treeScatterAmount;
        GfxWorldSunColor sunSettings[1];
    };

    struct float44
    {
        union
        {
            float m[4][4];
            float member[16];
        };
    };

    struct __declspec(align(16)) GfxLight
    {
        char type;
        char canUseShadowMap;
        __int16 cullDist;
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
        unsigned int spotShadowIndex;
        float angles[3];
        float spotShadowHiDistance;
        float diffuseColor[4];
        float specularColor[4];
        float shadowColor[4];
        float falloff[4];
        float attenuation[4];
        float aAbB[4];
        float cookieControl0[4];
        float cookieControl1[4];
        float cookieControl2[4];
        __declspec(align(8)) float44 viewMatrix;
        float44 projMatrix;
        GfxLightDef* def;
    };

    struct GfxLightCorona
    {
        float origin[3];
        float radius;
        float color[3];
        float intensity;
    };

    struct GfxShadowMapVolume
    {
        unsigned int control;
        unsigned int padding1;
        unsigned int padding2;
        unsigned int padding3;
    };

    struct GfxVolumePlane
    {
        float plane[4];
    };

    struct GfxExposureVolume
    {
        unsigned int control;
        float exposure;
        float luminanceIncreaseScale;
        float luminanceDecreaseScale;
        float featherRange;
        float featherAdjust;
    };

    struct GfxSkyDynamicIntensity
    {
        float angle0;
        float angle1;
        float factor0;
        float factor1;
    };

    struct GfxWorldDpvsPlanes
    {
        int cellCount;
        cplane_s* planes;
        unsigned __int16* nodes;
        unsigned int* sceneEntCellBits;
    };

    struct GfxCell;
    struct GfxPortal;

    struct GfxPortalWritable
    {
        bool isQueued;
        bool isAncestor;
        char recursionDepth;
        char hullPointCount;
        float(*hullPoints)[2];
        GfxPortal* queuedParent;
    };

    struct DpvsPlane
    {
        float coeffs[4];
        char side[3];
        char pad;
    };

    struct GfxPortal
    {
        GfxPortalWritable writable;
        DpvsPlane plane;
        GfxCell* cell;
        float(*vertices)[3];
        char vertexCount;
        float hullAxis[2][3];
    };

    struct GfxAabbTree
    {
        float mins[3];
        float maxs[3];
        unsigned __int16 childCount;
        unsigned __int16 surfaceCount;
        unsigned __int16 startSurfIndex;
        unsigned __int16 smodelIndexCount;
        unsigned __int16* smodelIndexes;
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

    struct GfxReflectionProbeVolumeData
    {
        float volumePlanes[6][4];
    };

    struct GfxReflectionProbe
    {
        float origin[3];
        GfxImage* reflectionImage;
        GfxReflectionProbeVolumeData* probeVolumes;
        unsigned int probeVolumeCount;
    };

    struct GfxLightmapArray
    {
        GfxImage* primary;
        GfxImage* secondary;
        GfxImage* secondaryB;
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
        void/*IDirect3DVertexBuffer9*/* worldVb;
    };

    struct GfxWorldVertexLayerData
    {
        char* data;
        void/*IDirect3DVertexBuffer9*/* layerVb;
    };

    struct GfxWorldDraw
    {
        unsigned int reflectionProbeCount;
        GfxReflectionProbe* reflectionProbes;
        GfxTexture* reflectionProbeTextures;
        int lightmapCount;
        GfxLightmapArray* lightmaps;
        GfxTexture* lightmapPrimaryTextures;
        GfxTexture* lightmapSecondaryTextures;
        GfxTexture* lightmapSecondaryTexturesB;
        GfxImage* terrainScorchImages[31];
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        unsigned int vertexStream2DataSize;
        int indexCount;
        unsigned __int16* indices;
    };

    struct type_align(4) GfxLightGridEntry
    {
        unsigned __int16 colorsIndex;
        char primaryLightIndex;
        char needsTrace;
    };

    struct type_align(4) GfxCompressedLightGridColors
    {
        char rgb[56][3];
    };

    struct GfxLightGrid
    {
        bool hasLightRegions;
        unsigned int sunPrimaryLightIndex;
        unsigned __int16 mins[3];
        unsigned __int16 maxs[3];
        unsigned int rowAxis;
        unsigned int colAxis;
        unsigned __int16* rowDataStart;
        unsigned int rawRowDataSize;
        char* rawRowData;
        unsigned int entryCount;
        GfxLightGridEntry* entries;
        unsigned int colorCount;
        GfxCompressedLightGridColors* colors;
    };

    struct GfxBrushModelWritable
    {
        float mins[3];
        float maxs[3];
        float mip1radiusSq;
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
        unsigned __int16 lod;
        unsigned __int16 surfId;
    };

    struct GfxSceneDynModel
    {
        XModelDrawInfo info;
        unsigned __int16 dynEntId;
    };

    struct BModelDrawInfo
    {
        unsigned __int16 surfId;
    };

    struct type_align(4) GfxSceneDynBrush
    {
        BModelDrawInfo info;
        unsigned __int16 dynEntId;
    };

    struct GfxShadowGeometry
    {
        unsigned __int16 surfaceCount;
        unsigned __int16 smodelCount;
        unsigned __int16* sortedSurfIndex;
        unsigned __int16* smodelIndex;
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
        float mins[3];
        float maxs[3];
        float lightingOrigin[3];
        GfxColor groundLighting;
    };

    struct srfTriangles_t
    {
        float mins[3];
        int vertexLayerData;
        float maxs[3];
        int firstVertex;
        unsigned __int16 vertexCount;
        unsigned __int16 triCount;
        int baseIndex;
        float himipRadiusSq;
        int stream2ByteOffset;
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
        int flags;
        unsigned __int16 smodelCacheIndex[4];
        unsigned __int16 lightingHandle;
        char reflectionProbeIndex;
        char primaryLightIndex;
    };

    struct GfxWorldDpvsStatic
    {
        unsigned int smodelCount;
        unsigned int dynamicSModelCount;
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
        char* smodelVisDataCameraSaved;
        char* surfaceVisDataCameraSaved;
        unsigned int* lodData;
        unsigned __int16* sortedSurfIndex;
        GfxStaticModelInst* smodelInsts;
        GfxSurface* surfaces;
        GfxCullGroup* cullGroups;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxDrawSurf* surfaceMaterials;
        unsigned int* surfaceCastsSunShadow;
        volatile int usageCount;
    };

    struct GfxWorldDpvsDynamic
    {
        unsigned int dynEntClientWordCount[2];
        unsigned int dynEntClientCount[2];
        unsigned int* dynEntCellBits[2];
        char* dynEntVisData[2][3];
    };

    struct __declspec(align(4)) GfxWorldLodChain
    {
        float origin[3];
        float lastDist;
        unsigned int firstLodInfo;
        unsigned __int16 lodInfoCount;
    };

    struct __declspec(align(4)) GfxWorldLodInfo
    {
        float dist;
        unsigned int firstSurf;
        unsigned __int16 surfCount;
    };

    struct GfxWaterBuffer
    {
        unsigned int bufferSize;
        float(*buffer)[4];
    };

    struct Occluder
    {
        unsigned int flags;
        char name[16];
        float points[4][3];
    };

    struct GfxOutdoorBounds
    {
        float bounds[2][3];
    };

    struct GfxHeroLight
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

    struct GfxHeroLightTree
    {
        float mins[3];
        float maxs[3];
    };

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        int surfaceCount;
        GfxWorldStreamInfo streamInfo;
        int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        char skySamplerState;
        const char* skyBoxModel;
        SunLightParseParams sunParse;
        GfxLight* sunLight;
        float sunColorFromBsp[3];
        unsigned int sunPrimaryLightIndex;
        unsigned int primaryLightCount;
        int cullGroupCount;
        unsigned int coronaCount;
        GfxLightCorona* coronas;
        unsigned int shadowMapVolumeCount;
        GfxShadowMapVolume* shadowMapVolumes;
        unsigned int shadowMapVolumePlaneCount;
        GfxVolumePlane* shadowMapVolumePlanes;
        unsigned int exposureVolumeCount;
        GfxExposureVolume* exposureVolumes;
        unsigned int exposureVolumePlaneCount;
        GfxVolumePlane* exposureVolumePlanes;
        GfxSkyDynamicIntensity skyDynIntensity;
        GfxWorldDpvsPlanes dpvsPlanes;
        int cellBitsCount;
        GfxCell* cells;
        GfxWorldDraw draw;
        GfxLightGrid lightGrid;
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
        GfxSceneDynModel* sceneDynModel;
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
        Material* coronaMaterial;
        Material* ropeMaterial;
        unsigned int numOccluders;
        Occluder* occluders;
        unsigned int numOutdoorBounds;
        GfxOutdoorBounds* outdoorBounds;
        unsigned int heroLightCount;
        unsigned int heroLightTreeCount;
        GfxHeroLight* heroLights;
        GfxHeroLightTree* heroLightTree;
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
        char style;
        char border;
        char modal;
        char frameSides;
        float frameTexSize;
        float frameSize;
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
        float rotation;
        Material* background;
    };

    struct ScriptCondition
    {
        bool fireOnTrue;
        int constructID;
        int blockID;
        ScriptCondition* next;
    };

    enum expDataType
    {
        VAL_INT = 0x0,
        VAL_FLOAT = 0x1,
        VAL_STRING = 0x2,
    };

    union operandInternalDataUnion
    {
        int intVal;
        float floatVal;
        const char* string;
    };

    struct Operand
    {
        expDataType dataType;
        operandInternalDataUnion internals;
    };

    union expressionRpnDataUnion
    {
        Operand constant;
        void* cmd;
        int cmdIdx;
    };

    struct expressionRpn
    {
        int type;
        expressionRpnDataUnion data;
    };

    struct ExpressionStatement
    {
        char* filename;
        int line;
        int numRpn;
        expressionRpn* rpn;
    };

    struct GenericEventScript
    {
        ScriptCondition* prerequisites;
        ExpressionStatement condition;
        int type;
        bool fireOnTrue;
        const char* action;
        int blockID;
        int constructID;
        GenericEventScript* next;
    };

    struct GenericEventHandler
    {
        const char* name;
        GenericEventScript* eventScript;
        GenericEventHandler* next;
    };

    struct ItemKeyHandler
    {
        int key;
        GenericEventScript* keyScript;
        ItemKeyHandler* next;
    };

    struct textExp_s
    {
        ExpressionStatement textExp;
    };

    struct enumDvarDef_s
    {
        const char* enumDvarName;
    };

    struct editFieldDef_s
    {
        int cursorPos[1];
        float minVal;
        float maxVal;
        float defVal;
        float range;
        int maxChars;
        int maxCharsGotoNext;
        int maxPaintChars;
        int paintOffset;
    };

    struct columnInfo_s
    {
        int elementStyle;
        int maxChars;
        rectDef_s rect;
    };

    struct MenuCell
    {
        int type;
        int maxChars;
        char* stringValue;
    };

    struct MenuRow
    {
        MenuCell* cells;
        char* eventName;
        char* onFocusEventName;
        bool disableArg;
        int status;
        int name;
    };

    struct listBoxDef_s
    {
        int mousePos;
        int cursorPos[1];
        int startPos[1];
        int endPos[1];
        int drawPadding;
        float elementWidth;
        float elementHeight;
        int numColumns;
        float special;
        columnInfo_s columnInfo[16];
        int notselectable;
        int noScrollBars;
        int usePaging;
        float selectBorder[4];
        float disableColor[4];
        float focusColor[4];
        float elementHighlightColor[4];
        float elementBackgroundColor[4];
        Material* selectIcon;
        Material* backgroundItemListbox;
        Material* highlightTexture;
        int noBlinkingHighlight;
        MenuRow* rows;
        int maxRows;
        int rowCount;
    };

    struct multiDef_s
    {
        const char* dvarList[32];
        const char* dvarStr[32];
        float dvarValue[32];
        int count;
        int actionOnEnterPressOnly;
        int strDef;
    };

    union focusDefData_t
    {
        listBoxDef_s* listBox;
        multiDef_s* multi;
        editFieldDef_s* editField;
        enumDvarDef_s* enumDvar;
        void* data;
    };

    struct focusItemDef_s
    {
        const char* mouseEnterText;
        const char* mouseExitText;
        const char* mouseEnter;
        const char* mouseExit;
        ItemKeyHandler* onKey;
        focusDefData_t focusTypeData;
    };

    struct gameMsgDef_s
    {
        int gameMsgWindowIndex;
        int gameMsgWindowMode;
    };

    union textDefData_t
    {
        focusItemDef_s* focusItemDef;
        gameMsgDef_s* gameMsgDef;
        void* data;
    };

    struct textDef_s
    {
        rectDef_s textRect[1];
        int alignment;
        int fontEnum;
        int itemFlags;
        int textAlignMode;
        float textalignx;
        float textaligny;
        float textscale;
        int textStyle;
        const char* text;
        textExp_s* textExpData;
        textDefData_t textTypeData;
    };

    struct imageDef_s
    {
        ExpressionStatement materialExp;
    };

    struct ownerDrawDef_s
    {
        ExpressionStatement dataExp;
    };

    union itemDefData_t
    {
        textDef_s* textDef;
        imageDef_s* imageDef;
        focusItemDef_s* blankButtonDef;
        ownerDrawDef_s* ownerDrawDef;
        void* data;
    };

    struct rectData_s
    {
        ExpressionStatement rectXExp;
        ExpressionStatement rectYExp;
        ExpressionStatement rectWExp;
        ExpressionStatement rectHExp;
    };

    struct animParamsDef_t
    {
        const char* name;
        rectDef_s rectClient;
        float borderSize;
        float foreColor[4];
        float backColor[4];
        float borderColor[4];
        float outlineColor[4];
        float textScale;
        float rotation;
        GenericEventHandler* onEvent;
    };

    struct UIAnimInfo
    {
        int animStateCount;
        animParamsDef_t** animStates;
        animParamsDef_t currentAnimState;
        animParamsDef_t nextAnimState;
        int animating;
        int animStartTime;
        int animDuration;
    };

    struct __declspec(align(8)) itemDef_s
    {
        windowDef_t window;
        int type;
        int dataType;
        int imageTrack;
        const char* dvar;
        const char* dvarTest;
        const char* enableDvar;
        int dvarFlags;
        itemDefData_t typeData;
        menuDef_t* parent;
        rectData_s* rectExpData;
        ExpressionStatement visibleExp;
        unsigned __int64 showBits;
        unsigned __int64 hideBits;
        ExpressionStatement forecolorAExp;
        int ui3dWindowId;
        GenericEventHandler* onEvent;
        UIAnimInfo* animInfo;
    };

    struct __declspec(align(8)) menuDef_t
    {
        windowDef_t window;
        const char* font;
        int fullScreen;
        int ui3dWindowId;
        int itemCount;
        int fontIndex;
        int cursorItem[1];
        int fadeCycle;
        int priority;
        float fadeClamp;
        float fadeAmount;
        float fadeInAmount;
        float blurRadius;
        int openSlideSpeed;
        int closeSlideSpeed;
        int openSlideDirection;
        int closeSlideDirection;
        rectDef_s initialRectInfo;
        int openFadingTime;
        int closeFadingTime;
        int fadeTimeCounter;
        int slideTimeCounter;
        GenericEventHandler* onEvent;
        ItemKeyHandler* onKey;
        ExpressionStatement visibleExp;
        unsigned __int64 showBits;
        unsigned __int64 hideBits;
        const char* allowedBinding;
        const char* soundName;
        int imageTrack;
        int control;
        float focusColor[4];
        float disableColor[4];
        ExpressionStatement rectXExp;
        ExpressionStatement rectYExp;
        itemDef_s** items;
    };

    struct LocalizeEntry
    {
        const char* value;
        const char* name;
    };

    enum weaponIconRatioType_t
    {
        WEAPON_ICON_RATIO_1TO1 = 0x0,
        WEAPON_ICON_RATIO_2TO1 = 0x1,
        WEAPON_ICON_RATIO_4TO1 = 0x2,
        WEAPON_ICON_RATIO_COUNT = 0x3,
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
        WEAPTYPE_MELEE = 0x7,
        WEAPTYPE_NUM = 0x8,
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
        WEAPCLASS_MELEE = 0xB,
        WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
        WEAPCLASS_NUM = 0xD,
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
        IMPACT_TYPE_BULLET_XTREME = 0x4,
        IMPACT_TYPE_SHOTGUN = 0x5,
        IMPACT_TYPE_GRENADE_BOUNCE = 0x6,
        IMPACT_TYPE_GRENADE_EXPLODE = 0x7,
        IMPACT_TYPE_RIFLE_GRENADE = 0x8,
        IMPACT_TYPE_ROCKET_EXPLODE = 0x9,
        IMPACT_TYPE_ROCKET_EXPLODE_XTREME = 0xA,
        IMPACT_TYPE_PROJECTILE_DUD = 0xB,
        IMPACT_TYPE_MORTAR_SHELL = 0xC,
        IMPACT_TYPE_TANK_SHELL = 0xD,
        IMPACT_TYPE_BOLT = 0xE,
        IMPACT_TYPE_BLADE = 0xF,
        IMPACT_TYPE_COUNT = 0x10,
    };

    enum weapInventoryType_t
    {
        WEAPINVENTORY_PRIMARY = 0x0,
        WEAPINVENTORY_OFFHAND = 0x1,
        WEAPINVENTORY_ITEM = 0x2,
        WEAPINVENTORY_ALTMODE = 0x3,
        WEAPINVENTORY_MELEE = 0x4,
        WEAPINVENTORYCOUNT = 0x5,
    };

    enum weapFireType_t
    {
        WEAPON_FIRETYPE_FULLAUTO = 0x0,
        WEAPON_FIRETYPE_SINGLESHOT = 0x1,
        WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
        WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
        WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
        WEAPON_FIRETYPE_STACKED = 0x5,
        WEAPON_FIRETYPE_MINIGUN = 0x6,
        WEAPON_FIRETYPECOUNT = 0x7,
    };

    enum weapClipType_t
    {
        WEAPON_CLIPTYPE_BOTTOM = 0x0,
        WEAPON_CLIPTYPE_TOP = 0x1,
        WEAPON_CLIPTYPE_LEFT = 0x2,
        WEAPON_CLIPTYPE_DP28 = 0x3,
        WEAPON_CLIPTYPE_PTRS = 0x4,
        WEAPON_CLIPTYPE_LMG = 0x5,
        WEAPON_CLIPTYPECOUNT = 0x6,
    };

    enum OffhandClass
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,
        OFFHAND_CLASS_GEAR = 0x4,
        OFFHAND_CLASS_COUNT = 0x5,
    };

    enum OffhandSlot
    {
        OFFHAND_SLOT_NONE = 0x0,
        OFFHAND_SLOT_LETHAL_GRENADE = 0x1,
        OFFHAND_SLOT_TACTICAL_GRENADE = 0x2,
        OFFHAND_SLOT_EQUIPMENT = 0x3,
        OFFHAND_SLOT_SPECIFIC_USE = 0x4,
        OFFHAND_SLOT_COUNT = 0x5,
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
        WEAPPROJEXP_FIRE = 0x7,
        WEAPPROJEXP_NAPALMBLOB = 0x8,
        WEAPPROJEXP_BOLT = 0x9,
        WEAPPROJEXP_NUM = 0xA,
    };

    enum WeapStickinessType
    {
        WEAPSTICKINESS_NONE = 0x0,
        WEAPSTICKINESS_ALL = 0x1,
        WEAPSTICKINESS_ALL_NO_SENTIENTS = 0x2,
        WEAPSTICKINESS_GROUND = 0x3,
        WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
        WEAPSTICKINESS_FLESH = 0x5,
        WEAPSTICKINESS_COUNT = 0x6,
    };

    enum WeapRotateType
    {
        WEAPROTATE_GRENADE_ROTATE = 0x0,
        WEAPROTATE_BLADE_ROTATE = 0x1,
        WEAPROTATE_CYLINDER_ROTATE = 0x2,
        WEAPROTATE_COUNT = 0x3,
    };

    enum guidedMissileType_t
    {
        MISSILE_GUIDANCE_NONE = 0x0,
        MISSILE_GUIDANCE_SIDEWINDER = 0x1,
        MISSILE_GUIDANCE_HELLFIRE = 0x2,
        MISSILE_GUIDANCE_JAVELIN = 0x3,
        MISSILE_GUIDANCE_BALLISTIC = 0x4,
        MISSILE_GUIDANCE_WIREGUIDED = 0x5,
        MISSILE_GUIDANCE_TVGUIDED = 0x6,
        MISSILE_GUIDANCE_COUNT = 0x7,
    };

    struct flameTable
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
        const char* flameOffLoopSound;
        const char* flameIgniteSound;
        const char* flameOnLoopSound;
        const char* flameCooldownSound;
    };

    struct WeaponDef
    {
        const char* szOverlayName;
        XModel** gunXModel;
        XModel* handXModel;
        const char* szModeName;
        unsigned __int16* notetrackSoundMapKeys;
        unsigned __int16* notetrackSoundMapValues;
        int playerAnimType;
        weapType_t weapType;
        weapClass_t weapClass;
        PenetrateType penetrateType;
        ImpactType impactType;
        weapInventoryType_t inventoryType;
        weapFireType_t fireType;
        weapClipType_t clipType;
        int itemIndex;
        const char* parentWeaponName;
        int iJamFireTime;
        int tracerFrequency;
        float tracerWidth;
        float tracerLength;
        int overheatWeapon;
        float overheatRate;
        float cooldownRate;
        float overheatEndVal;
        bool coolWhileFiring;
        bool fuelTankWeapon;
        int iTankLifeTime;
        OffhandClass offhandClass;
        OffhandSlot offhandSlot;
        weapStance_t stance;
        FxEffectDef* viewFlashEffect;
        FxEffectDef* worldFlashEffect;
        const char* pickupSound;
        const char* pickupSoundPlayer;
        const char* ammoPickupSound;
        const char* ammoPickupSoundPlayer;
        const char* projectileSound;
        const char* pullbackSound;
        const char* pullbackSoundPlayer;
        const char* fireSound;
        const char* fireSoundPlayer;
        const char* fireLoopSound;
        const char* fireLoopSoundPlayer;
        const char* fireLoopEndSound;
        const char* fireLoopEndSoundPlayer;
        const char* fireStopSound;
        const char* fireStopSoundPlayer;
        const char* fireLastSound;
        const char* fireLastSoundPlayer;
        const char* emptyFireSound;
        const char* emptyFireSoundPlayer;
        const char* crackSound;
        const char* whizbySound;
        const char* meleeSwipeSound;
        const char* meleeSwipeSoundPlayer;
        const char* meleeHitSound;
        const char* meleeMissSound;
        const char* rechamberSound;
        const char* rechamberSoundPlayer;
        const char* reloadSound;
        const char* reloadSoundPlayer;
        const char* reloadEmptySound;
        const char* reloadEmptySoundPlayer;
        const char* reloadStartSound;
        const char* reloadStartSoundPlayer;
        const char* reloadEndSound;
        const char* reloadEndSoundPlayer;
        const char* rotateLoopSound;
        const char* rotateLoopSoundPlayer;
        const char* deploySound;
        const char* deploySoundPlayer;
        const char* finishDeploySound;
        const char* finishDeploySoundPlayer;
        const char* breakdownSound;
        const char* breakdownSoundPlayer;
        const char* finishBreakdownSound;
        const char* finishBreakdownSoundPlayer;
        const char* detonateSound;
        const char* detonateSoundPlayer;
        const char* nightVisionWearSound;
        const char* nightVisionWearSoundPlayer;
        const char* nightVisionRemoveSound;
        const char* nightVisionRemoveSoundPlayer;
        const char* altSwitchSound;
        const char* altSwitchSoundPlayer;
        const char* raiseSound;
        const char* raiseSoundPlayer;
        const char* firstRaiseSound;
        const char* firstRaiseSoundPlayer;
        const char* putawaySound;
        const char* putawaySoundPlayer;
        const char* overheatSound;
        const char* overheatSoundPlayer;
        const char* adsZoomSound;
        const char** bounceSound;
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
        float vDuckedSprintOfs[3];
        float vDuckedSprintRot[3];
        float vDuckedSprintBob[2];
        float fDuckedSprintCycleScale;
        float vSprintOfs[3];
        float vSprintRot[3];
        float vSprintBob[2];
        float fSprintCycleScale;
        float vLowReadyOfs[3];
        float vLowReadyRot[3];
        float vDtpOfs[3];
        float vDtpRot[3];
        float vDtpBob[2];
        float fDtpCycleScale;
        float vMantleOfs[3];
        float vMantleRot[3];
        float vSlideOfs[3];
        float vSlideRot[3];
        float vDuckedRot[3];
        float vProneOfs[3];
        float vProneMove[3];
        float vProneRot[3];
        float vStrafeMove[3];
        float vStrafeRot[3];
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
        XModel* mountedModel;
        XModel* additionalMeleeModel;
        Material* hudIcon;
        weaponIconRatioType_t hudIconRatio;
        Material* indicatorIcon;
        weaponIconRatioType_t indicatorIconRatio;
        Material* ammoCounterIcon;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
        int iStartAmmo;
        int iHeatIndex;
        int iMaxAmmo;
        int shotCount;
        const char* szSharedAmmoCapName;
        int iSharedAmmoCapIndex;
        int iSharedAmmoCap;
        bool unlimitedAmmo;
        bool ammoCountClipRelative;
        int damage;
        float damageDuration;
        float damageInterval;
        int playerDamage;
        int iMeleeDamage;
        int iDamageType;
        unsigned __int16 explosionTag;
        int iFireDelay;
        int iMeleeDelay;
        int meleeChargeDelay;
        int iDetonateDelay;
        int iSpinUpTime;
        int iSpinDownTime;
        float spinRate;
        const char* spinLoopSound;
        const char* spinLoopSoundPlayer;
        const char* startSpinSound;
        const char* startSpinSoundPlayer;
        const char* stopSpinSound;
        const char* stopSpinSoundPlayer;
        int iFireTime;
        int iLastFireTime;
        int iRechamberTime;
        int iRechamberBoltTime;
        int iHoldFireTime;
        int iDetonateTime;
        int iMeleeTime;
        int meleeChargeTime;
        int iReloadTimeRight;
        int iReloadTimeLeft;
        int reloadShowRocketTime;
        int iReloadEmptyTimeLeft;
        int iReloadAddTime;
        int iReloadEmptyAddTime;
        int iReloadQuickAddTime;
        int iReloadQuickEmptyAddTime;
        int iReloadStartTime;
        int iReloadStartAddTime;
        int iReloadEndTime;
        int iDropTime;
        int iRaiseTime;
        int iAltDropTime;
        int quickDropTime;
        int quickRaiseTime;
        int iFirstRaiseTime;
        int iEmptyRaiseTime;
        int iEmptyDropTime;
        int sprintInTime;
        int sprintLoopTime;
        int sprintOutTime;
        int lowReadyInTime;
        int lowReadyLoopTime;
        int lowReadyOutTime;
        int contFireInTime;
        int contFireLoopTime;
        int contFireOutTime;
        int dtpInTime;
        int dtpLoopTime;
        int dtpOutTime;
        int slideInTime;
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
        int lockOnRadius;
        int lockOnSpeed;
        bool requireLockonToFire;
        bool noAdsWhenMagEmpty;
        bool avoidDropCleanup;
        unsigned int stackFire;
        float stackFireSpread;
        float stackFireAccuracyDecay;
        const char* stackSound;
        float autoAimRange;
        float aimAssistRange;
        bool mountableWeapon;
        float aimPadding;
        float enemyCrosshairRange;
        bool crosshairColorChange;
        float moveSpeedScale;
        float adsMoveSpeedScale;
        float sprintDurationScale;
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
        bool sharedAmmo;
        bool bRifleBullet;
        bool armorPiercing;
        bool bBoltAction;
        bool bUseAltTagFlash;
        bool bUseAntiLagRewind;
        bool bIsCarriedKillstreakWeapon;
        bool aimDownSight;
        bool bRechamberWhileAds;
        bool bReloadWhileAds;
        float adsViewErrorMin;
        float adsViewErrorMax;
        bool bCookOffHold;
        bool bClipOnly;
        bool bCanUseInVehicle;
        bool bNoDropsOrRaises;
        bool adsFireOnly;
        bool cancelAutoHolsterWhenEmpty;
        bool suppressAmmoReserveDisplay;
        bool laserSightDuringNightvision;
        bool bHideThirdPerson;
        bool bHasBayonet;
        bool bDualWield;
        bool bExplodeOnGround;
        bool bThrowBack;
        bool bRetrievable;
        bool bDieOnRespawn;
        bool bNoThirdPersonDropsOrRaises;
        bool bContinuousFire;
        bool bNoPing;
        bool bForceBounce;
        bool bUseDroppedModelAsStowed;
        bool bNoQuickDropWhenEmpty;
        bool bKeepCrosshairWhenADS;
        bool bUseOnlyAltWeaoponHideTagsInAltMode;
        Material* killIcon;
        weaponIconRatioType_t killIconRatio;
        bool flipKillIcon;
        bool bNoPartialReload;
        bool bSegmentedReload;
        bool bNoADSAutoReload;
        int iReloadAmmoAdd;
        int iReloadStartAdd;
        const char* szSpawnedGrenadeWeaponName;
        const char* szDualWieldWeaponName;
        unsigned int dualWieldWeaponIndex;
        int iDropAmmoMin;
        int iDropAmmoMax;
        int iDropClipAmmoMin;
        int iDropClipAmmoMax;
        bool blocksProne;
        bool bShowIndicator;
        int isRollingGrenade;
        int iExplosionRadius;
        int iExplosionRadiusMin;
        int iIndicatorRadius;
        int iExplosionInnerDamage;
        int iExplosionOuterDamage;
        float damageConeAngle;
        int iProjectileSpeed;
        int iProjectileSpeedUp;
        int iProjectileSpeedRelativeUp;
        int iProjectileSpeedForward;
        int iProjectileActivateDist;
        float projLifetime;
        float timeToAccelerate;
        float projectileCurvature;
        XModel* projectileModel;
        weapProjExposion_t projExplosion;
        FxEffectDef* projExplosionEffect;
        bool projExplosionEffectForceNormalUp;
        FxEffectDef* projExplosionEffect2;
        bool projExplosionEffect2ForceNormalUp;
        FxEffectDef* projExplosionEffect3;
        bool projExplosionEffect3ForceNormalUp;
        FxEffectDef* projExplosionEffect4;
        bool projExplosionEffect4ForceNormalUp;
        FxEffectDef* projExplosionEffect5;
        bool projExplosionEffect5ForceNormalUp;
        FxEffectDef* projDudEffect;
        const char* projExplosionSound;
        const char* projDudSound;
        const char* mortarShellSound;
        const char* tankShellSound;
        bool bProjImpactExplode;
        bool bBulletImpactExplode;
        WeapStickinessType stickiness;
        WeapRotateType rotateType;
        bool plantable;
        bool hasDetonator;
        bool timedDetonation;
        bool bNoCrumpleMissile;
        bool rotate;
        bool bKeepRolling;
        bool holdButtonToThrow;
        bool offhandHoldIsCancelable;
        bool freezeMovementWhenFiring;
        float lowAmmoWarningThreshold;
        float meleeChargeRange;
        bool bUseAsMelee;
        bool isCameraSensor;
        bool isAcousticSensor;
        float* parallelBounce;
        float* perpendicularBounce;
        FxEffectDef* projTrailEffect;
        float vProjectileColor[3];
        guidedMissileType_t guidedMissileType;
        float maxSteeringAccel;
        int projIgnitionDelay;
        FxEffectDef* projIgnitionEffect;
        const char* projIgnitionSound;
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
        const char* accuracyGraphName[2];
        float(*accuracyGraphKnots[2])[2];
        float(*originalAccuracyGraphKnots[2])[2];
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
        const char* reloadRumble;
        float adsDofStart;
        float adsDofEnd;
        float hipDofStart;
        float hipDofEnd;
        float scanSpeed;
        float scanAccel;
        int scanPauseTime;
        const char* flameTableFirstPerson;
        const char* flameTableThirdPerson;
        flameTable* flameTableFirstPersonPtr;
        flameTable* flameTableThirdPersonPtr;
        FxEffectDef* tagFx_preparationEffect;
        FxEffectDef* tagFlash_preparationEffect;
        bool doGibbing;
        float maxGibDistance;
    };

    struct WeaponVariantDef
    {
        const char* szInternalName;
        int iVariantCount;
        WeaponDef* weapDef;
        const char* szDisplayName;
        const char** szXAnims;
        const char* szAltWeaponName;
        unsigned __int16* hideTags;
        unsigned int altWeaponIndex;
        int iClipSize;
        int iReloadTime;
        int iReloadEmptyTime;
        int iReloadQuickTime;
        int iReloadQuickEmptyTime;
        int iAdsTransInTime;
        int iAdsTransOutTime;
        int iAltRaiseTime;
        const char* szAmmoName;
        int iAmmoIndex;
        const char* szClipName;
        int iClipIndex;
        float fAimAssistRangeAds;
        float fAdsSwayHorizScale;
        float fAdsSwayVertScale;
        float fAdsViewKickCenterSpeed;
        float fHipViewKickCenterSpeed;
        float fAdsZoomFov1;
        float fAdsZoomFov2;
        float fAdsZoomFov3;
        float fAdsZoomInFrac;
        float fAdsZoomOutFrac;
        float fOverlayAlphaScale;
        float fOOPosAnimLength[2];
        bool bSilenced;
        bool bDualMag;
        bool bFullMetalJacket;
        bool bHollowPoint;
        bool bRapidFire;
        Material* overlayMaterial;
        Material* overlayMaterialLowRes;
        Material* dpadIcon;
        weaponIconRatioType_t dpadIconRatio;
        float ikLeftHandOffset[3];
        float ikLeftHandRotation[3];
        float ikLeftHandProneOffset[3];
        float ikLeftHandProneRotation[3];
        float ikLeftHandUiViewerOffset[3];
        float ikLeftHandUiViewerRotation[3];
    };

    struct WeaponFullDef
    {
        WeaponVariantDef weapVariantDef;
        WeaponDef weapDef;
        XModel* gunXModel[16];
        const char* szXAnims[66];
        unsigned __int16 hideTags[32];
        unsigned __int16 notetrackSoundMapKeys[20];
        unsigned __int16 notetrackSoundMapValues[20];
        XModel* worldModel[16];
        float parallelBounce[31];
        float perpendicularBounce[31];
        float locationDamageMultipliers[19];
    };

    enum snd_category_t
    {
        SND_CATEGORY_SFX = 0x0,
        SND_CATEGORY_MUSIC = 0x1,
        SND_CATEGORY_VOICE = 0x2,
        SND_CATEGORY_UI = 0x3,
        SND_CATEGORY_COUNT = 0x4,
    };

    struct snd_group
    {
        char name[32];
        char parentName[32];
        unsigned int id;
        int parentIndex;
        snd_category_t category;
        unsigned __int16 attenuationSp;
        unsigned __int16 attenuationMp;
    };

    struct snd_curve
    {
        char name[32];
        unsigned int id;
        float points[8][2];
    };

    struct snd_pan
    {
        char name[32];
        unsigned int id;
        float front;
        float back;
        float center;
        float lfe;
        float left;
        float right;
    };

    struct snd_snapshot_group
    {
        char name[32];
    };

    struct snd_context
    {
        unsigned int type;
        unsigned int valueCount;
        unsigned int values[8];
    };

    struct snd_master
    {
        char name[32];
        unsigned int id;
        float notchE;
        float notchG;
        float notchF;
        float notchQ;
        float lowE;
        float lowG;
        float lowF;
        float lowQ;
        float peak1E;
        float peak1G;
        float peak1F;
        float peak1Q;
        float peak2E;
        float peak2G;
        float peak2F;
        float peak2Q;
        float hiE;
        float hiG;
        float hiF;
        float hiQ;
        float eqG;
        float compE;
        float compPG;
        float compMG;
        float compT;
        float compR;
        float compTA;
        float compTR;
        float limitE;
        float limitPG;
        float limitMG;
        float limitT;
        float limitR;
        float limitTA;
        float limitTR;
    };

    struct SndDriverGlobals
    {
        const char* name;
        unsigned int groupCount;
        snd_group* groups;
        unsigned int curveCount;
        snd_curve* curves;
        unsigned int panCount;
        snd_pan* pans;
        unsigned int snapshotGroupCount;
        snd_snapshot_group* snapshotGroups;
        unsigned int contextCount;
        snd_context* contexts;
        unsigned int masterCount;
        snd_master* masters;
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
        char behavior;
        char index;
        char fps;
        char loopCount;
        char colIndexBits;
        char rowIndexBits;
        unsigned __int16 entryCountAndIndexRange;
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
        void* anonymous;
        Material* material;
        XModel* model;
        FxEffectDefRef effectDef;
        const char* soundName;
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
        unsigned __int16* inds;
    };

    struct FxBillboardTrim
    {
        float topWidth;
        float bottomWidth;
    };

    union FxElemDefUnion
    {
        FxBillboardTrim billboard;
        FxIntRange cloudDensityRange;
    };

    struct FxElemSpawnSound
    {
        const char* spawnSound;
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
        unsigned int rotationAxis;
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
        FxEffectDefRef effectAttached;
        FxTrailDef* trailDef;
        char sortOrder;
        char lightingFrac;
        char unused[2];
        unsigned __int16 alphaFadeTimeMsec;
        unsigned __int16 maxWindStrength;
        unsigned __int16 spawnIntervalAtMaxWind;
        unsigned __int16 lifespanAtMaxWind;
        FxElemDefUnion u;
        FxElemSpawnSound spawnSound;
        float billboardPivot[2];
    };

    struct FxEffectDef
    {
        const char* name;
        char flags;
        char efPriority;
        char reserved[2];
        int totalSize;
        int msecLoopingLife;
        int elemDefCountLooping;
        int elemDefCountOneShot;
        int elemDefCountEmission;
        FxElemDef* elemDefs;
        float boundingBoxDim[3];
        float boundingSphere[4];
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
        __int16* cellIndex;
    };

    struct PackIndexHeader
    {
        unsigned int magic;
        unsigned int timestamp;
        unsigned int count;
        unsigned int alignment;
        unsigned int data_start;
    };

    struct PackIndexEntry
    {
        unsigned int hash;
        unsigned int offset;
        unsigned int size;
    };

    struct PackIndex
    {
        const char* name;
        PackIndexHeader header;
        PackIndexEntry* entries;
    };

    struct XGlobals
    {
        const char* name;
        int xanimStreamBufferSize;
        int cinematicMaxWidth;
        int cinematicMaxHeight;
        int extracamResolution;
        int gumpReserve;
        float screenClearColor[4];
    };

    struct ddlMemberDef_t
    {
        const char* name;
        int size;
        int offset;
        int type;
        int externalIndex;
        unsigned int min;
        unsigned int max;
        unsigned int serverDelta;
        unsigned int clientDelta;
        int arraySize;
        int enumIndex;
        int permission;
    };

    struct ddlStructDef_t
    {
        const char* name;
        int size;
        int memberCount;
        ddlMemberDef_t* members;
    };

    struct ddlEnumDef_t
    {
        const char* name;
        int memberCount;
        const char** members;
    };

    struct ddlDef_t
    {
        int version;
        int size;
        ddlStructDef_t* structList;
        int structCount;
        ddlEnumDef_t* enumList;
        int enumCount;
        ddlDef_t* next;
    };

    struct ddlRoot_t
    {
        const char* name;
        ddlDef_t* ddlDef;
    };

    struct GlassDef
    {
        const char* name;
        int maxHealth;
        float thickness;
        float minShardSize;
        float maxShardSize;
        float shardLifeProbablility;
        int maxShards;
        Material* pristineMaterial;
        Material* crackedMaterial;
        Material* shardMaterial;
        const char* crackSound;
        const char* shatterShound;
        const char* autoShatterShound;
        FxEffectDef* crackEffect;
        FxEffectDef* shatterEffect;
    };

    struct Glass
    {
        GlassDef* glassDef;
        unsigned int index;
        unsigned int brushModel;
        float origin[3];
        float angles[3];
        float absmin[3];
        float absmax[3];
        bool isPlanar;
        char numOutlineVerts;
        float(*outline)[2];
        float outlineAxis[3][3];
        float outlineOrigin[3];
        float uvScale;
        float thickness;
    };

    struct Glasses
    {
        const char* name;
        unsigned int numGlasses;
        Glass* glasses;
        char* workMemory;
        unsigned int workMemorySize;
        unsigned int smallAllocatorBlocks;
        unsigned int maxGroups;
        unsigned int maxShards;
        unsigned int maxPhysics;
        unsigned int shardMemorySize;
        unsigned int maxFreeCmd;
        unsigned int numSlots;
        unsigned int numVerts;
        unsigned int numIndices;
    };

    struct EmblemLayer
    {
        int cost;
        int unlockLevel;
        int unlockPLevel;
    };

    struct EmblemCategory
    {
        const char* name;
        const char* description;
    };

    struct EmblemIcon
    {
        GfxImage* image;
        const char* description;
        float outlineSize;
        int defaultColor;
        int cost;
        int unlockLevel;
        int unlockPLevel;
        int unclassifyAt;
        int sortKey;
        unsigned int category;
    };

    struct EmblemBackground
    {
        Material* material;
        const char* description;
        int cost;
        int unlockLevel;
        int unlockPLevel;
        int unclassifyAt;
    };

    struct EmblemSet
    {
        int colorCount;
        int layerCount;
        EmblemLayer* layers;
        int categoryCount;
        EmblemCategory* categories;
        int iconCount;
        EmblemIcon* icons;
        int backgroundCount;
        EmblemBackground* backgrounds;
        int backgroundLookupCount;
        __int16* backgroundLookup;
    };

#ifndef __zonecodegenerator
} // namespace T5
#endif

#endif

// EOF
