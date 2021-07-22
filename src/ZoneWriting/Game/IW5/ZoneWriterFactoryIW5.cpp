#include "ZoneWriterFactoryIW5.h"

#include <cstring>

#include "ContentWriterIW5.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/GameIW5.h"
#include "Game/IW5/ZoneConstantsIW5.h"
#include "Writing/Processor/OutputProcessorDeflate.h"
#include "Writing/Steps/StepAddOutputProcessor.h"
#include "Writing/Steps/StepWriteTimestamp.h"
#include "Writing/Steps/StepWriteXBlockSizes.h"
#include "Writing/Steps/StepWriteZero.h"
#include "Writing/Steps/StepWriteZoneContentToFile.h"
#include "Writing/Steps/StepWriteZoneContentToMemory.h"
#include "Writing/Steps/StepWriteZoneHeader.h"
#include "Writing/Steps/StepWriteZoneSizes.h"

using namespace IW5;

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
        m_writer->AddXBlock(XBLOCK_DEF(XFILE_BLOCK_SCRIPT, XBlock::Type::BLOCK_TYPE_NORMAL));

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

        auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(std::make_unique<ContentWriter>(), m_zone, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
        auto* contentInMemoryPtr = contentInMemory.get();
        m_writer->AddWritingStep(std::move(contentInMemory));

        // Write zone header
        m_writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams(isSecure, false)));

        // Write dummy byte that the game ignores as well. No clue what it means.
        m_writer->AddWritingStep(std::make_unique<StepWriteZero>(1));

        // Write timestamp
        m_writer->AddWritingStep(std::make_unique<StepWriteTimestamp>());

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
    return zone->m_game == &g_GameIW5;
}

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(Zone* zone) const
{
    Impl impl(zone);
    return impl.CreateWriter();
}
