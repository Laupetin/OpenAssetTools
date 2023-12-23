#include "IWDWriter.h"

#include <algorithm>
#include <iostream>
#include "../minizip/zip.h"

class IWDWriterImpl final : public IWDWriter
{
public:
    explicit IWDWriterImpl(const std::string& iwdFileName, ISearchPath* assetSearchPath)
        : m_filename(iwdFileName),
          m_asset_search_path(assetSearchPath),
          m_files(0)
    {
        m_zipfile = zipOpen(m_filename.c_str(), APPEND_STATUS_CREATE);
        if (!m_zipfile)
        {
            throw std::runtime_error("Error creating IWD file: \"" + m_filename + "\"");
        }
    }

    void AddFile(std::string fileName) override
    {
        m_files.emplace_back(std::move(fileName));
    }

    bool AddFileToArchive(const std::string& fileName, const std::unique_ptr<char []>& fileData, const size_t& fileSize)
    {
        zip_fileinfo zi = {};

        if (zipOpenNewFileInZip(m_zipfile, fileName.c_str(), &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK)
        {
            zipClose(m_zipfile, NULL);
            throw std::runtime_error( "Could not open new file in IWD \"" + fileName + "\"\n");
        }

        if (zipWriteInFileInZip(m_zipfile, fileData.get(), fileSize) != ZIP_OK)
        {
            zipClose(m_zipfile, NULL);
            throw std::runtime_error("Could not write file data to IWD \"" + fileName + "\"\n");
        }

        if (zipCloseFileInZip(m_zipfile) != ZIP_OK)
        {
            zipClose(m_zipfile, NULL);
            throw std::runtime_error("Could not close file in IWD \"" + fileName + "\"\n");
        }

        return true;
    }

    std::unique_ptr<char[]> ReadFileDataFromSearchPath(const std::string& fileName, size_t& fileSize) const
    {
        const auto openFile = m_asset_search_path->Open(fileName);
        if (!openFile.IsOpen())
        {
            std::cerr << "Could not open file for writing to IWD \"" << fileName << "\"\n";
            return nullptr;
        }

        fileSize = static_cast<size_t>(openFile.m_length);
        auto fileData = std::make_unique<char[]>(fileSize);
        openFile.m_stream->read(fileData.get(), fileSize);

        return fileData;
    }

    bool WriteFileData(const std::string& fileName)
    {
        size_t fileSize;
        const auto fileData = ReadFileDataFromSearchPath(fileName, fileSize);
        if (!fileData)
            return false;

        AddFileToArchive(fileName, fileData, fileSize);

        return true;
    }

    bool Write() override
    {
        const auto result = std::all_of(m_files.begin(),
                                        m_files.end(),
                                        [this](const std::string& fileName)
                                        {
                                            return WriteFileData(fileName);
                                        });

        zipClose(m_zipfile, NULL);
        return result;
    }

private:
    const std::string m_filename;

    ISearchPath* m_asset_search_path;
    std::vector<std::string> m_files;

    zipFile m_zipfile;
};

std::unique_ptr<IWDWriter> IWDWriter::Create(const std::string& iwdFileName, ISearchPath* assetSearchPath)
{
    return std::make_unique<IWDWriterImpl>(iwdFileName, assetSearchPath);
}
