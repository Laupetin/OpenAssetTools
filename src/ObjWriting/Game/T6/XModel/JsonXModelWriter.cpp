#include "JsonXModelWriter.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonDumper
    {
    public:
        JsonDumper(AssetDumpingContext& context, std::ostream& stream)
            : m_stream(stream)
        {
        }

        void Dump(const XModel* xmodel) const
        {
            JsonXModel jsonXModel;
            CreateJsonXModel(jsonXModel, *xmodel);
            json jRoot = jsonXModel;

            jRoot["_type"] = "xmodel";
            jRoot["_version"] = 1;

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static const char* AssetName(const char* input)
        {
            if (input && input[0] == ',')
                return &input[1];

            return input;
        }

        void CreateJsonXModel(JsonXModel& jXModel, const XModel& xmodel) const
        {
            jXModel.collLod = xmodel.collLod;

            if (xmodel.physPreset && xmodel.physPreset->name)
                jXModel.physPreset = AssetName(xmodel.physPreset->name);

            if (xmodel.physConstraints && xmodel.physConstraints->name)
                jXModel.physConstraints = AssetName(xmodel.physConstraints->name);

            jXModel.flags = xmodel.flags;
            jXModel.lightingOriginOffset.x = xmodel.lightingOriginOffset.x;
            jXModel.lightingOriginOffset.y = xmodel.lightingOriginOffset.y;
            jXModel.lightingOriginOffset.z = xmodel.lightingOriginOffset.z;
            jXModel.lightingOriginRange = xmodel.lightingOriginRange;
        }

        std::ostream& m_stream;
    };
} // namespace

namespace T6
{
    void DumpXModelAsJson(std::ostream& stream, const XModel* xmodel, AssetDumpingContext& context)
    {
        const JsonDumper dumper(context, stream);
        dumper.Dump(xmodel);
    }
} // namespace T6
