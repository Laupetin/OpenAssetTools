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

namespace
{
    void SetupBlocks(ZoneWriter& writer)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(IW3::XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));

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

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(Zone* zone) const
{
    std::unique_ptr<ZoneWriter> writer;

    SetupBlocks(*writer);

    auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(
        std::make_unique<ContentWriter>(), zone, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
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
