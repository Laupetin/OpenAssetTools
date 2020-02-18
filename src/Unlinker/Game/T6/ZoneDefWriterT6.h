#pragma once

#include "ContentLister/ZoneDefWriter.h"

class ZoneDefWriterT6 final : public IZoneDefWriter
{
public:
    bool CanHandleZone(Zone* zone) const override;
    void WriteZoneDef(Zone* zone, FileAPI::IFile* file) const override;
};