#pragma once

#include "Dumping/AssetDumpingContext.h"

class IObjWriter
{
public:
    IObjWriter() = default;
    virtual ~IObjWriter() = default;
    IObjWriter(const IObjWriter& other) = default;
    IObjWriter(IObjWriter&& other) noexcept = default;
    IObjWriter& operator=(const IObjWriter& other) = default;
    IObjWriter& operator=(IObjWriter&& other) noexcept = default;

    virtual bool DumpZone(AssetDumpingContext& assetDumpingContext) const = 0;

    static const IObjWriter* GetObjWriterForGame(GameId game);
};
