#include "ZoneLoaderFactoryIW5.h"

#include "ContentLoaderIW5.h"
#include "Game/GameLanguage.h"
#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/IW5/GameIW5.h"
#include "Game/IW5/IW5.h"
#include "Game/IW5/ZoneConstantsIW5.h"
#include "Loading/Processor/ProcessorAuthedBlocks.h"
#include "Loading/Processor/ProcessorCaptureData.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepLoadHash.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepRemoveProcessor.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepVerifyHash.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <type_traits>

using namespace IW5;

namespace
{
    bool CanLoad(const ZoneHeader& header, bool* isSecure, bool* isOfficial)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if (header.m_version != ZoneConstants::ZONE_VERSION)
        {
            return false;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_INFINITY_WARD, std::char_traits<char>::length(ZoneConstants::MAGIC_SIGNED_INFINITY_WARD)))
        {
            *isSecure = true;
            *isOfficial = true;
            return true;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, std::char_traits<char>::length(ZoneConstants::MAGIC_UNSIGNED)))
        {
            *isSecure = false;
            *isOfficial = true;
            return true;
        }

        return false;
    }

    void SetupBlock(ZoneLoader& zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_CALLBACK, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader.AddXBlock(XBLOCK_DEF(XFILE_BLOCK_SCRIPT, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    std::unique_ptr<cryptography::IPublicKeyAlgorithm> SetupRsa(const bool isOfficial)
    {
        if (isOfficial)
        {
            auto rsa = cryptography::CreateRsa(cryptography::HashingAlgorithm::RSA_HASH_SHA256, cryptography::RsaPaddingMode::RSA_PADDING_PSS);

            if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD, sizeof(ZoneConstants::RSA_PUBLIC_KEY_INFINITY_WARD)))
            {
                std::cerr << "Invalid public key for signature checking\n";
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

    void AddAuthHeaderSteps(const bool isSecure, const bool isOfficial, ZoneLoader& zoneLoader, std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!isSecure)
            return;

        // If file is signed setup a RSA instance.
        auto rsa = SetupRsa(isOfficial);

        zoneLoader.AddLoadingStep(std::make_unique<StepVerifyMagic>(ZoneConstants::MAGIC_AUTH_HEADER));
        zoneLoader.AddLoadingStep(std::make_unique<StepSkipBytes>(4)); // Skip reserved

        auto subHeaderHash = std::make_unique<StepLoadHash>(sizeof(DB_AuthHash::bytes), 1);
        auto* subHeaderHashPtr = subHeaderHash.get();
        zoneLoader.AddLoadingStep(std::move(subHeaderHash));

        auto subHeaderHashSignature = std::make_unique<StepLoadSignature>(sizeof(DB_AuthSignature::bytes));
        auto* subHeaderHashSignaturePtr = subHeaderHashSignature.get();
        zoneLoader.AddLoadingStep(std::move(subHeaderHashSignature));

        zoneLoader.AddLoadingStep(std::make_unique<StepVerifySignature>(std::move(rsa), subHeaderHashSignaturePtr, subHeaderHashPtr));

        auto subHeaderCapture = processor::CreateProcessorCaptureData(sizeof(DB_AuthSubHeader));
        auto* subHeaderCapturePtr = subHeaderCapture.get();
        zoneLoader.AddLoadingStep(std::make_unique<StepAddProcessor>(std::move(subHeaderCapture)));

        zoneLoader.AddLoadingStep(std::make_unique<StepVerifyFileName>(fileName, sizeof(DB_AuthSubHeader::fastfileName)));
        zoneLoader.AddLoadingStep(std::make_unique<StepSkipBytes>(4)); // Skip reserved

        auto masterBlockHashes =
            std::make_unique<StepLoadHash>(sizeof(DB_AuthHash::bytes), static_cast<unsigned>(std::extent_v<decltype(DB_AuthSubHeader::masterBlockHashes)>));
        auto* masterBlockHashesPtr = masterBlockHashes.get();
        zoneLoader.AddLoadingStep(std::move(masterBlockHashes));

        zoneLoader.AddLoadingStep(std::make_unique<StepVerifyHash>(cryptography::CreateSha256(), 0, subHeaderHashPtr, subHeaderCapturePtr));
        zoneLoader.AddLoadingStep(std::make_unique<StepRemoveProcessor>(subHeaderCapturePtr));

        // Skip the rest of the first chunk
        zoneLoader.AddLoadingStep(std::make_unique<StepSkipBytes>(ZoneConstants::AUTHED_CHUNK_SIZE - sizeof(DB_AuthHeader)));

        zoneLoader.AddLoadingStep(std::make_unique<StepAddProcessor>(
            processor::CreateProcessorAuthedBlocks(ZoneConstants::AUTHED_CHUNK_COUNT_PER_GROUP,
                                                   ZoneConstants::AUTHED_CHUNK_SIZE,
                                                   static_cast<unsigned>(std::extent_v<decltype(DB_AuthSubHeader::masterBlockHashes)>),
                                                   cryptography::CreateSha256(),
                                                   masterBlockHashesPtr)));
    }
} // namespace

std::unique_ptr<ZoneLoader> ZoneLoaderFactory::CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) const
{
    bool isSecure;
    bool isOfficial;

    // Check if this file is a supported IW4 zone.
    if (!CanLoad(header, &isSecure, &isOfficial))
        return nullptr;

    // Create new zone
    auto zone = std::make_unique<Zone>(fileName, 0, IGame::GetGameById(GameId::IW5));
    auto* zonePtr = zone.get();
    zone->m_pools = std::make_unique<GameAssetPoolIW5>(zonePtr, 0);
    zone->m_language = GameLanguage::LANGUAGE_NONE;

    // File is supported. Now setup all required steps for loading this file.
    auto zoneLoader = std::make_unique<ZoneLoader>(std::move(zone));

    SetupBlock(*zoneLoader);

    // Skip unknown 1 byte field that the game ignores as well
    zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(1));

    // Skip timestamp
    zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(8));

    // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
    AddAuthHeaderSteps(isSecure, isOfficial, *zoneLoader, fileName);

    zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(processor::CreateProcessorInflate(ZoneConstants::AUTHED_CHUNK_SIZE)));

    // Start of the XFile struct
    zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(8));
    // Skip size and externalSize fields since they are not interesting for us
    zoneLoader->AddLoadingStep(std::make_unique<StepAllocXBlocks>());

    // Start of the zone content
    zoneLoader->AddLoadingStep(std::make_unique<StepLoadZoneContent>(
        std::make_unique<ContentLoader>(*zonePtr), zonePtr, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK));

    return zoneLoader;
}
