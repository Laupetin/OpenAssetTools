#include "ZoneLoaderFactoryIW4.h"

#include <cassert>
#include <cstring>
#include <type_traits>

#include "Game/IW4/IW4.h"

#include "Utils/ClassUtils.h"
#include "ContentLoaderIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/GameIW4.h"
#include "Game/GameLanguage.h"
#include "Loading/Processor/ProcessorAuthedBlocks.h"
#include "Loading/Processor/ProcessorCaptureData.h"
#include "Loading/Processor/ProcessorInflate.h"
#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"
#include "Loading/Steps/StepLoadZoneContent.h"
#include "Loading/Steps/StepLoadHash.h"
#include "Loading/Steps/StepRemoveProcessor.h"
#include "Loading/Steps/StepVerifyHash.h"


using namespace IW4;

const std::string ZoneLoaderFactory::MAGIC_SIGNED_INFINITY_WARD = "IWff0100";
const std::string ZoneLoaderFactory::MAGIC_UNSIGNED = "IWffu100";
const int ZoneLoaderFactory::VERSION = 276;

const std::string ZoneLoaderFactory::MAGIC_AUTH_HEADER = "IWffs100";
const uint8_t ZoneLoaderFactory::RSA_PUBLIC_KEY_INFINITY_WARD[]
{
    0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01,
    0x00, 0xA5, 0x86, 0xCC, 0x18, 0xA9, 0x12, 0x17,
    0x4F, 0x3A, 0xC9, 0x0C, 0xD2, 0x38, 0x5D, 0xDB,
    0x67, 0x62, 0xA4, 0xE3, 0xD4, 0x42, 0x05, 0x8A,
    0x57, 0x0C, 0x31, 0x4E, 0x19, 0xE4, 0xBA, 0x89,
    0x73, 0x13, 0xDB, 0x72, 0x25, 0x63, 0xB1, 0x2F,
    0xD7, 0xF1, 0x08, 0x48, 0x34, 0x06, 0xD7, 0x84,
    0x5F, 0xC8, 0xCF, 0x2F, 0xB6, 0xA3, 0x5A, 0x8F,
    0x7E, 0xAA, 0x9D, 0x51, 0xE7, 0x0F, 0xB9, 0x07,
    0xB7, 0x30, 0x91, 0x04, 0x39, 0x9C, 0xDC, 0x1C,
    0xF1, 0x16, 0xCB, 0x96, 0x10, 0xEB, 0x38, 0xB1,
    0x3B, 0xBA, 0x42, 0xE3, 0xE2, 0x78, 0xBD, 0x77,
    0x82, 0x1A, 0x2B, 0x54, 0x27, 0x0A, 0xF7, 0x66,
    0x06, 0xAD, 0x46, 0x39, 0xC0, 0xEB, 0xB5, 0xC2,
    0x27, 0xDD, 0x2C, 0x08, 0x62, 0x2C, 0x0B, 0xC4,
    0x00, 0x0D, 0xCB, 0xAD, 0x22, 0x67, 0x01, 0xA6,
    0x92, 0x9C, 0x00, 0xAF, 0x9D, 0x55, 0xCC, 0x68,
    0xEC, 0x39, 0x49, 0x85, 0x7E, 0x2C, 0x98, 0xCF,
    0x4C, 0x12, 0x8D, 0xED, 0xC7, 0x1D, 0x21, 0x54,
    0x9C, 0x2F, 0xC9, 0x54, 0x36, 0x08, 0xA9, 0x67,
    0xEE, 0x91, 0xE6, 0xD9, 0xB1, 0xFA, 0xA9, 0x2B,
    0x88, 0xAD, 0x2A, 0xD0, 0xAA, 0x28, 0xF9, 0x47,
    0xA6, 0x0F, 0xCF, 0x55, 0x4C, 0x9B, 0x26, 0x41,
    0x89, 0x76, 0x11, 0xFD, 0x1B, 0x83, 0xE4, 0xE8,
    0x8E, 0x7E, 0xB4, 0x03, 0xA3, 0x29, 0xDD, 0x4F,
    0xAC, 0x99, 0xBE, 0x7C, 0xD3, 0xFD, 0x14, 0x28,
    0x1C, 0x59, 0x69, 0xE0, 0x79, 0x5F, 0x4B, 0xDA,
    0x6B, 0xAB, 0x48, 0x4E, 0x28, 0x39, 0x84, 0xC6,
    0x2B, 0xC6, 0x20, 0x05, 0xDB, 0x05, 0x21, 0xC3,
    0xE1, 0xD4, 0x20, 0x28, 0xDD, 0x3A, 0x4D, 0x51,
    0xE7, 0x49, 0x8A, 0x49, 0xEF, 0xF5, 0xDA, 0xDA,
    0x7D, 0x5D, 0xA8, 0x0B, 0xA1, 0x77, 0xCD, 0x62,
    0x7D, 0x9D, 0x40, 0x26, 0x44, 0x4B, 0x3B, 0x0A,
    0x89, 0x02, 0x03, 0x01, 0x00, 0x01
};

const size_t ZoneLoaderFactory::AUTHED_CHUNK_SIZE = 0x2000;
const size_t ZoneLoaderFactory::AUTHED_CHUNK_COUNT_PER_GROUP = 256;

const int ZoneLoaderFactory::OFFSET_BLOCK_BIT_COUNT = 4;
const block_t ZoneLoaderFactory::INSERT_BLOCK = XFILE_BLOCK_VIRTUAL;

class ZoneLoaderFactory::Impl
{
    static GameLanguage GetZoneLanguage(std::string& zoneName)
    {
        return GameLanguage::LANGUAGE_NONE;
    }

    static bool CanLoad(ZoneHeader& header, bool* isSecure, bool* isOfficial)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if (header.m_version != VERSION)
        {
            return false;
        }

        if (!memcmp(header.m_magic, MAGIC_SIGNED_INFINITY_WARD.c_str(), 8))
        {
            *isSecure = true;
            *isOfficial = true;
            return true;
        }

        if (!memcmp(header.m_magic, MAGIC_UNSIGNED.c_str(), 8))
        {
            *isSecure = false;
            *isOfficial = true;
            return true;
        }

        return false;
    }

    static void SetupBlock(ZoneLoader* zoneLoader)
    {
#define XBLOCK_DEF(name, type) std::make_unique<XBlock>(STR(name), name, type)

        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_TEMP, XBlock::Type::BLOCK_TYPE_TEMP));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_PHYSICAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_RUNTIME, XBlock::Type::BLOCK_TYPE_RUNTIME));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_VIRTUAL, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_LARGE, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_CALLBACK, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_VERTEX, XBlock::Type::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(IW4::XFILE_BLOCK_INDEX, XBlock::Type::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    static IPublicKeyAlgorithm* SetupRSA(const bool isOfficial)
    {
        if (isOfficial)
        {
            auto* rsa = Crypto::CreateRSA(IPublicKeyAlgorithm::HashingAlgorithm::RSA_HASH_SHA256,
                                          Crypto::RSAPaddingMode::RSA_PADDING_PSS);

            if (!rsa->SetKey(RSA_PUBLIC_KEY_INFINITY_WARD, sizeof(RSA_PUBLIC_KEY_INFINITY_WARD)))
            {
                printf("Invalid public key for signature checking\n");

                delete rsa;
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

    static void AddAuthHeaderSteps(const bool isSecure, const bool isOfficial, ZoneLoader* zoneLoader,
                                   std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if (!isSecure)
            return;

        // If file is signed setup a RSA instance.
        IPublicKeyAlgorithm* rsa = SetupRSA(isOfficial);

        zoneLoader->AddLoadingStep(std::make_unique<StepVerifyMagic>(MAGIC_AUTH_HEADER.c_str()));
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(4)); // Skip reserved

        auto subHeaderHash = std::make_unique<StepLoadHash>(sizeof DB_AuthHash::bytes, 1);
        auto* subHeaderHashPtr = subHeaderHash.get();
        zoneLoader->AddLoadingStep(std::move(subHeaderHash));

        auto subHeaderHashSignature = std::make_unique<StepLoadSignature>(sizeof DB_AuthSignature::bytes);
        auto* subHeaderHashSignaturePtr = subHeaderHashSignature.get();
        zoneLoader->AddLoadingStep(std::move(subHeaderHashSignature));

        zoneLoader->AddLoadingStep(std::make_unique<StepVerifySignature>(rsa, subHeaderHashSignaturePtr, subHeaderHashPtr));

        auto subHeaderCapture = std::make_unique<ProcessorCaptureData>(sizeof(DB_AuthSubHeader));
        auto* subHeaderCapturePtr = subHeaderCapture.get();
        zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(std::move(subHeaderCapture)));

        zoneLoader->AddLoadingStep(std::make_unique<StepVerifyFileName>(fileName, sizeof(DB_AuthSubHeader::fastfileName)));
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(4)); // Skip reserved

        auto masterBlockHashes = std::make_unique<StepLoadHash>(sizeof DB_AuthHash::bytes, std::extent<decltype(DB_AuthSubHeader::masterBlockHashes)>::value);
        auto* masterBlockHashesPtr = masterBlockHashes.get();
        zoneLoader->AddLoadingStep(std::move(masterBlockHashes));

        zoneLoader->AddLoadingStep(std::make_unique<StepRemoveProcessor>(subHeaderCapturePtr));
        zoneLoader->AddLoadingStep(std::make_unique<StepVerifyHash>(std::unique_ptr<IHashFunction>(Crypto::CreateSHA256()), 0, subHeaderHashPtr, subHeaderCapturePtr));

        // Skip the rest of the first chunk
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(AUTHED_CHUNK_SIZE - sizeof(DB_AuthHeader)));

        zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(std::make_unique<ProcessorAuthedBlocks>(
            AUTHED_CHUNK_COUNT_PER_GROUP, AUTHED_CHUNK_SIZE, std::extent<decltype(DB_AuthSubHeader::masterBlockHashes)>::value,
            std::unique_ptr<IHashFunction>(Crypto::CreateSHA256()), masterBlockHashesPtr)));
    }

public:
    static ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName)
    {
        bool isSecure;
        bool isOfficial;

        // Check if this file is a supported IW4 zone.
        if (!CanLoad(header, &isSecure, &isOfficial))
            return nullptr;

        // Create new zone
        auto* zone = new Zone(fileName, 0, &g_GameIW4);
        zone->m_pools = std::make_unique<GameAssetPoolIW4>(zone, 0);
        zone->m_language = GetZoneLanguage(fileName);

        // File is supported. Now setup all required steps for loading this file.
        auto* zoneLoader = new ZoneLoader(zone);

        SetupBlock(zoneLoader);

        // Skip unknown 1 byte field that the game ignores as well
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(1));

        // Skip timestamp
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(8));

        // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
        AddAuthHeaderSteps(isSecure, isOfficial, zoneLoader, fileName);

        zoneLoader->AddLoadingStep(std::make_unique<StepAddProcessor>(std::make_unique<ProcessorInflate>(AUTHED_CHUNK_SIZE)));

        // Start of the XFile struct
        zoneLoader->AddLoadingStep(std::make_unique<StepSkipBytes>(8));
        // Skip size and externalSize fields since they are not interesting for us
        zoneLoader->AddLoadingStep(std::make_unique<StepAllocXBlocks>());

        // Start of the zone content
        zoneLoader->AddLoadingStep(std::make_unique<StepLoadZoneContent>(std::make_unique<ContentLoader>(), zone, OFFSET_BLOCK_BIT_COUNT, INSERT_BLOCK));

        // Return the fully setup zoneloader
        return zoneLoader;
    }
};

ZoneLoader* ZoneLoaderFactory::CreateLoaderForHeader(ZoneHeader& header, std::string& fileName)
{
    return Impl::CreateLoaderForHeader(header, fileName);
}
