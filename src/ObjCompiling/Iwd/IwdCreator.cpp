#include "IwdCreator.h"

#include "Utils/FileToZlibWrapper.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <format>
#include <zip.h>

namespace fs = std::filesystem;

namespace
{
    std::tm GetLocalTime()
    {
        const auto now = std::time(nullptr);
        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif

        return localTime;
    }

    void FillFileInfoTime(zip_fileinfo& fileInfo)
    {
        const auto localTime = GetLocalTime();

        fileInfo.dosDate = 0u;
        fileInfo.tmz_date.tm_year = localTime.tm_year + 1900;
        fileInfo.tmz_date.tm_mon = localTime.tm_mon;
        fileInfo.tmz_date.tm_mday = localTime.tm_mday;
        fileInfo.tmz_date.tm_hour = localTime.tm_hour;
        fileInfo.tmz_date.tm_min = localTime.tm_min;
        fileInfo.tmz_date.tm_sec = localTime.tm_sec;
    }

    void AddCustomUserInclusions(ISearchPath& searchPath, zipFile zipFile, const std::string& iwdName)
    {
        const auto prefix = std::format("iwd/{}", iwdName);
        searchPath.Find(SearchPathSearchOptions().OnlyDiskFiles(true).FilterPrefix(prefix).IncludeSubdirectories(true),
                        [&searchPath, &zipFile, &prefix, &iwdName](const std::string& path)
                        {
                            const auto relativePath = fs::relative(path, prefix);

                            std::string normalizedPath(relativePath.string());
                            std::ranges::replace(normalizedPath, '\\', '/');

                            const auto readFile = searchPath.Open(path);
                            if (!readFile.IsOpen())
                            {
                                con::error("Failed to open file for iwd: {}", path);
                                return;
                            }

                            zip_fileinfo fileInfo{};
                            FillFileInfoTime(fileInfo);
                            zipOpenNewFileInZip(zipFile, normalizedPath.c_str(), &fileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);

                            char tempBuffer[0x1000];

                            do
                            {
                                readFile.m_stream->read(tempBuffer, sizeof(tempBuffer));
                                const auto readCount = readFile.m_stream->gcount();
                                if (readCount > 0)
                                    zipWriteInFileInZip(zipFile, tempBuffer, static_cast<unsigned>(readCount));
                            } while (!readFile.m_stream->eof());

                            zipCloseFileInZip(zipFile);

                            con::debug("Added {} to iwd {}", normalizedPath, iwdName);
                        });
    }
} // namespace

IwdToCreate::IwdToCreate(std::string name)
    : m_name(std::move(name))
{
}

void IwdToCreate::AddFile(std::string filePath)
{
    m_file_paths.emplace_back(std::move(filePath));
}

void IwdToCreate::Build(ISearchPath& searchPath, IOutputPath& outPath)
{
    const auto fileName = std::format("{}.iwd", m_name);
    const auto file = outPath.Open(fileName);
    if (!file)
    {
        con::error("Failed to open file for iwd {}", m_name);
        return;
    }

    auto functions = FileToZlibWrapper::CreateFunctions32ForFile(file.get());

    const auto zipFile = zipOpen2(fileName.c_str(), APPEND_STATUS_CREATE, nullptr, &functions);
    if (!zipFile)
    {
        con::error("Failed to open file as zip for iwd {}", m_name);
        return;
    }

    for (const auto& filePath : m_file_paths)
    {
        auto readFile = searchPath.Open(filePath);
        if (!readFile.IsOpen())
        {
            con::error("Failed to open file for iwd: {}", filePath);
            continue;
        }

        zip_fileinfo fileInfo{};
        FillFileInfoTime(fileInfo);
        zipOpenNewFileInZip(zipFile, filePath.c_str(), &fileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);

        char tempBuffer[0x1000];

        do
        {
            readFile.m_stream->read(tempBuffer, sizeof(tempBuffer));
            const auto readCount = readFile.m_stream->gcount();
            if (readCount > 0)
                zipWriteInFileInZip(zipFile, tempBuffer, static_cast<unsigned>(readCount));
        } while (!readFile.m_stream->eof());

        zipCloseFileInZip(zipFile);

        con::debug("Added {} to iwd {}", filePath, m_name);
    }

    AddCustomUserInclusions(searchPath, zipFile, m_name);

    zipClose(zipFile, nullptr);

    con::info("Created iwd {} with {} entries", m_name, m_file_paths.size());
}

const std::vector<std::string>& IwdToCreate::GetFilePaths() const
{
    return m_file_paths;
}

IwdToCreate* IwdCreator::GetOrAddIwd(const std::string& iwdName)
{
    const auto existingIwd = m_iwd_lookup.find(iwdName);
    if (existingIwd != m_iwd_lookup.end())
        return existingIwd->second;

    auto newIwd = std::make_unique<IwdToCreate>(iwdName);
    auto* result = newIwd.get();
    m_iwd_lookup.emplace(iwdName, result);
    m_iwds.emplace_back(std::move(newIwd));

    return result;
}

void IwdCreator::Finalize(ISearchPath& searchPath, IOutputPath& outPath)
{
    con::info("Writing {} iwd files to disk", m_iwds.size());
    for (const auto& iwdToCreate : m_iwds)
        iwdToCreate->Build(searchPath, outPath);

    m_iwds.clear();
    m_iwd_lookup.clear();
}
