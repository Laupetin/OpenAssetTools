#pragma once

#include "ObjContainer/ObjContainerReferenceable.h"
#include "ObjContainer/ObjContainerRepository.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "Utils/ClassUtils.h"
#include "Zone/Zone.h"

#include <istream>

class SoundBankEntryInputStream
{
public:
    SoundBankEntryInputStream();
    SoundBankEntryInputStream(std::unique_ptr<std::istream> stream, const SoundAssetBankEntry& entry);

    _NODISCARD bool IsOpen() const;

    std::unique_ptr<std::istream> m_stream;
    SoundAssetBankEntry m_entry;
};

class SoundBank final : public ObjContainerReferenceable
{
    bool ReadHeader();
    bool ReadEntries();
    bool ReadChecksums();

public:
    SoundBank(std::string fileName, std::unique_ptr<std::istream> stream, int64_t fileSize);
    ~SoundBank() override = default;
    SoundBank(const SoundBank& other) = delete;
    SoundBank(SoundBank&& other) noexcept = default;
    SoundBank& operator=(const SoundBank& other) = delete;
    SoundBank& operator=(SoundBank&& other) noexcept = default;

    static std::string GetFileNameForDefinition(bool streamed, const char* zone, const char* language);

    std::string GetName() override;

    bool Initialize();
    _NODISCARD const std::vector<std::string>& GetDependencies() const;

    _NODISCARD bool VerifyChecksum(const SoundAssetBankChecksum& checksum) const;
    _NODISCARD SoundBankEntryInputStream GetEntryStream(unsigned int id) const;

    static ObjContainerRepository<SoundBank, Zone> Repository;

private:
    std::string m_file_name;
    std::unique_ptr<std::istream> m_stream;
    int64_t m_file_size;

    bool m_initialized;
    SoundAssetBankHeader m_header;
    std::vector<std::string> m_dependencies;
    std::vector<SoundAssetBankEntry> m_entries;
    std::vector<SoundAssetBankChecksum> m_checksums;
    std::unordered_map<unsigned int, size_t> m_entries_by_id;
};
