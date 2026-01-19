#include "TechsetCompilerT6.h"

#include "Game/T6/T6.h"

namespace
{
    class TechsetCompilerT6 final : public AssetCreator<T6::AssetTechniqueSet>
    {
    public:
        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            return AssetCreationResult::NoAction();
        }
    };
} // namespace

namespace techset
{
    std::unique_ptr<IAssetCreator> CreateCompilerT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<TechsetCompilerT6>();
    }
} // namespace techset
