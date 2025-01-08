#pragma once

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
    typedef std::uint32_t Hash;

    IIPak() = default;
    virtual ~IIPak() = default;
    IIPak(const IIPak& other) = default;
    IIPak(IIPak&& other) noexcept = default;
    IIPak& operator=(const IIPak& other) = default;
    IIPak& operator=(IIPak&& other) noexcept = default;

    virtual bool Initialize() = 0;
    [[nodiscard]] virtual std::unique_ptr<iobjstream> GetEntryStream(Hash nameHash, Hash dataHash) const = 0;

    static std::unique_ptr<IIPak> Create(std::string path, std::unique_ptr<std::istream> stream);
    static Hash HashString(const std::string& str);
    static Hash HashData(const void* data, size_t dataSize);
};
