#include "ZoneWriterFactoryT6.h"

#include "ContentWriterT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/ZoneConstantsT6.h"
#include "Utils/ICapturedDataProvider.h"
#include "Writing/Processor/OutputProcessorXChunks.h"
#include "Writing/Steps/StepAddOutputProcessor.h"
#include "Writing/Steps/StepAlign.h"
#include "Writing/Steps/StepRemoveOutputProcessor.h"
#include "Writing/Steps/StepWriteXBlockSizes.h"
#include "Writing/Steps/StepWriteZero.h"
#include "Writing/Steps/StepWriteZoneContentToFile.h"
#include "Writing/Steps/StepWriteZoneContentToMemory.h"
#include "Writing/Steps/StepWriteZoneHeader.h"
#include "Writing/Steps/StepWriteZoneSizes.h"
#include "Zone/XChunk/XChunkProcessorDeflate.h"
#include "Zone/XChunk/XChunkProcessorSalsa20Encryption.h"

#include <cassert>
#include <cstring>

using namespace T6;

namespace
{
    void SetupBlocks(ZoneWriter& writer)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_VIRTUAL, XBlockType::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_PHYSICAL, XBlockType::BLOCK_TYPE_RUNTIME));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_VIRTUAL, XBlockType::BLOCK_TYPE_DELAY));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_PHYSICAL, XBlockType::BLOCK_TYPE_DELAY));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        writer.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_STREAMER_RESERVE, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    ZoneHeader CreateHeaderForParams(const bool isSecure, const bool isOfficial, const bool isEncrypted)
    {
        ZoneHeader header{};
        header.m_version = ZoneConstants::ZONE_VERSION_PC;

        if (isSecure)
        {
            if (isOfficial)
                memcpy(header.m_magic, ZoneConstants::MAGIC_SIGNED_TREYARCH, sizeof(ZoneHeader::m_magic));
            else
                memcpy(header.m_magic, ZoneConstants::MAGIC_SIGNED_OAT, sizeof(ZoneHeader::m_magic));
        }
        else
        {
            if (isEncrypted)
                memcpy(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, sizeof(ZoneHeader::m_magic));
            else
                memcpy(header.m_magic, ZoneConstants::MAGIC_UNSIGNED_SERVER, sizeof(ZoneHeader::m_magic));
        }

        return header;
    }

    void AddXChunkProcessor(ZoneWriter& writer,
                            const Zone& zone,
                            const bool isEncrypted,
                            ICapturedDataProvider** dataToSignProviderPtr,
                            OutputProcessorXChunks** xChunkProcessorPtr)
    {
        auto xChunkProcessor = std::make_unique<OutputProcessorXChunks>(
            ZoneConstants::STREAM_COUNT, ZoneConstants::XCHUNK_SIZE, ZoneConstants::XCHUNK_MAX_WRITE_SIZE, ZoneConstants::VANILLA_BUFFER_SIZE);
        if (xChunkProcessorPtr)
            *xChunkProcessorPtr = xChunkProcessor.get();

        // Decompress the chunks using zlib
        xChunkProcessor->AddChunkProcessor(std::make_unique<XChunkProcessorDeflate>());

        if (isEncrypted)
        {
            // If zone is encrypted, the decryption is applied before the decompression. T6 Zones always use Salsa20.
            auto chunkProcessorSalsa20 = std::make_unique<XChunkProcessorSalsa20Encryption>(
                ZoneConstants::STREAM_COUNT, zone.m_name, ZoneConstants::SALSA20_KEY_TREYARCH_PC, sizeof(ZoneConstants::SALSA20_KEY_TREYARCH_PC));

            // If there is encryption, the signed data of the zone is the final hash blocks provided by the Salsa20 IV adaption algorithm
            if (dataToSignProviderPtr)
                *dataToSignProviderPtr = chunkProcessorSalsa20.get();

            xChunkProcessor->AddChunkProcessor(std::move(chunkProcessorSalsa20));
        }

        writer.AddWritingStep(std::make_unique<StepAddOutputProcessor>(std::move(xChunkProcessor)));
    }
}; // namespace

std::unique_ptr<ZoneWriter> ZoneWriterFactory::CreateWriter(const Zone& zone) const
{
    auto writer = std::make_unique<ZoneWriter>();

    // TODO Support signed fastfiles
    bool isSecure = false;
    bool isEncrypted = true;

    SetupBlocks(*writer);

    auto contentInMemory = std::make_unique<StepWriteZoneContentToMemory>(
        std::make_unique<ContentWriter>(zone), zone, 32u, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK);
    auto* contentInMemoryPtr = contentInMemory.get();
    writer->AddWritingStep(std::move(contentInMemory));

    // Write zone header
    writer->AddWritingStep(std::make_unique<StepWriteZoneHeader>(CreateHeaderForParams(isSecure, false, isEncrypted)));

    // Setup loading XChunks from the zone from this point on.
    ICapturedDataProvider* dataToSignProvider;
    OutputProcessorXChunks* xChunksProcessor;
    AddXChunkProcessor(*writer, zone, isEncrypted, &dataToSignProvider, &xChunksProcessor);

    // Start of the XFile struct
    writer->AddWritingStep(std::make_unique<StepWriteZoneSizes>(contentInMemoryPtr));
    writer->AddWritingStep(std::make_unique<StepWriteXBlockSizes>(zone));

    // Start of the zone content
    writer->AddWritingStep(std::make_unique<StepWriteZoneContentToFile>(contentInMemoryPtr));

    // Stop writing in XChunks
    writer->AddWritingStep(std::make_unique<StepRemoveOutputProcessor>(xChunksProcessor));

    // Pad ending with zeros like the original linker does it. The game's reader needs it for some reason.
    // From my observations this is most likely the logic behind the amount of bytes: At least 0x40 bytes and aligned to the next 0x40
    writer->AddWritingStep(std::make_unique<StepWriteZero>(ZoneConstants::FILE_SUFFIX_ZERO_MIN_SIZE));
    writer->AddWritingStep(std::make_unique<StepAlign>(ZoneConstants::FILE_SUFFIX_ZERO_ALIGN, '\0'));

    return std::move(writer);
}
