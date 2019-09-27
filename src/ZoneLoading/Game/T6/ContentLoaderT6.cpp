#include "ContentLoaderT6.h"
#include "Game/T6/T6.h"
#include "Loading/Exception/UnsupportedAssetTypeException.h"

#include <cassert>

#include "Game/T6/XAssets/rawfile/rawfile_load_db2.h"

using namespace T6;

ContentLoaderT6::ContentLoaderT6()
{
    // Script String 0 is always empty string
    m_script_strings.emplace_back("");
}

void ContentLoaderT6::LoadScriptStringList(ScriptStringList* scriptStringList)
{
    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    if(scriptStringList->strings != nullptr)
    {
        assert(scriptStringList->strings == PTR_FOLLOWING);

        scriptStringList->strings = m_stream->Alloc<const char*>();
        LoadXStringArray(scriptStringList->strings, scriptStringList->count, true);

        for(int i = 0; i < scriptStringList->count; i++)
        {
            m_script_strings.emplace_back(scriptStringList->strings[i]);
        }
    }

    m_stream->PopBlock();
}

void ContentLoaderT6::LoadXAsset(XAsset* pXAsset, const bool atStreamStart)
{
    assert(pXAsset != nullptr);

    if(atStreamStart)
        m_stream->Load<XAsset>();

    switch(pXAsset->type)
    {
    case ASSET_TYPE_RAWFILE:
        {
            RawFileLoader rawFileLoader(this, m_zone, m_stream);

            rawFileLoader.LoadRawFilePtr(&pXAsset->header.rawfile);
            break;
        }

    default:
        {
            throw UnsupportedAssetTypeException(pXAsset->type);
        }
    }
}

void ContentLoaderT6::LoadXAssetArray(XAsset* pArray, const size_t count, const bool atStreamStart)
{
    assert(pArray != nullptr);

    if(atStreamStart)
        m_stream->Load<XAsset>(count);

    size_t assetCounts[ASSET_TYPE_COUNT]{0};

    for(size_t index = 0; index < count; index++)
    {
        assert(pArray[index].type >= 0 && pArray[index].type < ASSET_TYPE_COUNT);

        if(pArray[index].type >= 0 && pArray[index].type < ASSET_TYPE_COUNT)
        {
            assetCounts[pArray[index].type]++;
        }
    }

    // Special case: CLIPMAP and CLIPMAP_PVS are the same struct and therefore share the same asset pool
    assetCounts[ASSET_TYPE_CLIPMAP_PVS] += assetCounts[ASSET_TYPE_CLIPMAP];
    assetCounts[ASSET_TYPE_CLIPMAP] = 0;

    for(asset_type_t assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        m_zone->GetPools()->InitPoolStatic(assetType, assetCounts[assetType]);
    }

    for(size_t index = 0; index < count; index++)
    {
        LoadXAsset(&pArray[index], false);
    }
}

void ContentLoaderT6::Load(Zone* zone, IZoneInputStream* stream)
{
    m_zone = zone;
    m_stream = stream;

    m_stream->PushBlock(XFILE_BLOCK_VIRTUAL);

    auto* assetList = m_stream->Alloc<XAssetList>();
    m_stream->Load<XAssetList>();

    LoadScriptStringList(&assetList->stringList);

    if(assetList->depends != nullptr)
    {
        assert(assetList->depends == PTR_FOLLOWING);

        assetList->depends = m_stream->Alloc<const char*>();
        LoadXStringArray(assetList->depends, assetList->dependCount, true);
    }

    if(assetList->assets != nullptr)
    {
        assert(assetList->assets == PTR_FOLLOWING);

        assetList->assets = m_stream->Alloc<XAsset>();
        LoadXAssetArray(assetList->assets, assetList->assetCount, true);
    }

    m_stream->PopBlock();
}

std::string& ContentLoaderT6::GetZoneScriptString(const scr_string_t scrString)
{
    assert(scrString >= 0 && scrString < m_script_strings.size());

    if(scrString < 0 || scrString >= m_script_strings.size())
    {
        return m_script_strings[0];
    }

    return m_script_strings[scrString];
}