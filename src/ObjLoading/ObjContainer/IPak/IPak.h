#pragma once

#include "ObjContainer/IPak/IPakTypes.h"
#include "ObjContainer/ObjContainerReferenceable.h"
#include "ObjContainer/ObjContainerRepository.h"
#include "Utils/ObjStream.h"
#include "Zone/Zone.h"

#include <cstdint>
#include <istream>
#include <memory>
#include <string>

class IIPak : public ObjContainerReferenceable
{
public:
    static ObjContainerRepository<IIPak, Zone> Repository;

    IIPak() = default;
    ~IIPak() override = default;
    IIPak(const IIPak& other) = default;
    IIPak(IIPak&& other) noexcept = default;
    IIPak& operator=(const IIPak& other) = default;
    IIPak& operator=(IIPak&& other) noexcept = default;

    virtual bool Initialize() = 0;
    [[nodiscard]] virtual std::unique_ptr<iobjstream> GetEntryStream(IPakHash nameHash, IPakHash dataHash) const = 0;

    [[nodiscard]] virtual const std::vector<IPakIndexEntry>& GetIndexEntries() const = 0;

    static std::unique_ptr<IIPak> Create(std::string path, std::unique_ptr<std::istream> stream);
    static IPakHash HashString(const std::string& str);
    static IPakHash HashData(const void* data, size_t dataSize);
};
