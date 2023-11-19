#include "ZoneWriterFactoryIW3.h"

#include "ContentWriterIW3.h"
#include "Game/IW3/GameIW3.h"
#include "Game/IW3/IW3.h"
#include "Game/IW3/ZoneConstantsIW3.h"
#include "Writing/Processor/OutputProcessorDeflate.h"
#include "Writing/Steps/StepAddOutputProcessor.h"
#include "Writing/Steps/StepWriteXBlockSizes.h"
#include "Writing/Steps/StepWriteZoneContentToFile.h"
#include "Writing/Steps/StepWriteZoneContentToMemory.h"
#include "Writing/Steps/StepWriteZoneHeader.h"
#include "Writing/Steps/StepWriteZoneSizes.h"

#include <cstring>

using namespace IW3;

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

        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        m_writer->AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    static ZoneHeader CreateHeaderForParams()
    {
        ZoneHeader header{};
        header.m_version = ZoneConstants::ZONE_VERSION;
        memcpy(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, sizeof(ZoneHeader::m_magic));

        return header;
    }

    std::unique_ptr<ZoneWriter> CreateWriter()
    {
        SetupBlocks();

        auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(
            std::make_unique<ContentWriter>(), m_zone, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
        auto* contentInMemoryPtr = contentInMemory.get();
        m_writer->AddWritingStep(std::move(contentInMemory));

        // Write zone header
        m_writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams()));

        m_writer->AddWritingStep(std::make_unique<StepAddOutputProcessor>(std::make_unique<OutputProcessorDeflate>()));

        // Start of the XFile struct
        m_writer->AddWritingStep(std::make_unique<StepWriteZoneSizes>(contentInMemoryPtr));
        m_writer->AddWritingStep(std::make_unique<StepWriteXBlockSizes>(m_zone));

        // Start of the zone content
        m_writer->AddWritingStep(std::make_unique<StepWriteZoneContentToFile>(contentInMemoryPtr));

        // Return the fully setup zoneloader
        return std::move(m_writer);
    }
};

bool ZoneWriterFactory::SupportsZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW3;
}

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(Zone* zone) const
{
    Impl impl(zone);
    return impl.CreateWriter();
}
