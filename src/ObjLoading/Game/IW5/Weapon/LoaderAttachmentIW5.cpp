#include "LoaderAttachmentIW5.h"

#include "Game/IW5/IW5.h"
#include "JsonWeaponAttachmentLoader.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

namespace
{
    class AttachmentLoader final : public AssetCreator<AssetAttachment>
    {
    public:
        AttachmentLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(std::format("attachment/{}.json", assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* attachment = m_memory.Alloc<WeaponAttachment>();
            attachment->szInternalName = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetAttachment> registration(assetName, attachment);
            if (!LoadWeaponAttachmentAsJson(*file.m_stream, *attachment, m_memory, context, registration))
            {
                std::cerr << std::format("Failed to load attachment \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetAttachment>> CreateAttachmentLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<AttachmentLoader>(memory, searchPath);
    }
} // namespace IW5
