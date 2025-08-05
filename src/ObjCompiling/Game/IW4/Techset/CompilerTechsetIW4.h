#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "StateMap/StateMapDefinition.h"
#include "Techset/TechsetDefinition.h"
#include "Utils/MemoryManager.h"

#include <memory>
#include <string>

namespace IW4::techset
{
    class ICreator : public AssetCreator<AssetTechniqueSet>
    {
    public:
        ICreator() = default;
        virtual ~ICreator() = default;

        virtual ::techset::TechsetDefinition* LoadTechsetDefinition(const std::string& assetName, AssetCreationContext& context, bool& failure) = 0;
        virtual const state_map::StateMapDefinition* LoadStateMapDefinition(const std::string& stateMapName, AssetCreationContext& context) = 0;
    };

    std::unique_ptr<ICreator> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4::techset
