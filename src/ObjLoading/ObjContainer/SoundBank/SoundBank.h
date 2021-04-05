#pragma once

#include <istream>

#include "Utils/ClassUtils.h"
#include "ObjContainer/ObjContainerReferenceable.h"
#include "ObjContainer/ObjContainerRepository.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/FileUtils.h"
#include "Utils/ObjStream.h"
#include "Zone/Zone.h"

class SoundBank final : public ObjContainerReferenceable
{
    static constexpr uint32_t MAGIC = FileUtils::MakeMagic32('2', 'U', 'X', '#');
    static constexpr uint32_t VERSION = 14u;

    std::string m_file_name;
    std::unique_ptr<std::istream> m_stream;
    int64_t m_file_size;

    bool m_initialized;
    SndAssetBankHeader m_header;
    std::vector<std::string> m_dependencies;
    std::vector<SndAssetBankEntry> m_entries;
    std::vector<SndAssetBankChecksum> m_checksums;
    std::unordered_map<unsigned int, size_t> m_entries_by_id;

    bool ReadHeader();
    bool ReadEntries();
    bool ReadChecksums();

public:
    static ObjContainerRepository<SoundBank, Zone> Repository;

    static std::string GetFileNameForDefinition(bool streamed, const char* zone, const char* language);

    SoundBank(std::string fileName, std::unique_ptr<std::istream> stream, int64_t fileSize);
    SoundBank(const SoundBank& other) = delete;
    SoundBank(SoundBank&& other) noexcept = default;
    SoundBank& operator=(const SoundBank& other) = delete;
    SoundBank& operator=(SoundBank&& other) noexcept = default;

    std::string GetName() override;

    bool Initialize();
    _NODISCARD const std::vector<std::string>& GetDependencies() const;

    _NODISCARD bool VerifyChecksum(const SndAssetBankChecksum& checksum) const;
    _NODISCARD SearchPathOpenFile GetEntryStream(unsigned int id) const;
};
