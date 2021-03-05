#pragma once

#include "IAssetDumper.h"

#include <fstream>
#include <filesystem>
#include <iostream>

template<class T>
class AbstractFileDumper : public IAssetDumper<T>
{
protected:
    virtual bool ShouldDump(XAssetInfo<T>* asset) = 0;
    virtual std::string GetFileNameForAsset(Zone* zone, XAssetInfo<T>* asset) = 0;
    virtual void DumpAsset(AssetDumpingContext& context, XAssetInfo<T>* asset, std::ostream& stream) = 0;

public:
    void DumpPool(AssetDumpingContext& context, AssetPool<T>* pool) override
    {
        for(auto assetInfo : *pool)
        {
            if(assetInfo->m_name[0] == ','
                || !ShouldDump(assetInfo))
            {
                continue;
            }

            std::filesystem::path assetFilePath(context.m_base_path);
            assetFilePath.append(GetFileNameForAsset(context.m_zone, assetInfo));

            auto assetFileFolder(assetFilePath);
            assetFileFolder.replace_filename("");
            create_directories(assetFileFolder);

            std::ofstream file(assetFilePath, std::fstream::out | std::fstream::binary);
            if(file.is_open())
            {
                DumpAsset(context, assetInfo, file);

                file.close();
            }
            else
            {
                std::cout << "Failed to open file '" << assetFilePath.string() << "' to dump asset '" << assetInfo->m_name.c_str() << "'\n";
            }
        }
    }
};
