#include "IPak.h"
#include "zlib.h"
#include "Exception/IPakLoadException.h"

#include <sstream>

const uint32_t IPak::MAGIC = 'IPAK';
const uint32_t IPak::VERSION = 0x50000;

uint32_t IPak::R_HashString(const char* str, uint32_t hash)
{
    for (const char* pos = str; *pos; pos++)
    {
        hash = 33 * hash ^ (*pos | 0x20);
    }

    return hash;
}

IPak::IPak(FileAPI::IFile* file)
{
    m_file = file;
    m_initialized = false;
    m_index_section = nullptr;
    m_data_section = nullptr;
}

IPak::~IPak()
{
    delete m_index_section;
    m_index_section = nullptr;

    delete m_data_section;
    m_data_section = nullptr;
}

void IPak::ReadSection()
{
    IPakSection section{};

    if (m_file->Read(&section, sizeof section, 1) != sizeof section)
        throw IPakLoadException("Unexpected eof when trying to load section.");

    switch (section.type)
    {
    case 1:
        m_index_section = new IPakSection(section);
        break;

    case 2:
        m_data_section = new IPakSection(section);
        break;

    default:
        break;
    }
}

void IPak::ReadHeader()
{
    IPakHeader header{};

    if (m_file->Read(&header, sizeof header, 1) != sizeof header)
        throw IPakLoadException("Unexpected eof when trying to load header.");

    if (header.magic != MAGIC)
    {
        std::ostringstream oss;
        oss << "Invalid magic '0x" << std::hex << header.magic << "'.";

        throw IPakLoadException(oss.str());
    }

    if(header.version != VERSION)
    {
        std::ostringstream oss;
        oss << "Unsupported version '" << header.version << "'.";

        throw IPakLoadException(oss.str());
    }

    for(unsigned section = 0; section < header.sectionCount; section++)
    {
        ReadSection();
    }

    if(m_index_section == nullptr)
    {
        throw IPakLoadException("IPak does not contain an index section.");
    }

    if(m_data_section == nullptr)
    {
        throw IPakLoadException("IPak does not contain a data section.");
    }
}

void IPak::Initialize()
{
    if (m_initialized)
        return;

    ReadHeader();

    m_initialized = true;
}

FileAPI::IFile* IPak::GetEntryData(IPakHash nameHash, IPakHash dataHash)
{
    // TODO
    return nullptr;
}

IPakHash IPak::HashString(const std::string& str)
{
    return R_HashString(str.c_str(), 0);
}

IPakHash IPak::HashData(const void* data, const size_t dataSize)
{
    return crc32(0, static_cast<const Bytef*>(data), dataSize);
}