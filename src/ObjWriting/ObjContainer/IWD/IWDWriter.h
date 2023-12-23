#pragma once
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <ostream>

class IWDWriter
{
public:
    static constexpr auto USE_COMPRESSION = true;

    IWDWriter() = default;
    virtual ~IWDWriter() = default;

    IWDWriter(const IWDWriter& other) = default;
    IWDWriter(IWDWriter&& other) noexcept = default;
    IWDWriter& operator=(const IWDWriter& other) = default;
    IWDWriter& operator=(IWDWriter&& other) noexcept = default;

    virtual void AddFile(std::string fileName) = 0;
    virtual bool Write() = 0;

    static std::unique_ptr<IWDWriter> Create(const std::string& iwdFileName, ISearchPath* assetSearchPath);
};