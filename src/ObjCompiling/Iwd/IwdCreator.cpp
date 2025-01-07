#include "IwdCreator.h"

#include "Utils/FileToZlibWrapper.h"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <zip.h>

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
        std::cerr << std::format("Failed to open file for iwd {}\n", m_name);
        return;
    }

    auto functions = FileToZlibWrapper::CreateFunctions32ForFile(file.get());

    const auto zipFile = zipOpen2(fileName.c_str(), APPEND_STATUS_CREATE, nullptr, &functions);
    if (!zipFile)
    {
        std::cerr << std::format("Failed to open file as zip for iwd {}\n", m_name);
        return;
    }

    for (const auto& filePath : m_file_paths)
    {
        auto readFile = searchPath.Open(filePath);
        if (!readFile.IsOpen())
        {
            std::cerr << std::format("Failed to open file for iwd: {}\n", filePath);
            continue;
        }

        auto localNow = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}.get_local_time();
        auto nowDays = std::chrono::floor<std::chrono::days>(localNow);
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(localNow));
        std::chrono::hh_mm_ss hms(std::chrono::floor<std::chrono::milliseconds>(localNow - nowDays));

        zip_fileinfo fileInfo{};
        fileInfo.dosDate = 0u;
        fileInfo.tmz_date.tm_year = static_cast<int>(ymd.year());
        fileInfo.tmz_date.tm_mon = static_cast<int>(static_cast<unsigned>(ymd.month()) - static_cast<unsigned>(std::chrono::January));
        fileInfo.tmz_date.tm_mday = static_cast<int>(static_cast<unsigned>(ymd.day()));
        fileInfo.tmz_date.tm_hour = static_cast<int>(hms.hours().count());
        fileInfo.tmz_date.tm_min = static_cast<int>(hms.minutes().count());
        fileInfo.tmz_date.tm_sec = static_cast<int>(hms.seconds().count());
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
    }

    zipClose(zipFile, nullptr);

    std::cout << std::format("Created iwd {} with {} entries\n", m_name, m_file_paths.size());
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
    std::cout << std::format("Writing {} iwd files to disk\n", m_iwds.size());
    for (const auto& iwdToCreate : m_iwds)
        iwdToCreate->Build(searchPath, outPath);

    m_iwds.clear();
    m_iwd_lookup.clear();
}
