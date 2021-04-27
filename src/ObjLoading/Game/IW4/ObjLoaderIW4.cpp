#include "ObjLoaderIW4.h"

#include "Game/IW4/GameIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"
#include "AssetLoaders/AssetLoaderRawFile.h"
#include "AssetLoading/AssetLoadingManager.h"
#include "Image/Dx9TextureLoader.h"
#include "Image/Texture.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"

using namespace IW4;

ObjLoader::ObjLoader()
{
#define REGISTER_ASSET_LOADER(t) {auto l = std::make_unique<t>(); m_asset_loaders_by_type[l->GetHandlingAssetType()] = std::move(l);}
#define BASIC_LOADER(assetType, assetClass) BasicAssetLoader<assetType, assetClass>

    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_PHYSPRESET, PhysPreset))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_PHYSCOLLMAP, PhysCollmap))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_XANIMPARTS, XAnimParts))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_XMODEL_SURFS, XModelSurfs))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_XMODEL, XModel))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_MATERIAL, Material))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_PIXELSHADER, MaterialPixelShader))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_VERTEXSHADER, MaterialVertexShader))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_VERTEXDECL, MaterialVertexDeclaration))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_IMAGE, GfxImage))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_SOUND, snd_alias_list_t))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_SOUND_CURVE, SndCurve))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_LOADED_SOUND, LoadedSound))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_CLIPMAP_SP, clipMap_t))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_CLIPMAP_MP, clipMap_t))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_COMWORLD, ComWorld))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_GAMEWORLD_SP, GameWorldSp))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_GAMEWORLD_MP, GameWorldMp))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_MAP_ENTS, MapEnts))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_FXWORLD, FxWorld))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_GFXWORLD, GfxWorld))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_LIGHT_DEF, GfxLightDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_FONT, Font_s))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_MENULIST, MenuList))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_MENU, menuDef_t))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_LOCALIZE_ENTRY, LocalizeEntry))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_WEAPON, WeaponCompleteDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_FX, FxEffectDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_IMPACT_FX, FxImpactTable))
    REGISTER_ASSET_LOADER(AssetLoaderRawFile)
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_STRINGTABLE, StringTable))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_LEADERBOARD, LeaderboardDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_STRUCTURED_DATA_DEF, StructuredDataDefSet))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_TRACER, TracerDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_VEHICLE, VehicleDef))
    REGISTER_ASSET_LOADER(BASIC_LOADER(ASSET_TYPE_ADDON_MAP_ENTS, AddonMapEnts))

#undef BASIC_LOADER
#undef REGISTER_ASSET_LOADER
}

bool ObjLoader::SupportsZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW4;
}

bool ObjLoader::IsMpZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "mp_") == 0
        || zone->m_name.compare(zone->m_name.length() - 3, 3, "_mp") == 0;
}

bool ObjLoader::IsZmZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "zm_") == 0
        || zone->m_name.compare(zone->m_name.length() - 3, 3, "_zm") == 0;
}

void ObjLoader::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const
{
}

void ObjLoader::UnloadContainersOfZone(Zone* zone) const
{
}

void ObjLoader::LoadImageFromLoadDef(GfxImage* image, Zone* zone)
{
    const auto* loadDef = image->texture.loadDef;
    Dx9TextureLoader textureLoader(zone->GetMemory());

    textureLoader.Width(image->width).Height(image->height).Depth(image->depth);

    if ((loadDef->flags & iwi8::IMG_FLAG_MAPTYPE_MASK) == iwi8::IMG_FLAG_MAPTYPE_3D)
        textureLoader.Type(TextureType::T_3D);
    else if ((loadDef->flags & iwi8::IMG_FLAG_MAPTYPE_MASK) == iwi8::IMG_FLAG_MAPTYPE_CUBE)
        textureLoader.Type(TextureType::T_CUBE);
    else
        textureLoader.Type(TextureType::T_2D);

    textureLoader.Format(static_cast<D3DFORMAT>(loadDef->format));
    textureLoader.HasMipMaps(!(loadDef->flags & iwi8::IMG_FLAG_NOMIPMAPS));
    Texture* loadedTexture = textureLoader.LoadTexture(image->texture.loadDef->data);

    if (loadedTexture != nullptr)
    {
        image->texture.texture = loadedTexture;
        image->cardMemory.platform[0] = 0;

        const auto textureMipCount = loadedTexture->GetMipMapCount();
        for (auto mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
            image->cardMemory.platform[0] += static_cast<int>(loadedTexture->GetSizeOfMipLevel(mipLevel) * loadedTexture->GetFaceCount());
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
        image->cardMemory.platform[0] = 0;

        const auto textureMipCount = loadedTexture->GetMipMapCount();
        for (auto mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
            image->cardMemory.platform[0] += static_cast<int>(loadedTexture->GetSizeOfMipLevel(mipLevel) * loadedTexture->GetFaceCount());
    }
    else
    {
        printf("Could not find data for image \"%s\"\n", image->name);
    }
}

void ObjLoader::LoadImageData(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPool = dynamic_cast<GameAssetPoolIW4*>(zone->m_pools.get());

    if (assetPool && assetPool->m_image != nullptr)
    {
        for (auto* imageEntry : *assetPool->m_image)
        {
            auto* image = imageEntry->Asset();

            if (image->cardMemory.platform[0] > 0)
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