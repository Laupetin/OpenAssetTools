#include "ZoneWriterFactoryIW4.h"

#include <cassert>
#include <cstring>

#include "ContentWriterIW4.h"
#include "Utils/ICapturedDataProvider.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/GameIW4.h"
#include "Game/IW4/ZoneConstantsIW4.h"
#include "Writing/Steps/StepAddOutputProcessor.h"
#include "Writing/Steps/StepWriteXBlockSizes.h"
#include "Writing/Steps/StepWriteZoneContentToFile.h"
#include "Writing/Steps/StepWriteZoneContentToMemory.h"
#include "Writing/Steps/StepWriteZoneHeader.h"

using namespace IW4;

class ZoneWriterFactory::Impl
{
    Zone* m_zone;
    std::unique_ptr<ZoneWriter> m_writer;

public:
    explicit Impl(Zone* zone)
        : m_zone(zone),
          m_writer(std::make_unique<ZoneWriter>())
    {
    }

    void SetupBlocks() const
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_CALLBACK, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    static ZoneHeader CreateHeaderForParams(const bool isSecure, const bool isOfficial)
    {
        ZoneHeader header{};
        header.m_version = ZoneConstants::ZONE_VERSION;

        if (isSecure)
        {
            if (isOfficial)
                memcpy(header.m_magic, ZoneConstants::MAGIC_SIGNED_INFINITY_WARD, sizeof(ZoneHeader::m_magic));
            else
                memcpy(header.m_magic, ZoneConstants::MAGIC_SIGNED_OAT, sizeof(ZoneHeader::m_magic));
        }
        else
        {
            memcpy(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, sizeof(ZoneHeader::m_magic));
        }

        return header;
    }

    std::unique_ptr<ZoneWriter> CreateWriter()
    {
        // TODO Support signed fastfiles
        bool isSecure = false;

        SetupBlocks();

        // Write zone header
        m_writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams(isSecure, false)));

        // Return the fully setup zoneloader
        return std::move(m_writer);
    }
};

bool ZoneWriterFactory::SupportsZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW4;
}

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(Zone* zone) const
{
    Impl impl(zone);
    return impl.CreateWriter();
}
