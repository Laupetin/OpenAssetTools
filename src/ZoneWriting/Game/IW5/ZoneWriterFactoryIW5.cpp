#include "ZoneWriterFactoryIW5.h"

#include "ContentWriterIW5.h"
#include "Game/IW5/GameIW5.h"
#include "Game/IW5/IW5.h"
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

#include <cstring>

using namespace IW5;

namespace
{
    void SetupBlocks(ZoneWriter& writer)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_CALLBACK, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_SCRIPT, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    ZoneHeader CreateHeaderForParams(const bool isSecure, const bool isOfficial)
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
}; // namespace

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(const Zone& zone) const
{
    auto writer = std::make_unique<ZoneWriter>();

    // TODO Support signed fastfiles
    bool isSecure = false;

    SetupBlocks(*writer);

    auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(
        std::make_unique<ContentWriter>(zone), zone, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
    auto* contentInMemoryPtr = contentInMemory.get();
    writer->AddWritingStep(std::move(contentInMemory));

    // Write zone header
    writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams(isSecure, false)));

    // Write dummy byte that the game ignores as well. No clue what it means.
    writer->AddWritingStep(std::make_unique<StepWriteZero>(1));

    // Write timestamp
    writer->AddWritingStep(std::make_unique<StepWriteTimestamp>());

    writer->AddWritingStep(std::make_unique<StepAddOutputProcessor>(std::make_unique<OutputProcessorDeflate>()));

    // Start of the XFile struct
    writer->AddWritingStep(std::make_unique<StepWriteZoneSizes>(contentInMemoryPtr));
    writer->AddWritingStep(std::make_unique<StepWriteXBlockSizes>(zone));

    // Start of the zone content
    writer->AddWritingStep(std::make_unique<StepWriteZoneContentToFile>(contentInMemoryPtr));

    return std::move(writer);
}
