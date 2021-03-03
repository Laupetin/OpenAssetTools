#pragma once

#include <istream>

#include "Utils/ClassUtils.h"
#include "ObjContainer/ObjContainerReferenceable.h"
#include "ObjContainer/ObjContainerRepository.h"
#include "Utils/ObjStream.h"
#include "Zone/Zone.h"

class IPak final : public ObjContainerReferenceable
{
    class Impl;
    Impl* m_impl;

public:
    typedef uint32_t Hash;

    static ObjContainerRepository<IPak, Zone> Repository;

    IPak(std::string path, std::unique_ptr<std::istream> stream);
    ~IPak() override;

    std::string GetName() override;

    bool Initialize();
    _NODISCARD std::unique_ptr<iobjstream> GetEntryStream(Hash nameHash, Hash dataHash) const;

    static Hash HashString(const std::string& str);
    static Hash HashData(const void* data, size_t dataSize);
};
