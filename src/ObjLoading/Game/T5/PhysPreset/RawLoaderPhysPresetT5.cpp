#include "RawLoaderPhysPresetT5.h"

#include "Game/T5/ObjConstantsT5.h"
#include "Game/T5/T5.h"
#include "PhysPreset/AbstractPhysPresetLoader.h"

#include <algorithm>
#include <string>

using namespace T5;

namespace
{
    class RawLoaderPhysPreset final : public phys_preset::AbstractPhysPresetLoader<AssetPhysPreset>
    {
    public:
        using AbstractPhysPresetLoader::AbstractPhysPresetLoader;

    protected:
        bool FillFromTree(PhysPreset& physPreset, const std::string& assetName, PhysPresetParser::PhysPresetContext* tree) override
        {
            physPreset.name = m_memory.Dup(assetName.c_str());

            float mass = 0.0f;
            float friction = 0.0f;
            bool isFrictionInfinity = false;

            for (auto* pair : tree->pair())
            {
                const auto key = pair->key()->getText();
                const auto value = pair->value()->getText();

                if (key == "mass")
                    mass = std::stof(value);
                else if (key == "bounce")
                    physPreset.bounce = std::stof(value);
                else if (key == "friction")
                    friction = std::stof(value);
                else if (key == "isFrictionInfinity")
                    isFrictionInfinity = value != "0";
                else if (key == "bulletForceScale")
                    physPreset.bulletForceScale = std::stof(value);
                else if (key == "explosiveForceScale")
                    physPreset.explosiveForceScale = std::stof(value);
                else if (key == "piecesSpreadFraction")
                    physPreset.piecesSpreadFraction = std::stof(value);
                else if (key == "piecesUpwardVelocity")
                    physPreset.piecesUpwardVelocity = std::stof(value);
                else if (key == "canFloat")
                    physPreset.canFloat = std::stoi(value);
                else if (key == "gravityScale")
                    physPreset.gravityScale = std::stof(value);
                else if (key == "massOffsetX")
                    physPreset.centerOfMassOffset.x = std::stof(value);
                else if (key == "massOffsetY")
                    physPreset.centerOfMassOffset.y = std::stof(value);
                else if (key == "massOffsetZ")
                    physPreset.centerOfMassOffset.z = std::stof(value);
                else if (key == "buoyancyMinX")
                    physPreset.buoyancyBoxMin.x = std::stof(value);
                else if (key == "buoyancyMinY")
                    physPreset.buoyancyBoxMin.y = std::stof(value);
                else if (key == "buoyancyMinZ")
                    physPreset.buoyancyBoxMin.z = std::stof(value);
                else if (key == "buoyancyMaxX")
                    physPreset.buoyancyBoxMax.x = std::stof(value);
                else if (key == "buoyancyMaxY")
                    physPreset.buoyancyBoxMax.y = std::stof(value);
                else if (key == "buoyancyMaxZ")
                    physPreset.buoyancyBoxMax.z = std::stof(value);
            }

            physPreset.mass = std::clamp(mass, 1.0f, 2000.0f) * 0.001f;
            physPreset.friction = isFrictionInfinity ? MAX_FRICTION : friction;

            return true;
        }
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath);
    }
} // namespace phys_preset
