#pragma once

#include "SearchPath/ISearchPath.h"
#include "ObjContainer/ObjContainerRepository.h"

class IWD final : public ISearchPath, IObjContainer
{
    class Impl;
    Impl* m_impl;

public:
    static ObjContainerRepository<IWD, ISearchPath> Repository;

    IWD(std::string path, FileAPI::IFile* file);
    ~IWD();

    IWD(const IWD& other) = delete;
    IWD(IWD&& other) noexcept;
    IWD& operator=(const IWD& other) = delete;
    IWD& operator=(IWD&& other) noexcept;

    bool Initialize() const;

    FileAPI::IFile* Open(const std::string& fileName) override;
    std::string GetPath() override;
    std::string GetName() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;
};