#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Dumping/IAssetDumper.h"

#include <memory>
#include <vector>

class IObjWriter
{
public:
    IObjWriter() = default;
    virtual ~IObjWriter() = default;
    IObjWriter(const IObjWriter& other) = default;
    IObjWriter(IObjWriter&& other) noexcept = default;
    IObjWriter& operator=(const IObjWriter& other) = default;
    IObjWriter& operator=(IObjWriter&& other) noexcept = default;

    bool DumpZone(AssetDumpingContext& context);

    static IObjWriter* GetObjWriterForGame(GameId game);

protected:
    virtual void RegisterAssetDumpers(AssetDumpingContext& context) = 0;
    void RegisterAssetDumper(std::unique_ptr<IAssetDumper> dumper);

private:
    std::vector<std::unique_ptr<IAssetDumper>> m_asset_dumpers;
};
