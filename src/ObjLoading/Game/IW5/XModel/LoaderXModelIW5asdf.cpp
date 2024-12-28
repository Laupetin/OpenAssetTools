// #include "LoaderXModelIW5.h"

// #include "Game/IW5/IW5.h"
// #include "Game/IW5/XModel/XModelLoaderIW5.h"
// #include "Pool/GlobalAssetPool.h"

// #include <cstring>
// #include <format>
// #include <iostream>

// using namespace IW5;

// namespace
// {
//     class XModelLoader final : public AssetCreator<AssetXModel>
//     {
//     public:
//         XModelLoader(MemoryManager& memory, ISearchPath& searchPath)
//             : m_memory(memory),
//               m_search_path(searchPath)
//         {
//         }

//         AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
//         {
//             const auto file = m_search_path.Open(std::format("xmodel/{}.json", assetName));
//             if (!file.IsOpen())
//                 return AssetCreationResult::NoAction();

//             auto* xmodel = m_memory.Alloc<XModel>();
//             xmodel->name = m_memory.Dup(assetName.c_str());

//             AssetRegistration<AssetXModel> registration(assetName, xmodel);
//             if (!LoadXModel(*file.m_stream, *xmodel, m_memory, context, registration))
//             {
//                 std::cerr << std::format("Failed to load xmodel \"{}\"\n", assetName);
//                 return AssetCreationResult::Failure();
//             }

//             return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
//         }

//     private:
//         MemoryManager& m_memory;
//         ISearchPath& m_search_path;
//     };
// } // namespace

// namespace IW5
// {
//     std::unique_ptr<AssetCreator<AssetXModel>> CreateXModelLoader(MemoryManager& memory, ISearchPath& searchPath)
//     {
//         return std::make_unique<XModelLoader>(memory, searchPath);
//     }
// } // namespace IW5
