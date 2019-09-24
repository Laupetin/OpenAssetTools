#include "rawfile_load_db.h"
#include <cassert>

using namespace T6;

RawFileLoader::RawFileLoader(IZoneScriptStringProvider* scriptStringProvider, Zone* zone, IZoneInputStream* stream) 
    : AssetLoader(ASSET_TYPE_RAWFILE, scriptStringProvider, zone, stream){}

void RawFileLoader::LoadRawFile(RawFile* pRawFile, const bool atStreamStart)
{
    assert(pRawFile != nullptr);

    if(atStreamStart)
        m_stream->Load<RawFile>();

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    LoadXString(&pRawFile->name);

    if(pRawFile->buffer != nullptr)
    {
        assert(pRawFile->buffer == PTR_FOLLOWING);

        pRawFile->buffer = m_stream->Alloc<const char>(16);
        m_stream->Load<const char>(pRawFile->len + 1);
    }

    m_stream->PopBlock();
}

void RawFileLoader::LoadRawFileAsset(RawFile** pPtr)
{
    assert(pPtr != nullptr);
    *pPtr = static_cast<RawFile*>(LinkAsset(GetRawFileName(*pPtr), *pPtr));
}

void RawFileLoader::LoadRawFilePtr(RawFile** pPtr)
{
    assert(pPtr != nullptr);
    
    m_stream->PushBlock(XFILE_BLOCK_TEMP);

    if(*pPtr != nullptr)
    {
        if(*pPtr == PTR_FOLLOWING || *pPtr == PTR_INSERT)
        {
            RawFile** toInsert = nullptr;
            if(*pPtr == PTR_INSERT)
                toInsert = m_stream->InsertPointer<RawFile>();

            *pPtr = m_stream->Alloc<RawFile>();
            LoadRawFile(*pPtr, true);
            LoadRawFileAsset(pPtr);

            if(toInsert != nullptr)
                *toInsert = *pPtr;
        }
        else
        {
            *pPtr = m_stream->ConvertOffsetToAlias(*pPtr);
        }
    }

    m_stream->PopBlock();
}

std::string RawFileLoader::GetRawFileName(RawFile* pRawFile)
{
    return pRawFile->name;
}