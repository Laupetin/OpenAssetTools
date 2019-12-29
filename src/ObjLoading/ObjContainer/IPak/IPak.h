#pragma once

#include "Utils/FileAPI.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "ObjContainer/ObjContainerReferenceable.h"

#include <vector>

class IPak final : public ObjContainerReferenceable
{
    static const uint32_t MAGIC;
    static const uint32_t VERSION;

    FileAPI::IFile* m_file;

    bool m_initialized;

    IPakSection* m_index_section;
    IPakSection* m_data_section;

    std::vector<IPakIndexEntry> m_index_entries;

    static uint32_t R_HashString(const char* str, uint32_t hash);

    void ReadSection();
    void ReadHeader();

public:
    explicit IPak(FileAPI::IFile* file);
    ~IPak();

    void Initialize();
    FileAPI::IFile* GetEntryData(IPakHash nameHash, IPakHash dataHash);

    static IPakHash HashString(const std::string& str);
    static IPakHash HashData(const void* data, size_t dataSize);
};
