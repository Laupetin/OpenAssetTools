#include "ZoneLoaderFactoryT6.h"

#include <cassert>
#include <cstring>
#include <memory>

#include "Game/T6/T6.h"
#include "Game/T6/ZoneConstantsT6.h"

#include "Utils/ClassUtils.h"
#include "ContentLoaderT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/GameLanguage.h"
#include "Game/T6/GameT6.h"
#include "Loading/Processor/ProcessorXChunks.h"
#include "Zone/XChunk/XChunkProcessorSalsa20Decryption.h"
#include "Zone/XChunk/XChunkProcessorInflate.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadZoneSizes.h"

using namespace T6;

class ZoneLoaderFactory::Impl
{
    static GameLanguage GetZoneLanguage(std::string& zoneName)
    {
        auto languagePrefixes = g_GameT6.GetLanguagePrefixes();

        for (const auto& languagePrefix : languagePrefixes)
        {
            if (zoneName.compare(0, languagePrefix.m_prefix.length(), languagePrefix.m_prefix) == 0)
            {
                return languagePrefix.m_language;
            }
        }

        return GameLanguage::LANGUAGE_NONE;
    }

    static bool CanLoad(ZoneHeader& header, bool* isSecure, bool* isOfficial, bool* isEncrypted)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if (header.m_version != ZoneConstants::ZONE_VERSION)
        {
            return false;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_TREYARCH, 8))
        {
            *isSecure = true;
            *isOfficial = true;
            *isEncrypted = true;
            return true;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_SIGNED_OAT, 8))
        {
            *isSecure = true;
            *isOfficial = false;
            *isEncrypted = true;
            return true;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED, 8))
        {
            *isSecure = false;
            *isOfficial = true;
            *isEncrypted = true;
            return true;
        }

        if (!memcmp(header.m_magic, ZoneConstants::MAGIC_UNSIGNED_SERVER, 8))
        {
            *isSecure = false;
            *isOfficial = true;
            *isEncrypted = false;
            return true;
        }

        return false;
    }

    static void SetupBlock(ZoneLoader* zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_VIRTUAL, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_PHYSICAL, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_VIRTUAL, XBlock::Type::BLOCK_TYPE_DELAY));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_PHYSICAL, XBlock::Type::BLOCK_TYPE_DELAY));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_STREAMER_RESERVE, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    static std::unique_ptr<IPublicKeyAlgorithm> SetupRSA(const bool isOfficial)
    {
        if (isOfficial)
        {
            auto rsa = Crypto::CreateRSA(IPublicKeyAlgorithm::HashingAlgorithm::RSA_HASH_SHA256, Crypto::RSAPaddingMode::RSA_PADDING_PSS);

            if (!rsa->SetKey(ZoneConstants::RSA_PUBLIC_KEY_TREYARCH, sizeof(ZoneConstants::RSA_PUBLIC_KEY_TREYARCH)))
            {
                printf("Invalid public key for signature checking\n");
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

    static ISignatureProvider* AddAuthHeaderSteps(const bool isSecure, ZoneLoader* zoneLoader, std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!isSecure)
            return nullptr;

        zoneLoader->AddLoadingStep(std::make_unique<StepVerifyMagic>(ZoneConstants::MAGIC_AUTH_HEADER));
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(4)); // Loading Flags which are always zero
        zoneLoader->AddLoadingStep(std::make_unique<StepVerifyFileName>(fileName, 32));

        auto signatureLoadStep = std::make_unique<StepLoadSignature>(256);
        auto* signatureLoadStepPtr = signatureLoadStep.get();
        zoneLoader->AddLoadingStep(std::move(signatureLoadStep));

        return signatureLoadStepPtr;
    }

    static ICapturedDataProvider* AddXChunkProcessor(bool isEncrypted, ZoneLoader* zoneLoader, std::string& fileName)
    {
        ICapturedDataProvider* result = nullptr;
        auto xChunkProcessor = std::make_unique<ProcessorXChunks>(ZoneConstants::STREAM_COUNT, ZoneConstants::XCHUNK_SIZE, ZoneConstants::VANILLA_BUFFER_SIZE);

        if (isEncrypted)
        {
            // If zone is encrypted, the decryption is applied before the decompression. T6 Zones always use Salsa20.
            auto chunkProcessorSalsa20 = std::make_unique<XChunkProcessorSalsa20Decryption>(ZoneConstants::STREAM_COUNT, fileName, ZoneConstants::SALSA20_KEY_TREYARCH,
                                                                                 sizeof(ZoneConstants::SALSA20_KEY_TREYARCH));
            result = chunkProcessorSalsa20.get();
            xChunkProcessor->AddChunkProcessor(std::move(chunkProcessorSalsa20));
        }

        // Decompress the chunks using zlib
        xChunkProcessor->AddChunkProcessor(std::make_unique<XChunkProcessorInflate>());
        zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(std::move(xChunkProcessor)));

        // If there is encryption, the signed data of the zone is the final hash blocks provided by the Salsa20 IV adaption algorithm
        return result;
    }

public:
    static ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName)
    {
        bool isSecure;
        bool isOfficial;
        bool isEncrypted;

        // Check if this file is a supported T6 zone.
        if (!CanLoad(header, &isSecure, &isOfficial, &isEncrypted))
            return nullptr;

        // Create new zone
        auto zone = std::make_unique<Zone>(fileName, 0, &g_GameT6);
        auto* zonePtr = zone.get();
        zone->m_pools = std::make_unique<GameAssetPoolT6>(zonePtr, 0);
        zone->m_language = GetZoneLanguage(fileName);

        // File is supported. Now setup all required steps for loading this file.
        auto* zoneLoader = new ZoneLoader(std::move(zone));

        SetupBlock(zoneLoader);

        // If file is signed setup a RSA instance.
        auto rsa = isSecure ? SetupRSA(isOfficial) : nullptr;

        // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
        ISignatureProvider* signatureProvider = AddAuthHeaderSteps(isSecure, zoneLoader, fileName);

        // Setup loading XChunks from the zone from this point on.
        ICapturedDataProvider* signatureDataProvider = AddXChunkProcessor(isEncrypted, zoneLoader, fileName);

        // Start of the XFile struct
        zoneLoader->AddLoadingStep(std::make_unique<StepLoadZoneSizes>());
        zoneLoader->AddLoadingStep(std::make_unique<StepAllocXBlocks>());

        // Start of the zone content
        zoneLoader->AddLoadingStep(std::make_unique<StepLoadZoneContent>(std::make_unique<ContentLoader>(), zonePtr, ZoneConstants::OFFSET_BLOCK_BIT_COUNT, ZoneConstants::INSERT_BLOCK));

        if (isSecure)
        {
            zoneLoader->AddLoadingStep(std::make_unique<StepVerifySignature>(std::move(rsa), signatureProvider, signatureDataProvider));
        }

        // Return the fully setup zoneloader
        return zoneLoader;
    }
};

ZoneLoader* ZoneLoaderFactory::CreateLoaderForHeader(ZoneHeader& header, std::string& fileName)
{
    return Impl::CreateLoaderForHeader(header, fileName);
}
