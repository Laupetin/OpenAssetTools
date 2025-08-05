#include "AssetLoaderLocalizeIW3.h"

#include "Localize/CommonLocalizeLoader.h"

using namespace IW3;

namespace
{
    class LocalizeLoader final : public AssetCreator<AssetLocalize>, public CommonLocalizeLoader
    {
    public:
        LocalizeLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : CommonLocalizeLoader(searchPath, zone),
              m_memory(memory)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            return CreateLocalizeAsset(assetName, context);
        }

    protected:
        AssetCreationResult CreateAssetFromCommonAsset(const CommonLocalizeEntry& localizeEntry, AssetCreationContext& context) override

        {
            auto* asset = m_memory.Alloc<LocalizeEntry>();
            asset->name = m_memory.Dup(localizeEntry.m_key.c_str());
            asset->value = m_memory.Dup(localizeEntry.m_value.c_str());

            return AssetCreationResult::Success(context.AddAsset<AssetLocalize>(localizeEntry.m_key, asset));
        }

    private:
        MemoryManager& m_memory;
    };
} // namespace

namespace localize
{
    std::unique_ptr<AssetCreator<AssetLocalize>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<LocalizeLoader>(memory, searchPath, zone);
    }
} // namespace localize
