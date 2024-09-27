#include "ObjLoaderIW3.h"

#include "AssetLoaders/AssetLoaderGfxImage.h"
#include "AssetLoaders/AssetLoaderLocalizeEntry.h"
#include "AssetLoaders/AssetLoaderRawFile.h"
#include "AssetLoading/AssetLoadingManager.h"
#include "Game/IW3/GameIW3.h"
#include "ObjLoading.h"

using namespace IW3;

ObjLoader::ObjLoader()
{
#define REGISTER_ASSET_LOADER(t)                                                                                                                               \
    {                                                                                                                                                          \
        auto l = std::make_unique<t>();                                                                                                                        \
        m_asset_loaders_by_type[l->GetHandlingAssetType()] = std::move(l);                                                                                     \
    }

    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetPhysPreset>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetXAnim>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetXModel>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetMaterial>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetTechniqueSet>)
    REGISTER_ASSET_LOADER(AssetLoaderGfxImage)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetSound>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetSoundCurve>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetLoadedSound>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetClipMap>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetClipMapPvs>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetComWorld>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetGameWorldSp>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetGameWorldMp>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetMapEnts>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetGfxWorld>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetLightDef>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetFont>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetMenuList>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetMenu>)
    REGISTER_ASSET_LOADER(AssetLoaderLocalizeEntry)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetWeapon>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetSoundDriverGlobals>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetFx>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetImpactFx>)
    REGISTER_ASSET_LOADER(AssetLoaderRawFile)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetStringTable>)

#undef REGISTER_ASSET_LOADER
}

bool ObjLoader::SupportsZone(const Zone& zone) const
{
    return zone.m_game == &g_GameIW3;
}

bool ObjLoader::IsMpZone(const Zone& zone)
{
    return zone.m_name.compare(0, 3, "mp_") == 0 || zone.m_name.compare(zone.m_name.length() - 3, 3, "_mp") == 0;
}

bool ObjLoader::IsZmZone(const Zone& zone)
{
    return zone.m_name.compare(0, 3, "zm_") == 0 || zone.m_name.compare(zone.m_name.length() - 3, 3, "_zm") == 0;
}

void ObjLoader::LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const {}

void ObjLoader::UnloadContainersOfZone(Zone& zone) const {}

bool ObjLoader::LoadAssetForZone(AssetLoadingContext& context, const asset_type_t assetType, const std::string& assetName) const
{
    AssetLoadingManager assetLoadingManager(m_asset_loaders_by_type, context);
    return assetLoadingManager.LoadAssetFromLoader(assetType, assetName);
}

void ObjLoader::FinalizeAssetsForZone(AssetLoadingContext& context) const
{
    for (const auto& [type, loader] : m_asset_loaders_by_type)
        loader->FinalizeAssetsForZone(context);
}
