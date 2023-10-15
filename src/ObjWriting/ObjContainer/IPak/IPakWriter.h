#pragma once
#include <memory>
#include <ostream>

#include "SearchPath/ISearchPath.h"

class IPakWriter
{
public:
    static constexpr auto USE_COMPRESSION = true;

    IPakWriter() = default;
    virtual ~IPakWriter() = default;

    IPakWriter(const IPakWriter& other) = default;
    IPakWriter(IPakWriter&& other) noexcept = default;
    IPakWriter& operator=(const IPakWriter& other) = default;
    IPakWriter& operator=(IPakWriter&& other) noexcept = default;

    virtual void AddImage(std::string imageName) = 0;
    virtual bool Write() = 0;

    static std::unique_ptr<IPakWriter> Create(std::ostream& stream, ISearchPath* assetSearchPath);
};
