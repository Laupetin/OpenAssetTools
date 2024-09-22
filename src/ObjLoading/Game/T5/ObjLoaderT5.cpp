#include "ObjLoaderT5.h"

#include "AssetLoaders/AssetLoaderLocalizeEntry.h"
#include "AssetLoaders/AssetLoaderRawFile.h"
#include "AssetLoaders/AssetLoaderStringTable.h"
#include "AssetLoaders/AssetLoaderXModel.h"
#include "AssetLoading/AssetLoadingManager.h"
#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T5/GameT5.h"
#include "Image/Dx9TextureLoader.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"
#include "Image/Texture.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"

using namespace T5;

ObjLoader::ObjLoader()
{
#define REGISTER_ASSET_LOADER(t)                                                                                                                               \
    {                                                                                                                                                          \
        auto l = std::make_unique<t>();                                                                                                                        \
        m_asset_loaders_by_type[l->GetHandlingAssetType()] = std::move(l);                                                                                     \
    }

    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetPhysPreset>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetPhysConstraints>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetDestructibleDef>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetXAnim>)
    REGISTER_ASSET_LOADER(AssetLoaderXModel)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetMaterial>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetTechniqueSet>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetImage>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetSoundBank>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetSoundPatch>)
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
    REGISTER_ASSET_LOADER(AssetLoaderStringTable)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetPackIndex>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetXGlobals>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetDDL>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetGlasses>)
    REGISTER_ASSET_LOADER(BasicAssetLoader<AssetEmblemSet>)

#undef REGISTER_ASSET_LOADER
}

bool ObjLoader::SupportsZone(Zone* zone) const
{
    return zone->m_game == &g_GameT5;
}

bool ObjLoader::IsMpZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "mp_") == 0 || zone->m_name.compare(zone->m_name.length() - 3, 3, "_mp") == 0;
}

bool ObjLoader::IsZmZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "zm_") == 0 || zone->m_name.compare(zone->m_name.length() - 3, 3, "_zm") == 0;
}

void ObjLoader::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const {}

void ObjLoader::UnloadContainersOfZone(Zone* zone) const {}

void ObjLoader::LoadImageFromLoadDef(GfxImage* image, Zone* zone)
{
    const auto* loadDef = image->texture.loadDef;
    Dx9TextureLoader textureLoader(zone->GetMemory());

    textureLoader.Width(image->width).Height(image->height).Depth(image->depth);

    if (loadDef->flags & iwi13::IMG_FLAG_VOLMAP)
        textureLoader.Type(TextureType::T_3D);
    else if (loadDef->flags & iwi13::IMG_FLAG_CUBEMAP)
        textureLoader.Type(TextureType::T_CUBE);
    else
        textureLoader.Type(TextureType::T_2D);

    textureLoader.Format(static_cast<oat::D3DFORMAT>(loadDef->format));
    textureLoader.HasMipMaps(!(loadDef->flags & iwi13::IMG_FLAG_NOMIPMAPS));
    Texture* loadedTexture = textureLoader.LoadTexture(image->texture.loadDef->data);

    if (loadedTexture != nullptr)
    {
        image->texture.texture = loadedTexture;
        image->loadedSize = 0;

        const auto textureMipCount = loadedTexture->GetMipMapCount();
        for (auto mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
            image->loadedSize += static_cast<int>(loadedTexture->GetSizeOfMipLevel(mipLevel) * loadedTexture->GetFaceCount());
    }
}

void ObjLoader::LoadImageFromIwi(GfxImage* image, ISearchPath* searchPath, Zone* zone)
{
    Texture* loadedTexture = nullptr;
    IwiLoader loader(zone->GetMemory());

    const auto imageFileName = "images/" + std::string(image->name) + ".iwi";

    {
        const auto filePathImage = searchPath->Open(imageFileName);
        if (filePathImage.IsOpen())
        {
            loadedTexture = loader.LoadIwi(*filePathImage.m_stream);
        }
    }

    if (loadedTexture != nullptr)
    {
        image->texture.texture = loadedTexture;
        image->loadedSize = 0;

        const auto textureMipCount = loadedTexture->GetMipMapCount();
        for (auto mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
            image->loadedSize += static_cast<int>(loadedTexture->GetSizeOfMipLevel(mipLevel) * loadedTexture->GetFaceCount());
    }
    else
    {
        printf("Could not find data for image \"%s\"\n", image->name);
    }
}

void ObjLoader::LoadImageData(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPool = dynamic_cast<GameAssetPoolT5*>(zone->m_pools.get());

    if (assetPool && assetPool->m_image != nullptr)
    {
        for (auto* imageEntry : *assetPool->m_image)
        {
            auto* image = imageEntry->Asset();

            if (image->loadedSize > 0)
            {
                continue;
            }

            // Do not load linked assets
            if (image->name && image->name[0] == ',')
            {
                continue;
            }

            if (image->texture.loadDef && image->texture.loadDef->resourceSize > 0)
            {
                LoadImageFromLoadDef(image, zone);
            }
            else
            {
                LoadImageFromIwi(image, searchPath, zone);
            }
        }
    }
}

void ObjLoader::LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) const
{
    LoadImageData(searchPath, zone);
}

bool ObjLoader::LoadAssetForZone(AssetLoadingContext* context, asset_type_t assetType, const std::string& assetName) const
{
    AssetLoadingManager assetLoadingManager(m_asset_loaders_by_type, *context);
    return assetLoadingManager.LoadAssetFromLoader(assetType, assetName);
}

void ObjLoader::FinalizeAssetsForZone(AssetLoadingContext* context) const
{
    for (const auto& [type, loader] : m_asset_loaders_by_type)
        loader->FinalizeAssetsForZone(context);
}
