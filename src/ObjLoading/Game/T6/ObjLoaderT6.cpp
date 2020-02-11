#include "ObjLoaderT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"
#include "Image/Texture.h"
#include "Image/IwiLoader.h"

const int ObjLoaderT6::IPAK_READ_HASH = Com_HashKey("ipak_read", 64);
const int ObjLoaderT6::GLOBAL_HASH = Com_HashKey("GLOBAL", 64);

int ObjLoaderT6::Com_HashKey(const char* str, const int maxLen)
{
    if (str == nullptr)
        return 0;

    int hash = 0;
    for (int i = 0; i < maxLen; i++)
    {
        if (str[i] == '\0')
            break;

        hash += str[i] * (0x77 + i);
    }

    return hash ^ ((hash ^ (hash >> 10)) >> 10);
}

bool ObjLoaderT6::SupportsZone(Zone* zone)
{
    return zone->m_game == &g_GameT6;
}

void ObjLoaderT6::LoadIPakForZone(ISearchPath* searchPath, const std::string& ipakName, Zone* zone)
{
    if(ObjLoading::Configuration.Verbose)
        printf("Trying to load ipak '%s' for zone '%s'\n", ipakName.c_str(), zone->m_name.c_str());

    IPak* existingIPak = IPak::Repository.GetContainerByName(ipakName);
    if(existingIPak != nullptr)
    {
        if (ObjLoading::Configuration.Verbose)
            printf("Referencing loaded ipak '%s'.\n", ipakName.c_str());

        IPak::Repository.AddContainer(existingIPak, zone);
        return;
    }

    const std::string ipakFilename = ipakName + ".ipak";

    auto* file = searchPath->Open(ipakFilename);
    if(file && file->IsOpen())
    {
        IPak* ipak = new IPak(ipakFilename, file);

        if(ipak->Initialize())
        {
            IPak::Repository.AddContainer(ipak, zone);

            if (ObjLoading::Configuration.Verbose)
                printf("Found and loaded ipak '%s'.\n", ipakFilename.c_str());
        }
        else
        {
            delete ipak;
            file->Close();
            delete file;

            printf("Failed to load ipak '%s'!\n", ipakFilename.c_str());
        }
    }
}

bool ObjLoaderT6::IsMpZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "mp_") == 0
        || zone->m_name.compare(zone->m_name.length() - 3, 3, "_mp") == 0;
}

bool ObjLoaderT6::IsZmZone(Zone* zone)
{
    return zone->m_name.compare(0, 3, "zm_") == 0
        || zone->m_name.compare(zone->m_name.length() - 3, 3, "_zm") == 0;
}

void ObjLoaderT6::LoadCommonIPaks(ISearchPath* searchPath, Zone* zone)
{
    if(ObjLoading::Configuration.Verbose)
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

void ObjLoaderT6::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());
    const int zoneNameHash = Com_HashKey(zone->m_name.c_str(), 64);

    LoadCommonIPaks(searchPath, zone);

    if(assetPoolT6->m_key_value_pairs != nullptr)
    {
        for(auto* keyValuePairsEntry : *assetPoolT6->m_key_value_pairs)
        {
            auto* keyValuePairs = keyValuePairsEntry->m_asset;
            for(int variableIndex = 0; variableIndex < keyValuePairs->numVariables; variableIndex++)
            {
                T6::KeyValuePair* variable = &keyValuePairs->keyValuePairs[variableIndex];
                
                if(variable->namespaceHash == zoneNameHash && variable->keyHash == IPAK_READ_HASH)
                {
                    LoadIPakForZone(searchPath, variable->value, zone);
                }
            }
        }
    }
}

void ObjLoaderT6::UnloadContainersOfZone(Zone* zone)
{
    IPak::Repository.RemoveContainerReferences(zone);
}

void ObjLoaderT6::LoadImageFromLoadDef(T6::GfxImage* image, Zone* zone)
{
    // TODO: Load Texture from LoadDef here
}

void ObjLoaderT6::LoadImageFromIwi(T6::GfxImage* image, ISearchPath* searchPath, Zone* zone)
{
    Texture* loadedTexture = nullptr;
    IwiLoader loader(zone->GetMemory());

    if (image->streamedPartCount > 0)
    {
        for (auto* ipak : IPak::Repository)
        {
            auto* ipakStream = ipak->GetEntryStream(image->hash, image->streamedParts[0].hash);

            if (ipakStream != nullptr)
            {
                loadedTexture = loader.LoadIwi(ipakStream);

                ipakStream->Close();
                delete ipakStream;

                if (loadedTexture != nullptr)
                {
                    break;
                }
            }
        }
    }

    if(loadedTexture == nullptr)
    {
        const std::string imageFileName = "images/" + std::string(image->name) + ".iwi";
        auto* filePathImage = searchPath->Open(imageFileName);

        if (filePathImage != nullptr)
        {
            loadedTexture = loader.LoadIwi(filePathImage);

            filePathImage->Close();
            delete filePathImage;
        }
    }

    if(loadedTexture != nullptr)
    {
        image->texture.texture = loadedTexture;
        image->loadedSize = 0;

        const int textureMipCount = loadedTexture->GetMipMapCount();
        for(int mipLevel = 0; mipLevel < textureMipCount; mipLevel++)
            image->loadedSize += loadedTexture->GetSizeOfMipLevel(mipLevel);
    }
    else
    {
        printf("Could not find data for image \"%s\"\n", image->name);
    }
}

void ObjLoaderT6::LoadImageData(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());

    if (assetPoolT6 && assetPoolT6->m_image != nullptr)
    {
        for (auto* imageEntry : *assetPoolT6->m_image)
        {
            auto* image = imageEntry->m_asset;

            if(image->loadedSize > 0)
            {
                continue;
            }

            // Do not load linked assets
            if(image->name && image->name[0] == ',')
            {
                continue;
            }

            if(image->texture.loadDef && image->texture.loadDef->resourceSize > 0)
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

void ObjLoaderT6::LoadObjDataForZone(ISearchPath* searchPath, Zone* zone)
{
    LoadImageData(searchPath, zone);
}
