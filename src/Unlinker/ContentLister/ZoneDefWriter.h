#pragma once
#include "Utils/FileAPI.h"
#include "Zone/Zone.h"

class AbstractZoneDefWriter
{
protected:
    Zone* m_zone;
    FileAPI::IFile* m_file;

    static const std::string META_DATA_KEY_GAME;

    void EmptyLine() const;
    void WriteComment(const std::string& comment) const;
    void WriteMetaData(const std::string& metaDataKey, const std::string& metaDataValue) const;
    void WriteEntry(const std::string& entryKey, const std::string& entryValue) const;
    void WriteContent() const;

    AbstractZoneDefWriter(Zone* zone, FileAPI::IFile* file);

public:
    virtual void WriteZoneDef() = 0;
};

class IZoneDefWriter
{
public:
    virtual bool CanHandleZone(Zone* zone) const = 0;
    virtual void WriteZoneDef(Zone* zone, FileAPI::IFile* file) const = 0;
};