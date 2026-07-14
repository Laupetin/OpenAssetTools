#include "RawLoaderPhysPresetIW3.h"

#include "Game/IW3/IW3.h"
#include "PhysPreset/AbstractPhysPresetLoader.h"

#include <limits>
#include <string>

using namespace IW3;

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

            float friction = 0.0f;
            bool isFrictionInfinity = false;

            for (auto* pair : tree->pair())
            {
                const auto key = pair->key()->getText();
                const auto value = pair->value()->getText();

                if (key == "mass")
                    physPreset.mass = std::stof(value);
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
                else if (key == "sndAliasPrefix")
                    physPreset.sndAliasPrefix = m_memory.Dup(value.c_str());
                else if (key == "piecesSpreadFraction")
                    physPreset.piecesSpreadFraction = std::stof(value);
                else if (key == "piecesUpwardVelocity")
                    physPreset.piecesUpwardVelocity = std::stof(value);
                else if (key == "tempDefaultToCylinder")
                    physPreset.tempDefaultToCylinder = value != "0";
            }

            physPreset.friction = isFrictionInfinity ? std::numeric_limits<float>::max() : friction;

            return true;
        }
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath);
    }
} // namespace phys_preset
