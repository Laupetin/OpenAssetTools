#include "ZoneLoaderFactoryT6.h"

#include "ContentLoaderT6.h"
#include "Game/GameLanguage.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/ZoneConstantsT6.h"
#include "Loading/Processor/ProcessorXChunks.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepDumpData.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Utils/Endianness.h"
#include "Utils/Logging/Log.h"
#include "Zone/XChunk/XChunkProcessorInflate.h"
#include "Zone/XChunk/XChunkProcessorLzxDecompress.h"
#include "Zone/XChunk/XChunkProcessorSalsa20Decryption.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <memory>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    enum class ZoneCompressionTypeT6 : std::uint8_t
    {
        DEFLATE,
        LZX
    };

    struct ZoneLoaderInspectionResultT6
    {
        ZoneLoaderInspectionResult m_generic_result;
        ZoneCompressionTypeT6 m_compression_type;
    };

    std::optional<ZoneLoaderInspectionResultT6> InspectZoneHeaderT6(const ZoneHeader& header)
    {
        if (endianness::FromLittleEndian(header.m_version) == ZoneConstants::ZONE_VERSION_PC)
        {
            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_TREYARCH, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = true,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = true,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::DEFLATE,
                };
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_OAT, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = false,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = true,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::DEFLATE,
                };
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = false,
                                                   .m_is_signed = false,
                                                   .m_is_encrypted = true,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::DEFLATE,
                };
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED_SERVER, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = true,
                                                   .m_is_signed = false,
                                                   .m_is_encrypted = false,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::DEFLATE,
                };
            }
        }
        else if (endianness::FromBigEndian(header.m_version) == ZoneConstants::ZONE_VERSION_XENON)
        {
            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_TREYARCH, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::BE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::XBOX,
                                                   .m_is_official = true,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = true,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::DEFLATE,
                };
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_LZX_TREYARCH, 8))
            {
                return ZoneLoaderInspectionResultT6{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::T6,
                                                   .m_endianness = GameEndianness::BE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::XBOX,
                                                   .m_is_official = true,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = true,
                                                   },
                    .m_compression_type = ZoneCompressionTypeT6::LZX,
                };
            }
        }

        return std::nullopt;
    }

    GameLanguage GetZoneLanguage(const std::string& zoneName)
    {
        const auto& languagePrefixes = IGame::GetGameById(GameId::T6)->GetLanguagePrefixes();

        for (const auto& languagePrefix : languagePrefixes)
        {
            if (zoneName.starts_with(languagePrefix.m_prefix))
            {
                return languagePrefix.m_language;
            }
        }

        return GameLanguage::LANGUAGE_NONE;
    }

    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_VIRTUAL, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_PHYSICAL, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_VIRTUAL, XBlockType::BLOCK_TYPE_DELAY));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_PHYSICAL, XBlockType::BLOCK_TYPE_DELAY));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_STREAMER_RESERVE, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    std::unique_ptr<cryptography::IPublicKeyAlgorithm> SetupRsa(const bool isOfficial)
    {
        if (isOfficial)
        {
            auto rsa = cryptography::CreateRsa(cryptography::HashingAlgorithm::RSA_HASH_SHA256, cryptography::RsaPaddingMode::RSA_PADDING_PSS);

            if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_TREYARCH, sizeof(ZoneConstants::RSA_PUBLIC_KEY_TREYARCH)))
            {
                con::error("Invalid public key for signature checking");
                return nullptr;
            }

            return rsa;
        }
        else
        {
            assert(false);

            // TODO: Load custom RSA key here
            return nullptr;
        }
    }

    ISignatureProvider* AddAuthHeaderSteps(const bool isSecure, ZoneLoader& zoneLoader, const std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!isSecure)
            return nullptr;

        zoneLoader.AddLoadingStep(step::CreateStepVerifyMagic(ZoneConstants::MAGIC_AUTH_HEADER));
        zoneLoader.AddLoadingStep(step::CreateStepSkipBytes(4)); // Loading Flags which are always zero
        zoneLoader.AddLoadingStep(step::CreateStepVerifyFileName(fileName, 32));

        auto signatureLoadStep = step::CreateStepLoadSignature(256);
        auto* signatureLoadStepPtr = signatureLoadStep.get();
        zoneLoader.AddLoadingStep(std::move(signatureLoadStep));

        return signatureLoadStepPtr;
    }

    ICapturedDataProvider* AddXChunkProcessor(const ZoneLoaderInspectionResultT6& inspectResult, ZoneLoader& zoneLoader, const std::string& fileName)
    {
        ICapturedDataProvider* result = nullptr;
        auto xChunkProcessor = processor::CreateProcessorXChunks(
            ZoneConstants::STREAM_COUNT, ZoneConstants::XCHUNK_SIZE, inspectResult.m_generic_result.m_endianness, ZoneConstants::VANILLA_BUFFER_SIZE);

        const uint8_t (&salsa20Key)[32] = inspectResult.m_generic_result.m_platform == GamePlatform::XBOX ? ZoneConstants::SALSA20_KEY_TREYARCH_XENON
                                                                                                          : ZoneConstants::SALSA20_KEY_TREYARCH_PC;

        if (inspectResult.m_generic_result.m_is_encrypted)
        {
            // If zone is encrypted, the decryption is applied before the decompression. T6 Zones always use Salsa20.
            auto chunkProcessorSalsa20 =
                std::make_unique<XChunkProcessorSalsa20Decryption>(ZoneConstants::STREAM_COUNT, fileName, salsa20Key, sizeof(salsa20Key));
            result = chunkProcessorSalsa20.get();
            xChunkProcessor->AddChunkProcessor(std::move(chunkProcessorSalsa20));
        }

        if (inspectResult.m_compression_type == ZoneCompressionTypeT6::LZX)
        {
            // Decompress the chunks using lzx
            xChunkProcessor->AddChunkProcessor(std::make_unique<XChunkProcessorLzxDecompress>(ZoneConstants::STREAM_COUNT));
        }
        else
        {
            // Decompress the chunks using zlib
            xChunkProcessor->AddChunkProcessor(std::make_unique<XChunkProcessorInflate>());
        }

        zoneLoader.AddLoadingStep(step::CreateStepAddProcessor(std::move(xChunkProcessor)));

        // If there is encryption, the signed data of the zone is the final hash blocks provided by the Salsa20 IV adaption algorithm
        return result;
    }
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(const ZoneHeader& header) const
{
    auto resultT6 = InspectZoneHeaderT6(header);
    if (!resultT6)
        return std::nullopt;

    return resultT6->m_generic_result;
}

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(const ZoneHeader& header,
                                                                     const std::string& fileName,
                                                                     std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const
{
    const auto inspectResult = InspectZoneHeaderT6(header);
    if (!inspectResult)
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, GameId::T6);
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolT6>(zonePtr, 0);
    zone->m_language = GetZoneLanguage(fileName);

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    // If file is signed setup a RSA instance.
    auto rsa = inspectResult->m_generic_result.m_is_signed ? SetupRsa(inspectResult->m_generic_result.m_is_official) : nullptr;

    // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
    ISignatureProvider* signatureProvider = AddAuthHeaderSteps(inspectResult->m_generic_result.m_is_signed, *zoneLoader, fileName);

    // Setup loading XChunks from the zone from this point on.
    ICapturedDataProvider* signatureDataProvider = AddXChunkProcessor(*inspectResult, *zoneLoader, fileName);

    if (inspectResult->m_generic_result.m_endianness == GameEndianness::LE)
    {
        // Start of the XFile struct
        zoneLoader->AddLoadingStep(step::CreateStepLoadZoneSizes());
        zoneLoader->AddLoadingStep(step::CreateStepAllocXBlocks());

        // Start of the zone content
        zoneLoader->AddLoadingStep(step::CreateStepLoadZoneContent(
            [zonePtr](ZoneInputStream& stream)
            {
                return std::make_unique<ContentLoader>(*zonePtr, stream);
            },
            32u,
            ZoneConstants::OFFSET_BLOCK_BIT_COUNT,
            ZoneConstants::INSERT_BLOCK,
            zonePtr->Memory(),
            std::move(progressCallback)));

        if (inspectResult->m_generic_result.m_is_signed)
            zoneLoader->AddLoadingStep(step::CreateStepVerifySignature(std::move(rsa), signatureProvider, signatureDataProvider));
    }
    else
    {
        fs::path dumpFileNamePath = fs::path(fileName).filename();
        dumpFileNamePath.replace_extension(".dat");
        std::string dumpFileName = dumpFileNamePath.string();
        con::warn("Dumping xbox assets is not supported, making a full fastfile data dump to {}", dumpFileName);
        zoneLoader->AddLoadingStep(step::CreateStepDumpData(dumpFileName, 0xFFFFFFFF));
    }

    return zoneLoader;
}
