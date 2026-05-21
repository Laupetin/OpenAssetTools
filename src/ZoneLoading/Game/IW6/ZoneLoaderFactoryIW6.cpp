#include "ZoneLoaderFactoryIW6.h"

#include "ContentLoaderIW6.h"
#include "Game/GameLanguage.h"
#include "Game/IW6/GameIW6.h"
#include "Game/IW6/IW6.h"
#include "Game/IW6/ZoneConstantsIW6.h"
#include "Loading/Processor/ProcessorAuthedBlocks.h"
#include "Loading/Processor/ProcessorCaptureData.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepCallback.h"
#include "Loading/Steps/StepDumpData.h"
#include "Loading/Steps/StepLoadHash.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"
#include "Loading/Steps/StepReadValue.h"
#include "Loading/Steps/StepRemoveProcessor.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepVerifyHash.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Utils/ClassUtils.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <type_traits>

#define DO_RAW_DUMP

using namespace IW6;
namespace fs = std::filesystem;

namespace
{
    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_TEMP, XBlockType::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_PHYSICAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_RUNTIME, XBlockType::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VIRTUAL, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_LARGE, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_CALLBACK, XBlockType::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_SCRIPT, XBlockType::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    std::unique_ptr<cryptography::IPublicKeyAlgorithm> SetupRsa(const bool isOfficial)
    {
        if (isOfficial)
        {
            auto rsa = cryptography::CreateRsa(cryptography::HashingAlgorithm::RSA_HASH_SHA256, cryptography::RsaPaddingMode::RSA_PADDING_PSS);

            if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD, sizeof(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD)))
            {
                con::error("Invalid public key for signature checking");
                return nullptr;
            }

            return std::move(rsa);
        }
        else
        {
            assert(false);

            // TODO: Load custom RSA key here
            return nullptr;
        }
    }

    void AddAuthHeaderSteps(const ZoneLoaderInspectionResult& inspectResult, ZoneLoader& zoneLoader, const std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!inspectResult.m_is_signed)
            return;

        // If file is signed setup a RSA instance.
        auto rsa = SetupRsa(inspectResult.m_is_official);

        auto subHeaderHash = step::CreateStepLoadHash(sizeof(DB_AuthHash::bytes), 1);
        auto* subHeaderHashPtr = subHeaderHash.get();
        zoneLoader.AddLoadingStep(std::move(subHeaderHash));

        zoneLoader.AddLoadingStep(step::CreateStepVerifyMagic(ZoneConstants::MAGIC_AUTH_HEADER));
        zoneLoader.AddLoadingStep(step::CreateStepSkipBytes(4)); // Skip reserved

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

    struct IW6HeaderState
    {
        uint8_t isOnline{};
        uint8_t compressionType{};
        uint8_t pointerSize{};
        uint8_t longSize{};

        uint32_t unknown0{};
        uint32_t languageMask{};
        uint32_t entryCount{};
        uint32_t unknown1{};
    };

    enum FFCompressionType
    {
        FF_COMPRESSION_ZLIB = 0x0,
        FF_COMPRESSION_LZX = 0x1,
    };
} // namespace

std::optional<ZoneLoaderInspectionResult> ZoneLoaderFactory::InspectZoneHeader(const ZoneHeader& header) const
{
    if (header.m_version != ZoneConstants::ZONE_VERSION)
        return std::nullopt;

    if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_INFINITY_WARD, std::char_traits<char>::length(ZoneConstants::MAGIC_SIGNED_INFINITY_WARD)))
    {
        return ZoneLoaderInspectionResult{
            .m_game_id = GameId::IW6,
            .m_endianness = GameEndianness::LE,
            .m_word_size = GameWordSize::ARCH_64,
            .m_platform = GamePlatform::PC,
            .m_is_official = true,
            .m_is_signed = true,
            .m_is_encrypted = false,
        };
    }

    if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
    {
        return ZoneLoaderInspectionResult{
            .m_game_id = GameId::IW6,
            .m_endianness = GameEndianness::LE,
            .m_word_size = GameWordSize::ARCH_64,
            .m_platform = GamePlatform::PC,
            .m_is_official = false,
            .m_is_signed = false,
            .m_is_encrypted = false,
        };
    }

    return std::nullopt;
}

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(const ZoneHeader& header,
                                                                     const std::string& fileName,
                                                                     std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const
{
    const auto inspectResult = InspectZoneHeader(header);
    if (!inspectResult)
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, GameId::IW6, inspectResult->m_platform);
    auto* zonePtr = zone.get();
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    auto state = std::make_shared<IW6HeaderState>();
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->isOnline));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->compressionType));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->pointerSize));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->longSize));

    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->unknown0));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->languageMask));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->entryCount));
    zoneLoader->AddLoadingStep(step::CreateStepReadValue(&state->unknown1));

    zoneLoader->AddLoadingStep(step::CreateStepCallback(
        [state](ZoneLoader& zoneLoader, ILoadingStream& stream)
        {
            stream.Skip(state->entryCount * 24);
        }));

    zoneLoader->AddLoadingStep(step::CreateStepSkipBytes(12));

    // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
    AddAuthHeaderSteps(*inspectResult, *zoneLoader, fileName);

    auto compressionType = state.get()->compressionType;
    if (compressionType == FF_COMPRESSION_ZLIB)
    {
        zoneLoader->AddLoadingStep(step::CreateStepAddProcessor(processor::CreateProcessorInflate(20000000)));
    }
    else if (compressionType == FF_COMPRESSION_LZX)
    {
        con::warn("LZX not handled yet!");
    }
    else
    {
        con::warn("Unknown compression: {}", compressionType);
    }

    // Start of the XFile struct
    zoneLoader->AddLoadingStep(step::CreateStepLoadZoneSizes());
    zoneLoader->AddLoadingStep(step::CreateStepAllocXBlocks());

    // Start of the zone content
    zoneLoader->AddLoadingStep(step::CreateStepLoadZoneContent(
        [zonePtr](ZoneInputStream& stream)
        {
            return std::make_unique<ContentLoader>(*zonePtr, stream);
        },
        64u,
        ZoneConstants::OFFSET_BLOCK_BIT_COUNT,
        ZoneConstants::INSERT_BLOCK,
        zonePtr->Memory(),
        std::move(progressCallback)));

    return zoneLoader;
}
