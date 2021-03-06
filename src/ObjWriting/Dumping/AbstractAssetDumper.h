#pragma once

#include "IAssetDumper.h"

#include <fstream>
#include <filesystem>
#include <iostream>

template<class T>
class AbstractAssetDumper : public IAssetDumper<T>
{
protected:
    virtual bool ShouldDump(XAssetInfo<T>* asset)
    {
        return true;
    }

    virtual bool CanDumpAsRaw()
    {
        return false;
    }

    virtual bool CanDumpAsGdtEntry()
    {
        return false;
    }

    virtual std::string GetFileNameForAsset(Zone* zone, XAssetInfo<T>* asset)
    {
        return asset->m_name;
    }

    virtual void DumpRaw(AssetDumpingContext& context, XAssetInfo<T>* asset, std::ostream& stream)
    {
        
    }

    virtual GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<T>* asset)
    {
        return GdtEntry();
    }

public:
    void DumpPool(AssetDumpingContext& context, AssetPool<T>* pool) override
    {
        if(context.m_gdt && CanDumpAsGdtEntry())
        {
            for (auto assetInfo : *pool)
            {
                if (assetInfo->m_name[0] == ','
                    || !ShouldDump(assetInfo))
                {
                    continue;
                }

                auto entry = DumpGdtEntry(context, assetInfo);
                context.m_gdt->WriteEntry(entry);
            }
        }
        else if(CanDumpAsRaw())
        {
            for (auto assetInfo : *pool)
            {
                if (assetInfo->m_name[0] == ','
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
                if (file.is_open())
                {
                    DumpRaw(context, assetInfo, file);

                    file.close();
                }
                else
                {
                    std::cout << "Failed to open file '" << assetFilePath.string() << "' to dump asset '" << assetInfo->m_name.c_str() << "'\n";
                }
            }
        }
    }
};
