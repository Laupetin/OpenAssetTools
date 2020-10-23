#include "ObjLoaderIW4.h"
#include "Game/IW4/GameIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"
#include "Image/Texture.h"
#include "Image/IwiLoader.h"

namespace IW4
{
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
        // TODO: Load Texture from LoadDef here
    }

    void ObjLoader::LoadImageFromIwi(GfxImage* image, ISearchPath* searchPath, Zone* zone)
    {
        Texture* loadedTexture = nullptr;
        IwiLoader loader(zone->GetMemory());

        const std::string imageFileName = "images/" + std::string(image->name) + ".iwi";
        auto* filePathImage = searchPath->Open(imageFileName);

        if (filePathImage != nullptr)
        {
            loadedTexture = loader.LoadIwi(filePathImage);

            filePathImage->Close();
            delete filePathImage;
        }

        if (loadedTexture != nullptr)
        {
            image->texture.texture = loadedTexture;
            image->cardMemory.platform[0] = 0;

            const int textureMipCount = loadedTexture->GetMipMapCount();
            for (int mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
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
}
