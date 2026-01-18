#include "ZoneWriterFactoryT5.h"

#include "ContentWriterT5.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/T5.h"
#include "Game/T5/ZoneConstantsT5.h"
#include "Writing/Processor/OutputProcessorDeflate.h"
#include "Writing/Steps/StepAddOutputProcessor.h"
#include "Writing/Steps/StepWriteXBlockSizes.h"
#include "Writing/Steps/StepWriteZoneContentToFile.h"
#include "Writing/Steps/StepWriteZoneContentToMemory.h"
#include "Writing/Steps/StepWriteZoneHeader.h"
#include "Writing/Steps/StepWriteZoneSizes.h"

#include <cstring>

using namespace T5;

namespace
{
    void SetupBlocks(ZoneWriter& writer)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_LARGE_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_PHYSICAL_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(T5::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    ZoneHeader CreateHeaderForParams()
    {
        ZoneHeader header{};
        header.m_version = ZoneConstants::ZONE_VERSION;
        memcpy(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, sizeof(ZoneHeader::m_magic));

        return header;
    }
} // namespace

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(const Zone& zone) const
{
    auto writer = std::make_unique<ZoneWriter>();

    SetupBlocks(*writer);

    auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(
        std::make_unique<ContentWriter>(zone), zone, 32u, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
    auto* contentInMemoryPtr = contentInMemory.get();
    writer->AddWritingStep(std::move(contentInMemory));

    // Write zone header
    writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams()));

    writer->AddWritingStep(std::make_unique<StepAddOutputProcessor>(std::make_unique<OutputProcessorDeflate>()));

    // Start of the XFile struct
    writer->AddWritingStep(std::make_unique<StepWriteZoneSizes>(contentInMemoryPtr));
    writer->AddWritingStep(std::make_unique<StepWriteXBlockSizes>(zone));

    // Start of the zone content
    writer->AddWritingStep(std::make_unique<StepWriteZoneContentToFile>(contentInMemoryPtr));

    return std::move(writer);
}
