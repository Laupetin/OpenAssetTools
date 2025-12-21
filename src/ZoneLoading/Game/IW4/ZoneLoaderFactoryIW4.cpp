#include "ZoneLoaderFactoryIW4.h"

#include "ContentLoaderIW4.h"
#include "Game/GameLanguage.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/GameIW4.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/ZoneConstantsIW4.h"
#include "Loading/Processor/ProcessorAuthedBlocks.h"
#include "Loading/Processor/ProcessorCaptureData.h"
#include "Loading/Processor/ProcessorIW4xDecryption.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepDumpData.h"
#include "Loading/Steps/StepLoadHash.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Loading/Steps/StepRemoveProcessor.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepVerifyHash.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Utils/ClassUtils.h"
#include "Utils/Endianness.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <type_traits>

using namespace IW4;
namespace fs = std::filesystem;

namespace
{
    struct ZoneLoaderInspectionResultIW4
    {
        ZoneLoaderInspectionResult m_generic_result;
        bool m_is_iw4x;
    };

    std::optional<ZoneLoaderInspectionResultIW4> InspectZoneHeaderIw4(const ZoneHeader& header)
    {

        if (endianness::FromLittleEndian(header.m_version) == ZoneConstants::ZONE_VERSION)
        {
            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_IW4X, std::char_traits<char>::length(ZoneConstants::MAGIC_IW4X)))
            {
                if (*reinterpret_cast<const uint32_t*>(&header.m_magic[std::char_traits<char>::length(ZoneConstants::MAGIC_IW4X)])
                    == ZoneConstants::IW4X_ZONE_VERSION)
                {
                    return ZoneLoaderInspectionResultIW4{
                        .m_generic_result =
                            ZoneLoaderInspectionResult{
                                                       .m_game_id = GameId::IW4,
                                                       .m_endianness = GameEndianness::LE,
                                                       .m_word_size = GameWordSize::ARCH_32,
                                                       .m_platform = GamePlatform::PC,
                                                       .m_is_official = false,
                                                       .m_is_signed = false,
                                                       .m_is_encrypted = false,
                                                       },
                        .m_is_iw4x = true,
                    };
                }

                return std::nullopt;
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_INFINITY_WARD, std::char_traits<char>::length(ZoneConstants::MAGIC_SIGNED_INFINITY_WARD)))
            {
                return ZoneLoaderInspectionResultIW4{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::IW4,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = true,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = false,
                                                   },
                    .m_is_iw4x = false,
                };
            }

            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
            {
                return ZoneLoaderInspectionResultIW4{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::IW4,
                                                   .m_endianness = GameEndianness::LE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::PC,
                                                   .m_is_official = false,
                                                   .m_is_signed = false,
                                                   .m_is_encrypted = false,
                                                   },
                    .m_is_iw4x = false,
                };
            }
        }
        else if (endianness::FromBigEndian(header.m_version) == ZoneConstants::ZONE_VERSION_XENON)
        {
            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
            {
                return ZoneLoaderInspectionResultIW4{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::IW4,
                                                   .m_endianness = GameEndianness::BE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::XBOX,
                                                   .m_is_official = false,
                                                   .m_is_signed = false,
                                                   .m_is_encrypted = false,
                                                   },
                    .m_is_iw4x = false,
                };
            }
            if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_INFINITY_WARD, std::char_traits<char>::length(ZoneConstants::MAGIC_SIGNED_INFINITY_WARD)))
            {
                return ZoneLoaderInspectionResultIW4{
                    .m_generic_result =
                        ZoneLoaderInspectionResult{
                                                   .m_game_id = GameId::IW4,
                                                   .m_endianness = GameEndianness::BE,
                                                   .m_word_size = GameWordSize::ARCH_32,
                                                   .m_platform = GamePlatform::XBOX,
                                                   .m_is_official = true,
                                                   .m_is_signed = true,
                                                   .m_is_encrypted = false,
                                                   },
                    .m_is_iw4x = false,
                };
            }
        }

        return std::nullopt;
    }

    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_CALLBACK, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_VERTEX, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_INDEX, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    std::unique_ptr<cryptography::IPublicKeyAlgorithm> SetupRsa(const bool isOfficial, const GamePlatform platform)
    {
        if (isOfficial)
        {
            auto rsa = cryptography::CreateRsa(cryptography::HashingAlgorithm::RSA_HASH_SHA256, cryptography::RsaPaddingMode::RSA_PADDING_PSS);

            if (platform == GamePlatform::PC)
            {
                if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD, sizeof(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD)))
                {
                    con::error("Invalid public key for signature checking");
                    return nullptr;
                }
            }
            else if (platform == GamePlatform::XBOX)
            {
                if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD_XENON, sizeof(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD_XENON)))
                {
                    con::error("Invalid public key for signature checking");
                    return nullptr;
                }
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

    void AddAuthHeaderSteps(const ZoneLoaderInspectionResultIW4& inspectResult, ZoneLoader& zoneLoader, const std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!inspectResult.m_generic_result.m_is_signed)
            return;

        // If file is signed setup a RSA instance.
        auto rsa = SetupRsa(inspectResult.m_generic_result.m_is_official, inspectResult.m_generic_result.m_platform);

        zoneLoader.AddLoadingStep(step::CreateStepVerifyMagic(ZoneConstants::MAGIC_AUTH_HEADER));
        zoneLoader.AddLoadingStep(step::CreateStepSkipBytes(4)); // Skip reserved

        auto subHeaderHash = step::CreateStepLoadHash(sizeof(DB_AuthHash::bytes), 1);
        auto* subHeaderHashPtr = subHeaderHash.get();
        zoneLoader.AddLoadingStep(std::move(subHeaderHash));

        auto subHeaderHashSignature = step::CreateStepLoadSignature(sizeof(DB_AuthSignature::bytes));
        auto* subHeaderHashSignaturePtr = subHeaderHashSignature.get();
        zoneLoader.AddLoadingStep(std::move(subHeaderHashSignature));

        zoneLoader.AddLoadingStep(step::CreateStepVerifySignature(std::move(rsa), subHeaderHashSignaturePtr, subHeaderHashPtr));

        auto subHeaderCapture = processor::CreateProcessorCaptureData(sizeof(DB_AuthSubHeader));
        auto* subHeaderCapturePtr = subHeaderCapture.get();
        zoneLoader.AddLoadingStep(step::CreateStepAddProcessor(std::move(subHeaderCapture)));

        zoneLoader.AddLoadingStep(step::CreateStepVerifyFileName(fileName, sizeof(DB_AuthSubHeader::fastfileName)));
        zoneLoader.AddLoadingStep(step::CreateStepSkipBytes(4)); // Skip reserved

        auto masterBlockHashes =
            step::CreateStepLoadHash(sizeof(DB_AuthHash::bytes), static_cast<unsigned>(std::extent_v<decltype(DB_AuthSubHeader::masterBlockHashes)>));
        auto* masterBlockHashesPtr = masterBlockHashes.get();
        zoneLoader.AddLoadingStep(std::move(masterBlockHashes));

        zoneLoader.AddLoadingStep(step::CreateStepVerifyHash(cryptography::CreateSha256(), 0, subHeaderHashPtr, subHeaderCapturePtr));
        zoneLoader.AddLoadingStep(step::CreateStepRemoveProcessor(subHeaderCapturePtr));

        // Skip the rest of the first chunk
        zoneLoader.AddLoadingStep(step::CreateStepSkipBytes(ZoneConstants::AUTHED_CHUNK_SIZE - sizeof(DB_AuthHeader)));

        zoneLoader.AddLoadingStep(step::CreateStepAddProcessor(
            processor::CreateProcessorAuthedBlocks(ZoneConstants::AUTHED_CHUNK_COUNT_PER_GROUP,
                                                   ZoneConstants::AUTHED_CHUNK_SIZE,
                                                   static_cast<unsigned>(std::extent_v<decltype(DB_AuthSubHeader::masterBlockHashes)>),
                                                   cryptography::CreateSha256(),
                                                   masterBlockHashesPtr)));
    }
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(const ZoneHeader& header) const
{
    auto resultIw4 = InspectZoneHeaderIw4(header);
    if (!resultIw4)
        return std::nullopt;

    return resultIw4->m_generic_result;
}

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(const ZoneHeader& header,
                                                                     const std::string& fileName,
                                                                     std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const
{
    const auto inspectResult = InspectZoneHeaderIw4(header);
    if (!inspectResult)
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, GameId::IW4, inspectResult->m_generic_result.m_platform);
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolIW4>(zonePtr, 0);
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    // Skip unknown 1 byte field that the game ignores as well
    zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(1));

    // Skip timestamp
    zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(8));

    if (inspectResult->m_generic_result.m_platform == GamePlatform::XBOX)
    {

        // Xbox fastfiles have additional header data before the auth header:
        // - 4 bytes: language flags bitmask
        // - 4 bytes: image count
        // - sizeof(XAssetStreamFile) * image count (for each language in the bitmask)
        // - 4 bytes: unknown
        // - 4 bytes: unknown
        // struct XAssetStreamFile // sizeof=0xC
        // {
        //     unsigned int fileIndex;
        //     unsigned int offset;
        //     unsigned int offsetEnd;
        // };

        struct SkipXAssetStreamFiles : public ILoadingStep
        {
            void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
            {
                uint32_t languageFlags;
                stream.Load(&languageFlags, sizeof(languageFlags));
                languageFlags = endianness::FromBigEndian(languageFlags);

                uint32_t imageCount;
                stream.Load(&imageCount, sizeof(imageCount));
                imageCount = endianness::FromBigEndian(imageCount);

                // Count how many languages are set in the bitmask
                uint32_t languageCount = 0;
                for (int i = 0; i < 15; i++)
                {
                    if (languageFlags & (1 << i))
                    {
                        languageCount++;
                    }
                }

                // Skip image stream file data (12 bytes per image per language)
                const size_t imageDataSize = 12 * imageCount * languageCount;
                if (imageDataSize > 0)
                {
                    std::vector<uint8_t> tempBuffer(std::min(imageDataSize, size_t(8192)));
                    size_t skipped = 0;
                    while (skipped < imageDataSize)
                    {
                        const size_t toSkip = std::min(imageDataSize - skipped, tempBuffer.size());
                        stream.Load(tempBuffer.data(), toSkip);
                        skipped += toSkip;
                    }
                }

                // Skip the final 8 bytes (2 unknown 4-byte values)
                uint8_t finalBytes[8];
                stream.Load(finalBytes, sizeof(finalBytes));
            }
        };

        zoneLoader->AddLoadingStep(std::make_unique<SkipXAssetStreamFiles>());
    }

    // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
    AddAuthHeaderSteps(*inspectResult, *zoneLoader, fileName);

    zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorInflate(ZoneConstants::AUTHED_CHUNK_SIZE)));

    if (inspectResult->m_is_iw4x) // IW4x has one extra byte of padding here for protection purposes
    {
        zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorIW4xDecryption()));
        zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(1));
    }

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
