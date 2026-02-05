#include "ObjWriter.h"

#include "Game/IW3/ObjWriterIW3.h"
#include "Game/IW4/ObjWriterIW4.h"
#include "Game/IW5/ObjWriterIW5.h"
#include "Game/T5/ObjWriterT5.h"
#include "Game/T6/ObjWriterT6.h"
#include "ObjWriting.h"

#include <cassert>

bool IObjWriter::DumpZone(AssetDumpingContext& context)
{
    m_asset_dumpers.clear();
    RegisterAssetDumpers(context);

    if (context.ShouldTrackProgress())
    {
        size_t totalProgress = 0uz;
        for (const auto& dumper : m_asset_dumpers)
            totalProgress += dumper->GetProgressTotalCount(context);

        context.SetTotalProgress(totalProgress);
    }

    for (const auto& dumper : m_asset_dumpers)
        dumper->Dump(context);

    return true;
}

void IObjWriter::RegisterAssetDumper(std::unique_ptr<IAssetDumper> dumper)
{
    const auto maybeHandlingAssetType = dumper->GetHandlingAssetType();
    if (maybeHandlingAssetType.has_value() && !ObjWriting::ShouldHandleAssetType(*maybeHandlingAssetType))
        return;

    m_asset_dumpers.emplace_back(std::move(dumper));
}

IObjWriter* IObjWriter::GetObjWriterForGame(GameId game)
{
    static IObjWriter* objWriters[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ObjWriter(),
        new IW4::ObjWriter(),
        new IW5::ObjWriter(),
        new T5::ObjWriter(),
        new T6::ObjWriter(),
    };
    static_assert(std::extent_v<decltype(objWriters)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    auto* result = objWriters[static_cast<unsigned>(game)];
    assert(result);

    return result;
}
