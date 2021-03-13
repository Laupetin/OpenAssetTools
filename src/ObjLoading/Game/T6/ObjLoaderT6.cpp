#include "ObjLoaderT6.h"

#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"
#include "Image/Texture.h"
#include "Image/IwiLoader.h"
#include "Game/T6/CommonT6.h"

namespace T6
{
    const int ObjLoader::IPAK_READ_HASH = CommonT6::Com_HashKey("ipak_read", 64);
    const int ObjLoader::GLOBAL_HASH = CommonT6::Com_HashKey("GLOBAL", 64);

    bool ObjLoader::SupportsZone(Zone* zone) const
    {
        return zone->m_game == &g_GameT6;
    }

    void ObjLoader::LoadIPakForZone(ISearchPath* searchPath, const std::string& ipakName, Zone* zone)
    {
        if (ObjLoading::Configuration.Verbose)
            printf("Trying to load ipak '%s' for zone '%s'\n", ipakName.c_str(), zone->m_name.c_str());

        auto* existingIPak = IPak::Repository.GetContainerByName(ipakName);
        if (existingIPak != nullptr)
        {
            if (ObjLoading::Configuration.Verbose)
                printf("Referencing loaded ipak '%s'.\n", ipakName.c_str());

            IPak::Repository.AddContainerReference(existingIPak, zone);
            return;
        }

        const auto ipakFilename = ipakName + ".ipak";

        auto file = searchPath->Open(ipakFilename);
        if (file.IsOpen())
        {
            auto ipak = std::make_unique<IPak>(ipakFilename, std::move(file.m_stream));

            if (ipak->Initialize())
            {
                IPak::Repository.AddContainer(std::move(ipak), zone);

                if (ObjLoading::Configuration.Verbose)
                    printf("Found and loaded ipak '%s'.\n", ipakFilename.c_str());
            }
            else
            {
                printf("Failed to load ipak '%s'!\n", ipakFilename.c_str());
            }
        }
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

    void ObjLoader::LoadCommonIPaks(ISearchPath* searchPath, Zone* zone)
    {
        if (ObjLoading::Configuration.Verbose)
            printf("Loading common ipaks for zone \"%s\"\n", zone->m_name.c_str());

        LoadIPakForZone(searchPath, "base", zone);
        auto languagePrefixes = g_GameT6.GetLanguagePrefixes();
        for (const auto& languagePrefix : languagePrefixes)
        {
            LoadIPakForZone(searchPath, languagePrefix.m_prefix + "base", zone);
        }

        if (IsMpZone(zone))
        {
            if (ObjLoading::Configuration.Verbose)
                printf("Loading multiplayer ipaks for zone \"%s\"\n", zone->m_name.c_str());

            LoadIPakForZone(searchPath, "mp", zone);
            LoadIPakForZone(searchPath, "so", zone);
        }
        else if (IsZmZone(zone))
        {
            if (ObjLoading::Configuration.Verbose)
                printf("Loading zombie ipak for zone \"%s\"\n", zone->m_name.c_str());

            LoadIPakForZone(searchPath, "zm", zone);
        }
        else
        {
            if (ObjLoading::Configuration.Verbose)
                printf("Loading singleplayer ipak for zone \"%s\"\n", zone->m_name.c_str());

            LoadIPakForZone(searchPath, "sp", zone);
        }
    }

    void ObjLoader::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) const
    {
        auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->m_pools.get());
        const auto zoneNameHash = CommonT6::Com_HashKey(zone->m_name.c_str(), 64);

        LoadCommonIPaks(searchPath, zone);

        if (assetPoolT6->m_key_value_pairs != nullptr)
        {
            for (auto* keyValuePairsEntry : *assetPoolT6->m_key_value_pairs)
            {
                auto* keyValuePairs = keyValuePairsEntry->Asset();
                for (auto variableIndex = 0; variableIndex < keyValuePairs->numVariables; variableIndex++)
                {
                    auto* variable = &keyValuePairs->keyValuePairs[variableIndex];

                    if (variable->namespaceHash == zoneNameHash && variable->keyHash == IPAK_READ_HASH)
                    {
                        LoadIPakForZone(searchPath, variable->value, zone);
                    }
                }
            }
        }
    }

    void ObjLoader::UnloadContainersOfZone(Zone* zone) const
    {
        IPak::Repository.RemoveContainerReferences(zone);
    }

    void ObjLoader::LoadImageFromLoadDef(GfxImage* image, Zone* zone)
    {
        // TODO: Load Texture from LoadDef here
    }

    void ObjLoader::LoadImageFromIwi(GfxImage* image, ISearchPath* searchPath, Zone* zone)
    {
        Texture* loadedTexture = nullptr;
        IwiLoader loader(zone->GetMemory());

        if (image->streamedPartCount > 0)
        {
            for (auto* ipak : IPak::Repository)
            {
                auto ipakStream = ipak->GetEntryStream(image->hash, image->streamedParts[0].hash);

                if (ipakStream)
                {
                    loadedTexture = loader.LoadIwi(*ipakStream);

                    ipakStream->close();

                    if (loadedTexture != nullptr)
                    {
                        break;
                    }
                }
            }
        }

        if (loadedTexture == nullptr)
        {
            const auto imageFileName = "images/" + std::string(image->name) + ".iwi";

            {
                const auto filePathImage = searchPath->Open(imageFileName);
                if (filePathImage.IsOpen())
                {
                    loadedTexture = loader.LoadIwi(*filePathImage.m_stream);
                }
            }
        }

        if (loadedTexture != nullptr)
        {
            image->texture.texture = loadedTexture;
            image->loadedSize = 0;

            const auto textureMipCount = loadedTexture->GetMipMapCount();
            for (auto mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
                image->loadedSize += loadedTexture->GetSizeOfMipLevel(mipLevel) * loadedTexture->GetFaceCount();
        }
        else
        {
            printf("Could not find data for image \"%s\"\n", image->name);
        }
    }

    void ObjLoader::LoadImageData(ISearchPath* searchPath, Zone* zone)
    {
        auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->m_pools.get());

        if (assetPoolT6 && assetPoolT6->m_image != nullptr)
        {
            for (auto* imageEntry : *assetPoolT6->m_image)
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
}
