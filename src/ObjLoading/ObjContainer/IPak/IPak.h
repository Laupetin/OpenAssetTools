#pragma once

#include "Utils/FileAPI.h"
#include "ObjContainer/ObjContainerReferenceable.h"
#include "ObjContainer/ObjContainerRepository.h"
#include "Zone/Zone.h"

class IPak final : public ObjContainerReferenceable
{
    class Impl;
    Impl* m_impl;

public:
    typedef uint32_t Hash;

    static ObjContainerRepository<IPak, Zone> Repository;

    IPak(std::string path, FileAPI::IFile* file);
    ~IPak();

    std::string GetName() override;

    bool Initialize() const;
    FileAPI::IFile* GetEntryStream(Hash nameHash, Hash dataHash) const;

    static Hash HashString(const std::string& str);
    static Hash HashData(const void* data, size_t dataSize);
};
