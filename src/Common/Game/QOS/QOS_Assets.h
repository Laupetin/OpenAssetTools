#pragma once

#ifndef __QOS_ASSETS_H
#define __QOS_ASSETS_H

#include "../../Utils/TypeAlignment.h"

#ifndef __zonecodegenerator
namespace QOS
{
#endif
    enum XFileBlock
    {
        XFILE_BLOCK_TEMP,
        XFILE_BLOCK_RUNTIME,
        XFILE_BLOCK_VIRTUAL,
        XFILE_BLOCK_LARGE,
        XFILE_BLOCK_PHYSICAL,

        MAX_XFILE_COUNT
    };

    enum XAssetType
    {
        ASSET_TYPE_XMODELPIECES,
        ASSET_TYPE_PHYSPRESET,
        ASSET_TYPE_PHYSCONSTRAINTS,
        ASSET_TYPE_DESTRUCTIBLEDEF,
        ASSET_TYPE_XANIMPARTS,
        ASSET_TYPE_XMODEL,
        ASSET_TYPE_MATERIAL,
        ASSET_TYPE_TECHNIQUE_SET,
        ASSET_TYPE_IMAGE,
        ASSET_TYPE_SOUND,
        ASSET_TYPE_SND_CURVE,
        ASSET_TYPE_CLIPMAP_SP,
        ASSET_TYPE_CLIPMAP_MP,
        ASSET_TYPE_COMWORLD,
        ASSET_TYPE_GAMEWORLD_SP,
        ASSET_TYPE_GAMEWORLD_MP,
        ASSET_TYPE_MAP_ENTS,
        ASSET_TYPE_GFXWORLD,
        ASSET_TYPE_LIGHT_DEF,
        ASSET_TYPE_UI_MAP,
        ASSET_TYPE_FONT,
        ASSET_TYPE_MENUFILE,
        ASSET_TYPE_MENU,
        ASSET_TYPE_LOCALIZE_ENTRY,
        ASSET_TYPE_WEAPON,
        ASSET_TYPE_SNDDRIVER_GLOBALS,
        ASSET_TYPE_FX,
        ASSET_TYPE_IMPACT_FX,
        ASSET_TYPE_AITYPE,
        ASSET_TYPE_MPTYPE,
        ASSET_TYPE_CHARACTER,
        ASSET_TYPE_XMODELALIAS,
        ASSET_TYPE_RAWFILE,
        ASSET_TYPE_STRINGTABLE,
        ASSET_TYPE_XMLTREE,
        ASSET_TYPE_SCENE_ANIMATION,
        ASSET_TYPE_CUTSCENE,
        ASSET_TYPE_CUSTOM_CAMERA,

        ASSET_TYPE_COUNT,

        ASSET_TYPE_STRING = ASSET_TYPE_COUNT,
        ASSET_TYPE_ASSETLIST
    };

    struct XModelPieces;
    struct PhysPreset;
    struct PhysConstraints;
    struct DestructibleDef;
    struct XAnimParts;
    struct XModel;
    struct Material;
    struct MaterialTechniqueSet;
    struct GfxImage;
    struct snd_alias_list_t;
    struct SndCurve;
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
    struct AiType;
    struct MpType;
    struct Character;
    struct XModelAlias;
    struct RawFile;
    struct StringTable;
    struct XmlTree;
    struct SceneAnimation;
    struct Cutscene;
    struct CustomCamera;

    union XAssetHeader
    {
        XModelPieces* xmodelPieces;
        PhysPreset* physPreset;
        PhysConstraints* physConstraints;
        DestructibleDef* destructibleDef;
        XAnimParts* parts;
        XModel* model;
        Material* material;
        MaterialTechniqueSet* techniqueSet;
        GfxImage* image;
        snd_alias_list_t* sound;
        SndCurve* sndCurve;
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
        AiType* aiType;
        MpType* mpType;
        Character* character;
        XModelAlias* xmodelAlias;
        RawFile* rawfile;
        StringTable* stringTable;
        XmlTree* xmlTree;
        SceneAnimation* sceneAnimation;
        Cutscene* cutscene;
        CustomCamera* customCamera;
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
        float v[3];

        struct
        {
            float x;
            float y;
            float z;
        };
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

    typedef unsigned short ScriptString;
    typedef unsigned char raw_byte;
    typedef unsigned int raw_uint;

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
        char unknown0[24];
        const char* unknownString;
        char unknown1[16];
    };

    struct PhysConstraint
    {
        char unknown0[4];
        const char* target_bone1;
        const char* target_bone2;
        char unknown1[56];
    };

    struct PhysConstraints
    {
        const char* name;
        unsigned int count;
        PhysConstraint data[16];
    };

    struct DestructibleDef;
    struct FxImpactEntry;

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
        const char* unknownString;
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
        // Keeps randomDataShortCount at offset 36.
        unsigned char pad;
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

    struct XSurfaceTri
    {
        unsigned short i[3];
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

    struct GfxPackedVertex
    {
        vec3_t xyz;
        float binormalSign;
        GfxColor color;
        unsigned int unknown0; // Offset 20; Greyhound treats this as padding.
        vec2_t texCoord;       // QOS stores model UVs as two full floats at offsets 24 and 28.
        PackedUnitVec normal;
        PackedUnitVec tangent; // Likely the second tangent-space vector at offset 36.
    };

    struct GfxPackedVertex16
    {
        unsigned int data[4];
    };

    struct XSurfaceVertexInfo
    {
        short vertCount[4];
        unsigned short* vertsBlend;
        float* tensionData;
    };

    struct XRigidVertList
    {
        unsigned short boneOffset;
        unsigned short vertCount;
        unsigned short triOffset;
        unsigned short triCount;
    };

    struct XSurface
    {
        char tileMode;
        bool deformed;
        unsigned short vertCount;
        unsigned short triCount;
        char unknown0[2];
        XSurfaceTri* triIndices;
        XSurfaceVertexInfo vertInfo;
        GfxPackedVertex* verts0;
        void* vb0;
        GfxPackedVertex16* verts1;
        void* vb1;
        unsigned int vertListCount;
        XRigidVertList* vertList;
        int partBits[7];
    };

    struct XModelLodInfo
    {
        float dist;
        unsigned short numsurfs;
        unsigned short surfIndex;
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
        short firstAdjacentSideOffset;
        char edgeCount;
        char pad;
    };

    typedef unsigned char cbrushedge_t;

    struct BrushWrapper
    {
        float mins[3];
        int contents;
        float maxs[3];
        unsigned int numsides;
        cbrushside_t* sides;
        short axialMaterialNum[2][3];
        cbrushedge_t* baseAdjacentSide;
        short firstAdjacentSideOffsets[2][3];
        char edgeCount[2][3];
        unsigned int numverts;
        vec3_t* verts;
        int totalEdgeCount;
        cplane_s* planes;
        char unknown[8];
    };

    struct PhysGeomInfo
    {
        BrushWrapper* brush;
        int type;
        vec3_t orientation[3];
        vec3_t offset;
        vec3_t halfLengths;
    };

    struct PhysGeomList
    {
        unsigned int count;
        PhysGeomInfo* geoms;
        float mass[3];
    };

    struct XModelStreamInfo
    {
        char pad;
    };

    struct XModelQuat
    {
        short v[4];
    };

    struct XModel
    {
        const char* name;
        unsigned char numBones;
        unsigned char numRootBones;
        unsigned char numsurfs;
        unsigned char lodRampType;
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
        unsigned short numLods;
        short collLod;
        XModelStreamInfo streamInfo;
        int memUsage;
        unsigned char flags;
        bool bad;
        PhysPreset* physPreset;
        PhysGeomList* physGeoms;
        PhysConstraints* physConstraints;
    };

    struct MaterialVertexDeclaration
    {
        unsigned int data[28];
    };

    struct GfxVertexShaderLoadDef
    {
        unsigned int* program;
        unsigned short programSize;
        unsigned short loadForRenderer;
    };

    struct MaterialVertexShader
    {
        const char* name;
        GfxVertexShaderLoadDef loadDef;
    };

    struct GfxPixelShaderLoadDef
    {
        unsigned int* program;
        unsigned short programSize;
        unsigned short loadForRenderer;
    };

    struct MaterialPixelShader
    {
        const char* name;
        GfxPixelShaderLoadDef loadDef;
    };

    enum MaterialShaderArgumentType
    {
        MTL_ARG_MATERIAL_VERTEX_CONST = 0,
        MTL_ARG_LITERAL_VERTEX_CONST = 1,
        MTL_ARG_MATERIAL_PIXEL_SAMPLER = 2,
        MTL_ARG_CODE_VERTEX_CONST = 3,
        MTL_ARG_CODE_PIXEL_SAMPLER = 4,
        MTL_ARG_CODE_PIXEL_CONST = 5,
        MTL_ARG_MATERIAL_PIXEL_CONST = 6,
        MTL_ARG_LITERAL_PIXEL_CONST = 7,
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
        char unknown[8];
        MaterialTechnique* techniques[43];
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

    struct MaterialWaterDef
    {
        char unknown[28];
        water_t* water;
    };

    union MaterialTextureDefInfo
    {
        GfxImage* image;
        MaterialWaterDef* water;
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

    struct MaterialTextureDef
    {
        unsigned int nameHash;
        unsigned char nameStart;
        unsigned char nameEnd;
        unsigned char samplerState;
        unsigned char semantic;
        MaterialTextureDefInfo u;
    };

    struct MaterialConstantDef
    {
        unsigned int data[8];
    };

    struct GfxStateBits
    {
        unsigned int loadBits[2];
    };

    struct GfxImageLoadDef
    {
        char levelCount;
        char flags;
        unsigned short dimensions[3];
        int format;
        unsigned int resourceSize;
        // Embedded mip data is stored smallest-to-largest, matching IWI file order.
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
        int platform[2];
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
        unsigned short width;
        unsigned short height;
        unsigned short depth;
        char category;
        bool delayLoadPixels;
        const char* name;
    };

    struct MaterialInfo
    {
        const char* name;
    };

    struct Material
    {
        MaterialInfo info;
        char unknown0[63];
        unsigned char textureCount;
        unsigned char constantCount;
        unsigned char stateBitsCount;
        char unknown1[14];
        MaterialTechniqueSet* techniqueSet;
        MaterialTextureDef* textureTable;
        MaterialConstantDef* constantTable;
        GfxStateBits* stateBitsTable;
        char unknown2[4];
    };

    struct SndCurve
    {
        const char* name;
        char unknown0[68];
    };

    enum QosSoundFileType
    {
        // Only the primed branch is identified so far. Other values use the inline MssSound branch.
        QOS_SOUND_FILE_TYPE_PRIMED = 3,
    };

    struct QosMssSound
    {
        // Largely matches an MSS sound info block, but only the embedded data size is confirmed.
        int unknown0;
        raw_uint unknownDataPtr0;
        unsigned int dataSize;
        char unknown1[24];
        raw_uint unknownDataPtr1;
        raw_byte data[1];
    };

    struct QosPrimedSound
    {
        const char* name;
        raw_byte* buffer;
        unsigned int size;
    };

    union QosSoundFileRef
    {
        QosMssSound* mssSound;
        QosPrimedSound* primeSnd;
    };

    struct SoundFile
    {
        const char* unknownString0;
        const char* unknownString1;
        QosSoundFileRef u;
        // Scalar fields between the file reference and the type discriminator.
        char unknown0[184];
        QosSoundFileType type;
    };

    struct snd_alias_t
    {
        const char* aliasName;
        const char* subtitle;
        const char* secondaryAliasName;
        const char* chainAliasName;
        SoundFile* soundFile;
        char unknown0[56];
        SndCurve* volumeFalloffCurve;
    };

    struct snd_alias_list_t
    {
        const char* name;
        snd_alias_t* head;
        int count;
    };

    struct MapEnts
    {
        const char* name;
        char* entityString;
        int numEntityChars;
    };

    struct cStaticModel_t
    {
        char unknown0[4];
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

    struct cLeafBrushNodeLeaf_t
    {
        uint16_t* brushes;
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
        unsigned char triCount;
        unsigned char borderCount;
        char unknown0[2];
        int firstTri;
        int unknown1;
        int unknown2;
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
        char unknown0[2];
        unsigned int unknown1; // PC loader does not use this word while streaming; likely related to verts.
        vec3_t* verts;
    };

    struct DynEntityDef
    {
        char unknown0[32]; // Includes type and placement data.
        XModel* xModel;
        char unknown1[4]; // Matches IW-era brushModel/physicsBrushModel area.
        FxEffectDef* destroyFx;
        XModelPieces* destroyPieces;
        PhysPreset* physPreset;
        char unknown2[36];
    };

    struct DynEntityClient
    {
        char unknown[52];
    };

    struct DynEntityServer
    {
        char unknown[36];
    };

    struct DynEntityColl
    {
        char unknown[32];
    };

    struct DynEntityPhysConstraint
    {
        char unknown[72];
    };

    struct clipMap_t
    {
        const char* name;
        int isInUse;
        int planeCount;
        cplane_s* planes;
        unsigned int numStaticModels;
        cStaticModel_t* staticModelList;
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
        uint16_t* leafbrushes;
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
        uint16_t unknown0;
        cbrush_t* brushes;
        int numClusters;
        int clusterBytes;
        char* visibility;
        int unknown2;
        MapEnts* mapEnts;
        cbrush_t* box_brush;
        char unknown1[74];
        uint16_t dynEntCount[4];
        DynEntityDef* dynEntDefList[2];
        DynEntityClient* dynEntClientList[2];
        DynEntityServer* dynEntServerList[2];
        DynEntityColl* dynEntCollList[4];
        int num_constraints;
        DynEntityPhysConstraint* constraints;
        int unknown3;
    };

    struct ComPrimaryLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        unsigned char exponent;
        unsigned char unused;
        vec3_t color;
        vec3_t dir;
        vec3_t origin;
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        float cosHalfFovExpanded;
        float rotationLimit;
        float translationLimit;
        const char* defName;
    };

    struct ComWorldUnknownCell
    {
        char unknown[32];
        unsigned char* data;
    };

    struct ComWorld
    {
        const char* name;
        int isInUse;
        int unknown0;
        unsigned int primaryLightCount;
        ComPrimaryLight* primaryLights;
        char unknown1[16];
        unsigned int unknownCellCount;
        ComWorldUnknownCell* unknownCells;
    };

    struct pathnode_t;
    struct pathnode_tree_t;

    struct pathnode_tree_nodes_t
    {
        int nodeCount;
        pathnode_t** nodes;
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
        int unknown0;
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
        ScriptString targetname;
        ScriptString script_linkName;
        ScriptString script_noteworthy;
        ScriptString target;
        ScriptString animscript;
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

    // QOS PC/Wii loaders confirm this byte layout. Semantic field names are inherited from matching IW3/T4 path data.
    struct PathData
    {
        unsigned int nodeCount;
        pathnode_t* nodes;
        pathbasenode_t* basenodes;
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
        raw_byte* data;
        void* layerVb;
    };

    struct GfxLight
    {
        unsigned char type;
        unsigned char canUseShadowMap;
        char unused[2];
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
        unsigned int spotShadowIndex;
        unsigned int unknown;
        GfxLightDef* def;
    };

    struct GfxReflectionProbe
    {
        float origin[3];
        GfxImage* reflectionImage;
    };

    struct srfTriangles_t
    {
        int vertexLayerData;
        int firstVertex;
        unsigned short vertexCount;
        unsigned short triCount;
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

    struct GfxStaticModelDrawInst
    {
        char unknown[56];
        XModel* model;
        unsigned int unknown2;
    };

    struct GfxStaticModelInst
    {
        float mins[3];
        float maxs[3];
        GfxColor groundLighting;
        unsigned int unknown;
    };

    struct GfxAabbTreeChildren;

    struct GfxAabbTree
    {
        char unknown[32];
        unsigned int smodelIndexCount;
        raw_uint* smodelIndexes;
        unsigned int childCount;
        GfxAabbTreeChildren* children;
    };

    struct GfxAabbTreeChildren
    {
        GfxAabbTree child;
    };

    struct GfxCell;

    struct GfxPortal
    {
        char unknown[32];
        GfxCell* cell;
        vec3_t* vertices;
        unsigned char vertexCount;
        char unknown2[27];
    };

    struct GfxCell
    {
        float mins[3];
        float maxs[3];
        GfxAabbTree* aabbTree;
        int portalCount;
        GfxPortal* portals;
        int cullGroupCount;
        int* cullGroups;
        unsigned char reflectionProbeCount;
        char pad[3];
        raw_byte* reflectionProbes;
    };

    struct GfxLightmapArray
    {
        GfxImage* primary;
        GfxImage* secondary;
    };

    struct GfxLightGridEntry
    {
        unsigned short colorsIndex;
        unsigned char primaryLightIndex;
        unsigned char needsTrace;
    };

    struct GfxLightGridColors
    {
        unsigned char rgb[56][3];
    };

    struct GfxLightGrid
    {
        unsigned char hasLightRegions;
        unsigned char pad[3];
        unsigned short mins[3];
        unsigned short maxs[3];
        unsigned int rowAxis;
        unsigned int colAxis;
        unsigned short* rowDataStart;
        unsigned int rawRowDataSize;
        raw_byte* rawRowData;
        unsigned int entryCount;
        GfxLightGridEntry* entries;
        unsigned int colorCount;
        GfxLightGridColors* colors;
    };

    struct GfxBrushModel
    {
        char unknown[48];
        unsigned int surfaceCount;
        unsigned int startSurfIndex;
        unsigned int unknown2;
    };

    struct MaterialMemory
    {
        Material* material;
        int memory;
    };

    struct sunflare_t
    {
        char hasValidData;
        char unknown0[3];
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

    struct GfxDrawSurf
    {
        unsigned int packed[2];
    };

    struct GfxSceneDynModel
    {
        char unknown[8];
    };

    struct GfxSceneDynBrush
    {
        char unknown[8];
    };

    struct GfxDynEntCellRef
    {
        unsigned short dynEntIndex;
        unsigned short cellIndex;
        unsigned short nextIndex;
    };

    struct GfxShadowGeometry
    {
        unsigned short surfaceCount;
        unsigned short smodelCount;
        unsigned short* sortedSurfIndex;
        unsigned short* smodelIndex;
    };

    struct GfxLightRegion
    {
        char unknown[24];
    };

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        unsigned int planeCount;
        cplane_s* planes;
        unsigned int nodeCount;
        unsigned short* nodes;
        unsigned int indexCount;
        unsigned short* indices;
        unsigned int surfaceCount;
        GfxSurface* surfaces;
        char unknown0[20];
        unsigned int skySurfCount;
        int* skyStartSurfs;
        GfxImage* skyImage;
        unsigned char skySamplerState;
        char unknown1[3];
        const char* skyBoxModel;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        char unknown2[128];
        GfxLight* sunLight;
        char unknown3[12];
        unsigned int sunPrimaryLightIndex;
        unsigned int primaryLightCount;
        char unknown4[4];
        unsigned int cullGroupCount;
        unsigned int reflectionProbeCount;
        GfxReflectionProbe* reflectionProbes;
        GfxCullGroup* cullGroups;
        unsigned int smodelCount;
        GfxStaticModelDrawInst* smodelDrawInsts;
        GfxStaticModelInst* smodelInsts;
        unsigned int cellCount;
        unsigned int cellBitsCount;
        GfxCell* cells;
        unsigned int lightmapCount;
        GfxLightmapArray* lightmaps;
        GfxLightGrid lightGrid;
        unsigned int modelCount;
        GfxBrushModel* models;
        char unknown5[28];
        unsigned int materialMemoryCount;
        MaterialMemory* materialMemory;
        sunflare_t sun;
        float outdoorLookupMatrix[4][4];
        GfxImage* outdoorImage;
        unsigned int dynEntDataSize0;
        unsigned int dynEntDataSize1;
        unsigned int sceneDynModelCount;
        unsigned int sceneDynBrushCount;
        raw_byte* smodelVisData0;
        raw_byte* smodelVisData1;
        raw_byte* smodelVisData2;
        raw_byte* smodelVisData3;
        raw_byte* surfaceVisData0;
        raw_byte* surfaceVisData1;
        raw_byte* surfaceVisData2;
        raw_byte* surfaceVisData3;
        GfxDrawSurf* surfaceMaterials;
        raw_uint* surfaceCastsSunShadow;
        raw_uint* cellCasterBits;
        raw_byte* cellHasSunLitSurfsBits;
        raw_byte* dynEntVisData0_0;
        raw_byte* dynEntVisData0_1;
        raw_byte* dynEntVisData0_2;
        raw_byte* dynEntVisData0_3;
        raw_byte* dynEntVisData1_0;
        raw_byte* dynEntVisData1_1;
        raw_byte* dynEntVisData1_2;
        raw_byte* dynEntVisData1_3;
        raw_byte* dynEntCellBits0;
        raw_byte* dynEntCellBits1;
        unsigned short* dynEntSortList0;
        unsigned short* dynEntSortList1;
        GfxSceneDynModel* sceneDynModel;
        GfxSceneDynBrush* sceneDynBrush;
        GfxDynEntCellRef* dynEntCellRefs;
        raw_uint* dynEntUnknown;
        unsigned short* sortedSurfIndex;
        raw_uint* primaryLightEntityShadowVis;
        raw_uint* primaryLightDynEntShadowVis0;
        raw_uint* primaryLightDynEntShadowVis1;
        GfxShadowGeometry* shadowGeom;
        unsigned int lightRegionCount;
        GfxLightRegion* lightRegion;
        Material* unknownMaterial;
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
        unsigned short letter;
        char x0;
        char y0;
        unsigned char dx;
        unsigned char pixelWidth;
        unsigned char pixelHeight;
        unsigned char unknown0;
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

    struct menuDef_t;

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
        int dynamicFlags[4]; // QoS loader confirms four entries; IW3/T4 only use one.
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

    enum operationEnum
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
        OP_SIN = 0x17,
        OP_FIRSTFUNCTIONCALL = OP_SIN,
        OP_COS = 0x18,
        OP_MIN = 0x19,
        OP_MAX = 0x1A,
        OP_MILLISECONDS = 0x1B,
        OP_DVARINT = 0x1C,
        OP_DVARBOOL = 0x1D,
        OP_DVARFLOAT = 0x1E,
        OP_DVARSTRING = 0x1F,
        OP_STAT = 0x20,
        OP_UIACTIVE = 0x21,
        OP_FLASHBANGED = 0x22,
        OP_SCOPED = 0x23,
        OP_SCOREBOARDVISIBLE = 0x24,
        OP_INKILLCAM = 0x25,
        OP_PLAYERFIELD = 0x26,
        OP_SELECTINGLOCATION = 0x27,
        OP_TEAMFIELD = 0x28,
        OP_OTHERTEAMFIELD = 0x29,
        OP_MARINESFIELD = 0x2A,
        OP_OPFORFIELD = 0x2B,
        OP_MENUISOPEN = 0x2C,
        OP_WRITINGDATA = 0x2D,
        OP_INLOBBY = 0x2E,
        OP_INPRIVATEPARTY = 0x2F,
        OP_PRIVATEPARTYHOST = 0x30,
        OP_PRIVATEPARTYHOSTINLOBBY = 0x31,
        OP_ALONEINPARTY = 0x32,
        OP_ADSJAVELIN = 0x33,
        OP_WEAPLOCKBLINK = 0x34,
        OP_WEAPATTACKTOP = 0x35,
        OP_WEAPATTACKDIRECT = 0x36,
        OP_SECONDSASTIME = 0x37,
        OP_TABLELOOKUP = 0x38,
        OP_LOCALIZESTRING = 0x39,
        OP_LOCALVARINT = 0x3A,
        OP_LOCALVARBOOL = 0x3B,
        OP_LOCALVARFLOAT = 0x3C,
        OP_LOCALVARSTRING = 0x3D,
        OP_TIMELEFT = 0x3E,
        OP_SECONDSASCOUNTDOWN = 0x3F,
        OP_TOINT = 0x40,
        OP_TOSTRING = 0x41,
        OP_TOFLOAT = 0x42,
        OP_GAMETYPENAME = 0x43,
        OP_GAMETYPE = 0x44,
        OP_GAMETYPEDESCRIPTION = 0x45,
        OP_SCORE = 0x46,
        OP_FRIENDSONLINE = 0x47,
        OP_FOLLOWING = 0x48,
        OP_STATRANGEBITSSET = 0x49,
        NUM_OPERATORS = 0x4A,
    };

    struct Operand
    {
        expDataType dataType;
        operandInternalDataUnion internals;
    };

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

    struct expressionEntry
    {
        expressionEntryType type;
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
        int startPos[4];
        int endPos[4];
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
        ITEM_TYPE_GAME_MESSAGE_WINDOW = 0x13,
    };

    struct itemDef_s
    {
        windowDef_t window;
        rectDef_s textRect[4];
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
        int textSavegameInfo; // qos-xport name; loader only confirms this as an int at offset 312.
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
        int feeder; // qos-xport identifies this as feeder; structurally equivalent to IW3/T4 special.
        int cursorPos[4];
        itemDefData_t typeData;
        int imageTrack;
        statement_s visibleExp;
        statement_s textExp;
        statement_s materialExp;
        statement_s rectXExp;
        statement_s rectYExp;
        statement_s rectWExp;
        statement_s rectHExp;
        statement_s unknownStatement0; // Loader confirms one additional statement at offset 460.
    };

    struct menuDef_t
    {
        windowDef_t window;
        const char* font;
        int fullScreen;
        int itemCount;
        int fontIndex;
        int cursorItem[4];
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
        const char* name;
        const char* displayName;
        const char* overlayName;
        XModel* unknownModel12;
        XModel* unknownModel16;
        const char* xanimNames[35];
        const char* modeName;
        char unknownData164[32];
        const char* unknownString196;
        char unknownData200[4];
        FxEffectDef* unknownFx204;
        FxEffectDef* unknownFx208;
        SndAliasCustom unknownSoundAliases[53];
        SndAliasCustom* bounceSound;
        FxEffectDef* unknownFx428;
        FxEffectDef* unknownFx432;
        FxEffectDef* unknownFx436;
        FxEffectDef* unknownFx440;
        Material* unknownMaterial444;
        Material* unknownMaterial448;
        char unknownData452[152];
        XModel* unknownModels604[3];
        const char* unknownString616;
        const char* unknownString620;
        XModel* unknownModel624;
        Material* unknownMaterial628;
        char unknownData632[4];
        Material* unknownMaterial636;
        char unknownData640[12];
        const char* unknownString652;
        char unknownData656[4];
        const char* unknownString660;
        char unknownData664[16];
        const char* unknownString680;
        char unknownData684[280];
        Material* unknownMaterial964;
        char unknownData968[24];
        const char* unknownString992;
        char unknownData996[224];
        const char* unknownString1220;
        char unknownData1224[40];
        Material* unknownMaterial1264;
        char unknownData1268[8];
        Material* unknownMaterial1276;
        char unknownData1280[20];
        const char* unknownString1300;
        char unknownData1304[4];
        const char* unknownString1308;
        char unknownData1312[4];
        XModel* unknownModels1316[16];
        const char* unknownStrings1380[16];
        char unknownData1444[80];
        XModel* unknownModel1524;
        char unknownData1528[4];
        FxEffectDef* unknownFx1532;
        char unknownData1536[4];
        FxEffectDef* unknownFx1540;
        SndAliasCustom unknownSound1544;
        SndAliasCustom unknownSound1548;
        char unknownData1552[32];
        FxEffectDef* unknownFx1584;
        FxEffectDef* unknownFx1588;
        SndAliasCustom unknownSound1592;
        char unknownData1596[240];
        FxEffectDef* unknownFx1836;
        char unknownData1840[24];
        FxEffectDef* unknownFx1864;
        SndAliasCustom unknownSound1868;
        char unknownData1872[208];
        // Offset 2080..2108: names/counts inferred from IW3/T4 accuracy graph layout and QoS loader ordering.
        const char* aiVsAiAccuracyGraphName;
        const char* aiVsPlayerAccuracyGraphName;
        vec2_t* aiVsAiAccuracyGraphKnots;
        vec2_t* aiVsPlayerAccuracyGraphKnots;
        vec2_t* originalAiVsAiAccuracyGraphKnots;
        vec2_t* originalAiVsPlayerAccuracyGraphKnots;
        int aiVsAiAccuracyGraphKnotCount;
        int aiVsPlayerAccuracyGraphKnotCount;
        char unknownData2112[80];
        const char* unknownString2192;
        const char* unknownString2196;
        char unknownData2200[16];
        const char* unknownString2216;
        char unknownData2220[112];
        const char* unknownString2332;
        const char* unknownString2336;
        char unknownData2340[424];
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
        unsigned short* inds;
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

    struct DestructibleModel
    {
        XModel* model;
        float breakHealth;
        FxEffectDef* breakEffect;
        SndAliasCustom breakSound;
        const char* breakNotify;
        XModel* spawnModel[3];
    };

    struct DestructiblePiece
    {
        DestructibleModel dmodels[5];
        const char* attachBone;
        ScriptString attachBoneHash;
        unsigned char parentPiece;
        unsigned char unused[1];
        float parentDamagePercent;
        float bulletDamageScale;
        float explosiveDamageScale;
        PhysConstraints* physConstraints;
        int health;
        FxEffectDef* burnEffect;
        SndAliasCustom burnSound;
    };

    struct DestructibleDef
    {
        const char* name;
        int unknown0;
        int numPieces;
        DestructiblePiece* pieces;
    };

    struct FxImpactEntry
    {
        FxEffectDef* nonflesh[30];
        FxEffectDef* flesh[4];
    };

    struct FxImpactTable
    {
        const char* name;
        FxImpactEntry* table;
    };

    struct AiType
    {
        const char* name;
    };

    struct MpType
    {
        const char* name;
    };

    struct Character
    {
        const char* name;
    };

    struct XModelAlias
    {
        const char* name;
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

    struct xmlAttribute
    {
        const char* name;
        const char* value;
        int unknown0;
    };

    struct xmlNode;

    struct xmlNodeList
    {
        int nodeCount;
        xmlNode** nodes;
    };

    struct xmlAttribList
    {
        int unknown0;
        xmlAttribute* attributes[8];
    };

    struct xmlNodeComplex
    {
        const char* name;
        xmlAttribList attribList;
        xmlNodeList childList;
    };

    struct xmlNode
    {
        const char* name;
        int unknown0;
        xmlNodeComplex* complex;
    };

    struct XmlTree
    {
        xmlNodeList nodeList;
        const char* name;
    };

    struct AnimReference
    {
        const char* unknownString0;
        ScriptString unknownScriptString0;
        char unknown0[6];
        XAnimParts* animation;
    };

    struct SceneAnimation
    {
        const char* name;
        unsigned int animReferenceCount;
        unsigned int unknown0;
        AnimReference* animReferences;
    };

    struct CutsceneModelAttachment
    {
        const char* unknownString0;
        const char* unknownString1;
    };

    struct CutsceneEntityDef
    {
        const char* unknownString0;
        char unknown0[8];
        const char* unknownString1;
        char unknown1[2];
        ScriptString unknownScriptString0;
        unsigned int modelAttachmentCount;
        CutsceneModelAttachment* modelAttachments;
    };

    struct CutsceneSectionDef
    {
        unsigned int entityCount;
        CutsceneEntityDef* entities;
        unsigned int sceneAnimationCount;
        SceneAnimation** sceneAnimations;
    };

    struct Cutscene
    {
        const char* name;
        char unknown0[12];
        unsigned int entityCount;
        CutsceneEntityDef* entities;
        unsigned int sectionCount;
        CutsceneSectionDef* sections;
    };

    struct CustomCameraDef
    {
        const char* name;
        int unknown4;
        const char* unknownString0;
        const char* unknownString1;
        char unknown16[100];
    };

    struct CustomCamera
    {
        const char* name;
        int cameraCount;
        CustomCameraDef* cameras;
    };

#ifndef __zonecodegenerator
}
#endif

#endif

// EOF
