#include "ZoneLoaderFactoryT6.h"
#include "Game/T6/T6.h"

#include "Loading/Steps/StepVerifyMagic.h"
#include "Loading/Steps/StepSkipBytes.h"
#include "Loading/Steps/StepVerifyFileName.h"
#include "Loading/Steps/StepLoadSignature.h"
#include "Loading/Steps/StepVerifySignature.h"
#include "Loading/Steps/StepAddProcessor.h"
#include "Loading/Steps/StepAllocXBlocks.h"

#include "Loading/Processor/ProcessorXChunks.h"
#include "Loading/Processor/XChunks/ChunkProcessorSalsa20.h"
#include "Loading/Processor/XChunks/ChunkProcessorInflate.h"

#include "Utils/ClassUtils.h"
#include <cassert>
#include "Loading/Steps/StepLoadZoneContent.h"
#include "ContentLoaderT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"

const std::string ZoneLoaderFactoryT6::MAGIC_SIGNED_TREYARCH = "TAff0100";
const std::string ZoneLoaderFactoryT6::MAGIC_SIGNED_ASSET_BUILDER = "ABff0100";
const std::string ZoneLoaderFactoryT6::MAGIC_UNSIGNED = "TAffu100";
const std::string ZoneLoaderFactoryT6::MAGIC_UNSIGNED_SERVER = "TAsvu100";
const int ZoneLoaderFactoryT6::VERSION = 147;

const int ZoneLoaderFactoryT6::STREAM_COUNT = 4;
const int ZoneLoaderFactoryT6::XCHUNK_SIZE = 0x8000;
const int ZoneLoaderFactoryT6::OFFSET_BLOCK_BIT_COUNT = 3;
const block_t ZoneLoaderFactoryT6::INSERT_BLOCK = T6::XFILE_BLOCK_VIRTUAL;

const std::string ZoneLoaderFactoryT6::MAGIC_AUTH_HEADER = "PHEEBs71";
const uint8_t ZoneLoaderFactoryT6::SALSA20_KEY_TREYARCH[]
{
	0x64, 0x1D, 0x8A, 0x2F,
	0xE3, 0x1D, 0x3A, 0xA6,
	0x36, 0x22, 0xBB, 0xC9,
	0xCE, 0x85, 0x87, 0x22,
	0x9D, 0x42, 0xB0, 0xF8,
	0xED, 0x9B, 0x92, 0x41,
	0x30, 0xBF, 0x88, 0xB6,
	0x5E, 0xDC, 0x50, 0xBE
};

const uint8_t ZoneLoaderFactoryT6::RSA_PUBLIC_KEY_TREYARCH[]
{
    0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
    0x00, 0xc7, 0x9d, 0x33, 0xe0, 0x75, 0xaf, 0xef,
    0x08, 0x08, 0x2b, 0x89, 0xd9, 0x3b, 0xf3, 0xd5,
    0x9a, 0x65, 0xa6, 0xde, 0x3b, 0x1e, 0x20, 0xde,
    0x59, 0x19, 0x43, 0x88, 0x1a, 0x8b, 0x39, 0x13,
    0x60, 0x12, 0xd3, 0xb2, 0x77, 0x6d, 0xe1, 0x99,
    0x75, 0x24, 0xb4, 0x0d, 0x8c, 0xb7, 0x84, 0xf2,
    0x48, 0x8f, 0xd5, 0x4c, 0xb7, 0x64, 0x44, 0xa3,
    0xa8, 0x4a, 0xac, 0x2d, 0x54, 0x15, 0x2b, 0x1f,
    0xb3, 0xf4, 0x4c, 0x16, 0xa0, 0x92, 0x8e, 0xd2,
    0xfa, 0xcc, 0x11, 0x6a, 0x74, 0x6a, 0x70, 0xb8,
    0xd3, 0x34, 0x6b, 0x39, 0xc6, 0x2a, 0x69, 0xde,
    0x31, 0x34, 0xdf, 0xe7, 0x8b, 0x7e, 0x17, 0xa3,
    0x17, 0xd9, 0x5e, 0x88, 0x39, 0x21, 0xf8, 0x7d,
    0x3c, 0x29, 0x21, 0x6c, 0x0e, 0xf1, 0xb4, 0x09,
    0x54, 0xe8, 0x20, 0x34, 0x90, 0x2e, 0xb4, 0x1a,
    0x95, 0x95, 0x90, 0xe5, 0xfb, 0xce, 0xfe, 0x8a,
    0xbf, 0xea, 0xaf, 0x09, 0x0c, 0x0b, 0x87, 0x22,
    0xe1, 0xfe, 0x82, 0x6e, 0x91, 0xe8, 0xd1, 0xb6,
    0x35, 0x03, 0x4f, 0xdb, 0xc1, 0x31, 0xe2, 0xba,
    0xa0, 0x13, 0xf6, 0xdb, 0x07, 0x9b, 0xcb, 0x99,
    0xce, 0x9f, 0x49, 0xc4, 0x51, 0x8e, 0xf1, 0x04,
    0x9b, 0x30, 0xc3, 0x02, 0xff, 0x7b, 0x94, 0xca,
    0x12, 0x69, 0x1e, 0xdb, 0x2d, 0x3e, 0xbd, 0x48,
    0x16, 0xe1, 0x72, 0x37, 0xb8, 0x5f, 0x61, 0xfa,
    0x24, 0x16, 0x3a, 0xde, 0xbf, 0x6a, 0x71, 0x62,
    0x32, 0xf3, 0xaa, 0x7f, 0x28, 0x3a, 0x0c, 0x27,
    0xeb, 0xa9, 0x0a, 0x4c, 0x79, 0x88, 0x84, 0xb3,
    0xe2, 0x52, 0xb9, 0x68, 0x1e, 0x82, 0xcf, 0x67,
    0x43, 0xf3, 0x68, 0xf7, 0x26, 0x19, 0xaa, 0xdd,
    0x3f, 0x1e, 0xc6, 0x46, 0x11, 0x9f, 0x24, 0x23,
    0xa7, 0xb0, 0x1b, 0x79, 0xa7, 0x0c, 0x5a, 0xfe,
    0x96, 0xf7, 0xe7, 0x88, 0x09, 0xa6, 0x69, 0xe3,
    0x8b, 0x02, 0x03, 0x01, 0x00, 0x01
};

class ZoneLoaderFactoryT6::ZoneLoaderFactoryT6Impl
{
    static bool CanLoad(ZoneHeader& header, bool* isSecure, bool* isOfficial, bool* isEncrypted)
    {
        assert(isSecure != nullptr);
        assert(isOfficial != nullptr);

        if(header.m_version != VERSION)
        {
            return false;
        }

        if(!memcmp(header.m_magic, MAGIC_SIGNED_TREYARCH.c_str(), 8))
        {
            *isSecure = true;
            *isOfficial = true;
            *isEncrypted = true;
            return true;
        }
        
        if(!memcmp(header.m_magic, MAGIC_SIGNED_ASSET_BUILDER.c_str(), 8))
        {
            *isSecure = true;
            *isOfficial = false;
            *isEncrypted = true;
            return true;
        }

        if(!memcmp(header.m_magic, MAGIC_UNSIGNED.c_str(), 8))
        {
            *isSecure = false;
            *isOfficial = true;
            *isEncrypted = true;
            return true;
        }

        if(!memcmp(header.m_magic, MAGIC_UNSIGNED_SERVER.c_str(), 8))
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
#define XBLOCK_DEF(name, type) new XBlock(STR(name), name, type)

        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_TEMP, XBlock::BLOCK_TYPE_TEMP));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_VIRTUAL, XBlock::BLOCK_TYPE_RUNTIME));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_RUNTIME_PHYSICAL, XBlock::BLOCK_TYPE_RUNTIME));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_VIRTUAL, XBlock::BLOCK_TYPE_DELAY));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_DELAY_PHYSICAL, XBlock::BLOCK_TYPE_DELAY));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_VIRTUAL, XBlock::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_PHYSICAL, XBlock::BLOCK_TYPE_NORMAL));
        zoneLoader->AddXBlock(XBLOCK_DEF(T6::XFILE_BLOCK_STREAMER_RESERVE, XBlock::BLOCK_TYPE_NORMAL));

#undef XBLOCK_DEF
    }

    static IPublicKeyAlgorithm* SetupRSA(const bool isOfficial)
    {
        if(isOfficial)
        {
            auto* rsa = Crypto::CreateRSA(IPublicKeyAlgorithm::RSA_HASH_SHA256, Crypto::RSA_PADDING_PSS);

            if(!rsa->SetKey(RSA_PUBLIC_KEY_TREYARCH, sizeof(RSA_PUBLIC_KEY_TREYARCH)))
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

    static ISignatureProvider* AddAuthHeaderSteps(const bool isSecure, ZoneLoader* zoneLoader, std::string& fileName)
    {
        // Unsigned zones do not have an auth header
        if(!isSecure)
            return nullptr;
        
        zoneLoader->AddLoadingStep(new StepVerifyMagic(MAGIC_AUTH_HEADER.c_str()));
        zoneLoader->AddLoadingStep(new StepSkipBytes(4)); // Loading Flags which are always zero
        zoneLoader->AddLoadingStep(new StepVerifyFileName(fileName, 32));

        auto* signatureLoadStep = new StepLoadSignature(256);
        zoneLoader->AddLoadingStep(signatureLoadStep);

        return signatureLoadStep;
    }

    static ISignatureDataProvider* AddXChunkProcessor(bool isEncrypted, ZoneLoader* zoneLoader, std::string& fileName)
    {
        ISignatureDataProvider* result = nullptr;
        auto* xChunkProcessor = new ProcessorXChunks(STREAM_COUNT, XCHUNK_SIZE);

        if(isEncrypted)
        {
            // If zone is encrypted, the decryption is applied before the decompression. T6 Zones always use Salsa20.
            auto* chunkProcessorSalsa20 = new ChunkProcessorSalsa20(STREAM_COUNT, fileName, SALSA20_KEY_TREYARCH, sizeof(SALSA20_KEY_TREYARCH));
            xChunkProcessor->AddChunkProcessor(chunkProcessorSalsa20);
            result = chunkProcessorSalsa20;
        }

        // Decompress the chunks using zlib
        xChunkProcessor->AddChunkProcessor(new ChunkProcessorInflate());
        zoneLoader->AddLoadingStep(new StepAddProcessor(xChunkProcessor));

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
        if(!CanLoad(header, &isSecure, &isOfficial, &isEncrypted))
            return nullptr;

        // Create new zone
        auto* zone = new Zone(fileName, 0, new GameAssetPoolT6(0), &game_t6);
        
        // File is supported. Now setup all required steps for loading this file.
        auto* zoneLoader = new ZoneLoader(zone);

        SetupBlock(zoneLoader);

        // If file is signed setup a RSA instance.
        IPublicKeyAlgorithm* rsa = isSecure ? SetupRSA(isOfficial) : nullptr;

        // Add steps for loading the auth header which also contain the signature of the zone if it is signed.
        ISignatureProvider* signatureProvider = AddAuthHeaderSteps(isSecure, zoneLoader, fileName);

        // Setup loading XChunks from the zone from this point on.
        ISignatureDataProvider* signatureDataProvider = AddXChunkProcessor(isEncrypted, zoneLoader, fileName);

        // Start of the XFile struct
        zoneLoader->AddLoadingStep(new StepSkipBytes(8)); // Skip size and externalSize fields since they are not interesting for us
        zoneLoader->AddLoadingStep(new StepAllocXBlocks());

        // Start of the zone content
        zoneLoader->AddLoadingStep(new StepLoadZoneContent(new ContentLoaderT6(), zone, OFFSET_BLOCK_BIT_COUNT, INSERT_BLOCK));

        if(isSecure)
        {
            zoneLoader->AddLoadingStep(new StepVerifySignature(rsa, signatureProvider, signatureDataProvider));
        }

        // Return the fully setup zoneloader
        return zoneLoader;
    }
};

ZoneLoader* ZoneLoaderFactoryT6::CreateLoaderForHeader(ZoneHeader& header, std::string& fileName)
{
    return ZoneLoaderFactoryT6Impl::CreateLoaderForHeader(header, fileName);
}