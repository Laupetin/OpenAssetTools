#pragma once

#include "IAssetDumper.h"
#include "Utils/FileAPI.h"
#include "Utils/PathUtils.h"

#include <cstdio>

template<class T>
class AbstractAssetDumper : public IAssetDumper<T>
{
protected:
    virtual bool ShouldDump(T* asset) = 0;
    virtual std::string GetFileNameForAsset(Zone* zone, T* asset) = 0;
    virtual void DumpAsset(Zone* zone, T* asset, FileAPI::File* out) = 0;

public:
    void DumpPool(Zone* zone, AssetPool<T>* pool, const std::string& basePath) override
    {
        for(auto assetInfo : *pool)
        {
            T* asset = assetInfo->Asset();

            if(assetInfo->m_name[0] == ','
                || !ShouldDump(asset))
            {
                continue;
            }

            std::string assetFilePath = utils::Path::Combine(basePath, GetFileNameForAsset(zone, asset));

            FileAPI::DirectoryCreate(utils::Path::GetDirectory(assetFilePath));

            auto file = FileAPI::Open(assetFilePath, FileAPI::Mode::MODE_WRITE);

            if(file.IsOpen())
            {
                DumpAsset(zone, asset, &file);

                file.Close();
            }
            else
            {
                printf("Failed to open file '%s' to dump asset '%s'\n", assetFilePath.c_str(), assetInfo->m_name.c_str());
            }
        }
    }
};
