#include "ObjLoaderT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"

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
    {
        printf("Loading ipak '%s' for zone '%s'\n", ipakName.c_str(), zone->m_name.c_str());
    }

    const std::string ipakFilename = ipakName + ".ipak";

    auto* file = searchPath->Open(ipakFilename);
    if(file && file->IsOpen())
    {
        IPak* ipak = new IPak(ipakFilename, file);

        if(ipak->Initialize())
        {
            IPak::Repository.AddContainer(ipak, zone);
        }
        else
        {
            delete ipak;
            file->Close();
            delete file;

            printf("Failed to load ipak '%s'!\n", ipakFilename.c_str());
        }
    }

    if(ipakName == "base")
    {
        LoadIPakForZone(searchPath, "mp", zone);
        LoadIPakForZone(searchPath, "so", zone);
    }
}

void ObjLoaderT6::LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());
    const int zoneNameHash = Com_HashKey(zone->m_name.c_str(), 64);

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

void ObjLoaderT6::LoadImageDataFromFile(T6::GfxImage* image, FileAPI::IFile* file, Zone* zone)
{
}

void ObjLoaderT6::LoadImageData(ISearchPath* searchPath, Zone* zone)
{
    auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone->GetPools());

    if (assetPoolT6->m_image != nullptr)
    {
        for (auto* imageEntry : *assetPoolT6->m_image)
        {
            auto* image = imageEntry->m_asset;

            if(image->texture.loadDef && image->texture.loadDef->resourceSize > 0)
            {
                continue;
            }

            std::string imageFileName = "images/" + std::string(image->name) + ".iwi";
            auto* filePathImage = searchPath->Open(imageFileName);
            if(filePathImage != nullptr && filePathImage->IsOpen())
            {
                LoadImageDataFromFile(image, filePathImage, zone);
                filePathImage->Close();
                delete filePathImage;
            }
            else if(image->streamedPartCount > 0)
            {
                for (auto* ipak : IPak::Repository)
                {
                    auto* ipakEntry = ipak->GetEntryData(image->hash, image->streamedParts[0].hash);

                    if (ipakEntry != nullptr && ipakEntry->IsOpen())
                    {
                        LoadImageDataFromFile(image, ipakEntry, zone);
                        ipakEntry->Close();
                        delete ipakEntry;
                    }
                }
            }
        }
    }
}

void ObjLoaderT6::LoadObjDataForZone(ISearchPath* searchPath, Zone* zone)
{
    LoadImageData(searchPath, zone);
}
