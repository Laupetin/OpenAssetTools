#pragma once

// clang-format off: Order of includes matters here

#include "Game/IAsset.h"

#include "QOS_Assets.h"

// clang-format on

namespace QOS
{
    enum SubAssetType
    {
        SUB_ASSET_TYPE_TECHNIQUE,
        SUB_ASSET_TYPE_VERTEX_DECL,
        SUB_ASSET_TYPE_VERTEX_SHADER,
        SUB_ASSET_TYPE_PIXEL_SHADER,
        SUB_ASSET_TYPE_ACCURACY_GRAPH,

        SUB_ASSET_TYPE_COUNT
    };

    struct ScriptStringList
    {
        int count;
        const char** strings;
    };

    struct XAsset
    {
        XAssetType type;
        XAssetHeader header;
    };

    struct XAssetList
    {
        ScriptStringList stringList;
        int assetCount;
        XAsset* assets;
    };

    using AssetXModelPieces = Asset<ASSET_TYPE_XMODELPIECES, XModelPieces>;
    using AssetPhysPreset = Asset<ASSET_TYPE_PHYSPRESET, PhysPreset>;
    using AssetPhysConstraints = Asset<ASSET_TYPE_PHYSCONSTRAINTS, PhysConstraints>;
    using AssetDestructibleDef = Asset<ASSET_TYPE_DESTRUCTIBLEDEF, DestructibleDef>;
    using AssetXAnim = Asset<ASSET_TYPE_XANIMPARTS, XAnimParts>;
    using AssetXModel = Asset<ASSET_TYPE_XMODEL, XModel>;
    using AssetMaterial = Asset<ASSET_TYPE_MATERIAL, Material>;
    using AssetTechniqueSet = Asset<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>;
    using AssetImage = Asset<ASSET_TYPE_IMAGE, GfxImage>;
    using AssetSound = Asset<ASSET_TYPE_SOUND, snd_alias_list_t>;
    using AssetSndCurve = Asset<ASSET_TYPE_SND_CURVE, SndCurve>;
    using AssetClipMapSp = Asset<ASSET_TYPE_CLIPMAP_SP, clipMap_t>;
    using AssetClipMapMp = Asset<ASSET_TYPE_CLIPMAP_MP, clipMap_t>;
    using AssetComWorld = Asset<ASSET_TYPE_COMWORLD, ComWorld>;
    using AssetGameWorldSp = Asset<ASSET_TYPE_GAMEWORLD_SP, GameWorldSp>;
    using AssetGameWorldMp = Asset<ASSET_TYPE_GAMEWORLD_MP, GameWorldMp>;
    using AssetMapEnts = Asset<ASSET_TYPE_MAP_ENTS, MapEnts>;
    using AssetGfxWorld = Asset<ASSET_TYPE_GFXWORLD, GfxWorld>;
    using AssetLightDef = Asset<ASSET_TYPE_LIGHT_DEF, GfxLightDef>;
    using AssetFont = Asset<ASSET_TYPE_FONT, Font_s>;
    using AssetMenuFile = Asset<ASSET_TYPE_MENUFILE, MenuList>;
    using AssetMenu = Asset<ASSET_TYPE_MENU, menuDef_t>;
    using AssetLocalize = Asset<ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry>;
    using AssetWeapon = Asset<ASSET_TYPE_WEAPON, WeaponDef>;
    using AssetSoundDriverGlobals = Asset<ASSET_TYPE_SNDDRIVER_GLOBALS, SndDriverGlobals>;
    using AssetFx = Asset<ASSET_TYPE_FX, FxEffectDef>;
    using AssetImpactFx = Asset<ASSET_TYPE_IMPACT_FX, FxImpactTable>;
    using AssetAiType = Asset<ASSET_TYPE_AITYPE, AiType>;
    using AssetMpType = Asset<ASSET_TYPE_MPTYPE, MpType>;
    using AssetCharacter = Asset<ASSET_TYPE_CHARACTER, Character>;
    using AssetXModelAlias = Asset<ASSET_TYPE_XMODELALIAS, XModelAlias>;
    using AssetRawFile = Asset<ASSET_TYPE_RAWFILE, RawFile>;
    using AssetStringTable = Asset<ASSET_TYPE_STRINGTABLE, StringTable>;
    using AssetXmlTree = Asset<ASSET_TYPE_XMLTREE, XmlTree>;
    using AssetSceneAnimation = Asset<ASSET_TYPE_SCENE_ANIMATION, SceneAnimation>;
    using AssetCutscene = Asset<ASSET_TYPE_CUTSCENE, Cutscene>;
    using AssetCustomCamera = Asset<ASSET_TYPE_CUSTOM_CAMERA, CustomCamera>;

    using SubAssetTechnique = SubAsset<SUB_ASSET_TYPE_TECHNIQUE, MaterialTechnique>;
    using SubAssetVertexDecl = SubAsset<SUB_ASSET_TYPE_VERTEX_DECL, MaterialVertexDeclaration>;
    using SubAssetVertexShader = SubAsset<SUB_ASSET_TYPE_VERTEX_SHADER, MaterialVertexShader>;
    using SubAssetPixelShader = SubAsset<SUB_ASSET_TYPE_PIXEL_SHADER, MaterialPixelShader>;
} // namespace QOS

DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetXModelPieces, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetPhysPreset, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetPhysConstraints, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetDestructibleDef, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetXAnim, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetXModel, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetMaterial, info.name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetTechniqueSet, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetImage, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetSound, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetSndCurve, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetClipMapSp, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetClipMapMp, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetComWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetGameWorldSp, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetGameWorldMp, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetMapEnts, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetGfxWorld, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetLightDef, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetFont, fontName);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetMenuFile, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetMenu, window.name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetLocalize, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetWeapon, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetSoundDriverGlobals, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetFx, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetImpactFx, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetAiType, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetMpType, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetCharacter, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetXModelAlias, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetRawFile, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetStringTable, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetXmlTree, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetSceneAnimation, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetCutscene, name);
DEFINE_ASSET_NAME_ACCESSOR(QOS::AssetCustomCamera, name);
