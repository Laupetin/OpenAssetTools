#include "DynamicAssetsXModel.h"

#include "Context/ModManContext.h"
#include "Game/CommonAsset.h"
#include "Pool/XAssetInfo.h"
#include "Utils/Logging/Log.h"
#include "XModel/Gltf/GltfBinOutput.h"
#include "XModel/Gltf/GltfWriter.h"
#include "XModel/XModelToCommonConverter.h"

#include <sstream>

using namespace xmodel;

namespace
{
    bool FindXModel(const std::string& zoneName, const std::string& assetName, XAssetInfoGeneric*& outAssetInfo, Zone*& outZone)
    {
        auto& context = ModManContext::Get().m_fast_file;
        const auto loadedZones = context.GetLoadedZones();
        for (const auto& loadedZone : loadedZones.Data())
        {
            const auto& zone = loadedZone->GetZone();
            if (zone.m_name != zoneName)
                continue;

            const auto* assetTypeMapper = ICommonAssetTypeMapper::GetCommonAssetMapperByGame(zone.m_game_id);
            const auto gameAssetType = assetTypeMapper->CommonToGameAssetType(CommonAssetType::XMODEL);
            if (!gameAssetType)
                continue;

            outAssetInfo = zone.m_pools.GetAsset(*gameAssetType, assetName);
            if (outAssetInfo)
            {
                outZone = &loadedZone->GetZone();
                return true;
            }
        }

        return false;
    }

    void XModelGlb(const webwindowed::dynamic_asset_request& request, webwindowed::dynamic_asset_response& response)
    {
        const auto modelName = request.get_query("name");
        const auto zoneName = request.get_query("zone");
        if (!modelName.has_value() || modelName->empty() || !zoneName.has_value() || zoneName->empty())
        {
            con::error("Bad glb request (name={} zone={})", modelName.value_or(""), zoneName.value_or(""));
            response.send_response(400);
            return;
        }

        XAssetInfoGeneric* model;
        Zone* zone;
        if (!FindXModel(*zoneName, *modelName, model, zone))
        {
            con::warn("Could not find xmodel {} of zone {}", *modelName, *zoneName);
            response.send_response(404);
            return;
        }

        assert(model);
        assert(zone);

        const auto gameName = GameId_Names[std::to_underlying(zone->m_game_id)];
        const auto converter = ToCommonConverter::GetForGame(zone->m_game_id);
        if (!converter)
        {
            con::error("No xmodel converter for game {}", gameName);
            response.send_response(500);
            return;
        }

        const auto maybeCommon = converter->Convert(*model, 0);
        if (!maybeCommon)
        {
            con::warn("Failed to convert xmodel {} of zone {}", *modelName, *zoneName);
            response.send_response(500);
            return;
        }

        std::ostringstream ss;
        const gltf::BinOutput output(ss);
        const auto gltfWriter = gltf::Writer::CreateWriter(&output, gameName, *zoneName);
        gltfWriter->Write(*maybeCommon);

        const auto data = ss.str();
        response.set_content_type("model/gltf-binary");
        response.send_response(data.data(), data.size());
    }
} // namespace

namespace xmodel
{
    void RegisterDynamicAssets(webwindowed::asset_handler_plugin& assetHandler)
    {
        assetHandler.add_dynamic_asset(webwindowed::dynamic_asset("xmodel/glb", XModelGlb));
    }
} // namespace xmodel
